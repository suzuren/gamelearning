/*
 Copyright (c) 2011 Aaron Drew
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. Neither the name of the copyright holders nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

//#include <gtest/gtest.h>

#include "eventmanager.h"
#include "notification.h"

using epoll_threadpool::EventManager;
using epoll_threadpool::Notification;

#include <stdio.h>
#include <assert.h>
#include <string.h>



void TEST_EventManagerTest_StartStop() {
	EventManager em;

	assert(em.start(2));
	em.stop();
	assert(em.start(2));
	em.stop();
}

void TEST_EventManagerTest_NotificationDelayedRaise() {
	EventManager em;
	Notification n;
	EventManager::WallTime t = EventManager::currentTime();

	assert(em.start(2));
	em.enqueue(std::tr1::bind(&Notification::signal, &n), t + 0.001);
	assert(n.tryWait(t + 0.500));
	em.stop();
}

void TEST_EventManagerTest_NotificationPreDelayRaise() {
	EventManager em;
	Notification n;
	EventManager::WallTime t = EventManager::currentTime();

	assert(em.start(2));
	em.enqueue(std::tr1::bind(&Notification::signal, &n));
	usleep(10);
	assert(n.tryWait(t + 0.500));
	em.stop();
}

void TEST_EventManagerTest_StartEnqueueStop() {
	EventManager em;
	Notification n;
	EventManager::WallTime t = EventManager::currentTime();

	em.start(2);
	em.enqueue(std::tr1::bind(&Notification::signal, &n));
	assert(n.tryWait(t + 0.500));
	em.stop();
}

void TEST_EventManagerTest_StartEnqueueStop2() {
	EventManager em;
	Notification n;
	EventManager::WallTime t = EventManager::currentTime();

	assert(em.start(2));
	em.enqueue(std::tr1::bind(&Notification::signal, &n), t + 0.001);
	n.wait();
	em.stop();
}

void TEST_EventManagerTest_StartEnqueueStop3() {
	EventManager em;
	Notification n;
	EventManager::WallTime t = EventManager::currentTime();

	assert(em.start(2));
	em.enqueue(std::tr1::bind(&Notification::signal, &n), t + 0.005);
	assert(n.tryWait(t + 0.500));
	em.stop();
}

void EnqueuedCountCheck(pthread_mutex_t *mutex, int *cnt, Notification *n, int expected) {
	pthread_mutex_lock(mutex);
	assert(expected == *cnt);
	(*cnt)++;
	if (n)
	{
		n->signal();
	}
	pthread_mutex_unlock(mutex);
}

void TEST_EventManagerTest_StartEnqueueStop4()
{
	EventManager em;
	Notification n;
	EventManager::WallTime t = EventManager::currentTime();

	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, 0);
	int cnt = 1;

	// Check double precision - should be ok but just to be safe...
	assert(t + 0.001 < t + 0.002);
	assert(t + 0.002 < t + 0.003);
	assert(t + 0.003 < t + 0.004);
	assert(t + 0.004 < t + 0.005);

	em.enqueue(std::tr1::bind(&EnqueuedCountCheck, &mutex, &cnt, &n, 5), t + 0.005);
	em.enqueue(std::tr1::bind(&EnqueuedCountCheck, &mutex, &cnt, (Notification *)NULL, 4), t + 0.004);
	em.enqueue(std::tr1::bind(&EnqueuedCountCheck, &mutex, &cnt, (Notification *)NULL, 2), t + 0.002);
	em.enqueue(std::tr1::bind(&EnqueuedCountCheck, &mutex, &cnt, (Notification *)NULL, 1), t + 0.001);
	em.enqueue(std::tr1::bind(&EnqueuedCountCheck, &mutex, &cnt, (Notification *)NULL, 3), t + 0.003);

	// We start this AFTER adding the tasks to ensure we don't start one before
	// we've added them all. Note that we only start one thread because its 
	// otherwise possible we start two tasks at close to the same time and 
	// the second one runs first, leading to flaky tests.
	assert(em.start(1));
	assert(n.tryWait(t + 0.500));
	em.stop();

	pthread_mutex_destroy(&mutex);
}

void EnqueuedAfterCheck(volatile bool *flag) {
	bool flagcpy = *flag;
	usleep(1000);
	assert(*flag == flagcpy);
}

void EnqueuedAfterCheck2(volatile bool *flag, Notification *n) {
	*flag = true;
	if (n) {
		n->signal();
	}
}

void WorkerStartStop(EventManager *em)
{
	assert(!em->start(2));
	assert(!em->stop());
}

void TEST_EventManagerTest_CallMethodsFromWorkerThread() {
	EventManager em;
	Notification n;
	EventManager::WallTime t = EventManager::currentTime();

	em.enqueue(std::tr1::bind(&WorkerStartStop, &em));
	em.enqueue(std::tr1::bind(&Notification::signal, &n), t + 0.001);
	assert(em.start(2));
	assert(n.tryWait(t + 0.500));
	em.stop();
}

void WatchFdRead(Notification *n, int fd) {
  char buf[9];
  assert(9 == ::read(fd, buf, 9));
  assert(strcmp("testdata", buf) == 0);
  n->signal();
}

void WatchFdWrite(Notification *n, int fd, EventManager *em)
{
	assert(9 == ::write(fd, "testdata", 9));
	// Tests removeFd() from worker thread
	assert(em->removeFd(fd, EventManager::EM_WRITE));
	n->signal();
}

void TEST_EventManagerTest_WatchFdAndRemoveFdFromWorker() {
	EventManager em;
	Notification n, n2;
	EventManager::WallTime t = EventManager::currentTime();

	int fds[2];
	assert(0 == pipe2(fds, O_NONBLOCK));

	em.watchFd(fds[0], EventManager::EM_READ,
		std::tr1::bind(&WatchFdRead, &n, fds[0]));
	em.watchFd(fds[1], EventManager::EM_WRITE,
		std::tr1::bind(&WatchFdWrite, &n2, fds[1], &em));
	assert(em.start(1));
	assert(n.tryWait(t + 0.500));
	assert(n2.tryWait(t + 0.500));

	assert(em.removeFd(fds[0], EventManager::EM_READ));
	em.stop();
	close(fds[0]);
	close(fds[1]);
}

void TEST_EventManagerTest_WatchFdConcurrentReadWrite() {
	EventManager em;
	Notification n, n2;
	EventManager::WallTime t = EventManager::currentTime();

	int fds[2];
	assert(0 == socketpair(AF_UNIX, SOCK_STREAM, 0, fds));

	// Write test data to fds[1] so fds[0] has something to read.
	assert(9 == ::write(fds[1], "testdata", 9));

	em.watchFd(fds[0], EventManager::EM_READ,
		std::tr1::bind(&WatchFdRead, &n, fds[0]));
	em.watchFd(fds[0], EventManager::EM_WRITE,
		std::tr1::bind(&WatchFdWrite, &n2, fds[0], &em));
	assert(em.start(1));
	assert(n.tryWait(t + 0.500));
	assert(n2.tryWait(t + 0.500));

	assert(em.removeFd(fds[0], EventManager::EM_READ));
	em.stop();
	close(fds[0]);
	close(fds[1]);
}

void TestCancelA() {
  // We expect this to get called.
}
void TestCancelB() {
  // We expec this to get cancelled and never called.
  assert(false);
}

void TEST_EventManagerTest_TestCancel() {
	EventManager em;
	Notification n;
	EventManager::WallTime t = EventManager::currentTime();

	em.enqueue(&TestCancelA);
	em.enqueue(&TestCancelA);

	EventManager::Function f(&TestCancelB);
	em.enqueue(f);
	f.cancel();

	em.enqueue(std::tr1::bind(&Notification::signal, &n));

	assert(em.start(1));
	assert(n.tryWait(t + 0.500));
	em.stop();
}


// TODO: Test watchFd() with two events firing one at a time.
// TODO: Test watchFd() with two events firing at the same time.



int main(int argc, const char** argv)
{
	printf("hello world! eventmanager_test.cc\n");

	TEST_EventManagerTest_StartStop();

	TEST_EventManagerTest_NotificationDelayedRaise();

	TEST_EventManagerTest_NotificationPreDelayRaise();

	TEST_EventManagerTest_StartEnqueueStop();

	TEST_EventManagerTest_StartEnqueueStop2();

	TEST_EventManagerTest_StartEnqueueStop3();

	TEST_EventManagerTest_StartEnqueueStop4();

	TEST_EventManagerTest_CallMethodsFromWorkerThread();

	TEST_EventManagerTest_WatchFdAndRemoveFdFromWorker();

	TEST_EventManagerTest_WatchFdConcurrentReadWrite();

	TEST_EventManagerTest_TestCancel();

	return 0;
}


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
#include "iobuffer.h"
//#include <gtest/gtest.h>
#include <string>

using epoll_threadpool::IOBuffer;
using std::string;


#include <stdio.h>
#include <assert.h>


void TEST_IOBufferTest_AppendBytes()
{
	IOBuffer buf;
	const char *a = "abc", *b = "def";

	buf.append(a, 3);
	buf.append(b, 3);
	assert(6 == buf.size());
	assert(NULL == buf.pulldown(7));
	assert(string("abcdef") == string(buf.pulldown(6), 6));
	assert(true == buf.consume(1));
	assert(5 == buf.size());
	assert(NULL == buf.pulldown(6));
	assert(string("bcdef") == string(buf.pulldown(5), 5));
	assert(true == buf.consume(3));
	assert(2 == buf.size());
	assert(string("ef") == string(buf.pulldown(2), 2));
	assert(NULL == buf.pulldown(3));
}

void TEST_IOBufferTest_ConsumeBeforePullDown()
{
	IOBuffer buf;
	const char *a = "abc", *b = "def";

	buf.append(a, 3);
	buf.append(b, 3);
	assert(6 == buf.size());
	assert(true == buf.consume(1));
	assert(5 == buf.size());
	assert(NULL == buf.pulldown(6));
	assert(string("bcdef") == string(buf.pulldown(5), 5));
	assert(true == buf.consume(5));
	assert(0 == buf.size());
	assert(NULL == buf.pulldown(1));
	assert(NULL == buf.pulldown(0));
}


int main(int argc, const char** argv)
{
	printf("hello world! iobuffer_test.cc\n");

	TEST_IOBufferTest_AppendBytes();
	TEST_IOBufferTest_ConsumeBeforePullDown();

	return 0;
}


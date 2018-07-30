
#ifndef __SMART_POINTER_H_
#define __SMART_POINTER_H_

#include <functional>
#include <memory>
#include <iostream>
using namespace std;

void test_check_weak(weak_ptr<int> & wp)
{
	shared_ptr<int> sp = wp.lock();
	if (sp != nullptr)
	{
		cout << "test_check_weak - *sp:" << *sp << endl;
	}
	else
	{
		cout << "test_check_weak - sp is nullptr" << endl;
	}
}
void test_unique_shared()
{
	unique_ptr<int> up1(new int(39));
	cout << "test_unique_shared - *up1:" << *up1 << endl;
	unique_ptr<int> up3 = move(up1);
	cout << "test_unique_shared - *up3:" << *up3 << endl;
	up3.reset();

	shared_ptr<int> sp1(new int(13));
	shared_ptr<int> sp2 = sp1;
	cout << "test_unique_shared - *sp1:" << *sp1 << "   *sp2:" << *sp2 << endl;
	sp1.reset();
	cout << "test_unique_shared - *sp2:" << *sp2 << endl;


	weak_ptr<int> wp = sp2;
	test_check_weak(wp);
	sp2.reset();
	test_check_weak(wp);
}


void test_smart_pointer()
{
	test_unique_shared();
}

#endif

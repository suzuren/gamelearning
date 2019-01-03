#include <stdio.h>
#include <string>
#include <iostream>


class CTestCalss
{
public:
	CTestCalss() { std::cout << "constructor function CTestCalss" << endl; }
	~CTestCalss() { std::cout << "destructor function CTestCalss" << endl; }
	static void * operator new(size_t size)
	{
		std::cout << "operator new function CTestCalss" << endl;
		return ::operator new(size);
	}
	static void operator delete(void *ptrClass)
	{
		std::cout << "operator delete function CTestCals" << endl;
		::operator delete(ptrClass);
	}
};


int test_new_delete()
{
	// 使用重载后的 new delete
	CTestCalss *pTestCalss = new CTestCalss;
	//CTestCalssA::operator delete(pCTestCalssA);// 只释放空间不执行析构函数
	delete pTestCalss;

	// 使用重载前的 new delete
	CTestCalss *pCTestCalss2 = ::new CTestCalss;
	::delete pCTestCalss2;
	
	return 0;
}




/*
执行输出

operator new function CTestCalss
constructor function CTestCalss
destructor function CTestCalss
operator delete function CTestCals


constructor function CTestCalss
destructor function CTestCalss




C++的new是语言自定义的操作符，这个操作符的行为包含两件事，而且你不能改变。
第一件事：调用operator new分配内存。所以通常说的重载new函数其实重载的是 operator new，你无法重载 new。
第二件事：调用你要new的对象的所属类的构造函数初始化第一步中分配的内存。
这就是为什么你Object *pobj = new Object后，可以直接使用pobj->调用类中函数的原因。

delete操作符也是类似，不过先调用析构函数，然后释放内存，对应有一个operator delete。

代码的目的是重载 operator new 和 operator delete，把 new 和 delete 的历史通过一个 ostream 记录下来。

*/




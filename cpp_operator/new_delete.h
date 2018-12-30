#include <stdio.h>
#include <string>
#include <iostream>



class CTestCalss
{
public:
	CTestCalss() { std::cout << "constructor function" << endl; }
	~CTestCalss() { std::cout << "destructor function" << endl; }
	void * operator new(size_t size, ostream& out)
	{
		out << "new CTestCalss" << endl;
		return ::operator new(size);
	}
	void operator delete(void *mem, ostream& out)
	{
		out << "delete CTestCalss" << endl;
		::operator delete(mem);
	}
};


int test_new_delete()
{
	CTestCalss *pCTestCalss = new (std::cout)CTestCalss;
	CTestCalss *pCTestCalss2 = ::new CTestCalss;
	//delete pCTestCalss;   //这句编译不通过，说error: no suitable ‘operator delete’ for ‘CTestCalss’
	CTestCalss::operator delete(pCTestCalss, std::cout);
	::delete pCTestCalss2;
	
	return 0;
}

/*
执行输出
new CTestCalss
constructor function
constructor function
delete CTestCalss
destructor function




C++的new是语言自定义的操作符，这个操作符的行为包含两件事，而且你不能改变。
第一件事：调用operator new分配内存。所以通常说的重载new函数其实重载的是 operator new，你无法重载 new。
第二件事：调用你要new的对象的所属类的构造函数初始化第一步中分配的内存。
这就是为什么你Object *pobj = new Object后，可以直接使用pobj->调用类中函数的原因。

delete操作符也是类似，不过先调用析构函数，然后释放内存，对应有一个operator delete。

代码的目的是重载 operator new 和 operator delete，把 new 和 delete 的历史通过一个 ostream 记录下来。


CTestCalss *pCTestCalss = new (std::cout)CTestCalss; 这句 new 一个CTestCalss的对象，
因为 CTestCalss 类中重载了operator new，所以需要传递一个 ostream 的对象进去，就像这样传递。
如果你这样调用：  CTestCalss *pCTestCalss = new CTestCalss;那是会报错的，
因为 CTestCalss 类中的 operator new 把默认的隐藏掉了。这句产生输出：new CTestCalss 和 constructor function。
CTestCalss *pCTestCalss2 = ::new CTestCalss; 这句也是 new 一个 CTestCalss 的对象，
但是 new 前面多了两个冒号，意思是调用全局的 new，调用了上面说的被隐藏调的 operator new。这句产生输出：constructor function。
delete pCTestCalss; 这句想要 delete 掉 pCTestCalss 指向的对象，但是失败，原因在后面。
这句（期望）输出： delete CTestCalss 和 destructor function。

CTestCalss::operator delete(pCTestCalss, std::coutcout);输出delete CTestCalss，这函数里面直接释放内存，就不会再执行析构函数了
貌似这个delete是不能通过delete关键字在外面调用的，这个一般都是给系统在 placement new 出错时调用，
而且只要有使用 placement new，析构一般都是要手动进行的。
c++为什么没有内置的"placement new"是因为 没办法提供一个通用的。
CTestCalss *pCTestCalss = new (std::cout)CTestCalss;当我们写出这样的代码的时候，
c++不能推出 pCTestCalss 指针里面存放的对象类型。（虽然我们知道是什么类型）。所以不会有placement delete.
只有在placement new抛出异常的时候，编译器才会调用placement delete.这个时候因为构造不成功所以只需要负责释放内存即可。
所以只能是先显示调用析构函数，然后在显示调用 placement delete.

::delete pCTestCalss2; 这句 delete 掉 pCTestCalss2 指向的对象，调用全局的 delete ，没有问题，这句输出：destructor function。


*/




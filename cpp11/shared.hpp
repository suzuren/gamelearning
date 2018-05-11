
#ifndef _SHARED_H__
#define _SHARED_H__


#include <functional>
#include <tr1/memory>
#include <tr1/functional>
#include <string>
#include <stdio.h>

//using std::string;
//using std::tr1::function;
//using std::tr1::shared_ptr;

class Base;
class Derived;
void function1(std::tr1::shared_ptr<Base> derived);
void function2(std::tr1::shared_ptr<Derived> derived);

class Base : public std::tr1::enable_shared_from_this<Base>
{
public:
	Base() { /* ... */ }
	virtual ~Base() { }

	void method()
	{
		printf("hello Base method ...\n");
		// ...

		// 传递该类shared_ptr托管的this指针
		function1(shared_from_this());
	}
	// ...

private:
	// ...
};

void function1(std::tr1::shared_ptr<Base> derived)
{
	printf("hello function1 ...\n");
	// ...
}


class Derived : public Base
{
public:
	Derived() { /* ... */ }
	~Derived() { }

	void method(std::tr1::shared_ptr<Derived> derived)
	{
		printf("hello Derived method ...\n");
		// ...

		// 错误：不存在用户定义的从“std::shared_ptr<Base>”到“std::shared_ptr<Derived>”的适当转换
		//function2(shared_from_this());

		//1. 使用std::dynamic_pointer_cast<T>()需要基类中存在虚函数，这是由于这个转换函数使用输入的类型和目标类型中是否存在相同
		//签名的虚函数作为转换能否成功的标识。最简单也是正确的解决方法是将基类中的析构函数声明为虚函数。

		//2. 不能在构造函数中使用shared_form_this()。这是由于std::enable_share_from_this在实现时使用了一个对象的weak_ptr，而这
		//个weak_ptr需要对象的shared_ptr进行初始化。由于此时对象尚未构造完成，所以会抛出std::bad_weak_ptr的异常。关于这点目前没
		//有较为完美的方案，可以尝试写一个init()函数，在对象构造后手动调用。或是手动写一个std::shared_ptr<Derived>(this)使用，
		//但这种解决方案可能造成循环引用。

		std::tr1::shared_ptr<Derived> pointer = std::tr1::dynamic_pointer_cast<Derived>(shared_from_this());
		function2(pointer);
	}

private:
	// ...
};

void function2(std::tr1::shared_ptr<Derived> derived)
{
	printf("hello function1 ...\n");
	// ...
}





#include <iostream>
#include <memory>
#include <vector>

using namespace std;

class ClassB;

class ClassA
{
public:
	ClassA() { cout << "ClassA Constructor..." << endl; }
	~ClassA() { cout << "ClassA Destructor..." << endl; }
	shared_ptr<ClassB> pb;  // 在A中引用B
};

class ClassB
{
public:
	ClassB() { cout << "ClassB Constructor..." << endl; }
	~ClassB() { cout << "ClassB Destructor..." << endl; }
	shared_ptr<ClassA> pa;  // 在B中引用A
};



#endif


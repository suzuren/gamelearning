#include <stdio.h>
#include <iostream>
class CBaseTest;
void finish(CBaseTest * p);
class CBaseTest
{
public:
	virtual ~CBaseTest(){		finish(this);	}
	void echo()	{		std::cout << "eb# ";	}
	void print(){		std::cout << "pb# ";	}
};
class CDeriveTest : public CBaseTest
{
public:
	virtual~CDeriveTest(){		finish(this);	}
	void echo()	{		std::cout << "ed# ";	}
	void print(){		std::cout << "pd# ";	}
};
void finish(CBaseTest * p){	p->echo(); p->print();}
void test_CBaseTest()
{
	CBaseTest * pBaseTest = new CDeriveTest();
	delete pBaseTest;
	std::cout << std::endl;
}
//eb# pb# eb# pb# 

// 如果 echo() print() 是 virtual结果是
//ed# pd# eb# pb#

//因为finish函数里面的参数 p 的静态类型永远都是 CBaseTest *，
//不管 p 指向哪一个子类，他都会直接调用CBaseTest类的echo 和print，
//如果那两个函数是虚函数，才会去调用p指向的动态类型的echo和print

//静态类型：对象在声明时采用的类型，在编译期既已确定；
//动态类型：通常是指一个指针或引用目前所指对象的类型，是在运行期决定的；
//静态绑定：绑定的是静态类型，所对应的函数或属性依赖于对象的静态类型，发生在编译期；
//动态绑定：绑定的是动态类型，所对应的函数或属性依赖于对象的动态类型，发生在运行期；

/*

class A
{
public:
	void func() { std::cout << "A::func()\n"; }
};
class B : public A
{
public:
	void func() { std::cout << "B::func()\n"; }
};
class C : public A
{
public:
	void func() { std::cout << "C::func()\n"; }
};
下面逐步分析测试代码及结果，

1 C* pc = new C(); //pc的静态类型是它声明的类型C*，动态类型也是C*；
2 B* pb = new B(); //pb的静态类型和动态类型也都是B*；
3 A* pa = pc;      //pa的静态类型是它声明的类型A*，动态类型是pa所指向的对象pc的类型C*；
4 pa = pb;         //pa的动态类型可以更改，现在它的动态类型是B*，但其静态类型仍是声明时候的A*；
5 C *pnull = NULL; //pnull的静态类型是它声明的类型C*,没有动态类型，因为它指向了NULL；
如果明白上面代码的意思，请继续，

1 pa->func();      //A::func() pa的静态类型永远都是A*，不管其指向的是哪个子类，都是直接调用A::func()；
2 pc->func();      //C::func() pc的动、静态类型都是C*，因此调用C::func()；
3 pnull->func();   //C::func() 不用奇怪为什么空指针也可以调用函数，因为这在编译期就确定了，和指针空不空没关系；

如果注释掉类C中的func函数定义，其他不变，即

复制代码
1 class C : public A
2 {
3 };
4
5 pa->func();      //A::func() 理由同上；
6 pc->func();      //A::func() pc在类C中找不到func的定义，因此到其基类中寻找；
7 pnull->func();   //A::func() 原因也解释过了；
复制代码
如果为A中的void func()函数添加virtual特性，其他不变，即

复制代码
1 class A
2 {
3 public:
4     virtual void func() { std::cout << "A::func()\n"; }
5 };
6
7 pa->func();      //B::func() 因为有了virtual虚函数特性，pa的动态类型指向B*，因此先在B中查找，找到后直接调用；
8 pc->func();      //C::func() pc的动、静态类型都是C*，因此也是先在C中查找；
9 pnull->func();   //空指针异常，因为是func是virtual函数，因此对func的调用只能等到运行期才能确定，然后才发现pnull是空指针；
复制代码

分析：
在上面的例子中，
1. 如果基类A中的func不是virtual函数，那么不论pa、pb、pc指向哪个子类对象，对func的调用都是在定义pa、pb、pc时的静态类型决定，早已在编译期确定了。
同样的空指针也能够直接调用no-virtual函数而不报错（这也说明一定要做空指针检查啊！），因此静态绑定不能实现多态；
2. 如果func是虚函数，那所有的调用都要等到运行时根据其指向对象的类型才能确定，比起静态绑定自然是要有性能损失的，但是却能实现多态特性；
本文代码里都是针对指针的情况来分析的，但是对于引用的情况同样适用。




* /
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

// ��� echo() print() �� virtual�����
//ed# pd# eb# pb#

//��Ϊfinish��������Ĳ��� p �ľ�̬������Զ���� CBaseTest *��
//���� p ָ����һ�����࣬������ֱ�ӵ���CBaseTest���echo ��print��
//����������������麯�����Ż�ȥ����pָ��Ķ�̬���͵�echo��print

//��̬���ͣ�����������ʱ���õ����ͣ��ڱ����ڼ���ȷ����
//��̬���ͣ�ͨ����ָһ��ָ�������Ŀǰ��ָ��������ͣ����������ھ����ģ�
//��̬�󶨣��󶨵��Ǿ�̬���ͣ�����Ӧ�ĺ��������������ڶ���ľ�̬���ͣ������ڱ����ڣ�
//��̬�󶨣��󶨵��Ƕ�̬���ͣ�����Ӧ�ĺ��������������ڶ���Ķ�̬���ͣ������������ڣ�

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
�����𲽷������Դ��뼰�����

1 C* pc = new C(); //pc�ľ�̬������������������C*����̬����Ҳ��C*��
2 B* pb = new B(); //pb�ľ�̬���ͺͶ�̬����Ҳ����B*��
3 A* pa = pc;      //pa�ľ�̬������������������A*����̬������pa��ָ��Ķ���pc������C*��
4 pa = pb;         //pa�Ķ�̬���Ϳ��Ը��ģ��������Ķ�̬������B*�����侲̬������������ʱ���A*��
5 C *pnull = NULL; //pnull�ľ�̬������������������C*,û�ж�̬���ͣ���Ϊ��ָ����NULL��
�����������������˼���������

1 pa->func();      //A::func() pa�ľ�̬������Զ����A*��������ָ������ĸ����࣬����ֱ�ӵ���A::func()��
2 pc->func();      //C::func() pc�Ķ�����̬���Ͷ���C*����˵���C::func()��
3 pnull->func();   //C::func() �������Ϊʲô��ָ��Ҳ���Ե��ú�������Ϊ���ڱ����ھ�ȷ���ˣ���ָ��ղ���û��ϵ��

���ע�͵���C�е�func�������壬�������䣬��

���ƴ���
1 class C : public A
2 {
3 };
4
5 pa->func();      //A::func() ����ͬ�ϣ�
6 pc->func();      //A::func() pc����C���Ҳ���func�Ķ��壬��˵��������Ѱ�ң�
7 pnull->func();   //A::func() ԭ��Ҳ���͹��ˣ�
���ƴ���
���ΪA�е�void func()�������virtual���ԣ��������䣬��

���ƴ���
1 class A
2 {
3 public:
4     virtual void func() { std::cout << "A::func()\n"; }
5 };
6
7 pa->func();      //B::func() ��Ϊ����virtual�麯�����ԣ�pa�Ķ�̬����ָ��B*���������B�в��ң��ҵ���ֱ�ӵ��ã�
8 pc->func();      //C::func() pc�Ķ�����̬���Ͷ���C*�����Ҳ������C�в��ң�
9 pnull->func();   //��ָ���쳣����Ϊ��func��virtual��������˶�func�ĵ���ֻ�ܵȵ������ڲ���ȷ����Ȼ��ŷ���pnull�ǿ�ָ�룻
���ƴ���

������
������������У�
1. �������A�е�func����virtual��������ô����pa��pb��pcָ���ĸ�������󣬶�func�ĵ��ö����ڶ���pa��pb��pcʱ�ľ�̬���;����������ڱ�����ȷ���ˡ�
ͬ���Ŀ�ָ��Ҳ�ܹ�ֱ�ӵ���no-virtual��������������Ҳ˵��һ��Ҫ����ָ���鰡��������˾�̬�󶨲���ʵ�ֶ�̬��
2. ���func���麯���������еĵ��ö�Ҫ�ȵ�����ʱ������ָ���������Ͳ���ȷ��������̬����Ȼ��Ҫ��������ʧ�ģ�����ȴ��ʵ�ֶ�̬���ԣ�
���Ĵ����ﶼ�����ָ�������������ģ����Ƕ������õ����ͬ�����á�




* /
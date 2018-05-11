#include <stdio.h>
#include <string>
using namespace std;
#include "Timer.hpp"
#include "type_name.hpp"

#include "shared.hpp"


int main(int argc, const char** argv)
{
	printf("hello world!\n");
	
	printf("\n");
	printf("type_name:%s\n", type_name<int>().c_str());
	printf("type_name:%s\n", type_name<const int>().c_str());
	printf("type_name:%s\n", type_name<int const>().c_str());

	printf("\n");
	Timer objTimer;
	printf("objTimer\n");
	printf("elapsed:%ld\n", objTimer.elapsed());
	printf("elapsed_second:%f\n", objTimer.elapsed_second());
	printf("elapsed_micro:%ld\n", objTimer.elapsed_micro());
	printf("elapsed_nano:%ld\n", objTimer.elapsed_nano());
	printf("elapsed_minutes:%ld\n", objTimer.elapsed_minutes());
	printf("elapsed_hours:%ld\n", objTimer.elapsed_hours());


	std::tr1::shared_ptr<Base> pBase(new Base);
	pBase->method();

	std::tr1::shared_ptr<Derived> pDerived(new Derived);
	//pDerived->method();

	shared_ptr<ClassA> spa = make_shared<ClassA>();
	shared_ptr<ClassB> spb = make_shared<ClassB>();
	spa->pb = spb;
	spb->pa = spa;
	// 函数结束，思考一下：spa和spb会释放资源么？


	return 0;
}





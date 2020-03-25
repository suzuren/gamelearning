
#include <stdio.h>

#include <memory.h>


class Base
{
public:
	Base()
	{
		printf("Base func\n");
	}
	virtual ~Base()
	{
		printf("De Base func\n");
	}
};

class Derived : public Base
{
public:
	Derived()
	{
		printf("Derived func\n");
	}
	~Derived()
	{
		printf("De Derived func\n");
	}
};

void test_class()
{
	printf("---------------------------------\n");

	Derived* pDerived = new Derived();
	delete pDerived;


	printf("---------------------------------\n");

	Base* pBase = new Derived();
	delete pBase;


}


/*

�����������������麯�� ~Base()


---------------------------------
Base func
Derived func
De Derived func
De Base func
---------------------------------
Base func
Derived func
De Base func


���������������麯�� virtual ~Base()

---------------------------------
Base func
Derived func
De Derived func
De Base func
---------------------------------
Base func
Derived func
De Derived func
De Base func



*/
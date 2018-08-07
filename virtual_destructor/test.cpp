
#include <stdio.h>

void func_test()
{
	printf("func_test function.\n");
}

class CBase
{
public:
	CBase()
	{
		i = 9;
		printf("CBase function i:%d.\n",i);
	}
	virtual ~CBase()
	{
		printf("~CBase function.\n");
	}
private:
	int i;
public:
	//void (*)() pFunc;
};

class CDerive : public CBase
{
public:
	CDerive()
	{
		j = 8;
		printf("CDerive function j:%d.\n", j);
	}
	virtual~CDerive()
	{
		printf("~CDerive function.\n");
	}

private:
	int j;
};

int main(int argc, const char** argv)
{
	printf("test virtual_destructor . \n");

	CDerive * pDerive = new CDerive();
	CBase * pBase = pDerive;
	if (pBase != NULL)
	{
		delete pBase;
		pBase = NULL;
	}

	return 0;
}
/*

CBase function i : 9.
CDerive function j : 8.
~CDerive function.
~CBase function.

*/
#ifndef _TEMPLATE_H__
#define _TEMPLATE_H__

template <typename T> void templateFunction(T a)
{
	T b = a;
	T c = a + b;
	cout << "templateFunction - test temp : a + b = -> " << a << " + " << b << " = " <<  c << endl;
}

template <typename T1, typename T2 = int>
void templateDefParam(T1 a, T2 b = 2)
{
	cout << "templateDefParam - a: " << a << "    b: " << b << endl;
}


void test_templateFunc()
{
	templateFunction(1);
	//templateFunction("a");
	templateDefParam(1.1);

	templateDefParam(1.3, 16);
}


#endif

#include <iostream>
#include <string>
using namespace std;

void lambda_test1()
{
	cout << endl;
	auto basicLambda = [] { cout << "lambda_test1 - Hello, world!" << endl; };
	// 调用
	basicLambda();   // 输出：Hello, world!
}

void lambda_test2()
{
	// 指明返回类型
	auto add = [](int a, int b) -> int { return a + b; };
	// 自动推断返回类型
	auto multiply = [](int a, int b) { return a * b; };

	int sum = add(2, 5);   // 输出：7
	int product = multiply(2, 5);  // 输出：10

	cout << "lambda_test2 - " << "sum:"<< sum << "  product:" << product << endl;

}


void lambda_test3()
{
	int x = 10;

	auto add_x = [x](int a) { return a + x; };  // 复制捕捉x
	auto multiply_x = [&x](int a) { return a * x; };  // 引用捕捉x

	cout << "lambda_test3 - " << add_x(10) << "   " << multiply_x(10) << endl;
	// 输出：20 100
}

void lambda_test4()
{
	int x = 10;

	auto add_x = [x](int a) mutable { x *= 2; return a + x; };  // 复制捕捉x

	cout << "lambda_test4 - " << add_x(10) << endl; // 输出 30
}


/*
有以下几种情况来捕获其所在作用域中的变量：

[]		：默认不捕获任何变量；
[=]		：默认以值捕获所有变量；
[&]		：默认以引用捕获所有变量；
[x]		：仅以值捕获 x，其它变量不捕获；
[&x]	：仅以引用捕获 x，其它变量不捕获；
[=, &x]	：默认以值捕获所有变量，但是 x 是例外，通过引用捕获；
[&, x]	：默认以引用捕获所有变量，但是 x 是例外，通过值捕获；
[this]	：通过引用捕获当前对象（其实是复制指针）；
[*this]	：通过传值方式捕获当前对象；

*/
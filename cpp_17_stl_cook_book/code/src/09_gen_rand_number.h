

#include <vector>
#include <random>
#include <chrono>

//https://zh.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
//https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution

int RandRange(int iMin, int iMax)
{
	if (iMin == iMax)
	{
		return iMin;
	}
	if (iMin > iMax)
	{
		int itemp = iMin;
		iMin = iMax;
		iMax = itemp;
	}
	std::random_device rd;
	std::mt19937 g{ rd() };
	int iRandGen = (g() % (iMax - iMin + 1)) + iMin;
	//int iRandGen =  g() % 7;
	return iRandGen;
}

//[min.max] 包含边界
float Randf(float min, float max)
{
	std::random_device r;
	std::default_random_engine generator(r());
	std::uniform_real_distribution<float> dis(min, max);
	auto dice = std::bind(dis, generator);
	return dice();
}

//[min.max] 包含边界
int Rand(int min, int max)
{
	std::random_device r;
	std::default_random_engine generator(r());
	std::uniform_int_distribution<int> dis(min, max);
	auto dice = std::bind(dis, generator);
	return dice();
}


int test_gen_rand_number()
{
	std::cout << "test_gen_rand_number - vector raand -     print:";

	std::vector<int> vecRand{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::random_device rd;
	std::mt19937 g{ rd() };
	//std::cout << g()%10 << std::endl;
	std::shuffle(std::begin(vecRand), std::end(vecRand), g);
	std::copy(std::begin(vecRand), std::end(vecRand), std::ostream_iterator<int>{std::cout, ", "});
	std::cout << '\n';

	//std::mt19937 rng((int)std::chrono::steady_clock::now().time_since_epoch().count());
	//std::cout << rng() << std::endl;

	//for (int i = 0; i < 100; i++)
	//{
	//	int iRandGen = RandRange(3, 7);
	//	printf("i:%02d,iRandGen:%d\n",i, iRandGen);
	//}

	//for (int i = 0; i < 100; i++)
	{
		// 输出包含 3 4 5 6 四个数值
		int iRandGen = Rand(3, 6);
		//printf("i:%02d,iRandGen:%d\n",i, iRandGen);
	}

	for (int i = 0; i < 100; i++)
	{
		// 输出包含 3 4 5 6 四个数值
		float fmin = 3.3;
		float fmax = 3.7;
		float fRandGen = Randf(fmin, fmax);
		std::cout << "i:" << i << "fRandGen:" << fRandGen << std::endl;
	}

	return 0;
}



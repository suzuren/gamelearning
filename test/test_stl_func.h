
#include <stdio.h>
#include <vector>
#include <map>
using namespace std;

typedef unsigned int		uint32;
#define ROBOT_MAX_LEVEL 17

void test_stl_function()
{
	uint32 cfgLevel = 13;
	map<uint32, vector<uint32>> mpBatchCount;
	vector<uint32> vecTempCount;
	vecTempCount.resize(ROBOT_MAX_LEVEL);
	for (uint32 index = 0; index < vecTempCount.size(); index++)
	{
		vecTempCount.push_back(0);
	}
	vecTempCount[cfgLevel]++;
	mpBatchCount.insert(make_pair(12, vecTempCount));
}











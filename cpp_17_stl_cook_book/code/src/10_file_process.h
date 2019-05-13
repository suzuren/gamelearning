

#include <memory.h>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include "Utf8ToGbk.h"


std::vector<std::string> GetFileListInFolder(std::string folderPath, int depth)
{
	std::vector<std::string> result;
	DIR *pDir;
	struct dirent *ent;
	char absolutepath[512];
	pDir = opendir(folderPath.c_str());
	while ((ent = readdir(pDir)) != NULL)
	{
		if (ent->d_type & DT_DIR)
		{
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}
		}
		else
		{
			memset(absolutepath, 0, sizeof(absolutepath));
			sprintf(absolutepath, "%s/%s", folderPath.c_str(), ent->d_name);
			result.push_back(absolutepath);
		}
	}

	std::sort(result.begin(), result.end());
	return result;
}

void StringReplace(std::string & strBig, const std::string & strsrc, const std::string & strdst)
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}

#define GET_CLASS_NAME(className) (#className)

class LenRedisClient
{
public:
	LenRedisClient()
	{
		i = 0;
		j = 0;
	}
	~LenRedisClient()
	{
		i = -1;
		j = -1;
	}
protected:
	int i;
private:
	int j;
};



int test_file_process()
{
	std::cout << "test_file_process - 0 " << std::endl;

	std::string strInitPath = "./";

	auto fileList = GetFileListInFolder(strInitPath, 0);
	for (auto fileName : fileList)
	{
		std::cout << "fileName:" << fileName << std::endl;
	}

	std::cout << "test_file_process - 1" << std::endl;

	for (auto fileName : fileList)
	{
		//std::cout << "fileName 0:" << fileName << std::endl;
		//StringReplace(fileName, "\\", "/");
		//std::cout << "fileName 1:" << fileName << std::endl;
		StringReplace(fileName, "//", "/");
		//std::cout << "fileName 2:" << fileName << std::endl;
		auto strExt = fileName.substr(fileName.find_last_of('.') + 1, fileName.length() - fileName.find_last_of('.') - 1);
		auto strFileName = fileName.substr(fileName.find_last_of('/') + 1, fileName.find_last_of('.') - fileName.find_last_of('/') - 1);
		std::cout << "fileName 3:" << fileName <<" - strFileName:"<< strFileName<< " - strExt:" << strExt << std::endl;

	}

	std::cout << "test_file_process - 2" << std::endl;

	const char * pTestText = "中文字体测试";
	int length = strlen(pTestText);
	bool bIsTextUTF8 = IsTextUTF8(pTestText, length);
	
	std::cout << "pTestText:" << pTestText << std::endl;
	std::cout << "length:"<< length << std::endl;
	std::cout << "bIsTextUTF8:" << bIsTextUTF8 << std::endl;

	char strutf8[1024] = { 0 };
	int iGbkToUtf8 = GbkToUtf8(pTestText, strutf8);
	std::cout << "iGbkToUtf8:" << iGbkToUtf8 << std::endl;
	std::cout << "strutf8:" << strutf8 << std::endl;
	bIsTextUTF8 = IsTextUTF8(strutf8, (int)strlen(strutf8));
	std::cout << "bIsTextUTF8:" << bIsTextUTF8 << std::endl;

	std::string strClassName = GET_CLASS_NAME(LenRedisClient);
	std::cout << "strClassName:" << strClassName << std::endl;

	return 0;
}



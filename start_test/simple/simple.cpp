#include <iostream>
#include <string>

typedef struct gate_context_t
{
	int cid;
	int sid;
	gate_context_t()
	{
		Init();
	}
	bool operator==(const struct gate_context_t& info)
	{
		bool ret = false;
		if (sid > 0 && cid >= 0 && info.sid > 0 && info.cid >= 0 && cid == info.cid && sid == info.sid)
		{
			ret = true;
		}
		return ret;
	}
	void operator=(const struct gate_context_t& info)
	{
		cid = info.cid;
		sid = info.sid;
	}
	void Init()
	{
		cid = -1;
		sid = 0;
	}
	bool IsValid()
	{
		bool ret = false;
		if (sid > 0 && cid >= 0)
		{
			ret = true;
		}
		return ret;
	}
}gate_context;


int main(int argc, char const *argv[])
{
	gate_context cotext_1;
	gate_context cotext_2;

	bool b0 = (cotext_1 == cotext_2);
	std::cout << "b0 : " << b0 << std::endl;

	cotext_1.cid = 1;
	cotext_1.sid = 2;

	bool b1 = (cotext_1 == cotext_2);
	std::cout << "b1 : " << b1 << std::endl;

	cotext_2 = cotext_1;

	bool b2 = (cotext_1 == cotext_2);
	std::cout << "b2 : " << b2 << std::endl;

	/*
b0 : 0
b1 : 0
b2 : 1
	*/


	std::string strIpv4 = "127.0.0.1:34597";
	std::string strIpv6 = "127::0:0:1:68897";

	auto const find_pos_v4 = strIpv4.find_last_of(':');

	std::string subIpv4;
	std::string subportv4;
	if (find_pos_v4 != std::string::npos)
	{
		subIpv4 = strIpv4.substr(0, find_pos_v4);
	}
	if (find_pos_v4 + 1 != std::string::npos)
	{
		subportv4 = strIpv4.substr(find_pos_v4 + 1, std::string::npos);
	}

	std::cout << "      strIpv4 : " << strIpv4 << std::endl;
	std::cout << "  find_pos_v4 : " << find_pos_v4 << std::endl;
	std::cout << "      subIpv4 : " << subIpv4 << std::endl;
	std::cout << "    subportv4 : " << subportv4 << std::endl;


	auto const find_pos_v6 = strIpv6.find_last_of(':');

	std::string subIpv6;
	std::string subportv6;
	if (find_pos_v6 != std::string::npos)
	{
		subIpv6 = strIpv6.substr(0, find_pos_v6);
	}
	if (find_pos_v6 + 1 != std::string::npos)
	{
		subportv6 = strIpv6.substr(find_pos_v6 + 1, std::string::npos);
	}

	std::cout << "      strIpv6 : " << strIpv6 << std::endl;
	std::cout << "  find_pos_v6 : " << find_pos_v6 << std::endl;
	std::cout << "      subIpv6 : " << subIpv6 << std::endl;
	std::cout << "    subportv6 : " << subportv6 << std::endl;

	/*
	  strIpv4 : 127.0.0.1:34597
  find_pos_v4 : 9
	  subIpv4 : 127.0.0.1
	subportv4 : 34597
	  strIpv6 : 127::0:0:1:68897
  find_pos_v6 : 10
	  subIpv6 : 127::0:0:1
	subportv6 : 68897
	*/

	return 0;
}




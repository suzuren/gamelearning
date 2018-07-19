
#ifndef __CURL_PER_H__
#define __CURL_PER_H__

#include <curl/curl.h>

static size_t req_reply(void *ptr, size_t size, size_t nmemb, void *stream)
{
	std::string *str = (std::string *) stream;
	//cout<<*str<<endl;
	(*str).append((char*)ptr, size*nmemb);
	return size*nmemb;
}

int curl_per_post(const std::string &url, const std::string &data, std::string &response)
{
	CURLcode res;
	CURL *curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		//curl_easy_setopt(curl, CURLOPT_VERBOSE,1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
		//strat req
		res = curl_easy_perform(curl);
	}
	curl_easy_cleanup(curl);

	LOG_DEBUG("response:%s", response.c_str());

	return res;
}


#endif



#ifndef __HTTP_REQUEST_H_
#define __HTTP_REQUEST_H_

#include <vector>
#include <string>
#include "http.h"
#include "http_method.h"
#include "http_protocol.h"

class HttpRequest {
public:
    HttpRequest(void);
    ~HttpRequest(void);

    void SetUrl(std::string);
    std::string GetUrl(void) const;
    void SetProtocol(Protocol);
    void SetProtocol(std::string);
    Protocol GetProtocol(void) const;
    HttpProtocol GetProtocolClass(void) const;
    void AddHeader(std::string, std::string);
    void SetHeader(const std::vector<std::pair<std::string, std::string> > &);
    std::string GetData(void) const;
    void SetRequestBody(const std::string &);
    std::string GetRequestBody(void) const;
    void SetMethod(Method);
    void SetMethod(std::string);
    Method GetMethod(void) const;
    HttpMethod GetMethodClass(void) const;
    size_t GetLength(void);
    std::string* GetDataPtr(void);

    void AddData(const char *, const int &);
    void ShowData(void) const;
    bool Parse(void);
    bool MergeData(void);

private:
    std::string url_;
    std::string data_;
    std::string request_body_;
    HttpMethod method_;
    HttpProtocol protocol_;
    std::vector <std::pair<std::string, std::string> > header_;
};

#endif /* __HTTP_REQUEST_H_ */


#include <string>
#include "http.h"
#include "http_method.h"

HttpMethod::HttpMethod(void) {
}

HttpMethod::~HttpMethod(void) {
}

void HttpMethod::Set(Method method) {
    method_ = method;
}

void HttpMethod::Set(std::string method) {
    if(method == "GET") {
        Set(GET);
    }
    else {
        Set(NOT_IMPLEMENTED);
    }
}

void HttpMethod::Set(const char *str, int len) {
    Set(std::string(str, len));
}

Method HttpMethod::Get(void) const {
    return method_;
}

std::string HttpMethod::GetString(void) const {
    if(method_ == GET) {
        return "GET";
    }
    else {
        return "NON_IMPLEMENTED";
    }
}

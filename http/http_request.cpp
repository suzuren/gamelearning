
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
//#include <glog/logging.h>
#include "http_method.h"
#include "http_request.h"
#include "http_protocol.h"

HttpRequest::HttpRequest(void) {
    data_ = "";
    method_ = HttpMethod();
    protocol_ = HttpProtocol();
}

HttpRequest::~HttpRequest(void) {
}

void HttpRequest::SetUrl(std::string url) {
    url_ = url;
}

std::string HttpRequest::GetUrl(void) const {
    return url_;
}

void HttpRequest::SetProtocol(Protocol protocol) {
    protocol_.Set(protocol);
}

void HttpRequest::SetProtocol(std::string protocol) {
    protocol_.Set(protocol);
}

Protocol HttpRequest::GetProtocol(void) const {
    return protocol_.Get();
}

HttpProtocol HttpRequest::GetProtocolClass(void) const {
    return protocol_;
}

std::string HttpRequest::GetData(void) const {
    return data_;
}

void HttpRequest::AddHeader(std::string name, std::string content) {
    header_.push_back(std::make_pair(name, content));
}

void HttpRequest::SetHeader(
    const std::vector<std::pair<std::string, std::string> > &header) {
    for(auto each: header) {
        AddHeader(each.first, each.second);
    }
}

std::string HttpRequest::GetRequestBody(void) const {
    return request_body_;
}

void HttpRequest::SetMethod(Method method) {
    method_.Set(method);
}

void HttpRequest::SetMethod(std::string method) {
    method_.Set(method);
}

Method HttpRequest::GetMethod(void) const {
    return method_.Get();
}

HttpMethod HttpRequest::GetMethodClass(void) const {
    return method_;
}

void HttpRequest::SetRequestBody(const std::string &request_body) {
    request_body_ = request_body;
}

void HttpRequest::AddData(const char* buf, const int &len) {
    data_.append(buf, len);
}

void HttpRequest::ShowData(void) const {
    //LOG(INFO) << "Request data: \n" << data_;
    //LOG(INFO) << "Info: Reuqest data end!";
}

size_t HttpRequest::GetLength(void) {
    return data_.length();
}

std::string* HttpRequest::GetDataPtr(void) {
    return &data_;
}

bool HttpRequest::Parse(void) {
    /* Http Request Format:
     * <method> <request-URL> <version> CRLF
     * <headers> CRLF
     * CRLF
     * <request-body>*/

    size_t now_index = 0, next_index;

    // extract method
    next_index = data_.find_first_of(SPACE, now_index);
    if(next_index == std::string::npos) {
        //LOG(ERROR) << "Parse: Parse http request failed, the request method is not impletmented.";
        return false;
    }
    SetMethod(data_.substr(now_index, next_index - now_index));

    now_index = next_index + 1;   // skip SPACE
    // extract url
    next_index = data_.find_first_of(SPACE, now_index);
    SetUrl(data_.substr(now_index, next_index - now_index));
    
    now_index = next_index + 1;  // skip SPACE
    next_index = data_.find_first_of(CRLF, now_index);
    if(next_index == std::string::npos) {
        //LOG(ERROR) << "Parse: Parse http request failed, the protocol is not impletmented.";
        return false;
    }
    SetProtocol(data_.substr(now_index, next_index - now_index));

    now_index = next_index + 2;  // skip CRLF
    if(data_.length() < 2 + now_index || CRLF == data_.substr(now_index, 2)) {
        //LOG(ERROR) << "Parse: Parse http request failed, empty header.";
        return false;
    }

    while(true) {
        // extract header 
        next_index = data_.find_first_of(CRLF, now_index);
        if (next_index == std::string::npos) {
            //LOG(ERROR) << "Parse: Parse http request failed, invalid header.";
            return false;
        }
        std::string header = data_.substr(now_index, next_index - now_index);
        unsigned int mid_index = data_.find_first_of(':', now_index);
        if(mid_index == std::string::npos || mid_index > next_index) {
            //LOG(ERROR) << "Parse: Parse http request failed, invalid header, no \':\' between name and content.";
            return false;
        }
        AddHeader(data_.substr(now_index, mid_index - now_index), \
                  data_.substr(mid_index + 2, next_index - (mid_index + 2))); 
        now_index = next_index + 2;
        if (data_.length() < 2 + now_index || CRLF == data_.substr(now_index, 2)) {
            break;
        }
    }

    if(data_.length() < 2) {
        //LOG(ERROR) << "Parse: Parse http request failed, wrong format after header.";
        return false;
    }
    now_index += 2;  // skip CRLF
    SetRequestBody(data_.substr(now_index));
    return true;
}

bool HttpRequest::MergeData(void) {
    /* Request Data Format:
     * <method> <request-url> <version>CRLF
     * <headers>CRLF
     * CRLF
     * <body>*/
    data_ = "";
    data_ += method_.GetString() + " " + url_ + " " + protocol_.GetString() + CRLF;
    for(auto each: header_) {
        data_ += each.first + ": " + each.second + CRLF;
    }
    data_ += CRLF;
    data_ += request_body_;
    return true;
}

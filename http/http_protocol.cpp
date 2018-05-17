
#include <string>
#include "http.h"
#include "http_protocol.h"

HttpProtocol::HttpProtocol(void) {
}

HttpProtocol::~HttpProtocol(void) {
}

void HttpProtocol::Set(Protocol protocol) {
    protocol_ = protocol;
}

void HttpProtocol::Set(std::string protocol) {
    if(protocol == "HTTP/1.0") {
        Set(HTTP1_0);
    }
    else if(protocol == "HTTP/1.1") {
        Set(HTTP1_1);
    }
    else {
        Set(HTTP_UNSUPPORTED);
    }
}

void HttpProtocol::Set(const char* str, int len) {
    Set(std::string(str, len));
}

Protocol HttpProtocol::Get(void) const {
    return protocol_;
}

std::string HttpProtocol::GetString(void) const {
    if(protocol_ == HTTP1_0) {
        return "HTTP/1.0";
    }
    else if(protocol_ == HTTP1_1) {
        return "HTTP/1.1";
    }
    else {
        return "HTTP_UNSUPPORTED";
    }
}

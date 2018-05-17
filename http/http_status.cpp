
#include <string>
#include "http_status.h"

HttpStatus::HttpStatus(void) {
    status_code_ = 0;
    reason_phrase_ = "Initialize";
}

HttpStatus::~HttpStatus(void) {
}

bool HttpStatus::SetStatusCode(int status_code) {
    status_code_ = status_code;
    return true;
}

bool HttpStatus::SetReasonPhrase(void) {
    switch(status_code_){
        case 100:
            reason_phrase_ = "Continue";
            break;
        case 101:
            reason_phrase_ = "Switching Protocols";
            break;
        case 200:
            reason_phrase_ = "OK";
            break;
        case 201:
            reason_phrase_ = "Created";
            break;
        case 400:
            reason_phrase_ = "Bad Request";
            break;
        case 403:
            reason_phrase_ = "Forbidden";
            break;
        case 404:
            reason_phrase_ = "Not Found";
            break;
        case 411:
            reason_phrase_ = "Length Required";
            break;
        case 500:
            reason_phrase_ = "Internal Server Error";
            break;
        case 501:
            reason_phrase_ = "Not Implemented";
            break;
        case 502:
            reason_phrase_ = "Bad Gateway";
            break;
        case 505:
            reason_phrase_ = "HTTP Version Not Supported";
            break;
        default:
            return false;
            break;
    }
    return true;
}

bool HttpStatus::SetReasonPhrase(std::string reason_phrase) {
    reason_phrase_ = reason_phrase;
    return true;
}

int HttpStatus::GetStatusCode(void) const {
    return status_code_;
}

std::string HttpStatus::GetReasonPhrase(void) const {
    return reason_phrase_;
}


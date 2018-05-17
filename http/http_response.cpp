
#include <ctime>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>

using namespace std;

//#include <glog/logging.h>
#include "http_response.h"
#include "http_status.h"
#include "http_protocol.h"

HttpResponse::HttpResponse(void) {
    response_body_ = "";
    data_ = "";
    status_ = HttpStatus();
    protocol_ = HttpProtocol();
}

HttpResponse::~HttpResponse(void) {
}

void HttpResponse::AddHeader(std::string name, std::string content) {
    header_.push_back(std::make_pair(name, content));
}

std::string HttpResponse::GetHeaderByName(std::string header_name) const {
    for(auto each: header_) {
        if(each.first == header_name) {
            return each.second;
        }
    }
    return "";
}

bool HttpResponse::SetStatus(int status_code) {
    return status_.SetStatusCode(status_code) && status_.SetReasonPhrase();
}

bool HttpResponse::SetStatusCode(int status_code) {
    return status_.SetStatusCode(status_code);
}

bool HttpResponse::SetStatusCode(std::string status_code) {
    int code = atoi(status_code.c_str());
    if(code <= 0) {
        //LOG(ERROR) << "Invalid status code";
        return false;
    }
    return status_.SetStatusCode(code);
}

bool HttpResponse::SetReasonPhrase(std::string reason_phrase) {
    return status_.SetReasonPhrase(reason_phrase);
}

void HttpResponse::SetProtocol(Protocol protocol) {
    protocol_.Set(protocol);
}

void HttpResponse::SetProtocol(std::string protocol) {
    protocol_.Set(protocol);
}

void HttpResponse::SetResponseBody(std::string response_body) {
    response_body_ = response_body;
}

void HttpResponse::SetMimeType(std::string mime_type) {
    mime_type_ = mime_type;
}

std::string HttpResponse::GetMimeType(void) const {
    return mime_type_;
}

size_t HttpResponse::GetLength(void) const {
    return data_.size();
}

std::string HttpResponse::GetData(void) const {
    return data_;
}

std::string* HttpResponse::GetDataPtr(void) {
    return &data_;
}

void HttpResponse::AddData(std::string data) {
    data_ += data;
}

void HttpResponse::AddData(char *buf, int len) {
    data_.append(buf, len);
}

void HttpResponse::ShowData(void) {
    //LOG(INFO) << "Response data: \n" << data_;
    //LOG(INFO) << "Info: Response data end!";
}

void HttpResponse::GenerateHeader(void) { 
    time_t cur_time;
    time(&cur_time);
    std::string cur_time_str = ctime(&cur_time);
    *cur_time_str.rbegin() = '\0';

    AddHeader("Date", cur_time_str);
    AddHeader("Server", "Awesome HTTP Server"); 
    AddHeader("Accept-Ranges", "bytes");
    AddHeader("Content-Type", mime_type_);
    AddHeader("Connection", "close");
}

void HttpResponse::MergeData(void) {
    /* Response data format:
     * <version> <status> <reason-phrase>CRLF
     * <header>CRLF
     * CRLF
     * <body> */
    std::ostringstream stream;
    stream << protocol_.GetString() << " " << status_.GetStatusCode() << " " << status_.GetReasonPhrase() << CRLF;
    for(auto each: header_) {
        stream << each.first << ": " << each.second << CRLF;
    }
    stream << CRLF;
    stream << response_body_;
    data_ = stream.str();
}

bool HttpResponse::Parse(void) {
    /* Response data format:
     * <version> <status> <reason-phrase>CRLF
     * <headers>CRLF
     * CRLF
     * <body>*/

    unsigned int now_index = 0, next_index;

    // extract method
    next_index = data_.find_first_of(SPACE, now_index);
    if(next_index == std::string::npos) {
        //LOG(ERROR) << "Parse: Parse http response failed, the response protocol is not impletmented.";
        return false;
    }
    SetProtocol(data_.substr(now_index, next_index - now_index));

    now_index = next_index + 1;   // skip SPACE
    // extract status code 
    next_index = data_.find_first_of(SPACE, now_index);
    if(next_index == std::string::npos) {
        //LOG(ERROR) << "Parse: Parse http response failed, the status code is not impletmented.";
        return false;
    }
    SetStatusCode(data_.substr(now_index, next_index - now_index));
    
    now_index = next_index + 1;  // skip SPACE
    next_index = data_.find_first_of(CRLF, now_index);
    if(next_index == std::string::npos) {
        //LOG(ERROR) << "Parse: Parse http response failed, the reason phrase is not impletmented.";
        return false;
    }
    SetReasonPhrase(data_.substr(now_index, next_index - now_index));

    now_index = next_index + 2;  // skip CRLF
    if(data_.length() < 2 + now_index || CRLF == data_.substr(now_index, 2)) {
        //LOG(ERROR) << "Parse: Parse http response failed, empty header.";
        return false;
    }

    while(true) {
        // extract header 
        next_index = data_.find_first_of(CRLF, now_index);
        if (next_index == std::string::npos) {
            //LOG(ERROR) << "Parse: Parse http response failed, invalid header.";
            return false;
        }
        std::string header = data_.substr(now_index, next_index - now_index);
		unsigned int mid_index = data_.find_first_of(':', now_index);
        if(mid_index == std::string::npos || mid_index > next_index) {
            //LOG(ERROR) << "Parse: Parse http response failed, invalid header, no \':\' between name and content.";
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
        //LOG(ERROR) << "Parse: Parse http response failed, wrong format after header.";
        return false;
    }
    now_index += 2;  // skip CRLF
    SetResponseBody(data_.substr(now_index));
    return true;
}

bool HttpResponse::ReadFromFile(std::ifstream &in) {
    in.seekg(0, std::ifstream::end);
    int len = in.tellg();
    in.seekg(0, std::ifstream::beg);
    char *buffer = new char[len + 1];
    if(in.good()) {
        in.read(buffer, len);
    }
    if(in.bad()) {
        //LOG(ERROR) << "Read responsebody failed!";
        return false;
    } 
    response_body_.append(buffer, len);
    delete buffer;
    return true;
}

bool HttpResponse::WriteIntoFile(std::ofstream &out) {
    int len = atoi(GetHeaderByName("Content-Length").c_str());
    if(len <= 0) {
        //LOG(WARNING) << "WriteIntoFile: no content-length";
    }
    if(out.good()) {
        out.write(response_body_.c_str(), response_body_.length());
    }
    if(out.bad()) {
        //LOG(ERROR) << "WriteIntoFile: Failed";
        return false;
    }
    return true;
}


#include <iostream>
#include <string>
//#include <glog/logging.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <memory.h>
#include "http.h"
#include "socklib.h"
#include "http_method.h"
#include "http_response.h"
#include "http_request.h"
#include "http_client.h"

HttpClient::HttpClient(std::string request_url, std::string local_file_name)
{
    request_url_ = request_url;
    local_file_name_ = "../" + local_file_name; // the working directory now is bin.
    method_ = HttpMethod();
    method_.Set("GET");
} 

HttpClient::~HttpClient(void) {
}

bool HttpClient::MergeRequest(void) {
    http_request_->SetMethod(method_.Get());  
    http_request_->SetProtocol(HTTP1_0);
    http_request_->SetUrl(remote_file_name_);
    http_request_->AddHeader("Host", server_ip_);
    http_request_->AddHeader("User-Agent", "Awesome HTTP Client");
    http_request_->AddHeader("Content-Type", ExtractMimeType(remote_file_name_));
    http_request_->AddHeader("Connection", "close");
    http_request_->MergeData();
    return true;
}

bool HttpClient::CreateSocket(void) {
    //LOG(INFO) << "Start creating socket for server.";
    if((client_sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        //LOG(ERROR) << "CreateSocket: Failed to create socket!";
        return false;
    }
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_);
    StrToIp(server_ip_, &server_address.sin_addr);
    if(connect(client_sockfd_, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        //LOG(ERROR) << "CreateSocket: Failed to connect!";
        return false;
    }
    //LOG(INFO) << "Creating socket done!";
    return true;
}

bool HttpClient::SendRequest(void) {
    size_t len = http_request_->GetLength();
    std::string *request_data_ptr = http_request_->GetDataPtr();
    char *buf = new char[len];
    memcpy(buf, request_data_ptr->data(), len);
    if(send(client_sockfd_, buf, len, 0) < 0) {
        //LOG(ERROR) << "SendRequest: Failed to send request";
        return false;
    }

    delete buf;
    return true;
}

bool HttpClient::ReadResponse(void) {
    //LOG(INFO) << "Start reading response from " << client_sockfd_;
    char *buf = new char[kBufSize];
    int len;
    len = recv(client_sockfd_, buf, kBufSize, 0);
    if(0 == len) {
        //LOG(ERROR) << "ReadResponse: Read response failed, no data(blocking)!";
        return false;
    }
    
    http_response_->AddData(buf, len);
    while(true) {
        len = recv(client_sockfd_, buf, kBufSize, MSG_DONTWAIT);
        if(len < 0) {
            if(errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            }
            else {
                //LOG(ERROR) << "Receive response failed!";
                return false;
            }
        }
        http_response_->AddData(buf, len);
        if(len < kBufSize) {
            break;
        }
    }
    //LOG(INFO) << "Read response done!";
    http_response_->ShowData();
    delete buf;
    return true;
}

bool HttpClient::CompleteResponse(void) {
    std::ofstream out;
    switch(method_.Get()) {
        case GET:
            out.open(local_file_name_.c_str(), std::ofstream::out|std::ofstream::trunc);
            if(out.is_open()) {
                if(!http_response_->WriteIntoFile(out)) {
                    //LOG(ERROR) << "CompleteResponse: Failed to write response into " << local_file_name_;
                    return false;
                }
            }
            else {
                //LOG(ERROR) << "CompleteResponse: Failed to write response into " << local_file_name_;
                return false;
            }
            out.close();
            break;
        default:
            break;
    }
    return true;
}

bool HttpClient::HandlerResponse(void) {
    if(!ReadResponse()) {
        //LOG(ERROR) << "HandlerResponse: Failed to handler response";
        return false;
    }
    if(!http_response_->Parse()) {
        //LOG(ERROR) << "HandlerResponse: Failed to parse response";
        return false;
    }
    if(!CompleteResponse()) {
        //LOG(ERROR) << "HandlerResponse: Failed to complete response";
        return false;
    }
    return true;
}

bool HttpClient::ParseServerUrl(void) {
    size_t now_index = 0, next_index;
    std::string url_header = "http://";
    if((next_index = request_url_.find_first_of(url_header)) != std::string::npos) {
        if(next_index != now_index) {
            //LOG(ERROR) << "Failed to parse server url, " << url_header   << " exists, but not at the begining of the url";
            return false;
        } 
        now_index += url_header.size();
    }
    
    // the format of valid url is host:port/filename, and the port can be omitted by default
    if((next_index = request_url_.find_first_of("/", now_index)) != std::string::npos) {
        std::string host_port = request_url_.substr(now_index, next_index - now_index);
        now_index = next_index;
        //LOG(INFO) << host_port;
        if((next_index = host_port.find_first_of(":")) != std::string::npos) {
            host_name_ = host_port.substr(0, next_index);
            port_ = atoi(host_port.substr(next_index + 1).c_str());
        }
        else {
            host_name_ = host_port;
        }
        if(IsIp(host_name_)) {
            server_ip_ = host_name_;
        }
        else {
            //LOG(ERROR) << "Not implemented, convert host into ip address.";
            return false;
        }
    }
    else {
        //LOG(ERROR) << "Failed to parse server url, there is no \"/\""  << " between server address and request file name";
        return false;
    }

    remote_file_name_ = request_url_.substr(now_index + 1);
    //LOG(INFO) << "The request url is " << request_url_ << ", the host name is "  << host_name_ << ", the server ip is " << server_ip_ << ", the port is "  << port_;
    return true;
}

bool HttpClient::Run(void) {
    http_request_ = new HttpRequest();
    http_response_ = new HttpResponse();
    if(!ParseServerUrl()) {
        //LOG(ERROR) << "Run: Parse url failed!";
        return false;
    }
    if(!MergeRequest()) {
        //LOG(ERROR) << "Run: Request invalid!";
        return false;
    }
    http_request_->ShowData();
    if(!CreateSocket()) {
        //LOG(ERROR) << "Run: Failed to create socket";
        return false;
    }
    if(!SendRequest()) {
        //LOG(ERROR) << "Run: Failed to send request";
        return false;
    }
    if(!HandlerResponse()) {
        //LOG(ERROR) << "Run: Failed to handler reqsponse";
        return false;
    }
    delete http_request_;
    delete http_response_;
    return true;
}

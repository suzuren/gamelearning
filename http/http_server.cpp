#include <memory.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
//#include <glog/logging.h>
#include "http.h"
#include "http_server.h"
#include "http_request.h"

#include <signal.h>

extern int errno;

HttpServer::HttpServer(void) {
    new (this)HttpServer(kDefaultPort);
}

HttpServer::HttpServer(int port) {
    //LOG(INFO) << "new HttpServer: port(" << port << ")"; 
    SetPort(port);
}

HttpServer::~HttpServer(void) {
}

bool HttpServer::SetPort(int port) {
    if (port < 1024 || port > 65535) {
        //LOG(ERROR) << "SetPort: Invalid port value"; 
        return false;
    }
    port_ = port;
    return true;
}

bool HttpServer::CreateSocket(void) {
    //LOG(INFO) << "Start creating socket for server.";
    if((server_sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        //LOG(ERROR) << "CreateSocket: Failed to create socket!";
        return false;
    } 
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_);
    server_address.sin_addr.s_addr = INADDR_ANY;
    if(bind(server_sockfd_, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        //LOG(ERROR) << "CreateSocket: Failed to bind port!";
        return false;
    }
    if(listen(server_sockfd_, 5) < 0) {
        //LOG(ERROR) << "CreateSocket: Failed to listen!";
        return false;
    }
    //LOG(INFO) << "Creating socket done!";
    return true;
}

bool HttpServer::StartListening(void) {
    sockaddr_in client_address;
    socklen_t client_address_length;
    // To prevent zombie child process.
    signal(SIGCHLD, ChildProcessHandler);
    while(true) {
        client_sockfd_ = accept(server_sockfd_, (struct sockaddr *)&client_address, &client_address_length);
        if(-1 == client_sockfd_) {
            //LOG(ERROR) << "StartListening: Accept call failed!";
            return false;
        }

        // At first, both the reference count for server socket and client socket are 1.
        // When we fork a new process, the reference count will be doubled.
        
        //LOG(INFO) << "Parent process(" << getpid() << ") used fork() to create a new process!";
        if (0 == fork ()) {
            // The child process is assigned to handler the request from client.
            //LOG(INFO) << "Child process(" << getpid() << ") used to handle request!";
            if(!HandleRequest()) {
                //LOG(ERROR) << "StartListening: Handle request failed!";
                exit(-1);
            }  
            exit(0);
        }
        close(client_sockfd_); // parenct process
    }
    return true;
}

bool HttpServer::Run(void) {
    //LOG(INFO) << "Start running the server";
    if(!CreateSocket()) {
        //LOG(ERROR) << "Run: Failed to initialize socket!";
        return false;
    }
    if(!StartListening()) {
        //LOG(ERROR) << "Run: Start listening failed!";
        return false;
    }
    return true;
}

bool HttpServer::HandleRequest(void) {
    http_request_ = new HttpRequest();
    http_response_ = new HttpResponse();
    if(!ReadRequest()) {
        //LOG(ERROR) << "HandlerRequest: Read request failed!";
        return false;
    }
    if(!http_request_->Parse()) {
        //LOG(ERROR) << "HandleRequest: Parse request failed!";
        return false;
    }
    if(!GenerateResponse()) {
        //LOG(ERROR) << "HandleRequest: Generate response failed!";
        return false;
    }
    if(!MergeResponse()) {
        //LOG(ERROR) << "HandleRequest: Merger final response failed!";
        return false;
    }
    http_response_->ShowData();
    if(!SendResponse()) {
        //LOG(ERROR) << "HandleRequest: Send response failed!";
        return false;
    }

    delete http_request_;
    delete http_response_;
    return true;
}

bool HttpServer::ReadRequest(void) {
    //LOG(INFO) << "Start reading request from " << client_sockfd_;
    char *buf = new char[kBufSize];
    int len;
    len = recv(client_sockfd_, buf, kBufSize, 0);
    if(0 == len) {
        //LOG(ERROR) << "ReadRequest: Read request failed, no data(blocking)!";
        return false;
    }
    
    http_request_->AddData(buf, len);
    while(true) {
        len = recv(client_sockfd_, buf, kBufSize, MSG_DONTWAIT);
        if(len < 0) {
            if(errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            }
            else {
                //LOG(ERROR) << "Receive request failed!";
                return false;
            }
        }
        http_request_->AddData(buf, len);
        if(len < kBufSize) {
            break;
        }
    }
    //LOG(INFO) << "Read request done!";
    http_request_->ShowData();
    delete buf;
    return true;
}

bool HttpServer::GenerateResponse(void) {
    //LOG(INFO) << "Start generating response!";
    Method method = http_request_->GetMethod();
    if(http_request_->GetMethod() == NOT_IMPLEMENTED) {
        http_response_->SetStatus(505);
    }
    switch(method) {
        case GET:
            if(!HandlerGetRequest()) {
                //LOG(ERROR) << "Failed to handler GET request";
                return false;
            }
            break;
        default:
            http_response_->SetStatus(501);
    }
    //LOG(INFO) << "Generate response done!";
    return true;
}

bool HttpServer::HandlerGetRequest(void) {
    std::string url = SERVER_ROOT + http_request_->GetUrl();
    //LOG(INFO) << "the request url is " << url;
    std::ifstream in;
    in.open(url.c_str(), std::ifstream::in);
    if(in.is_open()) {
        if(!http_response_->ReadFromFile(in)) {
            //LOG(ERROR) << "Read from " << url << " for response body failed!";
            http_response_->SetStatus(500);
            return false;
        }
    }
    else {
        //LOG(WARNING) << "There is no resource named " << url;
        in.close();
        url = SERVER_ROOT + std::string("404.html");
        in.open(url.c_str(), std::ifstream::in);
        if(in.is_open()) {
            if(!http_response_->ReadFromFile(in)) {
                //LOG(ERROR) << "Read from " << url << " for response body failed!";
                http_response_->SetStatus(500);
                return false;
            }
            http_response_->SetStatus(404);
        }
        else {
            //LOG(ERROR) << "Open 404.html failed!";
            return false;
        }
    }
    http_response_->SetStatus(200);
    in.close();
    return true;
}

bool HttpServer::MergeResponse(void) {
    http_response_->SetProtocol(http_request_->GetProtocol());
    http_response_->SetMimeType(ExtractMimeType(http_request_->GetUrl()));
    http_response_->GenerateHeader();
    http_response_->MergeData();
    return true;
}

bool HttpServer::SendResponse(void) {
    size_t len = http_response_->GetLength();
    std::string* data_ptr = http_response_->GetDataPtr();

    //LOG(INFO) << "Start to send response, the data length: " << len;

    char *buf = new char[len];
    memset(buf, '\0', len);
    memcpy(buf, data_ptr->data(), len);
    if(send(client_sockfd_, buf, len, 0) < 0) {
        //LOG(ERROR) << "Send Response failed!";
        return false;
    }
    delete buf;
    return true;
}

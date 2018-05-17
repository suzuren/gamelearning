
#ifndef __HTTP_SERVER_H_
#define __HTTP_SERVER_H_

#include "http_request.h"
#include "http_response.h"


class HttpServer {
public:
    HttpServer(void);
    HttpServer(int);
    ~HttpServer(void);

    bool SetPort(int);
    bool Run(void);
    bool CreateSocket(void);
    bool StartListening(void);
    bool HandleRequest(void);
    bool ReadRequest(void);
    bool GenerateResponse(void);
    bool HandlerGetRequest(void);
    bool MergeResponse(void);
    bool SendResponse(void); 

private:
    int port_;
    int server_sockfd_;
    int client_sockfd_;
    HttpRequest *http_request_;
    HttpResponse *http_response_;
    static const int kDefaultPort = 1024;
    static const int kBufSize = 32;
};

#endif /* __HTTP_SERVER_H_ */

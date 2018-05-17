
#ifndef __HTTP_CLIENT_H_
#define __HTTP_CLIENT_H_

#include "http_request.h"
#include "http_response.h"
#include "http_method.h"

class HttpClient {
public:
    HttpClient(std::string, std::string);
    ~HttpClient(void);

    bool Run(void);
    bool CreateSocket(void);
    bool MergeRequest(void);
    bool SendRequest(void);
    bool HandlerResponse(void);
    bool ReadResponse(void);
    bool CompleteResponse(void);
    bool ParseServerUrl(void);

private:
    int client_sockfd_;
    int port_;
    std::string host_name_;
    std::string server_ip_;
    std::string remote_file_name_;
    std::string local_file_name_;
    std::string request_url_;
    HttpMethod method_;
    HttpRequest* http_request_;
    HttpResponse *http_response_;
    const static int kBufSize = 32;
};

#endif /* __HTTP_CLIENT_H_ */


#ifndef __HTTP_H_
#define __HTTP_H_

#include <string>

#define SPACE ' '
#define CRLF "\r\n"
#define SERVER_ROOT "../resource/"

typedef enum {GET, PUT, HEAD, POST, NOT_IMPLEMENTED} Method;
typedef enum {HTTP1_0, HTTP1_1, HTTP_UNSUPPORTED} Protocol; 

std::string ExtractMimeType(std::string);
bool IsIp(std::string);
void ChildProcessHandler(int);

#endif /* __HTTP_H_ */

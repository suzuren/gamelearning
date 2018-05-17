
#ifndef __HTTP_PROTOCOL_H_
#define __HTTP_PROTOCOL_H_

#include <string>
#include "http.h"

class HttpProtocol {
public:
    HttpProtocol(void);
    ~HttpProtocol(void);

    void Set(Protocol);
    void Set(std::string);
    void Set(const char *, int);

    Protocol Get(void) const;
    std::string GetString(void) const;

private:
    Protocol protocol_;
};

#endif /* __HTTP_PROTOCOL_H_ */

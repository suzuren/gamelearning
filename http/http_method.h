
#ifndef __HTTP_METHOD_H_
#define __HTTP_METHOD_H_

#include <string>
#include "http.h"

class HttpMethod {
public:
    HttpMethod(void);
    ~HttpMethod(void);

    void Set(std::string);
    void Set(const char*, int);
    void Set(Method);

    Method Get(void) const;
    std::string GetString(void) const;
private:
    Method method_;
};

#endif /* __HTTP_METHOD_H_ */

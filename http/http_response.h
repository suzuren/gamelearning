
#ifndef __HTTP_RESPONSE_H_
#define __HTTP_RESPONSE_H_

#include <vector>
#include <string>
#include <fstream>
#include "http_protocol.h"
#include "http_status.h"

class HttpResponse {
public:
    HttpResponse(void);
    ~HttpResponse(void);

    void SetMimeType(std::string);
    std::string GetMimeType(void) const;
    void SetProtocol(Protocol);
    void SetProtocol(std::string);
    void AddHeader(std::string, std::string);
    std::string GetHeaderByName(std::string) const; 
    void AddData(std::string);
    void AddData(char *, int);
    void ShowData(void);
    bool SetStatus(int);
    bool SetStatusCode(int);
    bool SetStatusCode(std::string);
    bool SetReasonPhrase(std::string);
    void SetResponseBody(std::string);
    size_t GetLength(void) const;
    std::string GetData(void) const;
    std::string* GetDataPtr(void);
    bool ReadFromFile(std::ifstream &);
    bool WriteIntoFile(std::ofstream &);
    void GenerateHeader(void);
    void MergeData(void);
    bool Parse(void);

private:
    HttpStatus status_;
    HttpProtocol protocol_;
    std::string mime_type_;
    std::string response_body_;
    std::string data_;
    std::vector<std::pair<std::string, std::string> > header_;
};

#endif /* __HTTP_RESPONSE_H_ */

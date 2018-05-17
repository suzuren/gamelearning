
#include <string>
#include <sys/wait.h>
#include <arpa/inet.h>
//#include <glog/logging.h>
#include "http.h"

std::string ExtractMimeType(std::string file_name) {
    size_t pos = file_name.find_last_of(".");
    std::string extension;
    std::string mime_type = "text/plain, charset=us-ascii";

    if(pos== std::string::npos){
        extension = "";
    }else{
        extension = file_name.substr(pos + 1);
    }

    switch(extension[0]){
        case 'b':
            if(extension == "bmp")
                mime_type = "image/bmp";
            if(extension == "bin")
                mime_type = "application/octet-stream";
            break;
        case 'c':
            if(extension == "csh")
                mime_type = "application/csh";
            if(extension == "css")
                mime_type = "text/css";
            break;
        case 'd':
            if(extension == "doc")
                mime_type = "application/msword";
            if(extension == "dtd")
                mime_type = "application/xml-dtd";
            break;
        case 'e':
            if(extension == "exe")
                mime_type = "application/octet-stream";
            break;
        case 'h':
            if(extension == "html" || extension == "htm")
                mime_type = "text/html";
            break;
        case 'i':
            if(extension == "ico")
                mime_type = "image/x-icon";
            break;
        case 'g':
            if(extension == "gif")
                mime_type = "image/gif";
            break;
        case 'j':
            if(extension == "jpeg" || extension == "jpg")
                mime_type = "image/jpeg";
            break;
        case 'l':
            if(extension == "latex")
                mime_type = "application/x-latex";
            break;
        case 'p':
            if(extension == "png")
                mime_type = "image/png";
            if(extension == "pgm")
                mime_type = "image/x-portable-graymap";
            break;
        case 'r':
            if(extension == "rtf")
                mime_type  = "text/rtf";
            break;
        case 's':
            if(extension == "svg")
                mime_type = "image/svg+xml";
            if(extension == "sh")
                mime_type = "application/x-sh";
            break;
        case 't':
            if(extension == "tar")
                mime_type = "application/x-tar";
            if(extension == "tex")
                mime_type = "application/x-tex";
            if(extension == "tif" || extension == "tiff")
                mime_type = "image/tiff";
            if(extension == "txt")
                mime_type = "text/plain";
            break;
        case 'x':
            if(extension == "xml")
                mime_type = "application/xml";
            break;
        default:
            break;
    }
    return mime_type;
}

bool IsIp(std::string ip_address) {
    return IsIp(ip_address.c_str());
}

bool IsIp(char *ip_address) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_address, &(sa.sin_addr));
    return result > 0;
}

void ChildProcessHandler(int num) {
    int status;
    //int pid_ = waitpid(-1, &status, WNOHANG);
	waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        //LOG(INFO) << "The child " << pid << " exit with code " << WEXITSTATUS(status);
    }
}

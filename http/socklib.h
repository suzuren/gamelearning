
#ifndef __SOCKLIB_H_
#define __SOCKLIB_H_

#include <string>
#include <netinet/in.h>

/* get the socket address according to the sockfd */
struct sockaddr_in* SockfdToAddr(int);
/* get the address family according to the sockfd */
int SockfdToFamily(int);
/* get the ip address from sockaddr */
std::string AddrToIp(struct sockaddr_in *sin); 
/* get the port from sockaddr */
int AddrToPort(struct sockaddr_in *sin); 
/* get the ip address according to the sockfd */
std::string SockfdToIp(int);
/* get the port according to the sockfd */
int SockfdToPort(int);
/* convert ip address from string */
int StrToIp(const char *, struct in_addr *);
int StrToIp(std::string, struct in_addr *);
/* convert ip address into string */
std::string IpToStr(struct in_addr*);

#endif /* __SOCKLIB_H_ */

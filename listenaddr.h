#ifndef _LISTEN_ADDR_H_
#define _LISTEN_ADDR_H_

#include <string>
#include <arpa/inet.h>

class ListenAddr{
public:
    explicit ListenAddr(const char*listenAddr, const int listenPort)
    :listenAddr_(listenAddr),
    listenPort_(listenPort)
    {}

    in_addr_t inetAddr(){
        return ::inet_addr(listenAddr_.c_str());
    }
    in_port_t port(){
        return ::htons(listenPort_);
    }
private:
    std::string listenAddr_;
    int listenPort_;
};


#endif
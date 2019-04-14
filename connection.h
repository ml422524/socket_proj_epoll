#ifndef _MLHMZ_CONNECTION_H_
#define _MLHMZ_CONNECTION_H_

#include <atomic>

namespace Mlhmz {

enum class ConnectionError{
    RECV_SUCCESS = 0,
    RECV_SOCK_CLOSE,
    RECV_SOCK_ERROR,
    SEND_SUCCESS,
    SEND_SOCK_ERROR,
    SHUTDOWN_SUCCESS,
    SHUTDOWN_FAIL
};

class Connection{
public:
    explicit Connection(int fd);
    virtual ~Connection();

    //
    int fd() { return fd_;}
    int close();
private:
    int fd_;
    std::atomic<int> closed_;
};

}

#endif
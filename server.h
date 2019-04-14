#ifndef _SERVER_H_
#define _SERVER_H_

#include <memory>
#include <vector>
#include <map>
#include "mutex.h"
#include "listenaddr.h"
#include "connection.h"

//
enum class ServerError{
    INIT_ERROR = 0,
    INIT_SUCCESS,
    FIND_CLIENT_SUCCESS,
    FIND_CLIENT_FAIL,
    ACCEPT_SUCCESS,
    ACCEPT_FAIL,
};

enum class FcntlError{
    SUCCESS = 0,
    FAIL
};

class Server{
public:
    //
    typedef std::shared_ptr<Mlhmz::Connection> ConnectionPtr;

    //
    explicit Server(const ListenAddr&lsnAddr)
    :listenfd_(-1),
    epollfd_(-1),
    lsnAddr_(lsnAddr),
    threadIds_()
    {
    }
    virtual ~Server();

    //
    ServerError start();

    //
    int onMessage(ConnectionPtr&conPtr);
private:
    //
    int listenfd() const;

    //
    int saveClient(int fd);
    int removeClient(int fd);
    ServerError findClient(int fd, ConnectionPtr&conPtr) const;

    //
    ServerError accept(int listenfd, sockaddr*addr, socklen_t *len, int&acceptedfd);

    //
    FcntlError addSockFlag(int fd, int flag);
    FcntlError rmSockFlag(int fd, int flag);

    //
    ServerError startWorkerThread();
    static void* workerThreadFunc(void *param);
    void* workerThreadProc();

private:
    Server(const Server&) = delete;
    Server&operator=(const Server&) = delete;
private:
    int listenfd_;
    ListenAddr lsnAddr_;
    std::vector<pthread_t> threadIds_;

    //
    int epollfd_;
    static const int MAX_EPOLL_EVNETS_ = 1024;

    //
    mutable Mlhmz::Mutex clientsMtx_;
    std::map<int, ConnectionPtr> clients_;
};

#endif

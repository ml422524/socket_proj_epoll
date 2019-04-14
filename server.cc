#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <error.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <poll.h>
#include "utils.h"
#include "log.h"
#include "server.h"

Server::~Server()
{
    ::close(listenfd_);
}

ServerError Server::start()
{
    // 
    listenfd_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenfd_ < 0)
    {
        LOG_ERROR("create listen socket failed. " << ::strerror(errno));
        return ServerError::INIT_ERROR;
    }

    //
    if(FcntlError::SUCCESS != addSockFlag(listenfd(), O_NONBLOCK))
    {
        LOG_ERROR("set O_NONBLOCK to listen error. " << strerror(errno));
        ::close(listenfd());
        return ServerError::INIT_ERROR;
    }

    //
    sockaddr_in sadin;
    ::memset(&sadin,0, sizeof(sadin));
    sadin.sin_family = AF_INET;
    sadin.sin_addr.s_addr = lsnAddr_.inetAddr();
    sadin.sin_port = lsnAddr_.port();
    auto retVal = ::bind(listenfd(), (sockaddr*)&sadin, sizeof(sadin));
    if(retVal < 0)
    {
        LOG_ERROR("bind socket error. " << strerror(errno));
        ::close(listenfd());
        return ServerError::INIT_ERROR;
    }
    //
    retVal = ::listen(listenfd(), 0);
    if(retVal < 0)
    {
        LOG_ERROR("listen socket error. " << strerror(errno));
        ::close(listenfd());
        return ServerError::INIT_ERROR;
    }
    
    //
    epollfd_ = ::epoll_create(1);
    if(epollfd_ < 0)
    {
        LOG_ERROR("create epollfd error. " << strerror(errno));
        ::close(listenfd());
        return ServerError::INIT_ERROR;
    }

    //
    epoll_event listenfdEvt;
    listenfdEvt.events = POLLIN;
    listenfdEvt.data.fd = listenfd();
    if(::epoll_ctl(epollfd_, EPOLL_CTL_ADD, listenfd(), &listenfdEvt) < 0)
    {
        LOG_ERROR("epoll_ctl error. " << strerror(errno));
        ::close(listenfd());
        return ServerError::INIT_ERROR;
    }

    //
    if(ServerError::INIT_SUCCESS != startWorkerThread())
    {
        LOG_ERROR("init thread failed.");
        ::close(listenfd());
        return ServerError::INIT_ERROR;
    }

    //
    for(;;)
    {
        ::sleep(10);
    }

    //
    return ServerError::INIT_SUCCESS;
}

ServerError Server::startWorkerThread()
{
    //
    int cores = Utils::getCores();
    pthread_t thrd;
    for(int i = 0; i < cores; ++i)
    {
        auto ret = pthread_create(&thrd, NULL, workerThreadFunc, this);
        if(ret < 0)
        {
            LOG_ERROR("create one thread failed. " << strerror(errno));
            continue;
        }
        threadIds_.push_back(thrd);
    }
    if(threadIds_.size() <= 0)
    {
        LOG_ERROR("init thread failed.");
        return ServerError::INIT_ERROR;
    }
    if(threadIds_.size() < cores)
    {
        LOG_WARN("The number of threads created is less than expected(cores).");
    }
    return ServerError::INIT_SUCCESS;
}

void* Server::workerThreadFunc(void *param)
{
    Server*pSvr = static_cast<Server*>(param);
    return pSvr->workerThreadProc();
}

int Server::listenfd() const
{
    return listenfd_;
}
int Server::saveClient(int fd)
{
    auto ptr = std::make_shared<Mlhmz::Connection>(fd);
    {
        Mlhmz::MutexLockGard gd(clientsMtx_);
        clients_[fd] = ptr;
    }
    return 0;
}
int Server::removeClient(int fd)
{
    Mlhmz::MutexLockGard gd(clientsMtx_);
    return clients_.erase(fd);
}

ServerError Server::findClient(int fd, ConnectionPtr&conPtr) const
{
    Mlhmz::MutexLockGard lg(clientsMtx_);
    auto it = clients_.find(fd);
    if(it != clients_.end())
    {
        conPtr = it->second;
        return ServerError::FIND_CLIENT_SUCCESS;
    }
    else
    {
        return ServerError::FIND_CLIENT_FAIL;
    }
}

ServerError Server::accept(int listenfd, sockaddr*addr, socklen_t *len, int&acceptedfd)
{
again:
    auto ret = ::accept(listenfd, addr, len);
    if(ret < 0)
    {
        if(errno == EINTR)
        {
            goto again;
        }
        return ServerError::ACCEPT_FAIL;  
    }
    acceptedfd = ret;
    return ServerError::ACCEPT_SUCCESS;
}

FcntlError Server::addSockFlag(int fd, int flag)
{
    int oldsockflag = ::fcntl(fd, F_GETFL, 0);
    int newsockflag = oldsockflag | flag;
    if(::fcntl(fd, F_SETFL, newsockflag) < 0)
    {
        return FcntlError::FAIL;
    }
    return FcntlError::SUCCESS;
}

FcntlError Server::rmSockFlag(int fd, int flag)
{
    int oldsockflag = ::fcntl(fd, F_GETFL, 0);
    int newsockflag = oldsockflag & (~flag);
    if(::fcntl(fd, F_SETFL, newsockflag) < 0)
    {
        return FcntlError::FAIL;
    }
    return FcntlError::SUCCESS;
}

void* Server::workerThreadProc()
{
    LOG_INFO("Worker Thread: " << pthread_self() << " started...");
    for(;;)
    {
        epoll_event epollEvts[MAX_EPOLL_EVNETS_];
        auto n = ::epoll_wait(epollfd_, epollEvts, MAX_EPOLL_EVNETS_, 200);
        if(n < 0)
        {
            if(EINTR == errno)
            {
                LOG_ERROR("epoll_wait is interrepted. " << strerror(errno));
                continue;
            }
            LOG_ERROR("epoll_wait error." << strerror(errno));
            break;
        }
        else if(n == 0)
        { // timeout
            continue;
        }
        //
        for(int i = 0; i < n; ++i)
        {
            if(epollEvts[i].events & POLLIN)
            {
                auto fd = epollEvts[i].data.fd;
                if(fd == listenfd())
                { // accept new connection
                    int newconfd = -1;
                    auto err = accept(listenfd(), NULL, NULL, newconfd);
                    if(err != ServerError::ACCEPT_SUCCESS)
                    {
                        LOG_ERROR("accept a new client error. error: " << strerror(errno));
                    }
                    else
                    {
                        LOG_DEBUG("accept a new client success.");
                        if(FcntlError::SUCCESS != addSockFlag(newconfd, O_NONBLOCK))
                        {
                            ::close(newconfd);
                        }
                        else
                        {
                            epoll_event clientfdEvt;
                            clientfdEvt.events = POLLIN | POLLOUT;// | EPOLLET;
                            clientfdEvt.data.fd = newconfd;
                            if(::epoll_ctl(epollfd_, EPOLL_CTL_ADD, newconfd, &clientfdEvt) < 0)
                            {
                                LOG_ERROR("add fd: " << newconfd << " to epoll failed. error:" << strerror(errno));
                                ::close(newconfd);
                            }
                            else
                            {
                                saveClient(newconfd);
                            }
                        }
                    }                    
                }
                else
                { // receive data
                    ConnectionPtr conPtr;
                    ServerError err = findClient(fd, conPtr);
                    if(err != ServerError::FIND_CLIENT_SUCCESS)
                    {
                        LOG_ERROR("can not find fd:" << fd << " in saved clients.");
                    }
                    else
                    {
                        LOG_DEBUG("find fd: " << fd << " success.");
                        assert(fd == conPtr->fd());
                        const int len = 5;
                        char buf[len] = {'\0'};
                    recvagain:
                        int n = ::recv(fd, buf, len, 0);
                        if(n < 0)
                        {
                            if(errno == EINTR)
                            {
                                LOG_WARN("recv fd:"<< fd << " is interrepted." << strerror(errno));
                                goto recvagain;
                            }
                            else if(errno == EWOULDBLOCK)
                            {
                                LOG_WARN("recv fd:"<< fd << " EWOULDBLOCK." << strerror(errno));
                            }
                            else
                            { //
                                LOG_ERROR("recv fd:" << fd << " error:" << strerror(errno));
                                if(::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL) < 0)
                                {
                                    LOG_ERROR("epoll_ctl del fd:" << fd << " failed. " << strerror(errno));
                                }
                                //
                                removeClient(fd);
                                conPtr->close();                            
                            }
                        }
                        else if(0 == n) // peer closed.
                        {
                            LOG_DEBUG("fd: " << fd << " closed.");
                            if (::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL) < 0)
                            {
                                LOG_ERROR("epoll_ctl del fd:" << fd << " failed. " << strerror(errno));
                            }
                            removeClient(fd);
                            conPtr->close();
                        }
                        else // recv data normally
                        {
                            buf[n] = '\0';
                            LOG_DEBUG("recv client fd:" << fd << " data: " << buf);
                        }
                    }                    
                }    
            }
            if(epollEvts[i].events & POLLOUT)
            {
                int fd = epollEvts[i].data.fd;
                LOG_DEBUG("fd: " << fd << " pollout triggered.");
                // just testing
                char buf[] = "hello client.";
                ::send(fd, buf, sizeof(buf), 0);
            }
            if(epollEvts[i].events & POLLERR)
            { //
                int fd = epollEvts[i].data.fd;
                LOG_DEBUG("fd: " << fd << " pollerr triggered.");
            }
        }
    }
    //
    LOG_INFO("Worker Thread: " << pthread_self() << " exit...");
    return NULL;
}

int Server::onMessage(ConnectionPtr &conPtr)
{
    //
    return 0;
}
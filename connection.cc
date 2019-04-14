#include <unistd.h>
#include <errno.h>
#include <iterator>
#include <sys/ioctl.h>
#include <linux/sockios.h>

#include "connection.h"

namespace Mlhmz{

Connection::Connection(int fd) : 
fd_(fd),
closed_(0)
{

}

Connection::~Connection()
{
    if(!closed_)
    {
        ::close(fd());
    }
}

int Connection::close()
{
    ::close(fd());
    closed_ = 1;
}

}
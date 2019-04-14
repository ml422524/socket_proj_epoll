#include "server.h"
#include "log.h"

int main(int argc,char *argv[])
{
    //
    LOG_INIT();

#ifndef __ASSEMBLER__
   LOG_INFO("no __ASSEMBLER__");
    #if !defined _LIBC || defined _LIBC_REENTRANT
        LOG_INFO("When using threads, errno is a per-thread value.");
    #endif
# endif /* !__ASSEMBLER__ */

    //
    LOG_INFO("start server...");
    Server svr(ListenAddr("127.0.0.1",666));
    svr.start();

    //
    return 0;
}
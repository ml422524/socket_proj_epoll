#include "utils.h"
#include <string>
#include <iostream>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace Utils
{
int getCores(int flag)
{
    if(0 == flag)
    {
        return get_nprocs_conf();
    }
    else
    {
        return get_nprocs();
    }
}

}  
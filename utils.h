#ifndef _UTILS_H_
#define _UTILS_H_

#include <time.h>

namespace Utils
{
// 0 - sys core number, others - sys enable number.
int getCores(int flag= 0);

//
class Timestamp{
public:
    explicit Timestamp(const time_t&tsec):sec_(tsec)
    {
    }
    long long getTime(){
        return sec_;
    }
private:
    time_t sec_;
};

}


#endif
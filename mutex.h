#ifndef __MUTEX__H__
#define __MUTEX__H__

#include <pthread.h>

namespace Mlhmz
{
class MutexLockGard;
class Condition;
//
class Mutex{
public:
    friend class MutexLockGard;
    friend class Condition;
    Mutex():mtx_(PTHREAD_MUTEX_INITIALIZER)
    {
    }
private:
    //
    int lock()
    {
        return pthread_mutex_lock(&mtx_);
    }
    int unlock()
    {
        return pthread_mutex_unlock(&mtx_);
    }
    //
    pthread_mutex_t mtx_;
};

//
class MutexLockGard
{
public:
    MutexLockGard(Mutex&mtx):mtx_(mtx)
    {
        mtx_.lock();
    }
    ~MutexLockGard()
    {
        mtx_.unlock();
    }
    Mutex&mtx_;
};

//
class Condition{
public:
    Condition(){
        pthread_cond_init(&con_,NULL);
    }
    ~Condition(){
        pthread_cond_destroy(&con_);
    }
    int wait(Mutex&mtx)
    {
        return pthread_cond_wait(&con_, &(mtx.mtx_));
    }
    int signal()
    {
        return pthread_cond_signal(&con_);
    }
private:
    Condition(const Condition&)=delete;
    Condition&operator=(const Condition&)=delete;
private:
    pthread_cond_t con_;
};

}

#endif
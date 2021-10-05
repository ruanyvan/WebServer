#pragma once
#include <asm-generic/errno.h>
#include <bits/types/struct_timespec.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <cstdint>
#include "noncopyable.h"
#include "MutexLock.h"

class Condition : noncopyable{
    public:
    explicit Condition(MutexLock& _mutex):mutex(_mutex){
        pthread_cond_init(&cond, nullptr);
    }
    ~Condition() { pthread_cond_destroy(&cond); }

    void wait(){ pthread_cond_wait(&cond, mutex.get()); }
    void notify() { pthread_cond_signal(&cond); }
    void notifyAll() { pthread_cond_broadcast(&cond); }
    bool waitForSeconds(int seconds){
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_nsec+=static_cast<time_t>(seconds);
        return ETIMEDOUT==pthread_cond_timedwait(&cond, mutex.get(), &abstime);
    }

    private:
    MutexLock& mutex;
    pthread_cond_t cond;
};
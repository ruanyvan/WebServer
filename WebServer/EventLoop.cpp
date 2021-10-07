#include"EventLoop.h"
#include<sys/epoll.h>
#include<sys/eventfd.h>
#include<iostream>
#include"Util.h"
#include"base/Logging.h"
using namespace std;

__thread EventLoop* t_loopInThisThread = 0;

int createEventfd(){
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd<0){
        LOG<<" Failed in eventfd";
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
        : looping_(false),
        poller_(new Epoll()),
        wakeupFd_(createEventfd()),
        quit_(false),
        eventHandling_(false),
        callingPendingFunctors_(false),
        threadId_(CurrentThread::tid()),
        pwakeupChannel_(new Channel(this, wakeupFd_)){
            if(t_loopInThisThread){
                LOG<<" Another EventLoop "<<t_loopInThisThread<<" exists in this thread "<<threadId_;
            }else
            {
                t_loopInThisThread = this;
            }

            // pwakeupChannel_->setEvents(EPOLLIN|EPOLLET|EPOLLONESHOT);
            pwakeupChannel_->setEvents(EPOLLIN|EPOLLET);
            pwakeupChannel_->setReadHandler(bind(&EventLoop::handleRead, this);
            pwakeupChannel_->setConnHandler(bind(&EventLoop::handleConn, this);
            poller_->epoll_add(pwakeupChannel_, 0);    
}

void EventLoop::handleConn(){
    // poller_->epoll_mod(wakeupFd_, pwakeupChannel_, (EPOLLIN | EPOLLET | EPOLLONESHOT), 0);
    updatePoller(pwakeupChannel_, 0);
}

EventLoop::~EventLoop(){
    close(wakeupFd_);
    t_loopInThisThread = nullptr;
}



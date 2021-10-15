#include "Epoll.h"
#include <assert.h>
#include <cstdio>
#include <errno.h>
#include <memory>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <deque>
#include <queue>
#include "Util.h"
#include "base/Logging.h"

#include <arpa/inet.h>
#include <iostream>
#include <vector>
using namespace std;


const int EVENTSNUM=4096;
const int EPOLLWAIT_TIME=3000;

typedef shared_ptr<Channel> SP_Channel;

Epoll::Epoll():epollFd_(epoll_create1(EPOLL_CLOEXEC)),events_(EVENTSNUM) {
    assert(epollFd_ > 0);
}

Epoll::~Epoll() { }

// 注册新描述符 
void Epoll::epoll_add(SP_Channel request, int timeout){
    int fd = request->getFd();
    if(timeout>0){
        add_time(request, timeout);
        fd2http_[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd=fd;
    event.events=request->getEvents();

    request->EqualAndUpdateLastEvents();

    fd2chan_[fd] = request;
    if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event)<0){
        perror("epoll_add error");
        fd2chan_[fd].reset();
    }
}


// 修改描述符状态
void Epoll::epoll_mod(SP_Channel request, int timeout){
    if(timeout > 0) add_time(request, timeout);
    int fd=request->getFd();
    if(!request->EqualAndUpdateLastEvents()){
        struct epoll_event event;
        event.data.fd=fd;
        event.events=request->getEvents();
        if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event)<0){
            perror("epoll_mod error");
            fd2chan_[fd].reset();
        }
    }
}

// 从epoll中删除描述符 
void Epoll::epoll_del(SP_Channel request){
    int fd=request->getFd();
    struct epoll_event event;
    event.events=request->getLastEvents();
    // event.events=0;
    // request->EqualAndUpdateLastEvents();
    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event)<0){
        perror("epoll_del error");
    }
    fd2chan_[fd].reset();
    fd2http_[fd].reset();
}

// 返回活跃事件 
std::vector<SP_Channel> Epoll::poll(){
    while (true) {
        int event_count=epoll_wait(epollFd_, events_.data(), events_.size(), EPOLLWAIT_TIME);
        if(event_count<0) perror("epoll wait error");
        std::vector<SP_Channel> req_data = getEventRequest(event_count);
        if(req_data.size()>0) return req_data;
    }
}

void Epoll::handleExpired() { timerManager_.handleExpiredEvent(); }

//分发处理函数 
std::vector<SP_Channel> Epoll::getEventRequest(int events_num){
    std::vector<SP_Channel> req_data;
    for (int i=0; i<events_num; ++i) {
        // 获取有事件产生的描述符
        int fd = events_[i].data.fd;

        SP_Channel cur_req = fd2chan_[fd];

        if (cur_req) {
            cur_req->setRevents(events_[i].events);
            cur_req->setEvents(0);
            // 加入线程池之前将timer和request分离
            // cur_req->seperateTimer();
            req_data.push_back(cur_req);
        }else {
            LOG<<"SP cur_req is invalid";
        }
    }
    return req_data;
}

void Epoll::add_time(std::shared_ptr<Channel> request_data, int timeout){
    shared_ptr<HttpData> t = request_data->getHolder();
    if(t){
        timerManager_.addTimer(t, timeout);
    }else {
        LOG<<"timer add fail";
    }
}

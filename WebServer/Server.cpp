#include"Server.h"
#include<arpa/inet.h>
#include <cstring>
#include<netinet/in.h>
#include<sys/socket.h>
#include<functional>
#include <unistd.h>
#include"Util.h"
#include"base/Logging.h"

Server::Server(EventLoop* loop, int threadNum, int port)
        : loop_(loop),
        threadNum_(threadNum),
        eventLoopThreadPool_(new EventLoopThreadPool(loop_, threadNum)),
        started_(false),
        acceptChannel_(new Channel(loop_)),
        port_(port),
        listenFd_(socket_bind_listen(port_)){
            acceptChannel_->setFd(listenFd_);
            handle_for_sigpipe();
            if(setSocketNonBlocking(listenFd_)<0){
                perror("set socket non block failed");
                abort();
            }
}

void Server::start(){
    eventLoopThreadPool_->start();
    acceptChannel_->setEvents(EPOLLIN|EPOLLET);
    acceptChannel_->setReadHandler(bind(&Server::handNewConn, this));
    acceptChannel_->setConnHandler(bind(&Server::handThisConn, this));
    loop_->addToPoller(acceptChannel_, 0);
    started_ = true;
}

void Server::handNewConn(){
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    
    
}


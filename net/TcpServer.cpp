//
// Created by srdczk on 2020/5/29.
//

#include "TcpServer.h"
#include "NetHelper.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

// vias

//EventLoop *loop_;
//int threadNum_;
//std::shared_ptr<EventLoopThreadPool> threadPool_;
//bool started_;
//std::shared_ptr<Channel> acceptChannel_;
//int port_;
//int listenFd_;

TcpServer::TcpServer(EventLoop *loop, int threadNum, int port):
loop_(loop),
threadNum_(threadNum),
threadPool_(std::make_shared<EventLoopThreadPool>(loop_, threadNum_)),
started_(false),
port_(port),
listenFd_(NetHelper::BindAndListen(port_)),
acceptChannel_(std::make_shared<Channel>(loop_, listenFd_)) {
    // ignore signal
    NetHelper::IgnorePipe();
    NetHelper::SetNonBlocking(listenFd_);
}

void TcpServer::Start() {
    threadPool_->Start();
    acceptChannel_->SetEvents(EPOLLIN | EPOLLET);
    acceptChannel_->SetReadCallback(std::bind(&TcpServer::HandleNewConnect, this));
    acceptChannel_->SetUpdateCallback(std::bind(&TcpServer::HandleUpdate, this));
    loop_->AddToPoller(acceptChannel_);
    started_ = true;
}

// new Connect from // add to Thread Pool
void TcpServer::HandleNewConnect() {
    // epoll in -> new connection
    int cfd;
    struct sockaddr_in caddr;
    memset(&caddr, '\0', sizeof(caddr));
    socklen_t size = sizeof(caddr);

    if ((cfd = accept(listenFd_, (struct sockaddr *)&caddr, &size)) > 0) {
        auto loop = threadPool_->NextLoop();
        LOG_DEBUG("New connection from %s: %d", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
        NetHelper::SetNonBlocking(cfd);
        auto newChannel = std::make_shared<Channel>(loop, cfd);
        // has thing to write,
        loop->QueueInLoop([&] {
            newChannel->SetEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
            newChannel->SetReadCallback(std::bind(&Channel::EchoRead, newChannel));
            loop->AddToPoller(newChannel);
        });
    }
    acceptChannel_->SetEvents(EPOLLET | EPOLLIN);
}



#include "TcpServer.h"
#include "Channel.h"
#include "NetHelper.h"
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>

//EventLoop *loop_;
//int threadNum_;
//std::shared_ptr<EventLoopThreadPool> threadPool_;
//bool started_;
//int port_;
//int listenFd_;
//std::shared_ptr<Channel> acceptChannel_;

TcpServer::TcpServer(EventLoop *loop, int threadNum, int port):
loop_(loop),
threadNum_(threadNum),
threadPool_(std::make_shared<EventLoopThreadPool>(loop_, threadNum_)),
started_(false),
port_(port),
listenFd_(NetHelper::BindAndListen(port_)),
acceptChannel_(new Channel(loop_, listenFd_)) {
    // set Non Block
    NetHelper::IgnorePipe();
    NetHelper::SetNonBlocking(listenFd_);
}

void TcpServer::HandleNewConnect() {
    // has new connection
    int cfd;
    struct sockaddr_in caddr;
    memset(&caddr, '\0', sizeof(caddr));
    socklen_t size = sizeof(caddr);
    if ((cfd = accept(listenFd_, (struct sockaddr*)&caddr, &size)) > 0) {
        auto loop = threadPool_->NextLoop();
        // TODO: Add Log
        NetHelper::SetNonBlocking(cfd);
        NetHelper::SetNodelay(cfd);

        auto newChannel = new Channel(loop, cfd);
        loop->QueueInLoop(std::bind(&Channel::TestNewEvent, newChannel));
        // loop -> Next Loop
    }
    acceptChannel_->SetEvents(EPOLLET | EPOLLIN);
}

void TcpServer::Start() {
    threadPool_->Start();
    acceptChannel_->SetEvents(EPOLLIN | EPOLLET);
    acceptChannel_->SetReadCallback(std::bind(&TcpServer::HandleNewConnect, this));
    acceptChannel_->SetUpdateCallback(std::bind(&TcpServer::HandleUpdate, this));
    loop_->AddToPoller(acceptChannel_);
    started_ = true;
}

TcpServer::~TcpServer() {
    loop_->RemovePoller(acceptChannel_);
}

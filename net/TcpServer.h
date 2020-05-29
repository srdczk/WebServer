//
// Created by srdczk on 2020/5/29.
//

#ifndef WEBSERVER_TCPSERVER_H
#define WEBSERVER_TCPSERVER_H

#include "EventLoopThreadPool.h"

// implement echo server
class TcpServer {
public:
    TcpServer(EventLoop *loop, int threadNum, int port);
    ~TcpServer() = default;

    void Start();

private:
    void HandleNewConnect();
    void HandleUpdate() { loop_->UpdatePoller(acceptChannel_); }
private:
    // max fds
    EventLoop *loop_;
    int threadNum_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;
    bool started_;
    int port_;
    int listenFd_;
    std::shared_ptr<Channel> acceptChannel_;
};


#endif //WEBSERVER_TCPSERVER_H

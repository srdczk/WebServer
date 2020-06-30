//
// Created by srdczk on 2020/5/29.
//

#ifndef WEBSERVER_TCPSERVER_H
#define WEBSERVER_TCPSERVER_H

#include "EventLoopThreadPool.h"

// implement echo server
class TcpServer {
public:
    typedef Channel *ChannelPtr;
    TcpServer(EventLoop *loop, int threadNum, int port);
    ~TcpServer();

    void Start();

private:
    void HandleNewConnect();
    void HandleUpdate() { loop_->UpdatePoller(acceptChannel_); }
private:
    static const int kMaxFds = 65536;
    // max fds
    EventLoop *loop_;
    int threadNum_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;
    bool started_;
    int port_;
    int listenFd_;
    ChannelPtr acceptChannel_;
//    ChannelPtr channels_[kMaxFds];
};


#endif //WEBSERVER_TCPSERVER_H

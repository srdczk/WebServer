//
// Created by srdczk on 2020/5/26.
//

#ifndef WEBSERVER_TCPSERVER_H
#define WEBSERVER_TCPSERVER_H

#include "EventLoopThreadPool.h"
#include "Channel.h"

class TcpServer {
public:
    typedef EventLoop::ChannelPtr ChannelPtr;
    typedef std::function<void(std::shared_ptr<Channel>)> CallbackType;
    typedef std::function<int(std::shared_ptr<Channel>)> ReadCallbackType;

    TcpServer(const char *serverIp, const uint16_t port, int threadNum);
    ~TcpServer();





private:
    void NewConnectionHandler(EventLoop &loop, std::shared_ptr<Channel> channel);
    void ReadFromFdToBuffer(EventLoop &loop, std::shared_ptr<Channel> channel);
private:
    int serverFd_;
    ChannelPtr listenChannel_;
    EventLoop loop_;
    EventLoopThreadPool threadPool_;
    // ReadCallback -> read 结束以后

};


#endif //WEBSERVER_TCPSERVER_H

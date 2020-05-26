//
// Created by srdczk on 2020/5/26.
//

#ifndef WEBSERVER_EVENTLOOPTHREAD_H
#define WEBSERVER_EVENTLOOPTHREAD_H


#include "EventLoop.h"
#include "Channel.h"
#include <mutex>

class EventLoopThread {
public:
    typedef Channel::CallbackType CallbackType;
    typedef EventLoop::ChannelPtr ChannelPtr;

    EventLoopThread();
    ~EventLoopThread();

    EventLoopThread(const EventLoopThread &) = delete;
    EventLoopThread &operator=(const EventLoopThread &) = delete;


    void SetReadCallback(CallbackType readCb) { readCb_ = readCb; }
    void SetWriteCallback(CallbackType writeCb) { writeCb_ = writeCb; }
    void SetErrorCallback(CallbackType errorCb) { errorCb_ = errorCb; }

    void SetThreadId(int threadId) { threadId_ = threadId; }

    void Push(int fd);
    void Loop();
    void Quit();

private:

    void ReadHandler(EventLoop &eventLoop, std::shared_ptr<Channel> channel);

private:
    // listen Channel
    int pipe_[2];
    ChannelPtr listenChannel_;
    std::mutex mutex_;
    std::vector<int> fds_;
    EventLoop loop_;
    CallbackType readCb_;
    CallbackType writeCb_;
    CallbackType errorCb_;
    int threadId_;
};


#endif //WEBSERVER_EVENTLOOPTHREAD_H

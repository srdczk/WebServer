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
    EventLoopThread();
    ~EventLoopThread();

    // create Event Loop
    EventLoop *Loop();
private:
    void ThreadFunc();
private:
    EventLoop *loop_;
    bool exiting_;
    std::shared_ptr<std::thread> thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
};


#endif //WEBSERVER_EVENTLOOPTHREAD_H

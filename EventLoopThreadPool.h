//
// Created by srdczk on 2020/5/26.
//

#ifndef WEBSERVER_EVENTLOOPTHREADPOOL_H
#define WEBSERVER_EVENTLOOPTHREADPOOL_H

#include "EventLoopThread.h"

class EventLoopThreadPool {
public:
    EventLoopThreadPool(EventLoop *loop, int threadNum = std::thread::hardware_concurrency());
    ~EventLoopThreadPool() {
        // TODO: Add Log
//        LOG_DEBUG("Thread Pool Exit");
    }

    void Start();

    EventLoop *NextLoop();
private:
    EventLoop *loop_;
    bool started_;
    int threadNum_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
};


#endif //WEBSERVER_EVENTLOOPTHREADPOOL_H

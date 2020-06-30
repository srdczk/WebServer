#ifndef WEBSERVER_EVENTLOOPTHREAD_H
#define WEBSERVER_EVENTLOOPTHREAD_H

#include "EventLoop.h"
#include <condition_variable>

class EventLoopThread {
public:
    EventLoopThread();

    ~EventLoopThread();

    EventLoopThread(const EventLoopThread &) = delete;
    EventLoopThread &operator=(const EventLoopThread &) = delete;

    EventLoop *Loop();
private:

    void ThreadFunc();

private:
    EventLoop *loop_;
    bool exited_;
    std::shared_ptr<std::thread> thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
};


#endif //WEBSERVER_EVENTLOOPTHREAD_H

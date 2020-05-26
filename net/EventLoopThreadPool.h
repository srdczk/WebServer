//
// Created by srdczk on 2020/5/26.
//

#ifndef WEBSERVER_EVENTLOOPTHREADPOOL_H
#define WEBSERVER_EVENTLOOPTHREADPOOL_H

#include "EventLoopThread.h"
#include <thread>

class EventLoopThreadPool {
public:
    typedef EventLoopThread::CallbackType CallbackType;

    EventLoopThreadPool(int threadNum = std::thread::hardware_concurrency()): threadNum_(threadNum), index_(0) {}
    ~EventLoopThreadPool() { Quit(); }

    EventLoopThreadPool(const EventLoopThreadPool &) = delete;
    EventLoopThreadPool &operator=(const EventLoopThreadPool &) = delete;


    void Push(int fd);
    void Quit();

    void Loop();

    int ThreadNum() { return threadNum_; }

    void SetReadCallback(CallbackType readCb) { readCb_ = readCb; }
    void SetWriteCallback(CallbackType writeCb) { writeCb_ = writeCb; }
    void SetErrorCallback(CallbackType errorCb) { errorCb_ = errorCb; }

private:
    void ThreadFunc(int threadId);
private:
    int threadNum_;
    int index_;
    std::vector<std::shared_ptr<std::thread>> threads_;
    std::vector<EventLoopThread *> eventThreads_;
    CallbackType readCb_;
    CallbackType writeCb_;
    CallbackType errorCb_;
};


#endif //WEBSERVER_EVENTLOOPTHREADPOOL_H

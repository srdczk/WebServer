//
// Created by srdczk on 2020/5/26.
//

#include "EventLoopThread.h"
#include <unistd.h>
#include <fcntl.h>


EventLoopThread::EventLoopThread(): loop_(nullptr), exited_(false) { }

EventLoopThread::~EventLoopThread() {
    exited_ = true;
    if (loop_) {
        loop_->Quit();
        if (thread_ && thread_->joinable())
            thread_->join();
    }
}

void EventLoopThread::ThreadFunc() {
    EventLoop loop;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        loop_ = &loop;
        cv_.notify_all();
    }
    loop.Loop();
    loop_ = nullptr;
}

EventLoop *EventLoopThread::Loop() {
    thread_ = std::make_shared<std::thread>(std::bind(&EventLoopThread::ThreadFunc, this));
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!loop_) cv_.wait(lock);
    }
    return loop_;
}



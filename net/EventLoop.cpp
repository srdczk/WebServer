//
// Created by Admin on 2020/5/14.
//

#include "EventLoop.h"
#include "../base/AsyncLog.h"
#include <unistd.h>
#include <sys/poll.h>

__thread EventLoop *threadEventLoop = nullptr;


EventLoop::EventLoop(): threadId_(std::this_thread::get_id()), looping_(false) {
    if (threadEventLoop) {
        LOG_FATAL("Create EventLoop Error!");
    } else {
        threadEventLoop = this;
    }
}

bool EventLoop::InThisThread() {
    return std::this_thread::get_id() == threadId_;
}

void EventLoop::Loop() {
    looping_ = true;
    AssertInThread();
    ::poll(nullptr, 0, 5 * 1000);
    looping_ = false;
}

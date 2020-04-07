//
// Created by srdczk on 2020-04-07.
//

#include "EventLoop.h"
#include "../base/AsyncLog.h"
#include <functional>
#include <thread>

EventLoop::EventLoop() {
    looping_ = false;
    threadId_ = std::hash<std::thread::id>{}(std::this_thread::get_id());
}

void EventLoop::AssertInLoopThread() {
    if (!IsInLoopThread()) AbortNotInThread();
}

void EventLoop::Loop() {
    AssertInLoopThread();
    looping_ = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    LOG_TRACE("From this %d\n", threadId_);
    looping_ = false;
}

bool EventLoop::IsInLoopThread() const { return threadId_ == std::hash<std::thread::id>{}(std::this_thread::get_id()); }

void EventLoop::AbortNotInThread() { LOG_FATAL("Wrong!\n"); }

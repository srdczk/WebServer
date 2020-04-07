//
// Created by srdczk on 2020-04-07.
//

#pragma once

#include <cstdint>
#include <memory>
// 事件循环， 不可复制

class EventLoop : std::enable_shared_from_this<EventLoop> {
public:
    EventLoop();
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;
    void AssertInLoopThread();
    void Loop();
private:
    bool IsInLoopThread() const;
    void AbortNotInThread();
    bool looping_;
    uint64_t threadId_;
};




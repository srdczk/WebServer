//
// Created by srdczk on 2020/5/26.
//

#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop, int threadNum): loop_(loop), started_(false), threadNum_(threadNum), next_(0) { }

void EventLoopThreadPool::Start() {
    loop_->AssertInLoop();
    started_ = true;
    for (int i = 0; i < threadNum_; ++i) {
        auto thread = std::make_shared<EventLoopThread>();
        threads_.push_back(thread);
        loops_.push_back(thread->Loop());
    }
}

EventLoop *EventLoopThreadPool::NextLoop() {
    loop_->AssertInLoop();
    assert(started_);
    auto res = loop_;
    if (!loops_.empty()) {
        res = loops_[next_];
        next_ = (next_ + 1) % threadNum_;
    }
    return res;
}

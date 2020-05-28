//
// Created by srdczk on 2020/5/28.
//

#include "Timer.h"

size_t TimerNode::CalculateTime(int timeout) {
    struct timeval now;
    gettimeofday(&now, nullptr);
    return static_cast<size_t>(((now.tv_sec % 10000) * 1000) + now.tv_usec / 1000 + timeout);
}

TimerNode::TimerNode(TimerNode::MessagePtr http, int timeout):
unused_(false), http_(http) {
    expiredTime_ = CalculateTime(timeout);
}

TimerNode::~TimerNode() {
    if (http_) {
        http_->HandleClose();
    }
}

TimerNode::TimerNode(const TimerNode &node): http_(node.http_), expiredTime_(0) {}

void TimerNode::Update(int timeout) {
    expiredTime_ = CalculateTime(timeout);
}

bool TimerNode::IsValid() {
    if (unused_) return false;
    // pd expired time < ;
    auto now = CalculateTime(0);
    if (now < expiredTime_) {
        return true;
    } else {
        Delete();
        return false;
    }
}

void TimerNode::Clear() {
    http_.reset();
    Delete();
}


void TimerHeap::HandleTimeout() {
    while (!heap_.empty() && !heap_.top()->IsValid()) {
        heap_.pop();
    }
}

void TimerHeap::AddTimer(TimerNode::MessagePtr http, int timeout) {
    auto node = std::make_shared<TimerNode>(http, timeout);
    heap_.push(node);
    http->SetTimer(node);
}

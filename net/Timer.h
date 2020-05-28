//
// Created by srdczk on 2020/5/28.
//

#ifndef WEBSERVER_TIMER_H
#define WEBSERVER_TIMER_H

#include <memory>
#include <vector>
#include <queue>
#include <sys/time.h>
#include "HttpMessage.h"


class HttpMessage;

class TimerNode {
public:
    typedef std::shared_ptr<HttpMessage> MessagePtr;
    TimerNode(MessagePtr http, int timeout);
    ~TimerNode();
    TimerNode(const TimerNode &);
    void Update(int timeout);
    bool IsValid();
    void Clear();
    void Delete() { unused_ = true; }
    bool InUse() const { return !unused_; }
    size_t ExpiredTime() const { return expiredTime_; }

private:
    static size_t CalculateTime(int timeout);
private:
    bool unused_;
    MessagePtr http_;
    size_t expiredTime_;
};

typedef std::shared_ptr<TimerNode> TimerNodePtr;

struct TimerCmp {
    bool operator()(TimerNodePtr &a, TimerNodePtr &b) const {
        return a->ExpiredTime() > b->ExpiredTime();
    }
};

class TimerHeap {
public:
    TimerHeap() = default;
    ~TimerHeap() = default;
    void AddTimer(TimerNode::MessagePtr http, int timeout);
    void HandleTimeout();
private:
    std::priority_queue<TimerNodePtr, std::vector<TimerNodePtr>, TimerCmp> heap_;
};


#endif //WEBSERVER_TIMER_H

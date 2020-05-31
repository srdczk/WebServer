//
// Created by Admin on 2020/5/14.
//

#include "Channel.h"
#include "../base/AsyncLog.h"
#include "NetHelper.h"
#include <sys/epoll.h>

//
//int lastEvents_;
//// Channel, index in Heap
//int index_;
//uint64_t expiredTime_;
//CallbackType readCb_;

Channel::Channel(EventLoop *loop, int fd):
loop_(loop),
fd_(fd),
revents_(0),
events_(0),
lastEvents_(0),
// not In Heap
index_(-1),
expiredTime_(0) { }

bool Channel::UpdateLastEvents() {
    bool res = (lastEvents_ == events_);
    lastEvents_ = events_;
    return res;
}

void Channel::HandleRead() {
    if (readCb_)
        readCb_();
}

void Channel::HandleWrite() {
    if (writeCb_)
        writeCb_();
}

void Channel::HandleUpdate() {
    if (updateCb_)
        updateCb_();
}

void Channel::HandleEvents() {
    // lastEvents -> update events
    events_ = 0;
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        events_ = 0;
        return;
    }
    if (revents_ & EPOLLERR) {
        if (errorCb_) errorCb_();
        events_ = 0;
        return;
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        HandleRead();
    }
    if (revents_ & EPOLLOUT) {
        HandleWrite();
    }
    HandleUpdate();
}

void Channel::EchoRead() {
    std::string res;
    NetHelper::ReadN(fd_, res);
    LOG_DEBUG("Receive from client:%s", res.data());
    for (auto &c : res) {
        c = static_cast<char>(toupper(c));
    }
    NetHelper::WriteN(fd_, res);
}

std::shared_ptr<HttpMessage> Channel::Holder() {
    //
    std::shared_ptr<HttpMessage> res(http_.lock());
    return res;
}





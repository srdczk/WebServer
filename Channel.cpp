#include "Channel.h"
#include "NetHelper.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/timeb.h>

//EventLoop *loop_;
//int fd_;
//int events_;
//// return events -> decide return events
//int rEvents_;
//int lastEvents_;
//// some callback
//CallbackType readCb_;
//CallbackType writeCb_;
//CallbackType updateCb_;

const int Channel::kBufSize = 1024;
const uint64_t Channel::kDefaultTime = 2000;


Channel::Channel(EventLoop *loop, int fd):
loop_(loop),
fd_(fd),
events_(0),
rEvents_(0),
lastEvents_(0),
index_(-1),
expiredTime_(0) { }

Channel::~Channel() {
    close(fd_);
}

bool Channel::UpdateLastEvents() {
    auto res = (lastEvents_ == events_);
    lastEvents_ = events_;
    return !res;
}

void Channel::HandleEvents() {
    // handle events
    if ((rEvents_ & EPOLLIN) && readCb_) {
        readCb_();
    }
    if ((rEvents_ & EPOLLOUT) && writeCb_) {
        writeCb_();
    }
    if (updateCb_)
        updateCb_();
}

void Channel::TestNewEvent() {
    SetReadCallback(std::bind(&Channel::TestRead, this));
    SetUpdateCallback(std::bind(&Channel::TestUpdate, this));
    SetEvents(EPOLLIN | EPOLLET);
    loop_->AddToPoller(this, kDefaultTime);
}

void Channel::TestRead() {
    std::string res;
    NetHelper::ReadN(fd_, res);
    for (auto &c : res) {
        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    }
    NetHelper::WriteN(fd_, res);
    SetEvents(EPOLLIN | EPOLLET);
}

void Channel::TestUpdate() {
    loop_->UpdatePoller(this, kDefaultTime);
}



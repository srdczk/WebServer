//
// Created by Admin on 2020/5/14.
//

#include "Channel.h"
#include "../base/AsyncLog.h"
#include <sys/epoll.h>

Channel::Channel(int fd): fd_(fd), events_(0), heapIndex_(-1) {}

void Channel::SetLastActive(const timeval &time) {
    lastActive_.tv_sec = time.tv_sec;
    lastActive_.tv_usec = time.tv_usec;
}

void Channel::HandleEvent(EventLoop &loop, std::shared_ptr<Channel> ptrChannel, int events) {

    if (events & (EPOLLHUP | EPOLLERR)) {
        if (errCb_) errCb_(loop, ptrChannel);
        LOG_ERROR("handle event error:%d", fd_);

    } else {
        if (events & (EPOLLIN | EPOLLRDHUP)) {
            if (readCb_)
                readCb_(loop, ptrChannel);
        }
        if (events & (EPOLLOUT)) {
            if (writeCb_)
                writeCb_(loop, ptrChannel);
        }
    }
}



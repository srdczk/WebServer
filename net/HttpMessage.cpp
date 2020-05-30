//
// Created by srdczk on 2020/5/28.
//

#include "HttpMessage.h"
#include "NetHelper.h"
#include "EventLoop.h"
#include <iostream>
#include <sys/epoll.h>

HttpMessage::HttpMessage(EventLoop *loop, int fd):
loop_(loop),
fd_(fd),
channel_(std::make_shared<Channel>(loop_, fd)) {
    channel_->SetReadCallback(std::bind(&HttpMessage::ReadCallback, this));
    channel_->SetUpdateCallback(std::bind(&HttpMessage::UpdateCallback, this));
}

void HttpMessage::ReadCallback() {
    NetHelper::ReadN(fd_, buff);
    for (auto &c : buff) {
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
    }
    NetHelper::WriteN(fd_, buff);
    channel_->SetEvents(EPOLLET | EPOLLIN);
    // has WriteThings
}

void HttpMessage::NewEvent() {
    channel_->SetEvents(EPOLLIN | EPOLLET);
    loop_->AddToPoller(channel_);
}


void HttpMessage::UpdateCallback() {
    loop_->UpdatePoller(channel_);
}



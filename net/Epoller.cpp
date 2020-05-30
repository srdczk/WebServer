//
// Created by Admin on 2020/5/14.
//

#include "Epoller.h"
#include <cassert>
#include <unistd.h>
#include "../base/AsyncLog.h"
#include <cstring>

const size_t Epoller::kMaxEvents = 4096;

const int Epoller::kEpollWait = 100000;

Epoller::Epoller(): readyEvents_(kMaxEvents) {
    assert((epFd_ = epoll_create(EPOLL_CLOEXEC)) != -1);
}

Epoller::~Epoller() {
    close(epFd_);
}

std::vector<Epoller::ChannelPtr> Epoller::ReadyChannels(int num) {
    // from channel Map
    std::vector<Epoller::ChannelPtr> res;
    for (int i = 0; i < num; ++i) {
        auto event = readyEvents_[i];
        auto channel = channelMap_[event.data.fd];
        if (channel) {
            channel->SetREvents(event.events);
            channel->SetEvents(0);
            res.push_back(channel);
        } else {
            LOG_DEBUG("No Channel for Fd: %d", event.data.fd);
        }
    }
    return res;
}

void Epoller::EpollAdd(Epoller::ChannelPtr channel, int timeout) {
    // add fd
    auto fd = channel->Fd();
    struct epoll_event epEvent;
    epEvent.events = static_cast<uint32_t>(channel->Events());
    epEvent.data.fd = fd;

    // update lastEvents
    channel->UpdateLastEvents();

    channelMap_[fd] = channel;
    if (epoll_ctl(epFd_, EPOLL_CTL_ADD, fd, &epEvent) == -1) {
        LOG_ERROR("Epoll Create Error, fd:%d", fd);
        channelMap_.erase(fd);
    }

    // TODO: timeout use to add to TimerHeap
}

void Epoller::EpollMod(Epoller::ChannelPtr channel, int timeout) {
    auto fd = channel->Fd();
    // last events != events => need update
    if (!channel->UpdateLastEvents()) {
        struct epoll_event epEvent;
        epEvent.events = static_cast<uint32_t>(channel->Events());
        epEvent.data.fd = fd;

        if (epoll_ctl(epFd_, EPOLL_CTL_MOD, fd, &epEvent) == -1) {
            LOG_ERROR("Epoll Modify Error, fd:%d", fd);
            channelMap_.erase(fd);
        }

    }

    // TODO: timeout use to add to TimerHeap
}

void Epoller::EpollDel(Epoller::ChannelPtr channel) {
    auto fd = channel->Fd();
    struct epoll_event epEvent;
    epEvent.data.fd = fd;
    epEvent.events = static_cast<uint32_t>(channel->LastEvents());
    if (epoll_ctl(epFd_, EPOLL_CTL_DEL, fd, &epEvent) == -1) {
        LOG_ERROR("Epoll Delete Error, fd:%d", fd);
    }
    channelMap_.erase(fd);
}

std::vector<Epoller::ChannelPtr> Epoller::EpollWait() {
    // epoll wait for long time
    while (true) {
        int cnt = epoll_wait(epFd_, &*readyEvents_.begin(), readyEvents_.size(), -1);
        if (cnt <= 0) {
            std::string errString(strerror(errno));
            if (errString != "Interrupted system call") {
                LOG_DEBUG("Epoll Wait Error");
            }
            continue;
        }
        auto res = ReadyChannels(cnt);
        if (res.size()) return res;
    }
}



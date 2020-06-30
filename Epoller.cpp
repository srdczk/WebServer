#include "Epoller.h"
#include "NetHelper.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>

const int Epoller::kEpollSize = 4096;

Epoller::Epoller():
epFd_(epoll_create(kEpollSize)),
readyEvents_(kEpollSize) {
    assert(epFd_ != -1);
}

int Epoller::EpollAdd(Epoller::ChannelPtr channel, uint64_t timeout) {
    assert(channel->Index() == -1);
    if (timeout) {
        // add to heap
        heap_.Insert(channel, timeout);
    }
    auto fd = channel->Fd();
    assert(!channelMap_.count(fd));
    channelMap_[fd] = channel;
    auto events = channel->GetEvents();
    channel->UpdateLastEvents();
    // add to poller -> set events = 0
    channel->SetEvents(0);
    struct epoll_event event;
    event.data.fd = fd;
    event.events = static_cast<uint32_t>(events);

    if (epoll_ctl(epFd_, EPOLL_CTL_ADD, fd, &event) == -1) {
        // TODO:LOG EPOLL FAILED
        return -1;
    }

    return 0;
}

int Epoller::EpollMod(Epoller::ChannelPtr channel, uint64_t timeout) {
    if (timeout) {
        assert(channel->Index() != -1);
        heap_.Change(channel, timeout);
    }
    auto fd = channel->Fd();
    // should have add to poller
    assert(channelMap_.count(fd));
    auto events = channel->GetEvents();
    if (channel->UpdateLastEvents()) {
        channel->SetEvents(0);
        struct epoll_event event;
        event.data.fd = fd;
        event.events = static_cast<uint32_t>(events);
        if (epoll_ctl(epFd_, EPOLL_CTL_MOD, fd, &event) == -1) {
            // TODO: Add LOG
            return -1;
        }
        return 0;
    }
    // needn't mod
    return -1;
}

int Epoller::EpollDel(Epoller::ChannelPtr channel) {
    auto fd = channel->Fd();
    // should have add to poller
    assert(channelMap_.count(fd));
    auto events = channel->GetLastEvents();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = static_cast<uint32_t>(events);
    if (epoll_ctl(epFd_, EPOLL_CTL_DEL, fd, &event) == -1) {
        // TODO: Add Log
        return -1;
    }
    // channel will ~Channel -> close
    channelMap_.erase(fd);
    // delete Channel (will not use)
    if (channel->Index() != -1) {
        heap_.Delete(channel);
    }
    delete channel;
    return 0;
}

bool Epoller::Contains(ChannelPtr channel) {
    return channelMap_.count(channel->Fd());
}

std::vector<Epoller::ChannelPtr> Epoller::ReadyEvents(int num) {
    std::vector<ChannelPtr> res;

    for (int i = 0; i < num; ++i) {
        auto event = readyEvents_[i];
        if (channelMap_.count(event.data.fd)) {
            auto channel = channelMap_[event.data.fd];
            channel->SetREvents(event.events);
            res.push_back(channel);
        }
    }

    return res;
}

std::vector<Epoller::ChannelPtr> Epoller::EpollWait() {
    while (true) {

        int num = epoll_wait(epFd_, &*readyEvents_.begin(), readyEvents_.size(),-1);
        if (num <= 0) {
            if (num < 0)
                char *p = strerror(errno);
            continue;
        }
        auto res = ReadyEvents(num);
        if (!res.empty())
            return res;
    }
}

void Epoller::HandleExpired() {
    // delete from heap_
    auto now = NetHelper::GetExpiredTime(0);
    while (heap_.Top() && heap_.Top()->ExpiredTime() < now) {
        auto top = heap_.Top();
        EpollDel(top);
    }
}






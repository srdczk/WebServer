//
// Created by Admin on 2020/5/14.
//

#include "EventLoop.h"
#include <unistd.h>

const int EventLoop::kInitActiveSize = 4;

const int EventLoop::kMinMilSeconds = 10000;

EventLoop::EventLoop(): started_(false) {
    activeEvents_.resize(kInitActiveSize);
}

bool EventLoop::FindChannel(EventLoop::ChannelPtr channel) {
    return channelMap_.count(channel->Fd());
}

void EventLoop::AddChannel(EventLoop::ChannelPtr channel, int events) {
    if (events) {
        if (FindChannel(channel)) {
            int oldEvents = channel->Events();
            channel->SetEvents(oldEvents | events);

            epoller_.EpollCtl(channel->Fd(), EPOLL_CTL_MOD, channel->Events());

        } else {
            channel->SetEvents(events);
            epoller_.EpollCtl(channel->Fd(), EPOLL_CTL_MOD, channel->Events());
            // add to channel map
            channelMap_[channel->Fd()] = channel;
        }
    }
}

void EventLoop::RemoveChannel(EventLoop::ChannelPtr channel, int events) {
    if (FindChannel(channel)) {
        int oldEvents = channel->Events();

        if (events & EPOLLIN)
            oldEvents &= (~EPOLLIN);

        if (events & EPOLLOUT)
            oldEvents &= (~EPOLLOUT);

        channel->SetEvents(0);

        if (!oldEvents) {

            epoller_.EpollCtl(channel->Fd(), EPOLL_CTL_DEL, 0);
            channelMap_.erase(channel->Fd());

        } else {

            epoller_.EpollCtl(channel->Fd(), EPOLL_CTL_MOD, channel->Events());

        }
    }
}

const timeval &EventLoop::LastActiveTime() { return lastActive_; }

void EventLoop::Loop() {
    started_ = true;
    while (started_) {
        int timeout = -1;
        if (!minHeap_.Empty()) {
            timeval now;
            gettimeofday(&now, nullptr);
            auto minTime = minHeap_.Top()->LastActive();
            timeout = (minTime.tv_sec - now.tv_sec) * 1000 + ((minTime.tv_usec - now.tv_usec) / 1000) + kMinMilSeconds;
            if (timeout < 0)
                timeout = 0;
        }

        activeEventsCnt_ = epoller_.EpollWait(activeEvents_, timeout);
        // process events
        EventsProcess();
        // process timer
        TimerProcess();
    }
}

void EventLoop::Quit() { started_ = false; }

void EventLoop::EventsProcess() {
    gettimeofday(&lastActive_, nullptr);

    for (auto &it : activeEvents_) {
        auto channel = channelMap_.find(it.data.fd);
        if (channel != channelMap_.end()) {
            auto ptr = channel->second;
            ptr->HandleEvent(*this, ptr, it.events);
        }
    }

    activeEventsCnt_ = 0;

}

void EventLoop::TimerProcess() {
    long long time = lastActive_.tv_sec * 1000 + (lastActive_.tv_usec / 1000) - kMinMilSeconds;
    timeval expired{time / 1000, (time % 1000)  * 1000};
    while (!minHeap_.Empty() && TimevalGreater(expired, minHeap_.Top()->LastActive())) {
        static const char msg[] = "timeout";
        auto channel = minHeap_.Top();
        write(channel->Fd(), msg, sizeof(msg));
        RemoveChannel(channel, EPOLLIN | EPOLLOUT);
        close(channel->Fd());
        DeleteLastActiveTime(channel);
    }
}







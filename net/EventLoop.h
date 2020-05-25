//
// Created by Admin on 2020/5/14.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include "Epoller.h"
#include "Heap.h"
#include "Channel.h"
#include <unordered_map>
#include <memory>
#include <sys/time.h>

class Channel;

class EventLoop {
public:
    typedef std::shared_ptr<Channel> ChannelPtr;
    // vector<epoll_event>
    typedef Epoller::ReadyEvents ReadyEvents;
    EventLoop();
    void AddChannel(ChannelPtr channel, int events);
    void RemoveChannel(ChannelPtr channel, int events);
    const timeval &LastActiveTime();

    void AddLastActiveTime(ChannelPtr channel, const timeval & lastTime) { minHeap_.Push(channel, lastTime); }

    void ChangeLastActiveTime(ChannelPtr channel, const timeval & lastTime) { minHeap_.Change(channel, lastTime); }

    void DeleteLastActiveTime(ChannelPtr channel) { minHeap_.Delete(channel); }


    void Loop();

    void Quit();

    void EventsProcess();

    void TimerProcess();

private:
    bool FindChannel(ChannelPtr channel);
private:

    static const int kInitActiveSize;

    static const int kMinMilSeconds;

    bool started_;
    // fd 2 channel
    std::unordered_map<int, ChannelPtr> channelMap_;
    int activeEventsCnt_;
    ReadyEvents activeEvents_;
    Epoller epoller_;
    Heap minHeap_;
    // timeval -> sys/time.h
    timeval lastActive_;
};


#endif //WEBSERVER_EVENTLOOP_H

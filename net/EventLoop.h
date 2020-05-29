//
// Created by Admin on 2020/5/14.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include "Epoller.h"
#include "Channel.h"
#include "../base/AsyncLog.h"
#include <unordered_map>
#include <memory>
#include <mutex>
#include <sys/time.h>
#include <thread>
#include <cassert>

class EventLoop {
public:
    typedef std::function<void()> Function;

    typedef std::shared_ptr<Channel> ChannelPtr;

    EventLoop();

    ~EventLoop();

    bool InThisThread();

    void AssertInThread() { assert(InThisThread()); }

    void Loop();

    void Quit();

    void RunInLoop(Function &&);

    void QueueInLoop(Function &&);

    void RemoveFromPoller(ChannelPtr channel) { poller_->EpollDel(channel); }

    // timeout = 1, don't add to timer
    void UpdatePoller(ChannelPtr channel, int timeout = 0) { poller_->EpollMod(channel, timeout); }

    void AddToPoller(ChannelPtr channel, int timeout = 0) { poller_->EpollAdd(channel, timeout); }

private:
    int CreateEventFd();
    void Wakeup();
    void HandleRead();
    void HandleUpdate();
    void DoFunctions();
private:
    std::thread::id threadId_;
    bool looping_;
    bool quit_;
    bool handlingEvent_;
    bool callingFunctions_;
    std::shared_ptr<Epoller> poller_;
    // wakeup eventLoop, in Other event Loop to Add Function
    int wakeupFd_;
    std::shared_ptr<Channel> wakeupChannel_;
    std::mutex mutex_;
    std::vector<Function> functions_;
};


#endif //WEBSERVER_EVENTLOOP_H

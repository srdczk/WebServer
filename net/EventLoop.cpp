//
// Created by Admin on 2020/5/14.
//

#include "EventLoop.h"
#include "NetHelper.h"
#include "../base/AsyncLog.h"
#include <unistd.h>
#include <sys/poll.h>
#include <sys/eventfd.h>
#include <cstring>

__thread EventLoop *threadEventLoop = nullptr;

// vias
//std::thread::id threadId_;
//bool looping_;
//bool quit_;
//bool handlingEvent_;
//bool callingFunctions_;
//std::shared_ptr<Epoller> poller_;
//// wakeup eventLoop, in Other event Loop to Add Function
//int wakeupFd_;
//std::shared_ptr<Channel> wakeupChannel_;
//std::mutex mutex_;
//std::vector<Function> functions_;

EventLoop::EventLoop():
threadId_(std::this_thread::get_id()),
looping_(false),
quit_(false),
handlingEvent_(false),
callingFunctions_(false),
poller_(std::make_shared<Epoller>()),
wakeupFd_(CreateEventFd()),
wakeupChannel_(std::make_shared<Channel>(this, wakeupFd_)) {
    if (threadEventLoop) {
        LOG_FATAL("Create EventLoop Error!");
    } else {
        threadEventLoop = this;
    }
    // poller, have not add to poller
    wakeupChannel_->SetEvents(EPOLLIN | EPOLLET);
    wakeupChannel_->SetReadCallback(std::bind(&EventLoop::HandleRead, this));
    wakeupChannel_->SetUpdateCallback(std::bind(&EventLoop::HandleUpdate, this));
    poller_->EpollAdd(wakeupChannel_, 0);
}

bool EventLoop::InThisThread() {
    return std::this_thread::get_id() == threadId_;
}

void EventLoop::Loop() {
    AssertInThread();
    looping_ = true;
    quit_ = false;
    while (!quit_) {
        auto channels = poller_->EpollWait();
        handlingEvent_ = true;
        for (auto &channel : channels) {
            channel->HandleEvents();
        }
        handlingEvent_ = false;
        DoFunctions();
        // TODO:remove inactive connections
//        poller_->HandleExpired();
    }
    looping_ = false;
}

int EventLoop::CreateEventFd() {
    int res = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (res < 0) {
        LOG_DEBUG("Fail in Create Event fd");
        abort();
    }
    return res;
}

void EventLoop::Wakeup() {
    uint64_t send = 1;
    ssize_t res = NetHelper::WriteN(wakeupFd_, (char*)(&send), sizeof send);
    if (res != sizeof(send)) {
        char *p = strerror(errno);
        LOG_DEBUG("EventLoop::Wakeup Write To %d", res);
    }
}

void EventLoop::HandleRead() {
    // read from File
    // Read From;
    int32_t receive;
    auto res = NetHelper::ReadN(wakeupFd_, (void *)(&receive), sizeof(receive));
    if (res < sizeof(int32_t)) {
        LOG_DEBUG("EventLoop::Read to %d", res);
    }
    // setEvents
    wakeupChannel_->SetEvents(EPOLLIN | EPOLLET);
    // update ->
}

void EventLoop::HandleUpdate() {
    // update,
    poller_->EpollMod(wakeupChannel_, 0);
}

void EventLoop::RunInLoop(EventLoop::Function &&function) {
    if (InThisThread()) {
        function();
    } else {
        // add to functions
        QueueInLoop(std::move(function));
    }
}

void EventLoop::QueueInLoop(EventLoop::Function &&function) {
    // add to functions
    {
        std::lock_guard<std::mutex> guard(mutex_);
        functions_.emplace_back(function);
    }
    if (!InThisThread() || callingFunctions_) Wakeup();
}

void EventLoop::DoFunctions() {
    std::vector<Function> tmp;
    callingFunctions_ = true;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        tmp.swap(functions_);
    }
    for (auto &function : tmp) function();
    callingFunctions_ = false;
}

EventLoop::~EventLoop() {
    // close
    close(wakeupFd_);
    threadEventLoop = nullptr;
}

void EventLoop::Quit() {
    quit_ = true;
    if (!InThisThread())
        Wakeup();
}


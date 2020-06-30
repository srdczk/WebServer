#include "EventLoop.h"
#include "Epoller.h"
#include "NetHelper.h"
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/poll.h>

// EventLoop belong to thread
__thread EventLoop *threadLoop = nullptr;


EventLoop::EventLoop():
threadId_(std::this_thread::get_id()),
loop_(false),
quit_(false),
handlingEvents_(false),
doingTasks_(false),
poller_(std::make_shared<Epoller>()),
wakeupFd_(CreateEventFd()),
wakeupChannel_(new Channel(this, wakeupFd_)) {
    // add wakeupFd to poller
    assert(!threadLoop);
    threadLoop = this;
    wakeupChannel_->SetEvents(EPOLLIN | EPOLLET);
    wakeupChannel_->SetReadCallback(std::bind(&EventLoop::WakeReadCallback, this));
    wakeupChannel_->SetUpdateCallback(std::bind(&EventLoop::WakeUpdateCallback, this));
    // poller -> Add Channel
    poller_->EpollAdd(wakeupChannel_);
}

bool EventLoop::IsInThread() {
    return std::this_thread::get_id() == threadId_;
}

void EventLoop::AssertInLoop() { assert(IsInThread()); }

int EventLoop::CreateEventFd() {
    // create non blocking fd
    int res = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    assert(res >= 0);
    return res;
}

void EventLoop::Loop() {
    loop_ = true;
    AssertInLoop();

    while (!quit_) {
        auto readyEvents = poller_->EpollWait();
        handlingEvents_ = true;
        for (auto &event : readyEvents)
            event->HandleEvents();
        handlingEvents_ = false;
        DoTasks();
        poller_->HandleExpired();
    }

    loop_ = false;
}



EventLoop::~EventLoop() {
    RemovePoller(wakeupChannel_);
    threadLoop = nullptr;
}

void EventLoop::Wakeup() {
    // write simple bytes to wakeup Fd
    uint64_t send = 1;
    auto res = NetHelper::WriteN(wakeupFd_, (char *)&send, sizeof(send));
    assert(res == sizeof(send));
    // TODO: Add Log
}

void EventLoop::WakeReadCallback() {
    uint64_t rec = 1;
    ssize_t res = NetHelper::ReadN(wakeupFd_, (char *)&rec, sizeof(rec));
    assert(res == sizeof(rec));
    // TODO: Add Log
    wakeupChannel_->SetEvents(EPOLLIN | EPOLLET);
}

void EventLoop::WakeUpdateCallback() {
    poller_->EpollMod(wakeupChannel_);
}

void EventLoop::Quit() {
    quit_ = true;
    Wakeup();
}

void EventLoop::DoTasks() {
    doingTasks_ = true;
    std::vector<Task> tasks;
    {
        // visit for task should add lock
        std::lock_guard<std::mutex> guard(mutex_);
        tasks.swap(tasks_);
    }
    for (auto &task : tasks)
        task();
    doingTasks_ = false;
}

void EventLoop::RunInLoop(EventLoop::Task task) {
    if (IsInThread())
        task();
    else QueueInLoop(task);
}

void EventLoop::QueueInLoop(EventLoop::Task task) {
    // visit for tasks
    {
        std::lock_guard<std::mutex> guard(mutex_);
        tasks_.push_back(task);
    }
    if (!IsInThread() || doingTasks_) Wakeup();
}

void EventLoop::AddToPoller(Channel *channel, uint64_t timeout) {
    poller_->EpollAdd(channel, timeout);
}

void EventLoop::UpdatePoller(Channel *channel, uint64_t timeout) {
    poller_->EpollMod(channel, timeout);
}

void EventLoop::RemovePoller(Channel *channel) {
    poller_->EpollDel(channel);
}


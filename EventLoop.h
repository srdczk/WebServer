#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H


#include <thread>
#include <memory>
#include <cassert>
#include <functional>
#include <vector>
#include <mutex>

// add declare
class Channel;
class Epoller;

class EventLoop {
public:

    typedef std::function<void()> Task;
    EventLoop();
    ~EventLoop();

    //not copyable
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;

    bool IsInThread();
    void AssertInLoop();
    void Loop();

    void Wakeup();
    void Quit();
    void RunInLoop(Task task);
    void QueueInLoop(Task task);

    void AddToPoller(Channel *channel, uint64_t timeout = 0);
    void UpdatePoller(Channel *channel, uint64_t timeout = 0);
    void RemovePoller(Channel *channel);
private:

    int CreateEventFd();
    void WakeReadCallback();
    void WakeUpdateCallback();
    void DoTasks();

private:
    std::thread::id threadId_;
    bool loop_;
    bool quit_;
    // add Tag to
    bool handlingEvents_;
    bool doingTasks_;
    std::shared_ptr<Epoller> poller_;
    int wakeupFd_;
    Channel *wakeupChannel_;
    // lock to protect task vector
    std::mutex mutex_;
    std::vector<Task> tasks_;
};


#endif //WEBSERVER_EVENTLOOP_H

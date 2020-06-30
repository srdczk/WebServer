#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H

#include "EventLoop.h"

class Channel {
public:
    typedef std::function<void()> CallbackType;
    Channel(EventLoop *loop, int fd);

    // channel -> release
    ~Channel();

    // not copy able
    Channel(const Channel &) = delete;
    Channel &operator=(const Channel &) = delete;

    // getter and setter for fd and events
    int Fd() { return fd_; }
    int GetEvents() { return events_; }
    int GetREvents() { return rEvents_; }
    int GetLastEvents() { return lastEvents_; }

    void SetEvents(int events) { events_ = events; }
    void SetREvents(int rEvents) { rEvents_ = rEvents; }
    void SetLastEvents(int lastEvents) { lastEvents_ = lastEvents; }

    bool UpdateLastEvents();

    void SetReadCallback(CallbackType readCb) { readCb_ = readCb; }
    void SetWriteCallback(CallbackType writeCb) { writeCb_ = writeCb; }
    void SetUpdateCallback(CallbackType updateCb) { updateCb_ = updateCb; }

    void HandleEvents();

    void TestRead();

    void TestUpdate();

    void TestNewEvent();

    // viarables in heap
    int Index() { return index_; }
    void SetIndex(int index) { index_ = index; }

    uint64_t ExpiredTime() { return expiredTime_; }
    void SetExpiredTime(uint64_t expiredTime) { expiredTime_ = expiredTime; }
private:
    static const int kBufSize;
    static const uint64_t kDefaultTime;
    EventLoop *loop_;
    int fd_;
    int events_;
    // return events -> decide return events
    int rEvents_;
    int lastEvents_;
    // add to timer ->
    int index_;
    uint64_t expiredTime_;
    // some callback
    CallbackType readCb_;
    CallbackType writeCb_;
    CallbackType updateCb_;
    // if is http channel
//    bool isHttp_;
};


#endif //WEBSERVER_CHANNEL_H

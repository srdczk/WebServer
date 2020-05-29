//
// Created by Admin on 2020/5/14.
//

#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H

#include <functional>
#include <memory>
#include <sys/time.h>
#include "Buffer.h"


class EventLoop;

class Channel {
public:
    typedef std::function<void()> CallbackType;

    Channel(EventLoop *loop, int fd);

    void SetREvents(int revents) { revents_ = revents; }
    void SetEvents(int events) { events_ = events; }
    bool UpdateLastEvents();
    int Fd() { return fd_; }
    int Events() { return events_; }
    int LastEvents() { return lastEvents_; }

    void SetReadCallback(CallbackType &&readCb) { readCb_ = readCb; }
    void SetWriteCallback(CallbackType &&writeCb) { writeCb_ = writeCb; }
    void SetUpdateCallback(CallbackType &&updateCb) { updateCb_ = updateCb; }
    void SetErrorCallback(CallbackType &&errorCb) { errorCb_ = errorCb; }

    void EchoRead();

    void HandleRead();
    void HandleWrite();
    void HandleUpdate();
    void HandleEvents();

private:
    EventLoop *loop_;
    int fd_;
    int revents_;
    int events_;
    int lastEvents_;
    CallbackType readCb_;
    CallbackType writeCb_;
    CallbackType updateCb_;
    CallbackType errorCb_;
};

#endif //WEBSERVER_CHANNEL_H

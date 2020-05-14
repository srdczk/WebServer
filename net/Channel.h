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
    typedef std::function<void (EventLoop &, std::shared_ptr<Channel>)> CallbackType;
    Channel(int fd);

    void SetReadCallback(CallbackType cb) { readCb_ = cb; }
    void SetWriteCallback(CallbackType cb) { writeCb_ = cb; }
    void SetErrorCallback(CallbackType cb) { errCb_ = cb; }
    void SetEvents(int events) { events_ = events; }
    int Fd() { return fd_; }
    int Events() { return events_; }
    Buffer &ReadBuffer() { return readBuffer_; }
    Buffer &WriteBuffer() { return writeBuffer_; }

    timeval &LastActive() { return lastActive_; }

    void SetLastActive(const struct timeval &time);

    int HeapIndex() { return heapIndex_; }
    void SetHeapIndex(int index) { heapIndex_ = index; }

    void HandleEvent(EventLoop &loop, std::shared_ptr<Channel> ptrChannel, int events);

private:
    int fd_;
    int events_;
    CallbackType writeCb_;
    CallbackType readCb_;
    CallbackType errCb_;

    Buffer readBuffer_;
    Buffer writeBuffer_;

    timeval lastActive_;
    int heapIndex_;
};

#endif //WEBSERVER_CHANNEL_H

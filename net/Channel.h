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
    void SetREvents(int revents) { revents_ = revents; }
    void SetEvents(int events) { events_ = events; }
    bool UpdateLastEvents();
    int Fd() { return fd_; }
    int Events() { return events_; }
    int LastEvents() { return lastEvents_; }
private:
    int fd_;
    int revents_;
    int events_;
    int lastEvents_;
};

#endif //WEBSERVER_CHANNEL_H

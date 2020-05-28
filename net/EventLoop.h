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
#include <sys/time.h>
#include <thread>
#include <cassert>

class Channel;

class EventLoop {
public:
    EventLoop();

    bool InThisThread();

    void AssertInThread() { assert(InThisThread()); }

    void Loop();


private:
    std::thread::id threadId_;
    bool looping_;
};


#endif //WEBSERVER_EVENTLOOP_H

//
// Created by Admin on 2020/5/14.
//

#include "Channel.h"
#include "../base/AsyncLog.h"
#include <sys/epoll.h>


bool Channel::UpdateLastEvents() {
    bool res = (lastEvents_ == events_);
    lastEvents_ = events_;
    return res;
}

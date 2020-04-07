//
// Created by srdczk on 2020/4/5.
//
#include "base/AsyncLog.h"
#include "net/EventLoop.h"

int main() {
    CAsyncLog::Init("laowu");
    LOG_DEBUG("TEST %d", 23);
    return 0;
}

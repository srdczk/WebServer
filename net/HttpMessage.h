//
// Created by srdczk on 2020/5/28.
//

#ifndef WEBSERVER_HTTPMESSAGE_H
#define WEBSERVER_HTTPMESSAGE_H

#include "Timer.h"

class TimerNode;

class HttpMessage {
public:
    void HandleClose() {}
    void SetTimer(std::shared_ptr<TimerNode> timer) {}
};


#endif //WEBSERVER_HTTPMESSAGE_H

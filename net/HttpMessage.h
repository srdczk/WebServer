//
// Created by srdczk on 2020/5/28.
//

#ifndef WEBSERVER_HTTPMESSAGE_H
#define WEBSERVER_HTTPMESSAGE_H

//#include "Timer.h"
#include "Channel.h"

class TimerNode;

class HttpMessage {
public:
    HttpMessage(EventLoop *loop, int fd);
    ~HttpMessage();
    void HandleClose() {}
    void SetTimer(std::shared_ptr<TimerNode> timer) {}
    void NewEvent();
    std::shared_ptr<Channel> GetChannel() { return channel_; }
private:
    void ReadCallback();
    void WriteCallback();
    void UpdateCallback();
private:
    static const size_t kKeepMilSeconds;
    EventLoop *loop_;
    int fd_;
    std::shared_ptr<Channel> channel_;
    std::string buff;
};


#endif //WEBSERVER_HTTPMESSAGE_H

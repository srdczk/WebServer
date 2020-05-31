//
// Created by Admin on 2020/5/14.
//

#ifndef WEBSERVER_EPOLLER_H
#define WEBSERVER_EPOLLER_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <sys/epoll.h>
#include "Channel.h"
#include "Heap.h"

class Epoller {
public:
    typedef std::vector<struct epoll_event> ReadyEvents;
    typedef std::shared_ptr<Channel> ChannelPtr;

    Epoller();
    ~Epoller();

    void EpollAdd(ChannelPtr channel, int timeout);

    void EpollMod(ChannelPtr channel, int timeout);

    void EpollDel(ChannelPtr channel);

    std::vector<ChannelPtr> EpollWait();

    void HandleExpired();
private:
    std::vector<ChannelPtr> ReadyChannels(int num);
private:
    static const size_t kMaxEvents;
    static const int kEpollWait;
    int epFd_;
    ReadyEvents readyEvents_;
    // update revents
    std::unordered_map<int, ChannelPtr> channelMap_;
    Heap heap_;
};


#endif //WEBSERVER_EPOLLER_H

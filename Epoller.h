#ifndef WEBSERVER_EPOLLER_H
#define WEBSERVER_EPOLLER_H

#include "Channel.h"
#include "Heap.h"
#include <unordered_map>


class Epoller {
public:
    typedef Channel *ChannelPtr;
    Epoller();
    //not copyable
    Epoller(const Epoller &)= delete;
    Epoller &operator=(const Epoller &) = delete;

    int EpollAdd(ChannelPtr channel, uint64_t timeout = 0);
    int EpollMod(ChannelPtr channel, uint64_t timeout = 0);
    int EpollDel(ChannelPtr channel);

    void HandleExpired();

    bool Contains(ChannelPtr channel);

    std::vector<ChannelPtr> EpollWait();

private:

    std::vector<ChannelPtr> ReadyEvents(int num);

private:
    static const int kEpollSize;
    int epFd_;
    std::vector<struct epoll_event> readyEvents_;
    // every thread -> one
    std::unordered_map<int, ChannelPtr> channelMap_;
    Heap heap_;
};


#endif //WEBSERVER_EPOLLER_H

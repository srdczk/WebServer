//
// Created by Admin on 2020/5/14.
//

#ifndef WEBSERVER_EPOLLER_H
#define WEBSERVER_EPOLLER_H

#include <vector>
#include <sys/epoll.h>


class Epoller {
public:
    typedef std::vector<struct epoll_event> ReadyEvents;

    Epoller();
    ~Epoller();

    int EpollCtl(int fd, int op, int flag);
    int EpollWait(ReadyEvents &events, int timeout = -1);
private:
    int epFd_;
};


#endif //WEBSERVER_EPOLLER_H

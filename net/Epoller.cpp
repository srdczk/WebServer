//
// Created by Admin on 2020/5/14.
//

#include "Epoller.h"
#include <cassert>
#include <unistd.h>
#include "../base/AsyncLog.h"



Epoller::Epoller() {
    assert((epFd_ = epoll_create(EPOLL_CLOEXEC)) != -1);
}

Epoller::~Epoller() {
    close(epFd_);
}

int Epoller::EpollCtl(int fd, int op, int flag) {
    // ET MOD + NOBLOCKING

    struct epoll_event event;
    event.data.fd = fd;
    event.events = flag;

    if (!epoll_ctl(epFd_, op, fd, &event)) {
        LOG_DEBUG("epoll op: fd = %d, op = %d", fd, op);
        return 0;
    }

    switch (op) {
        case EPOLL_CTL_MOD:
            if (errno == ENOENT) {
                if (!epoll_ctl(epFd_,EPOLL_CTL_ADD, fd, &event)) {
                    LOG_DEBUG("epoll mod %d on %d retried as add, succeed", op, fd);
                    return 0;
                } else {
                    LOG_ERROR("epoll mod %d on %d retried as add, failed", op, fd);
                    return -1;
                }
            }
            break;
        case EPOLL_CTL_ADD:
            if (errno == EEXIST) {
                if (!epoll_ctl(epFd_, EPOLL_CTL_MOD, fd, &event)) {
                    LOG_DEBUG("epoll add %d on %d retried as mod, succeed", op, fd);
                    return 0;
                } else {
                    LOG_ERROR("epoll add %d on %d retried as mod, failed", op, fd);
                    return -1;
                }
            }
            break;
        case EPOLL_CTL_DEL:
            if (errno == ENOENT || errno == EBADF || errno == EPERM) {
                std::string strErr = errno == ENOENT ? "ENOENT" : (errno == EBADF ? "EBADF" : "EPERM");
                LOG_DEBUG("epoll del %d on %d: del failed, errno %s",  op, fd, strErr.data());
                return 0;
            }
            break;
        default:
            break;
    }

    LOG_ERROR("epoll ctl %d on %d failed!", op, fd);
    return -1;

}

int Epoller::EpollWait(Epoller::ReadyEvents &events, int timeout) {
    int res = epoll_wait(epFd_, &events[0], events.capacity(), timeout);
    if (res == -1) {
        if (errno != EINTR) {
            LOG_ERROR("epoll wait failed!");
            return -1;
        }
        return 0;
    }
    // resize
    if (res == events.capacity()) {
        events.resize(2 * events.capacity());
        LOG_DEBUG("events vector resize");
    }
    return res;
}


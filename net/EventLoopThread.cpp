//
// Created by srdczk on 2020/5/26.
//

#include "EventLoopThread.h"
#include "../base/AsyncLog.h"
#include <unistd.h>
#include <fcntl.h>

EventLoopThread::EventLoopThread() {
    // init pipe
    pipe2(pipe_, O_CLOEXEC | O_NONBLOCK);
    // listenFd
    listenChannel_ = std::make_shared<Channel>(pipe_[0]);
    fds_.reserve(32);

}

void EventLoopThread::ReadHandler(EventLoop &eventLoop, std::shared_ptr<Channel> channel) {
    char buff[1024];

    std::lock_guard<std::mutex> guard(mutex_);
    read(pipe_[0], buff, sizeof(buff));
    for (auto &fd : fds_) {
        auto ptr = std::make_shared<Channel>(fd);
        ptr->SetReadCallback(readCb_);
        ptr->SetWriteCallback(writeCb_);
        ptr->SetErrorCallback(errorCb_);
        loop_.AddChannel(ptr, EPOLLIN);
        loop_.AddLastActiveTime(ptr, loop_.LastActiveTime());
        LOG_DEBUG("Add listen threadId=%d, fd=%d", threadId_, fd);
    }
    fds_.clear();
}

EventLoopThread::~EventLoopThread() {
    Quit();
    close(pipe_[1]);
    close(pipe_[0]);
}

void EventLoopThread::Loop() {
    using namespace std::placeholders;
    listenChannel_->SetReadCallback(std::bind(&EventLoopThread::ReadHandler, this, _1, _2));
    loop_.AddChannel(listenChannel_, EPOLLIN);
    loop_.Loop();
}

void EventLoopThread::Quit() { loop_.Quit(); }

void EventLoopThread::Push(int fd) {
    std::lock_guard<std::mutex> guard(mutex_);
    fds_.push_back(fd);
    // notify ReadHandler
    write(pipe_[1], " ", 1);
}



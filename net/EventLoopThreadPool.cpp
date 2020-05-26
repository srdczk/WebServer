//
// Created by srdczk on 2020/5/26.
//

#include "EventLoopThreadPool.h"
#include "../base/AsyncLog.h"

void EventLoopThreadPool::Push(int fd) {
    index_ %= threadNum_;
    eventThreads_[index_]->Push(fd);
    LOG_DEBUG("push fd %d to index %d", fd, index_);
    ++index_;
}

void EventLoopThreadPool::Quit() {
    for (int i = 0; i < threadNum_; ++i) {
        if (threads_[i]->joinable())
            threads_[i]->join();
        eventThreads_[i]->Quit();
    }

}

void EventLoopThreadPool::ThreadFunc(int threadId) {
    EventLoopThread eventLoopThread;
    eventThreads_[threadId] = &eventLoopThread;
    // Set Callback
    eventThreads_[threadId]->SetReadCallback(readCb_);
    eventThreads_[threadId]->SetWriteCallback(writeCb_);
    eventThreads_[threadId]->SetErrorCallback(errorCb_);
    eventThreads_[threadId]->SetThreadId(threadId);
    LOG_DEBUG("running thread id: %d", threadId);
    eventLoopThread.Loop();
}

void EventLoopThreadPool::Loop() {
    threads_.reserve(threadNum_);
    eventThreads_.resize(threadNum_);
    for (int i = 0; i < threadNum_; ++i) {
        threads_.emplace_back(std::make_shared<std::thread>(std::bind(&EventLoopThreadPool::ThreadFunc, this, i)));
    }
}

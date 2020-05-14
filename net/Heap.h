//
// Created by Admin on 2020/5/14.
//

#ifndef WEBSERVER_HEAP_H
#define WEBSERVER_HEAP_H

class Channel;

#include <memory>
#include <sys/time.h>

static bool TimevalGreater(const timeval &left, const timeval &right) {
    return left.tv_sec != right.tv_sec ? (left.tv_usec > right.tv_usec) : left.tv_sec > right.tv_sec;
}

// min heap to del inactive connection

class Heap {
public:
    typedef std::shared_ptr<Channel> ChannelPtr;
    Heap();
    ~Heap();

    void Push(ChannelPtr ptr, const timeval &time);
    ChannelPtr Top();
    void Change(ChannelPtr ptr, const timeval &time);
    void Delete(ChannelPtr ptr);
    bool Empty();

private:
    void Heapify(int index);

private:

    ChannelPtr *data_;
    int size_;
    int capacity_;

};


#endif //WEBSERVER_HEAP_H

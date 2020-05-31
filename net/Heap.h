#ifndef WEBSERVER_HEAP_H
#define WEBSERVER_HEAP_H


#include <memory>

class Channel;

// not active connection to delete
// handle Expired Client

class Heap {
public:
    typedef std::shared_ptr<Channel> ChannelPtr;

    Heap();
    ~Heap();

    void Insert(ChannelPtr channel, uint64_t timeout);
    ChannelPtr Top();
    void Change(ChannelPtr channel, uint64_t timeout);
    void Delete(ChannelPtr channel);
    void Pop();
    size_t Size() { return size_; }
private:
    // min heap heapify
    // active
    void Heapify(int index);
private:
    static const size_t kInitHeapSize;
    ChannelPtr *ptrs_;
    size_t size_;
    size_t capacity_;
};


#endif //WEBSERVER_HEAP_H

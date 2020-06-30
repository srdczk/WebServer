#ifndef WEBSERVER_HEAP_H
#define WEBSERVER_HEAP_H

#include <vector>
#include <cstdint>

class Channel;

class Heap {
public:
    // default timeout
    void Insert(Channel *channel, uint64_t timeout);
    void Change(Channel *channel, uint64_t timeout);
    void Delete(Channel *channel);

    Channel *Top();

    void Pop();

private:
    void Heapify(int index);
private:
    std::vector<Channel *> channels_;
};


#endif //WEBSERVER_HEAP_H

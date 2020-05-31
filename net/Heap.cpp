#include "Heap.h"
#include "Channel.h"
#include "NetHelper.h"
#include <cstdlib>

const size_t Heap::kInitHeapSize = 8;

Heap::Heap() {
    ptrs_ = (ChannelPtr *)(malloc(sizeof(void *) * kInitHeapSize));
    size_ = 0;
    capacity_ = 4;
}

Heap::~Heap() {
    if (ptrs_)
        free(ptrs_);
    ptrs_ = nullptr;
}



void Heap::Heapify(int index) {
    int left = 2 * index + 1;
    while (left < size_) {
        int smallest = left + 1 < size_ \
        && ptrs_[left + 1]->ExpiredTime() < ptrs_[left]->ExpiredTime() \
        ? left + 1 : left;
        smallest = ptrs_[index]->ExpiredTime() < ptrs_[smallest]->ExpiredTime() \
        ? index : smallest;
        if (smallest == index) break;
        std::swap(ptrs_[index], ptrs_[smallest]);
        ptrs_[index]->SetIndex(index);
        ptrs_[smallest]->SetIndex(smallest);
        index = smallest;
        left = 2 * index + 1;
    }
}

void Heap::Insert(Heap::ChannelPtr channel, uint64_t timeout) {
    if (size_++ == capacity_) {
        capacity_ *= 2;
        ptrs_ = (ChannelPtr *)(realloc(ptrs_, (capacity_ * sizeof(void *))));
    }
    int index = size_ - 1;
    ptrs_[index] = channel;
    ptrs_[index]->SetIndex(index);
    ptrs_[index]->SetExpiredTime(NetHelper::GetExpiredTime(timeout));

    while (ptrs_[(index - 1) / 2]->ExpiredTime() > ptrs_[index]->ExpiredTime()) {
        std::swap(ptrs_[(index - 1) / 2], ptrs_[index]);
        ptrs_[(index - 1) / 2]->SetIndex((index - 1) / 2);
        ptrs_[index]->SetIndex(index);
        index = (index - 1) / 2;
    }
}


Heap::ChannelPtr Heap::Top() {
    if (!size_) return nullptr;
    return ptrs_[0];
}

void Heap::Change(Heap::ChannelPtr channel, uint64_t timeout) {
    channel->SetExpiredTime(NetHelper::GetExpiredTime(timeout));
    Heapify(channel->Index());
}

void Heap::Delete(Heap::ChannelPtr channel) {
    auto index = channel->Index();
    std::swap(ptrs_[index], ptrs_[--size_]);

    ptrs_[size_] = nullptr;
    ptrs_[index]->SetIndex(-1);
    Heapify(index);
    // channel -> close
}

void Heap::Pop() { Delete(Top()); }







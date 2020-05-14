//
// Created by Admin on 2020/5/14.
//

#include "Heap.h"
#include "Channel.h"

Heap::Heap(): data_(nullptr), size_(0), capacity_(0) {}

Heap::~Heap() {
    if (data_)
        free(data_);
    data_ = nullptr;
}

void Heap::Push(Heap::ChannelPtr ptr, const timeval &time) {
    // Set Min Heap Index
    if (size_++ == capacity_) {
        capacity_ *= 2;
        data_ = static_cast<ChannelPtr *>(realloc(data_, (++capacity_ * sizeof(void  *))));
    }
    // heapify
    int i = size_ - 1;
    data_[i] = ptr;
    data_[i]->SetHeapIndex(i);
    data_[i]->SetLastActive(time);
    while (TimevalGreater(data_[(i - 1) / 2]->LastActive(), data_[i]->LastActive())) {
        std::swap(data_[i], data_[(i - 1) / 2]);
        data_[i]->SetHeapIndex(i);
        data_[(i - 1) / 2]->SetHeapIndex((i - 1) / 2);
    }
}

Heap::ChannelPtr Heap::Top() {
    if (!data_) return nullptr;
    return data_[0];
}

void Heap::Change(Heap::ChannelPtr ptr, const timeval &time) {
    ptr->SetLastActive(time);
    Heapify(ptr->HeapIndex());
}

void Heap::Heapify(int index) {
    int left = 2 * index + 1;
    while (left < size_) {
        int smallest = left + 1 < size_ && TimevalGreater(data_[left]->LastActive(), data_[left + 1]->LastActive()) ? left + 1 : left;
        smallest = TimevalGreater(data_[smallest]->LastActive(), data_[index]->LastActive()) ? index : smallest;
        if (smallest == index)
            break;
        std::swap(data_[index], data_[smallest]);
        data_[index]->SetHeapIndex(index);
        data_[smallest]->SetHeapIndex(smallest);
        index = smallest;
        left = 2 * index + 1;
    }
}

void Heap::Delete(Heap::ChannelPtr ptr) {
    auto index = ptr->HeapIndex();
    // del from ptr
    std::swap(data_[index], data_[--size_]);
    data_[size_]->SetHeapIndex(-1);
    Heapify(index);
}

bool Heap::Empty() {
    return !size_;
}







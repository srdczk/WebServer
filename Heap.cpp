#include "Heap.h"
#include "Channel.h"
#include "NetHelper.h"

void Heap::Insert(Channel *channel, uint64_t timeout) {
    // add to Heap
    channel->SetExpiredTime(NetHelper::GetExpiredTime(timeout));
    channels_.push_back(channel);
    channel->SetIndex(channels_.size() - 1);
    int index = channels_.size() - 1;
    while (channels_[(index - 1) / 2]->ExpiredTime() > channels_[index]->ExpiredTime()) {
        std::swap(channels_[(index - 1) / 2], channels_[index]);
        channels_[index]->SetIndex(index);
        channels_[(index - 1) / 2]->SetIndex((index - 1) / 2);
        index = (index - 1) / 2;
    }
}

void Heap::Change(Channel *channel, uint64_t timeout) {
    auto index = channel->Index();
    assert(index != -1);
    channel->SetExpiredTime(NetHelper::GetExpiredTime(timeout));
    Heapify(index);
}

void Heap::Delete(Channel *channel) {
    auto index = channel->Index();
    assert(index != -1);
    // channel's pointer delete
    // Delete (Epoller should delete)
    std::swap(channels_[channels_.size() - 1], channels_[index]);
    // set channel's
    channels_[index]->SetIndex(index);
    channel->SetIndex(-1);
    channels_.pop_back();
    Heapify(index);
}

void Heap::Heapify(int index) {
    int left = 2 * index + 1;
    while (left < channels_.size()) {
        int smallest = left + 1 < channels_.size() && \
        channels_[left + 1]->ExpiredTime() < channels_[left]->ExpiredTime() ?\
        left + 1 : left;
        smallest = channels_[smallest]->ExpiredTime() < channels_[index]->ExpiredTime() ?\
        smallest : index;
        if (smallest == index) break;
        std::swap(channels_[index], channels_[smallest]);
        channels_[smallest]->SetIndex(smallest);
        channels_[index]->SetIndex(index);
        index = smallest;
        left = 2 * index + 1;
    }
}

Channel *Heap::Top() {
    if (channels_.empty()) return nullptr;
    Channel *res = channels_[0];
    return channels_[0];
}

void Heap::Pop() {
    if (channels_.empty()) return;
    Delete(channels_[0]);
}

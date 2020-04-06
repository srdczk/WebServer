//
// Created by srdczk on 2020/4/5.
//
#include "Buffer.h"

Buffer::Buffer(uint32_t initSize):buffer_(initSize + PREPARE), readIndex_(PREPARE), writeIndex_(PREPARE) {}

void Buffer::Swap(Buffer &buffer) {
    buffer_.swap(buffer.buffer_);
    std::swap(readIndex_, buffer.readIndex_);
    std::swap(writeIndex_, buffer.writeIndex_);
}

uint32_t Buffer::ReadableBytes() const { return writeIndex_ - readIndex_; }

uint32_t Buffer::WritableBytes() const { return static_cast<uint32_t>(buffer_.size() - writeIndex_); }

uint32_t Buffer::PreparableBytes() const { return readIndex_; }

const char *Buffer::Peek() const { return Begin() + readIndex_; }



char *Buffer::Begin() { return &*buffer_.begin(); }

const char *Buffer::Begin() const { return &*buffer_.begin(); }










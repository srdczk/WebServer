//
// Created by srdczk on 2020/4/5.
//
#include "Buffer.h"
#include <errno.h>
#include <sys/uio.h>
#include <algorithm>
#include <cstring>
#include <cassert>

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

const char *Buffer::BeginWrite() const { return Begin() + writeIndex_; }

const char *Buffer::FindCRLF(const char *start) const {
    auto res = std::search(start, BeginWrite(), CRLF, CRLF + 2);
    return res == BeginWrite() ? nullptr : res;
}

const char *Buffer::FindEOL() const {
    auto res = memchr(Peek(), '\n', ReadableBytes());
    return static_cast<const char*>(res);
}

void Buffer::Retrive(uint32_t len) {
    assert(len <= ReadableBytes());
    if (len < ReadableBytes()) readIndex_ += len;
    else RetriveAll();
}

void Buffer::RetriveUntil(const char *end) {
    Retrive(static_cast<uint32_t>(end - Peek()));
}

void Buffer::RetriveAll() {
    readIndex_ = PREPARE;
    writeIndex_ = PREPARE;
}

std::string Buffer::RetriveAsString(uint32_t len) {
    assert(len <= ReadableBytes());
    std::string res(Peek(), len);
    Retrive(len);
    return res;
}

std::string Buffer::RetriveAllAsString() { return RetriveAsString(ReadableBytes()); }

std::string Buffer::ToString() const { return std::string(Peek(), ReadableBytes()); }

void Buffer::Append(const char *data, uint32_t len) {
    EnsureWritableBytes(len);
    std::copy(data, data + len, Begin() + writeIndex_);
    writeIndex_ += len;
}

void Buffer::Append(const std::string &str) {
    Append(str.data(), static_cast<uint32_t>(str.length()));
}

void Buffer::MakeSpace(uint32_t len) {
    // 如果 不够大小, vector进行扩容
    if (WritableBytes() + PreparableBytes() < len + PREPARE) {
        buffer_.resize(writeIndex_ + len);
    } else {
        // 否则移动到 preparen
        auto read = ReadableBytes();
        std::copy(Begin() + readIndex_, Begin() + writeIndex_, Begin() + PREPARE);
        readIndex_ = PREPARE;
        writeIndex_ = readIndex_ + read;
    }
}

bool Buffer::EnsureWritableBytes(uint32_t len) {
    if (WritableBytes() < len) MakeSpace(len);
    return true;
}

bool Buffer::Unwrite(uint32_t len) {
    if (len > ReadableBytes()) return false;
    writeIndex_ -= len;
    return true;
}

void Buffer::Shrink(uint32_t size) {
    Buffer other;
    other.EnsureWritableBytes(ReadableBytes() + size);
    other.Append(ToString());
    Swap(other);
}

int32_t Buffer::ReadFd(int fd, int *err) {
    // linux 平台下 iovec 非阻塞 IO
    char buff[65536];
    auto write = WritableBytes();
    struct iovec vec[2];

    vec[0].iov_base = Begin() + writeIndex_;
    vec[0].iov_len = write;
    vec[1].iov_base = buff;
    vec[1].iov_len = sizeof(buff);

    auto cnt = (write < sizeof(buff)) ? 2 : 1;
    auto n = readv(fd, vec, cnt);

    if (n <= 0) *err = errno;
    else if ((n) <= write) writeIndex_ += n;
    else {
        writeIndex_ = static_cast<uint32_t>(buffer_.size());
        Append(buff, static_cast<uint32_t>(n - write));
    }
    return static_cast<int32_t>(n);
}

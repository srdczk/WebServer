//
// Created by srdczk on 2020/4/5.
//
#pragma once

#include <cstdint>
#include <vector>

#define PREPARE 8
#define INITIAL_SIZE 1024
// Buffer 类, 不可复制

class Buffer {
public:
    Buffer(uint32_t initSize = INITIAL_SIZE);
    // 不可复制类
    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;
    void Swap(Buffer &buffer);
    uint32_t ReadableBytes() const;
    uint32_t WritableBytes() const;
    uint32_t PreparableBytes() const;
    const char *Peek() const;
private:
    char *Begin();
    const char *Begin() const;
private:
    std::vector<char> buffer_;
    uint32_t readIndex_;
    uint32_t writeIndex_;

};
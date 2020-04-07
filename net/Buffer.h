//
// Created by srdczk on 2020/4/5.
//
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define PREPARE 8
#define INITIAL_SIZE 1024
// Buffer 类, 不可复制
static const char *CRLF = "\r\n";

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
    const char *BeginWrite() const;
    const char *FindCRLF(const char *) const;
    const char *FindEOL() const;
    void Retrive(uint32_t len);
    void RetriveUntil(const char *end);
    void RetriveAll();
    std::string RetriveAsString(uint32_t len);
    std::string RetriveAllAsString();
    std::string ToString() const;
    void Append(const char *data, uint32_t len);
    void Append(const std::string &str);
    bool EnsureWritableBytes(uint32_t len);
    bool Unwrite(uint32_t len);
    void Shrink(uint32_t size);
    int32_t ReadFd(int fd, int *err);
private:
    char *Begin();
    const char *Begin() const;
    void MakeSpace(uint32_t len);
private:
    std::vector<char> buffer_;
    uint32_t readIndex_;
    uint32_t writeIndex_;
};
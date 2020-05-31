//
// Created by Admin on 2020/5/14.
//

#ifndef WEBSERVER_NETHELPER_H
#define WEBSERVER_NETHELPER_H

#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <cstdint>

//util class
class NetHelper {
public:
    NetHelper() = delete;
    ~NetHelper() = delete;
    NetHelper(const NetHelper &) = delete;
    NetHelper &operator=(const NetHelper &) = delete;
    static int ReadN(int fd, void *buff, uint32_t n);
    static int ReadN(int fd, std::string &buffer);
    static int ReadN(int fd, std::string &buffer, bool &zero);
    static int WriteN(int fd, void *buff, size_t n);
    static int WriteN(int fd, std::string &buff);

    static void IgnorePipe();
    static void SetNonBlocking(int fd);
    static void SetNodelay(int fd);
    static void ShutdownWR(int fd);
    static int BindAndListen(int port);
    static uint64_t GetExpiredTime(uint64_t timeout);

private:
    static const size_t kMaxBuffer;
};


#endif //WEBSERVER_NETHELPER_H

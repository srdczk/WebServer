//
// Created by Admin on 2020/5/14.
//

#ifndef WEBSERVER_NETHELPER_H
#define WEBSERVER_NETHELPER_H

#include <unistd.h>
#include <fcntl.h>

//util class
class NetHelper {
public:
    static void SetNonBlocking(int fd);
private:
    NetHelper() = delete;
    ~NetHelper() = delete;
    NetHelper(const NetHelper &) = delete;
    NetHelper &operator=(const NetHelper &) = delete;
};


#endif //WEBSERVER_NETHELPER_H

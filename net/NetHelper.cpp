//
// Created by Admin on 2020/5/14.
//

#include "NetHelper.h"

void NetHelper::SetNonBlocking(int fd) {
    int flag = fcntl(fd, F_GETFL, nullptr);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

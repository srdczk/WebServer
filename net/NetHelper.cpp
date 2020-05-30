//
// Created by Admin on 2020/5/14.
//

#include "NetHelper.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>

const size_t NetHelper::kMaxBuffer = 4096;

void NetHelper::SetNonBlocking(int fd) {
    int flag = fcntl(fd, F_GETFL, nullptr);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

void NetHelper::SetNodelay(int fd) {
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
}

void NetHelper::ShutdownWR(int fd) {
    shutdown(fd, SHUT_WR);
}

int NetHelper::BindAndListen(int port) {
    // port short
    if (port < 0 || port > 65535) return -1;

    int sfd;
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;
    // set socket fd reuse
    int opt = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        close(sfd);
        return -1;
    }

    struct sockaddr_in saddr;
    memset(&saddr, '\0', sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    // bind and listen
    if (bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
        close(sfd);
        return -1;
    }

    if (listen(sfd, 2048) == -1) {
        close(sfd);
        return -1;
    }

    if (sfd == -1) {
        close(sfd);
        return -1;
    }
    return sfd;
}

void NetHelper::IgnorePipe() {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigaction(SIGPIPE, &sa, nullptr);
}

int NetHelper::ReadN(int fd, void *buff, uint32_t n) {
    size_t left = n;
    ssize_t hRead = 0;
    ssize_t readSum = 0;
    char *restore = static_cast<char *>(buff);
    while (left > 0) {
        if ((hRead = read(fd, restore, left)) < 0) {
            if (errno == EAGAIN) {
                return static_cast<int>(readSum);
            } else if (errno == EINTR) {
                hRead = 0;
            } else {
                return -1;
            }
        } else if (!hRead)
            break;
        readSum += hRead;
        left -= hRead;
        restore += hRead;
    }
    return static_cast<int>(readSum);
}

int NetHelper::ReadN(int fd, std::string &buffer) {
    ssize_t hRead = 0;
    ssize_t readSum = 0;
    while (true) {
        char buff[kMaxBuffer];
        if ((hRead = read(fd, buff, kMaxBuffer)) == -1) {
            if (errno == EAGAIN) {
                return static_cast<int>(readSum);
            } else if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        } else if (!hRead)
            break;
        readSum += hRead;
        buffer += std::string(buff, buff + hRead);
    }
    return static_cast<int>(readSum);
}

int NetHelper::ReadN(int fd, std::string &buffer, bool &zero) {
    ssize_t hRead = 0;
    ssize_t readSum = 0;
    while (true) {
        char buff[kMaxBuffer];
        if ((hRead = read(fd, buff, kMaxBuffer)) == -1) {
            if (errno == EAGAIN) {
                return static_cast<int>(readSum);
            } else if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        } else if (!hRead) {
            zero = true;
            break;
        }
        readSum += hRead;
        buffer += std::string(buff, buff + hRead);
    }
    return static_cast<int>(readSum);
}

int NetHelper::WriteN(int fd, void *buff, size_t n) {
    ssize_t writeSum = 0;
    ssize_t hWrite = 0;
    size_t left = n;
    char *output = static_cast<char *>(buff);
    while (left > 0) {
        if ((hWrite = write(fd, output, left)) < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                return static_cast<int>(writeSum);
            } else {
                char *p = strerror(errno);
                return -1;
            }
        }
        writeSum += hWrite;
        left -= hWrite;
        output += hWrite;
    }
    return static_cast<int>(writeSum);
}

int NetHelper::WriteN(int fd, std::string &buff) {
    ssize_t writeSum = 0;
    ssize_t hWrite = 0;
    size_t left = buff.length();
    char *output = buff.data();
    while (left > 0) {
        if ((hWrite = write(fd, output, left)) < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                break;
            } else {
                return -1;
            }
        }
        writeSum += hWrite;
        left -= hWrite;
        output += hWrite;
    }
    if (writeSum == static_cast<ssize_t>(buff.length()))
        buff.clear();
    else
        buff = buff.substr(static_cast<unsigned long>(writeSum));
    return static_cast<int>(writeSum);
}





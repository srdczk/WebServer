
#include "NetHelper.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h>

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
                char *p = strerror(errno);
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
                std::string p(strerror(errno));
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

uint64_t NetHelper::GetExpiredTime(uint64_t timeout) {
    struct timeval now;
    gettimeofday(&now, nullptr);

    // return ms

    return (now.tv_sec) * 1000 + now.tv_usec / 1000 + timeout;
}

std::string NetHelper::FileType(const char *name) {
    auto dot = strrchr(name, '.');
    if (!dot)
        return "text/plain; charset=utf-8";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp( dot, ".wav" ) == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";

    return "text/plain; charset=utf-8";
}

int NetHelper::GetLine(int cfd, char *buff, int size) {
    int sum = 0;
    char c = '\0';
    // last buff must be '\0'
    while (sum < size - 1 && c != '\n') {
        ssize_t len = recv(cfd, &c, 1, 0);
        if (len > 0) {
            if (c == '\r') {
                // peek a char ->
                len = recv(cfd, &c, 1, MSG_PEEK);
                if (len > 0 && c == '\n') {
                    recv(cfd, &c, 1, 0);
                } else {
                    c = '\n';
                }
            }
            buff[sum++] = c;
        } else {
            c = '\n';
        }
    }
    buff[sum] = '\0';
    return sum;
}

void NetHelper::HttpRequest(char *line, int cfd) {
    char method[12], path[1024], protocol[12];
    // 正则表达式
    sscanf(line, "%[^ ] %[^ ] %[^ ]", method, path, protocol);
    char *file = path + 1;
    // 如果没有输入路径
    if (!strcmp(path, "/")) {
        file = "./";
    }
    struct stat st;
    int ret = stat(file, &st);
    // 判断文件的属性
    // 如果, 不存在该文件
    if (ret == -1) {
        // 发送 404 的页面
        SendResponseHeader(cfd, 404, "File Not Found", ".html", -1);
        SendFile(cfd, "404.html");
    }
    // 如果是文件夹
    if (S_ISDIR(st.st_mode)) {
        // 不要Content-Length
        SendResponseHeader(cfd, 200, "OK", FileType(".html").data(), -1);
        SendDir(cfd, file);
    } else if (S_ISREG(st.st_mode)) {
        // 如果是文件
        // 查找文件的信息
        SendResponseHeader(cfd, 200, "OK", FileType(file).data(), st.st_size);
        SendFile(cfd, file);
    }
}

void NetHelper::SendResponseHeader(int cfd, int no, const char *desp, const char *type, long len) {

    char buff[1024];
    memset(buff, '\0', sizeof(buff));
    sprintf(buff, "HTTP/1.1 %d %s\r\n", no, desp);
    send(cfd, buff, strlen(buff), 0);
    sprintf(buff, "Content-Type:%s\r\n", type);
    sprintf(buff + strlen(buff), "Content-Length:%ld\r\n", len);
    send(cfd, buff, strlen(buff), 0);
    // 协议规定 --> 一定要有空行
    send(cfd, "\r\n", 2, 0);
}

void NetHelper::SendDir(int cfd, const char *name) {

    char buff[1024];
    memset(buff, '\0', sizeof(buff));
    sprintf(buff, "<html><head><title>DIR: %s</title></head>", name);
    sprintf(buff + strlen(buff), "<body><h1>Current Dir: %s</h1><table>", name);
    char path[1024];
    memset(path, '\0', sizeof(path));
    struct dirent** ptr;
    int num = scandir(name, &ptr, nullptr, alphasort);
    for (int i = 0; i < num; ++i) {
        char *cname = ptr[i]->d_name;
        if (strlen(name) > 0 && name[strlen(name) - 1] == '/') sprintf(path, "%s%s", name, cname);
        else sprintf(path, "%s/%s", name, cname);

        sprintf(buff + strlen(buff),
                "<tr><td><a href=\"%s\">%s</a></td></tr>",
                path, cname);
        send(cfd, buff, strlen(buff), 0);
        memset(buff, '\0', sizeof(buff));
    }
    sprintf(buff + strlen(buff), "</table></body></html>");
    send(cfd, buff, strlen(buff), 0);

}

void NetHelper::SendFile(int cfd, const char *name) {
    char buff[kMaxBuffer];
    int fd = open(name, O_RDONLY);
    ssize_t len;
    // 循环读取文件
    while ((len = read(fd, buff, kMaxBuffer)) > 0) {
        send(cfd, buff, len, 0);
    }
    close(fd);
}






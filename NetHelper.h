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
    static std::string FileType(const char *name);
    static int GetLine(int cfd, char *buff, int size);
    static void HttpRequest(char *line, int cfd);
    static void SendResponseHeader(int cfd, int no, const char *desp, const char *type, long len);
    static void SendDir(int cfd, const char *name);
    static void SendFile(int cfd, const char *name);
private:
    static const size_t kMaxBuffer;
};



#endif //WEBSERVER_NETHELPER_H

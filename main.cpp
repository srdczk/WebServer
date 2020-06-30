#include <iostream>
#include "TcpServer.h"



int main() {
    EventLoop loop;
    TcpServer server(&loop, 2, 9009);
    server.Start();
    loop.Loop();
    return 0;
}
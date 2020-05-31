//
// Created by srdczk on 2020/4/5.
//
#include "base/AsyncLog.h"
#include "net/TcpServer.h"
#include <functional>
#include <iostream>
void func(int a, int b, int c) {
    std::cout << a << " " << b << " " << c << "\n";
}

void Heapify(std::vector<int> &a, int index, int size) {
    int left = 2 * index + 1;
    while (left < size) {
        int smallest = left + 1 < size && a[left + 1] < a[left] ? left + 1 : left;
        smallest = a[index] < a[smallest] ? index : smallest;
        if (index == smallest)
            break;
        std::swap(a[index], a[smallest]);
        index = smallest;
        left = 2 * index + 1;
    }
}

EventLoop *g_loop;

int main() {
    CAsyncLog::Init("TCPSERVERDUGOUZI");
    EventLoop loop;
    TcpServer server(&loop, 2, 9090);
    server.Start();
    loop.Loop();
    CAsyncLog::Uninit();
    return 0;
}

//
// Created by srdczk on 2020/4/4.
//
// 异步日志类, 不能创建实例, 所提供方法全部为 static
#pragma once

#include <cstdio>
#include <string>
#include <list>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

// 日志级别
enum LOG_LEVEL {
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,    //用于业务错误
    LOG_LEVEL_SYSERROR, //同步输出错误日志，用于程序退出前输出
    LOG_LEVEL_FATAL,    //FATAL 级别的日志会让在程序输出日志后退出
    LOG_LEVEL_CRITICAL  //CRITICAL 日志不受日志级别控制，总是输出
};

#define LOG_API

class LOG_API CAsyncLog {
public:
    static bool Init(const char* logFileName = nullptr, bool truncateLongLog = false, uint64_t maxFileSize = 10 * 1024 * 1024);
    static void Uninit();

    static void SetLevel(LOG_LEVEL level);
    static bool IsRunning();

    // 不输出线程ID号 和函数, 行号
    static bool Output(long level, const char* fmt, ...);
    // 输出线程ID 号和所在的函数签名
    static bool Output(long level, const char* fileName, int line, const char* fmt, ...);
private:
    // 不能创建
    CAsyncLog() = delete;
    ~CAsyncLog() = delete;
    // 不可拷贝
    CAsyncLog(const CAsyncLog &) = delete;
    CAsyncLog &operator=(const CAsyncLog &) = delete;

    static void GetPrefix(long level, std::string &prefix);
    static void GetTime(char *curTime, uint32_t length);
    static bool CreateNewFile(const char* logFileName);
    static bool WriteToFile(const std::string& data);
    static void Crash();
    static const char *HexString(int n);

    static void WriteThreadFunc();

private:
    // 日志写入的文件
    static FILE * logFile_;
    // 日志文件名
    static std::string fileName_;
    // 文件名中的进程id
    static std::string fileNamePID_;
    // 是否截断长日志
    static bool truncateLongLog_;
    static LOG_LEVEL logLevel_;
    // 单个文件的最大字节数
    static uint64_t maxFileSize_;
    // 已经写入的字节数
    static uint64_t writtenSize_;
    // 待写入的日志
    static std::list<std::string> linesToWrite_;
    // 写入线程
    static std::shared_ptr<std::thread> writeThread_;
    // 日志锁和条件变量
    static std::mutex writeMutex_;
    static std::condition_variable writeCv_;
    // 是否退出
    static bool exit_;
    // 是否正在运行
    static bool running_;
};

// 日志宏
#define LOG_TRACE(...)    CAsyncLog::Output(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)    CAsyncLog::Output(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)    CAsyncLog::Output(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...)    CAsyncLog::Output(LOG_LEVEL_WARNING, __FILE__, __LINE__,__VA_ARGS__)
#define LOG_ERROR(...)    CAsyncLog::Output(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_SYSE(...) CAsyncLog::Output(LOG_LEVEL_SYSERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)    CAsyncLog::Output(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)        //为了让FATAL级别的日志能立即crash程序，采取同步写日志的方法
#define LOG_CRITICAL(...)    CAsyncLog::Output(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, __VA_ARGS__)     //关键信息，无视日志级别，总是输出


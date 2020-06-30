//
// Created by srdczk on 2020/4/4.
//

#include "AsyncLog.h"
#include <unistd.h>
#include <sys/timeb.h>
#include <sys/syscall.h>
#include <cstring>
#include <cstdarg>

#define MAX_LINE_LENGTH 256
// 默认的file大小
#define DEAULT_ROLL_SIZE (10 * 1024 * 1024)


FILE * CAsyncLog::logFile_ = nullptr;
std::string CAsyncLog::fileName_ = "default";
std::string CAsyncLog::fileNamePID_ = "";
bool CAsyncLog::truncateLongLog_ = false;
LOG_LEVEL CAsyncLog::logLevel_ = LOG_LEVEL_TRACE;
uint64_t CAsyncLog::maxFileSize_ = DEAULT_ROLL_SIZE;
uint64_t CAsyncLog::writtenSize_ = 0;
std::list<std::string> CAsyncLog::linesToWrite_;
std::shared_ptr<std::thread> CAsyncLog::writeThread_;
std::mutex CAsyncLog::writeMutex_;
std::condition_variable CAsyncLog::writeCv_;
bool CAsyncLog::exit_ = false;
bool CAsyncLog::running_ = false;

bool CAsyncLog::Init(const char *logFileName, bool truncateLongLog, uint64_t maxFileSize) {
    // 设置 init 的参数
    truncateLongLog_ = truncateLongLog;
    maxFileSize_ = maxFileSize;
    if (!logFileName) fileName_.clear();
    else fileName_ = logFileName;
    // 获取进程 id
    char pid[8];
    memset(pid, '\0', 8);
    snprintf(pid, sizeof(pid), "%05d", (int)::getpid());
    fileNamePID_ = pid;

    writeThread_.reset(new std::thread(WriteThreadFunc));

    return true;
}

void CAsyncLog::Uninit() {
    exit_ = true;
    // 条件变量通知, 把还未写完的日志完成
    if (writeThread_ && writeThread_->joinable()) writeThread_->join();

    if (logFile_) fclose(logFile_);
    logFile_ = nullptr;
}

void CAsyncLog::SetLevel(LOG_LEVEL level) {
    if (level < LOG_LEVEL_TRACE || level > LOG_LEVEL_CRITICAL) return;
    logLevel_ = level;
}

bool CAsyncLog::IsRunning() { return running_; }

void CAsyncLog::GetPrefix(long level, std::string &prefix) {
    // 根据 level 获取前缀
    switch (level) {
        case LOG_LEVEL_TRACE:
            prefix = "[TRACE]";
            break;
        case LOG_LEVEL_DEBUG:
            prefix = "[DEBUG]";
            break;
        case LOG_LEVEL_WARNING:
            prefix = "[WARN]";
            break;
        case LOG_LEVEL_ERROR:
            prefix = "[ERROR]";
            break;
        case LOG_LEVEL_SYSERROR:
            prefix = "[SYSE]";
            break;
        case LOG_LEVEL_FATAL:
            prefix = "[FATAL]";
            break;
        case LOG_LEVEL_CRITICAL:
            prefix = "[CRITICAL]";
            break;
        default:
            prefix = "[INFO]";
            break;
    }
    // 获取当前时间
    char now[64];
    memset(now, '\0', sizeof(now));
    GetTime(now, sizeof(now));

    prefix += "[";
    prefix += now;
    prefix += "]";
    // 获取当前线程信息
    char threadID[32];
    memset(threadID, '\0', sizeof(threadID));

    auto tid = syscall(SYS_gettid);
    snprintf(threadID, sizeof(threadID), "[%d]", (int32_t)tid);

    prefix += threadID;
}

void CAsyncLog::GetTime(char *curTime, uint32_t length) {
    struct timeb tp;
    ftime(&tp);
    time_t now = tp.time;
    tm time;
    localtime_r(&now, &time);
    snprintf(curTime, length, "%04d-%02d-%02d %02d:%02d:%02d:%03d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, tp.millitm);
}

bool CAsyncLog::CreateNewFile(const char *logFileName) {
    if (logFile_) fclose(logFile_);

    // 新建文件
    logFile_ = fopen(logFileName, "w+");
//    printf("%s\n", logFileName);
    return logFile_;
}

bool CAsyncLog::WriteToFile(const std::string &data) {
    std::string str(data);
    int res = 0;
    while (true) {
        res = (int)fwrite(str.data(), 1, str.length(), logFile_);
        if (res <= 0) return false;
        else if (res <= str.length()) str.erase(0, (uint64_t)res);

        if (!str.length()) break;
    }

    fflush(logFile_);
    return true;
}

// 主动让程序崩溃
void CAsyncLog::Crash() {
    char *p = nullptr;
    *p = 0;
}

const char *CAsyncLog::HexString(int n) {
    static char res[64 + 1];
    memset(res, '\0', sizeof(res));
    sprintf(res, "%06u", n);
    return res;
}

void CAsyncLog::WriteThreadFunc() {
    running_ = true;

    while (true) {
        if (!fileName_.empty()) {
            if (!logFile_ || writtenSize_ >= maxFileSize_) {
                // 重置 written 大小
                writtenSize_ = 0;
                // 创建新文件
                char nowStr[64];
                memset(nowStr, '\0', sizeof(nowStr));
                time_t now = time(nullptr);
                tm time;
                localtime_r(&now, &time);
                // 格式化时间
                strftime(nowStr, sizeof(nowStr), "%Y%m%d%H%M%S", &time);

                std::string newFileName(fileName_);
                newFileName += ".";
                newFileName += nowStr;
                newFileName += ".";
                newFileName += fileNamePID_;
                newFileName += ".log";
                if (!CreateNewFile(newFileName.data())) return;
            }
        }

        // 需要写入的日志
        std::string line;
        {
            std::unique_lock<std::mutex> lock(writeMutex_);
            while (linesToWrite_.empty()) {
                if (exit_) return;
//                printf("There!\n");
                writeCv_.wait(lock);
            }

            line = linesToWrite_.front();
            linesToWrite_.pop_front();
        }

        printf("%s\n", line.data());

        if (!fileName_.empty()) {
            if (!WriteToFile(line)) return;
            writtenSize_ += line.length();
        }
    }

    running_ = false;

}

bool CAsyncLog::Output(long level, const char *fmt, ...) {
    if (level != LOG_LEVEL_CRITICAL) {
        if (level < logLevel_) return false;
    }
    // 日志头
    std::string head;
    GetPrefix(level, head);
    // 日志正文
    std::string msg;

    va_list ap;
    va_start(ap, fmt);
    auto logLen = vsnprintf(nullptr, 0, fmt, ap);
    va_end(ap);

    if ((int)msg.capacity() < logLen + 1) {
        msg.resize(logLen + 1);
    }
    va_list aq;
    va_start(aq, fmt);
    vsnprintf(msg.data(), msg.capacity(), fmt, aq);
    va_end(aq);

    std::string msgFormat;
    msgFormat.append(msg.data(), logLen);

    // 如果开启长日志截断
    if (truncateLongLog_) msgFormat = msgFormat.substr(0, MAX_LINE_LENGTH);

    head += msgFormat;

    if (!fileName_.empty()) head += "\n";

    if (level != LOG_LEVEL_FATAL && level != LOG_LEVEL_SYSERROR) {
        // 生产者
        std::lock_guard<std::mutex> guard(writeMutex_);
        linesToWrite_.push_back(head);
        writeCv_.notify_one();
    } else {
        printf("%s", head.data());
        // FATAL 立即Crash, 同步写日志
        if (!fileName_.empty()) {
            if (!logFile_) {
                char nowStr[64];
                memset(nowStr, '\0', sizeof(nowStr));
                time_t now = time(nullptr);
                tm time;
                localtime_r(&now, &time);
                strftime(nowStr, sizeof(nowStr), "%Y%m%d%H%M%S", &time);

                std::string newFileName(fileName_);
                newFileName += ".";
                newFileName += nowStr;
                newFileName += ".";
                newFileName += fileNamePID_;
                newFileName += ".log";
                if (!CreateNewFile(newFileName.data())) return false;
            }
        }
        WriteToFile(head);
    }
    // 程序主动崩溃
    if (level == LOG_LEVEL_FATAL) Crash();

    return true;
}

bool CAsyncLog::Output(long level, const char *fileName, int line, const char *fmt, ...) {

    if (level != LOG_LEVEL_CRITICAL) {
        if (level < logLevel_) return false;
    }

    std::string head;
    GetPrefix(level, head);

    // 函数名
    char funcName[512];
    memset(funcName, '\0', sizeof(funcName));
    snprintf(funcName, sizeof(funcName), "[%s:%d]", fileName, line);
    head += funcName;

    // 正文
    std::string msg;

    va_list ap;
    va_start(ap, fmt);
    auto logLen = vsnprintf(nullptr, 0, fmt, ap);
    va_end(ap);

    if ((int)msg.capacity() < logLen + 1) {
        msg.resize(logLen + 1);
    }
    va_list aq;
    va_start(aq, fmt);
    vsnprintf(msg.data(), msg.capacity(), fmt, aq);
    va_end(aq);

    std::string msgFormat;
    msgFormat.append(msg.data(), logLen);

    // 如果开启长日志截断
    if (truncateLongLog_) msgFormat = msgFormat.substr(0, MAX_LINE_LENGTH);

    head += msgFormat;

    if (!fileName_.empty()) head += "\n";

    if (level != LOG_LEVEL_FATAL && level != LOG_LEVEL_SYSERROR) {
        // 生产者
        std::lock_guard<std::mutex> guard(writeMutex_);
        linesToWrite_.push_back(head);
        writeCv_.notify_one();
    } else {
        printf("%s", head.data());
        // FATAL 立即Crash, 同步写日志
        if (!fileName_.empty()) {
            if (!logFile_) {
                char nowStr[64];
                memset(nowStr, '\0', sizeof(nowStr));
                time_t now = time(nullptr);
                tm time;
                localtime_r(&now, &time);
                strftime(nowStr, sizeof(nowStr), "%Y%m%d%H%M%S", &time);

                std::string newFileName(fileName_);
                newFileName += ".";
                newFileName += nowStr;
                newFileName += ".";
                newFileName += fileNamePID_;
                newFileName += ".log";
                if (!CreateNewFile(newFileName.data())) return false;
            }
        }
        WriteToFile(head);
    }
    // 程序主动崩溃
    if (level == LOG_LEVEL_FATAL) Crash();

    return true;
}














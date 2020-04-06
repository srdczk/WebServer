//
// Created by srdczk on 2020/4/5.
//

#include "Timestamp.h"
#include <cstring>
#include <algorithm>
#include <chrono>

int64_t Timestamp::perSeconds_ = 1000 * 1000;

Timestamp::Timestamp(): microSeconds_(0) {}

Timestamp::Timestamp(int64_t microSeconds): microSeconds_(microSeconds) {}

Timestamp &Timestamp::operator+=(const Timestamp &timestamp) {
    this->microSeconds_ += timestamp.microSeconds_;
    return *this;
}

Timestamp &Timestamp::operator+=(int64_t time) {
    this->microSeconds_ += time;
    return *this;
}

Timestamp &Timestamp::operator-=(const Timestamp &timestamp) {
    this->microSeconds_ -= timestamp.microSeconds_;
    return *this;
}

Timestamp &Timestamp::operator-=(int64_t time) {
    this->microSeconds_ -= time;
    return *this;
}

void Timestamp::Swap(Timestamp &timestamp) { std::swap(this->microSeconds_, timestamp.microSeconds_); }

std::string Timestamp::ToString() const {
    char res[64];
    memset(res, '\0', sizeof(res));
    auto seconds = microSeconds_ / perSeconds_;
    auto micro = microSeconds_ % perSeconds_;
    snprintf(res, sizeof(res) - 1, "%lld.%06lld", (long long)seconds, (long long) micro);
    return res;
}

std::string Timestamp::FormatString(bool showMicro) const {
    auto seconds = microSeconds_ / perSeconds_;

    struct tm tm_time;

    struct tm *ptm;
    ptm = localtime(&seconds);
    tm_time = *ptm;

    char res[32];
    memset(res, '\0', sizeof(res));

    if (showMicro) {
        auto micro = microSeconds_ % perSeconds_;
        snprintf(res, sizeof(res), "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 static_cast<int>(micro));
    } else {
        snprintf(res, sizeof(res), "%4d%02d%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }

    return res;
}

bool Timestamp::IsValid() const { return microSeconds_ > 0; }

int64_t Timestamp::MicroSeconds() const { return microSeconds_; }

time_t Timestamp::Seconds() const { return microSeconds_ / perSeconds_; }

Timestamp Timestamp::Now() {

    auto now = std::chrono::time_point_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now());

    auto microSeconds = now.time_since_epoch().count();
    return Timestamp(microSeconds);
}

Timestamp Timestamp::Invalid() { return Timestamp(); }

























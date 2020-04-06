//
// Created by srdczk on 2020/4/5.
//
// 时间戳类
#pragma once

#include <cstdint>
#include <string>

class Timestamp {
public:
    Timestamp();
    Timestamp(int64_t);
    Timestamp &operator+=(const Timestamp &);
    Timestamp &operator+=(int64_t);
    Timestamp &operator-=(const Timestamp &);
    Timestamp &operator-=(int64_t);
    void Swap(Timestamp &);
    std::string ToString() const;
    std::string FormatString(bool showMicro = true) const;
    bool IsValid() const;
    int64_t MicroSeconds() const;
    time_t Seconds() const;
    static Timestamp Now();
    static Timestamp Invalid();
    static int64_t perSeconds_;
private:
    int64_t microSeconds_;
};

inline bool operator<(const Timestamp &lhs, const Timestamp &rhs) { return lhs.MicroSeconds() < rhs.MicroSeconds(); }

inline bool operator>(const Timestamp &lhs, const Timestamp &rhs) { return lhs.MicroSeconds() > rhs.MicroSeconds(); }

inline bool operator<=(const Timestamp &lhs, const Timestamp &rhs) { return !(lhs > rhs); }

inline bool operator>=(const Timestamp &lhs, const Timestamp &rhs) { return !(lhs < rhs); }

inline bool operator==(const Timestamp &lhs, const Timestamp &rhs) { return lhs.MicroSeconds() == rhs.MicroSeconds(); }

inline bool operator!=(const Timestamp &lhs, const Timestamp &rhs) { return !(lhs == rhs); }

inline double TimeDiff(const Timestamp &high, const Timestamp &low) {
    auto diff = high.MicroSeconds() - low.MicroSeconds();
    return (double)diff / Timestamp::perSeconds_;
}

inline Timestamp AddTime(const Timestamp &timestamp, int64_t micro) { return Timestamp(timestamp.MicroSeconds() + micro); }


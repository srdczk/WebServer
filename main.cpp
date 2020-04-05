//
// Created by srdczk on 2020/4/5.
//
#include "base/AsyncLog.h"

int main() {
    CAsyncLog::Init("duwu");
    LOG_DEBUG("NIAMSILE %d", 23);
    LOG_DEBUG("SHIDEMA %d", 45);
    CAsyncLog::Uninit();
    return 0;
}

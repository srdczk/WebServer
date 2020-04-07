//
// Created by srdczk on 2020/4/5.
//

// 配置文件读取类

#pragma once

#include <map>
#include <string>

class ConfigFileReader {
public:
    ConfigFileReader(const char *filename);
    ~ConfigFileReader();

    char *GetConfigName(const char *name);
    int SetConfigValue(const char *name, const char *value);
private:
    void LoadFile(const char *filename);
    int WriteFile(const char *filename = nullptr);
    void ParseLine(char *line);
    char *TrimSpace(char *name);
private:
    bool loadOk_;
    // 有序表 保证 write 唯一
    std::map<std::string, std::string> configMap_;
    std::string configFile_;
};
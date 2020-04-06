//
// Created by srdczk on 2020/4/5.
//

#include "ConfigFileReader.h"
#include <cstring>


ConfigFileReader::ConfigFileReader(const char *filename) { LoadFile(filename); }

ConfigFileReader::~ConfigFileReader() {}

char *ConfigFileReader::GetConfigName(const char *name) {
    if (!loadOk_) return nullptr;
    std::string key(name);
    if (!configMap_.count(key)) return nullptr;
    return configMap_[key].data();
}

int ConfigFileReader::SetConfigValue(const char *name, const char *value) {
    if (!loadOk_) return -1;
    std::string key(name), val(value);
    configMap_[key] = val;
    return WriteFile();
}

void ConfigFileReader::LoadFile(const char *filename) {
    configFile_.clear();
    configFile_.append(filename);
    FILE *fp = fopen(filename, "r");
    if (!fp) return;
    char buff[256];
    while (true) {
        // 每次取一行
       char *p = fgets(buff, 256, fp);
       if (!p) break;
       auto len = strlen(buff);
       if (buff[len - 1] == '\n') buff[len - 1] = '\0';
       char *ch = strchr(buff, '#');
       if (ch) *ch = 0;
       if (!strlen(buff)) continue;
       ParseLine(buff);
    }
    fclose(fp);
    loadOk_ = true;
}

int ConfigFileReader::WriteFile(const char *filename) {
    FILE *fp = nullptr;
    if (!filename) fp = fopen(configFile_.data(), "w");
    else fp = fopen(filename, "w");
    if (!fp) return -1;
    char entry[128];
    for (auto &it : configMap_) {
        memset(entry, '\0', sizeof(entry));
        snprintf(entry, sizeof(entry), "%s=%s\n", it.first.data(), it.second.data());
        auto res = fwrite(entry, strlen(entry), 1, fp);
        if (res != 1) {
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);
    return 0;
}

void ConfigFileReader::ParseLine(char *line) {
    char *p = strchr(line, '=');
    if (!p) return;
    *p = '\0';
    char *key = TrimSpace(line);
    char *val = TrimSpace(p + 1);
    if (key && val) {
        std::string keyStr(key), value(val);
        configMap_[keyStr] = value;
    }
}

char *ConfigFileReader::TrimSpace(char *name) {
    char *pos = name;
    while ((*pos == ' ') || (*pos == '\t') || (*pos == '\r')) pos++;
    if (!strlen(pos)) return nullptr;
    char *end = name + strlen(name);
    while ((*end == ' ') || (*end == '\t') || (*end == '\r')) *end-- = '\0';
    int len = (int)(end - pos);
    if (len <= 0) return nullptr;
    return pos;
}











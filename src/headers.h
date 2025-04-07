#ifndef HEADERS_H
#define HEADERS_H

#include <string>
#include <unordered_map>
#include "utils.h"

class Headers {
public:
    Headers();

    void setHeader(const std::string &key, const std::string &value);
    std::string getHeader(const std::string &key);
    void removeHeader(const std::string &key);
    bool hasHeader(const std::string &key) const;
    std::string to_string() const;

private:
    std::unordered_map<std::string, std::string> headers_;
};

#endif
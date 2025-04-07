#include "headers.h"

Headers::Headers() = default;

void Headers::setHeader(const std::string& key, const std::string& value) {
    headers_[Utils::toLowerCase(key)] = value;
}

std::string Headers::getHeader(const std::string& key) {
    return headers_[Utils::toLowerCase(key)];
}


void Headers::removeHeader(const std::string& key) {
    headers_.erase(Utils::toLowerCase(key));
}

bool Headers::hasHeader(const std::string& key) const {
    return headers_.contains(Utils::toLowerCase(key));
}

std::string Headers::to_string() const {
    std::string result;
    for (const auto& header : headers_) {
        result += header.first + ": " + header.second + "\r\n";
    }
    return result;
}

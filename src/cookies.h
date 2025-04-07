#ifndef COOKIES_H
#define COOKIES_H

#include <string>
#include <map>

class Cookies {
public:
    explicit Cookies(const std::string& cookieHeader = "");
    bool hasCookie(const std::string& key) const;
    std::string getCookie(const std::string& key);
    void setCookie(const std::string& key, const std::string& value);
    int size() const;
    std::string toHeader();

private:
    std::map<std::string, std::string> cookies;

    void parseCookies(std::string cookieHeader);
};

#endif //COOKIES_H

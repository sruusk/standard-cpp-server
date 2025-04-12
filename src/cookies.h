#ifndef COOKIES_H
#define COOKIES_H

#include <string>
#include <map>
#include <memory>
#include <optional>

class Cookies {
public:
    explicit Cookies(const std::string& cookieHeader = "");
    [[nodiscard]] bool hasCookie(const std::string& key) const;
    std::optional<std::string> getCookie(const std::string& key);
    void setCookie(const std::string& key, const std::string& value);
    [[nodiscard]] unsigned long size() const;
    std::string toHeader();

    // Static methods for signing and verifying cookies.
    static std::string signCookie(const std::string& cookieContent);
    // Returns unique_ptr to decoded cookie value if verification is successful, nullptr otherwise.
    static std::unique_ptr<std::string> verifyCookie(const std::string& cookieValueWithSig);

private:
    std::map<std::string, std::string> cookies;

    void parseCookies(std::string cookieHeader);
};

#endif //COOKIES_H

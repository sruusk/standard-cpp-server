#include "cookies.h"
#include <regex>

Cookies::Cookies(const std::string& cookieHeader) {
    if (cookieHeader.length() > 1) {
        parseCookies(cookieHeader);
    }
}

bool Cookies::hasCookie(const std::string& key) const { return cookies.contains(key); }


int Cookies::size() const { return cookies.size(); }


std::string Cookies::getCookie(const std::string& key) { return cookies[key]; }


void Cookies::setCookie(const std::string& key, const std::string& value) { cookies[key] = value; }


std::string Cookies::toHeader() {
    std::string out;
    for (auto [key, value] : cookies) {
        out += key + "=" + value + "; ";
    }
    return out.substr(0, out.size() - 3);
}

void Cookies::parseCookies(std::string cookieHeader) {
    const std::regex del("; ");
    std::sregex_token_iterator it(cookieHeader.begin(), cookieHeader.end(), del, -1);
    const std::sregex_token_iterator end;

    while (it != end) {
        const std::string cookie = *it;
        const size_t pos = cookie.find('=');
        if (pos != std::string::npos)
            cookies[cookie.substr(0, pos)] = cookie.substr(pos + 1);
        ++it;
    }
}

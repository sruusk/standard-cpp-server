#include "cookies.h"

#include <optional>
#include <regex>
#include "crypto.h"

Cookies::Cookies(const std::string& cookieHeader) {
    if (cookieHeader.length() > 1) {
        parseCookies(cookieHeader);
    }
}

bool Cookies::hasCookie(const std::string& key) const { return cookies.contains(key); }


unsigned long Cookies::size() const { return cookies.size(); }


std::optional<std::string> Cookies::getCookie(const std::string& key) {
    const auto it = cookies.find(key);
    return it != cookies.end() ? std::optional(it->second) : std::nullopt;
}


void Cookies::setCookie(const std::string& key, const std::string& value) { cookies[key] = value; }


std::string Cookies::toHeader() {
    if (cookies.empty()) return "";
    std::string out;
    for (const auto& [key, value] : cookies) {
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

std::string Cookies::signCookie(const std::string& cookieContent) {
    // Create a Crypto instance with the secret.
    const auto crypto = Crypto();
    // Use Crypto to base64 encode the cookie content.
    const std::string encodedContent = Crypto::encodeBase64(cookieContent);
    // Generate the signature on the encoded content.
    const std::string encodedSignature = crypto.sign(encodedContent);
    // Use a period as the separator between the encoded data and signature.
    return encodedContent + "." + encodedSignature;
}

std::unique_ptr<std::string> Cookies::verifyCookie(const std::string& cookieValueWithSig) {
    const size_t separatorPos = cookieValueWithSig.find('.');
    if (separatorPos == std::string::npos)
        return nullptr;

    const std::string encodedContent = cookieValueWithSig.substr(0, separatorPos);
    const std::string providedSignature = cookieValueWithSig.substr(separatorPos + 1);

    // Recompute the signature using the Crypto class.
    const auto crypto = Crypto();
    const std::string computedSignature = crypto.sign(encodedContent);

    if (computedSignature == providedSignature) {
        // If valid, return the decoded cookie content.
        std::string decodedCookie = Crypto::decodeBase64(encodedContent);
        return std::make_unique<std::string>(decodedCookie);
    }
    return nullptr;
}

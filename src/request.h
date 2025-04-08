#ifndef REQUEST_H
#define REQUEST_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "cookies.h"
#include "headers.h"

using json = nlohmann::json;

enum class Method {
    GET,
    POST,
    PUT,
    DELETE_,
    HEAD,
    OPTIONS,
    PATCH,
    UNKNOWN
};

// Helper operator for streaming Method values.
inline std::ostream& operator<<(std::ostream& os, const Method& method) {
    switch (method) {
    case Method::GET:
        os << "GET";
        break;
    case Method::POST:
        os << "POST";
        break;
    case Method::PUT:
        os << "PUT";
        break;
    case Method::DELETE_:
        os << "DELETE";
        break;
    case Method::HEAD:
        os << "HEAD";
        break;
    case Method::OPTIONS:
        os << "OPTIONS";
        break;
    case Method::PATCH:
        os << "PATCH";
        break;
    default:
        os << "UNKNOWN";
        break;
    }
    return os;
}

enum class ResponseCode {
    OK = 200,
    NotFound = 404,
    InternalServerError = 500,
};

class Request {
public:
    explicit Request(int client_fd);

    void respond(ResponseCode responseCode,
                 Headers* headers = new Headers(),
                 const std::string& content = "",
                 MimeType mimeType = MimeType::TXT);

    bool hasQuery(const std::string& key) const;

    std::vector<std::string> getQuery(const std::string& key);

    Method getMethod() const;

    const std::string& getPath() const;

    Headers& getHeaders();

    const Cookies& getCookies() const;

    void setCookie(const std::string& key, const std::string& value);

    json parseBodyJSON();

private:
    int client_fd;
    Method method{Method::UNKNOWN};
    std::string path;
    std::map<std::string, std::vector<std::string>> queryParams;
    Headers requestHeaders;
    std::string rawRequest;
    Cookies cookies;
    Cookies responseCookies;
    std::string body;

    void parseQueryParams(const std::string& query);

    static std::string buildHttpResponse(ResponseCode responseCode,
                                         Headers* headers,
                                         const std::string& content);

    // Helper to parse a method string into a Method enum.
    static Method parseMethod(const std::string& methodStr);
};

#endif

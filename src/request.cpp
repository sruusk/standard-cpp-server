#include "request.h"
#include <cstring>
#include <regex>

Request::Request(const int client_fd) : client_fd(client_fd) {
    char buffer[2048] = {};
#ifdef _WIN32
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        std::cerr << "Error reading from client, bytes_received = " << bytes_received
                  << ", error: " << WSAGetLastError() << std::endl;
        return;
    }
#else
    ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_received < 0) {
        std::cerr << "Error reading from client, bytes_received = " << bytes_received
            << ", error: " << std::strerror(errno) << std::endl;
        return;
    }
#endif
    buffer[bytes_received] = '\0';
    rawRequest = std::string(buffer);

    // Parse the request line "METHOD PATH HTTP/1.1"
    const size_t pos = rawRequest.find("\r\n");
    if (pos != std::string::npos) {
        std::string requestLine = rawRequest.substr(0, pos);
        const size_t pos1 = requestLine.find(' ');
        const size_t pos2 = requestLine.find(' ', pos1 + 1);
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            const std::string methodStr = requestLine.substr(0, pos1);
            method = parseMethod(methodStr);

            // Get full route url and separate query from path
            const std::string url = requestLine.substr(pos1 + 1, pos2 - pos1 - 1);
            const size_t queryStart = url.find('?');
            path = url.substr(0, queryStart);

            if (queryStart != std::string::npos)
                parseQueryParams(url.substr(queryStart + 1));

            // for (const auto& param : queryParams)
            //     for (const auto& value: param.second)
            //         std::cout << param.first << ": " << value << std::endl;
        }
    }

    // Parse headers until an empty line is reached
    size_t headerStart = pos + 2; // After "\r\n"
    while (true) {
        const size_t headerEnd = rawRequest.find("\r\n", headerStart);
        if (headerEnd == std::string::npos || headerEnd == headerStart)
            break;
        std::string headerLine = rawRequest.substr(headerStart, headerEnd - headerStart);
        const size_t colonPos = headerLine.find(':');
        if (colonPos != std::string::npos) {
            std::string key = headerLine.substr(0, colonPos);
            size_t valueStart = colonPos + 1;
            // Trim any leading spaces from the header value
            while (valueStart < headerLine.size() && headerLine[valueStart] == ' ')
                valueStart++;
            std::string value = headerLine.substr(valueStart);
            requestHeaders.setHeader(key, value);
        }
        headerStart = headerEnd + 2;
    }

    // Read the cookie header
    if (requestHeaders.hasHeader("cookie"))
        cookies = Cookies(requestHeaders.getHeader("cookie"));


    // std::cout << headerStart << " - " << rawRequest.size() << std::endl;
    // const std::string substr = rawRequest.substr(headerStart);
    // std::cout << static_cast<int>(substr[0]) << " - " << static_cast<int>(substr[1]) << std::endl;

    // Read body
    if (requestHeaders.hasHeader("Content-Length")) {
        const int contentLength = std::stoi(requestHeaders.getHeader("Content-Length"));

        // Skip carriage return and line feed characters after the headers
        body = rawRequest.substr(headerStart + 2);
        while (body.size() < contentLength) {
#ifdef _WIN32
            ssize_t bodyBytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
#else
            ssize_t bodyBytesReceived = read(client_fd, buffer, sizeof(buffer) - 1);
#endif
            if (bodyBytesReceived <= 0) break;
            buffer[bodyBytesReceived] = '\0';
            body += buffer;
        }
    }
}

void Request::respond(const ResponseCode responseCode,
                      const std::string& content,
                      Headers* headers,
                      const MimeType mimeType) {
    // Set set-cookie header
    if (responseCookies.size() > 0)
        headers->setHeader("Set-Cookie", responseCookies.toHeader());

    headers->setHeader("Content-Type", Utils::toMimeString(mimeType));

    const std::string response = buildHttpResponse(responseCode, headers, content);

    const size_t total = response.size();
    size_t totalSent = 0;
    while (totalSent < total) {
        const ssize_t sent = send(client_fd, response.data() + totalSent, total - totalSent, 0);
        if (sent <= 0) break; // Error
        totalSent += sent;
    }
#ifdef _WIN32
    shutdown(client_fd, SD_RECEIVE);
    closesocket(client_fd);
#else
    shutdown(client_fd, SHUT_RD);
    close(client_fd);
#endif
}

void Request::respond(const ResponseCode responseCode, const json& content, Headers* headers) {
    respond(responseCode, content.dump(), headers, MimeType::JSON);
}

void Request::parseQueryParams(const std::string& query) {
    std::regex del("&");

    std::sregex_token_iterator it(query.begin(), query.end(), del, -1);
    std::sregex_token_iterator end;

    while (it != end) {
        const std::string param = *it;
        const std::string key = Utils::urlDecode(param.substr(0, param.find('=')));
        const std::string value = param.substr(param.find('=') + 1);

        std::regex del2(",");
        std::sregex_token_iterator it2(value.begin(), value.end(), del2, -1);
        std::sregex_token_iterator end2;

        auto parsedValues = std::vector<std::string>();
        while (it2 != end2) {
            parsedValues.push_back(Utils::urlDecode(*it2));
            ++it2;
        }

        // Push parsed values to queryParams
        if (!queryParams.contains(key)) queryParams[key] = parsedValues;
        else
            for (const auto& parsed_value : parsedValues) {
                queryParams[key].push_back(parsed_value);
            }

        ++it;
    }
}

std::string Request::buildHttpResponse(ResponseCode responseCode,
                                       Headers* headers,
                                       const std::string& content) {
    // Set a default Content-Type header if not present.
    if (!headers->hasHeader("Content-Type")) {
        headers->setHeader("Content-Type", Utils::toMimeString(MimeType::TXT));
    }

    // Automatically set the Content-Length and X-Powered-By headers.
    headers->setHeader("Content-Length", std::to_string(content.size()));
    headers->setHeader("Server", "Custom C++ HTTP Server");
    headers->setHeader("X-Powered-By", "github.com/sruusk");

    // Determine the reason text based on the response code.
    std::string reason;
    switch (responseCode) {
    case ResponseCode::OK: reason = "OK";
        break;
    case ResponseCode::NotFound: reason = "Not Found";
        break;
    case ResponseCode::InternalServerError:
    default:
        reason = "Internal Server Error";
        break;
    }

    // Build the status line.
    std::string response = "HTTP/1.1 " + std::to_string(static_cast<int>(responseCode))
        + " " + reason + "\r\n";

    // Append formatted headers.
    response += headers->to_string();

    // End the header section with an empty line and add the content.
    response += "\r\n" + content;

    return response;
}

Method Request::parseMethod(const std::string& methodStr) {
    if (methodStr == "GET") return Method::GET;
    if (methodStr == "POST") return Method::POST;
    if (methodStr == "PUT") return Method::PUT;
    if (methodStr == "DELETE") return Method::DELETE_;
    if (methodStr == "HEAD") return Method::HEAD;
    if (methodStr == "OPTIONS") return Method::OPTIONS;
    if (methodStr == "PATCH") return Method::PATCH;
    return Method::UNKNOWN;
}

Method Request::getMethod() const {
    return method;
}

const std::string& Request::getPath() const {
    return path;
}

Headers& Request::getHeaders() {
    return requestHeaders;
}

bool Request::hasQuery(const std::string& key) const {
    return queryParams.contains(key);
}

std::vector<std::string> Request::getQuery(const std::string& key) {
    return queryParams[key];
}

const Cookies& Request::getCookies() const { return cookies; }

void Request::setCookie(const std::string& key, const std::string& value) {
    responseCookies.setCookie(key, value);
}

/**
 * Parses the request body to json.
 * Returns a nullptr and sends a 400 response if the body cannot be parsed.
 */
json Request::parseBodyJSON() {
    const std::string contentType = requestHeaders.getHeader("Content-Type");
    if (contentType != "application/json")
        std::cerr << "Invalid Content-Type for JSON: " << contentType << " at " << path << "\n";
    try {
        const auto json = json::parse(body);
        return json;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        respond(ResponseCode::BadRequest);
        return nullptr;
    }
}

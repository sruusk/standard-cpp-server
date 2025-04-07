#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#include <functional>
#include <unordered_map>
#include <string>
#include <map>
#include "request.h"

#define RouteHandler std::function<void(Request)>

struct RouteKey {
    std::string route;
    Method method;

    bool operator==(const RouteKey& other) const {
        return route == other.route && method == other.method;
    }
};

struct RouteKeyHash {
    std::size_t operator()(const RouteKey& key) const {
        return std::hash<std::string>()(key.route) ^ (std::hash<int>()(static_cast<int>(key.method)) << 1);
    }
};

class Server {
public:
    explicit Server(int port = 8080, const std::string& publicDir = "public");

    void registerRoute(const Method& method, const std::string& route, const RouteHandler& handler);

    [[noreturn]] void run();

    ~Server();

private:
    int server_fd;
    int port;
    sockaddr_in server_addr{};
    std::unordered_map<RouteKey, RouteHandler, RouteKeyHash> routes;
    std::map<std::string, std::string> publicFiles;

    void processRequest(int client_fd);
};

#endif

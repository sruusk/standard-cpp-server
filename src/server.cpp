#include "server.h"
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <filesystem>

using std::string;
namespace fs = std::filesystem;

Server::Server(const int port, const string& publicDir) : port(port) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
    }
#endif

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // Configure socket
    const int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket options.\n";
#ifdef _WIN32
        closesocket(server_fd);
#else
        close(server_fd);
#endif
    }

    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (::bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(server_fd, 5) < 0) {
        throw std::runtime_error("Failed to listen on socket");
    }

    // Read public folder filepaths to memory
    // The key in publicFiles is the route (e.g. "/index.html") and the value is the file's full path.
    if (fs::exists(publicDir) && fs::is_directory(publicDir)) {
        std::cout << "Using public directory " << publicDir << std::endl;
        for (const auto& entry : fs::recursive_directory_iterator(publicDir)) {
            if (fs::is_regular_file(entry.path())) {
                // Get the path relative to the public folder.
                fs::path relativePath = fs::relative(entry.path(), publicDir);
                // Create the default route starting with '/'
                std::string route = "/" + relativePath.generic_string();
                publicFiles[route] = entry.path().string();

                // If the file is index.html, add an extra mapping.
                if (relativePath.filename() == "index.html") {
                    if (relativePath.parent_path().empty()) {
                        // In the root folder, map "/" to index.html.
                        publicFiles["/"] = entry.path().string();
                    }
                    else {
                        // In a subdirectory, map the directory route (e.g. /dir) to index.html.
                        std::string dirRoute = "/" + relativePath.parent_path().generic_string();
                        publicFiles[dirRoute] = entry.path().string();
                    }
                }
            }
        }
    }
    else if (fs::create_directory(publicDir))
        std::cout << "Created public dir " << publicDir << std::endl;
}

void Server::registerRoute(const Method& method, const string& route, const RouteHandler& handler) {
    const RouteKey key = {route, method};
    const auto it = routes.find(key);
    if (it == routes.end()) routes[key] = handler;
    else std::cerr << "Route " << method << " " << route << " already registered.\n";
}

void Server::processRequest(const int client_fd) {
    auto req = Request(client_fd);

    const RouteKey key = {req.getPath(), req.getMethod()};
    const auto it = routes.find(key);
    // Run registered routes
    if (it != routes.end()) {
#ifdef DEBUG
        std::cout << "200 " << req.getMethod() << " " << req.getPath() << "\n";
#endif
        it->second(req);
    }
    // Serve public files
    else if (req.getMethod() == Method::GET && publicFiles.contains(req.getPath())) {
        std::ifstream file(publicFiles[req.getPath()], std::ios::in | std::ios::binary);
        if (file) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
#ifdef DEBUG
            std::cout << "200 " << req.getMethod() << " " << req.getPath() << "\n";
#endif
            req.respond(ResponseCode::OK, new Headers(), content, Utils::getMimeType(publicFiles[req.getPath()]));
        }
        else {
            req.respond(ResponseCode::InternalServerError);
        }
    }
    // 404 - Not Found
    else {
        std::cout << "404 " << req.getMethod() << " " << req.getPath() << "\n";
        req.respond(ResponseCode::NotFound);
    }
}

[[noreturn]] void Server::run() {
    std::cout << "Server listening on port " << port << "...\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
        if (client_fd < 0) {
            std::cerr << "Failed to accept connection.\n";
            continue;
        }

        std::thread([this, client_fd]() {
            processRequest(client_fd);
        }).detach();
    }
}

Server::~Server() {
#ifdef _WIN32
    closesocket(server_fd);
    WSACleanup();
#else
    close(server_fd);
#endif
}

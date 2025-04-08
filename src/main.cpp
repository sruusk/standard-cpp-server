#include "server.h"

int main(const int argc, char* argv[]) {
    int port = 3000;
    std::string publicDir = "public";
    if (argc > 1)
        port = atoi(argv[1]);
    if (argc > 2)
        publicDir = argv[2];


    Server server = Server(port, publicDir);

    server.registerRoute(Method::GET, "/hello", [](Request req) {
        req.respond(ResponseCode::OK, "<h2>Hello World!</h2>", new Headers(), MimeType::HTML);
    });

    server.registerRoute(Method::POST, "/json", [](Request req) {
        const json json = req.parseBodyJSON();
        if (json == nullptr) return;

        std::cout << json << std::endl;
        req.respond(ResponseCode::OK, json);
    });

    server.run();

    return 0;
}

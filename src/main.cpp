#include "server.h"

int main(int argc, char* argv[]) {
    int port = 3000;
    std::string publicDir = "public";
    if (argc > 1)
        port = atoi(argv[1]);
    if (argc > 2)
        publicDir = argv[2];


    Server server = Server(port, publicDir);

    const RouteHandler hello = [](Request req) {
        req.respond(ResponseCode::OK, new Headers(), "<h2>Hello World!</h2>", MimeType::HTML);
    };
    server.registerRoute(Method::GET, "/hello", hello);

    server.run();

    return 0;
}

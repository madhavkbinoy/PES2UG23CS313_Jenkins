#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

int main() {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    std::cout << "Listening on 8080 (" << hostname << ")" << std::endl;

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) continue;

        char buffer[4096];
        int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes < 0) {
            close(client_fd);
            continue;
        }

        std::string body = "Served by backend: " + std::string(hostname) + "\n";

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            body;

        send(client_fd, response.c_str(), response.size(), 0);

        close(client_fd);
    }
}

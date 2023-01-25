#include <iostream>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string>
#include <cstring>

static void do_something(int connfd) {
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        std::cout << "read error" << std::endl;
        return;
    }
    std::cout << "client says " << rbuf << std::endl;

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0);    // 0.0.0.0
    int rv = bind(fd, (const sockaddr*) &addr, sizeof(addr));
    if (rv) {
        perror("bind error");
    }

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        perror("listen error");
    }

    while (true) {
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr*) &client_addr, &socklen);
        if (connfd < 0) {
            perror("connect error");
        }

        do_something(connfd);
        close(connfd);
    }
}
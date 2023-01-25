#include <iostream>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string>
#include <cstring>

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket error");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
    int rv = connect(fd, (struct sockaddr*) &addr, sizeof(addr));
    if (rv) {
        perror("connect error");
    }

    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        perror("read error");
    }
    std::cout << "Server says " << rbuf << std::endl;
    close(fd);
}
#include <iostream>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <fcntl.h>

const size_t k_max_msg = 4096;

/**
 * print error message without terminate
*/
static void msg(const char* msg) {
    fprintf(stderr, "%s\n", msg);
}

/**
 * print error message and terminate
*/
static void die(const char* msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s", err, msg);
    abort();
}

/**
 * set a fd to nonblocking mode by fcntl
*/
static void set_fd_nb(int fd) {
    errno = 0;
    int flags = fcntl(fd, F_GETFL, 0);
    if (errno) {
        die("fcntl err");
        return;
    }

    flags |= O_NONBLOCK;
    
    errno = 0;
    fcntl(fd, F_SETFD, flags);
    if (errno) {
        die("fcntl err");
        return;
    }
}

/**
 * read all partial contents until read all n bytes
*/
static int32_t read_full(int fd, char* buf, size_t n) {
    
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error, or EOF
        }
        assert((size_t) rv <= n);
        n -= (size_t) rv;
        buf += rv;
    }
    return 0;
}

/**
 * write all partial contents until write all n bytes
*/
static int32_t write_all(int fd, char* buf, size_t n) {
    
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error, or EOF
        }
        assert((size_t) rv <= n);
        n -= (size_t) rv;
        buf += rv;
    }
    return 0;
}

/**
 * send query to server and receive reply
*/
static int32_t query(int fd, const char* text) {
    uint32_t len = (uint32_t) strlen(text);
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], text, len);
    if (int32_t err = write_all(fd, wbuf, 4 + len)) {
        return err;
    }

    // 4 bytes header
    char rbuf[4 + k_max_msg];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if (err) {
        if (errno == 0) {
            msg("EOF");
        } else {
            msg("read err");
        }
        return err;
    }

    memcpy(&len, rbuf, 4);  // we assume little ending
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    // reply body
    err = read_full(fd, &rbuf[4], len);
    printf("client says %s\n", &rbuf[4]);
    return 0;
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket err");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
    int rv = connect(fd, (struct sockaddr*) &addr, sizeof(addr));
    if (rv) {
        die("connect err");
    }

    // test multiple requests
    int32_t err = query(fd, "hello1");
    if (err) {
        goto L_DONE;
    }
    err = query(fd, "hello2");
    if (err) {
        goto L_DONE;
    }
    err = query(fd, "hello3");
    if (err) {
        goto L_DONE;
    }

L_DONE:
    close(fd);
    return 0;
}
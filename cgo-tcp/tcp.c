#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "tcp.h"

void handler(int con_sock_fd) {
    char buff[1024] = {'\0'};
    int n = read(con_sock_fd, buff, sizeof(buff) - 1);
    if (n > 0) {
        buff[n] = '\0';
        printf("[client] %s\n", buff);
    }

    char str_response[] = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nhelo\n";
    write(con_sock_fd, str_response, strlen(str_response));
    close(con_sock_fd);
}

int tcp_server(char *addr, int port) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Failed to open inet socket");
        return -1;
    }

    int opt = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, addr, &address.sin_addr);

    if (bind(sock_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(sock_fd);
        perror("Failed to bind socket");
        return -1;
    }

    if (listen(sock_fd, 3) < 0) {
        close(sock_fd);
        perror("Failed to listen connections");
        return -1;
    }

    return sock_fd;
}

void accept_conns(int sock_fd) {
    struct sockaddr_in cli_address;
    socklen_t addrlen;

    for(;;) {
        addrlen = sizeof(cli_address);

        int con_sock_fd = accept(sock_fd, (struct sockaddr*)&cli_address, &addrlen);
        if (con_sock_fd < 0) {
            if (errno == EINTR) continue;
            if (errno == ECONNABORTED) continue;
            if (errno == EINVAL) break;
            perror("Failed to accept connections");
            break;
        }
        handler(con_sock_fd);
    }
    printf("Close main socket\n");
    close(sock_fd);
}

void stop_listening(int sock_fd) {
    int saved_errno = errno;
    if (sock_fd >= 0) {
        shutdown(sock_fd, SHUT_RDWR);
    }
    errno = saved_errno;
}

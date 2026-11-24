#ifndef TCP_H
#define TCP_H

int tcp_server(char *addr, int port);
void accept_conns(int sock_fd);
void stop_listening(int sock_fd);

#endif

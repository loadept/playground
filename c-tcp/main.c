#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // read(), write(), close()
#include <stdio.h>
#include <string.h>

#define ADDR "127.0.0.1"
#define PORT 8080

int sock_fd = -1;

void tcp_server(char *addr, int port) {
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Failed to open inet socket");
        return;
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
        return;
    }

    if (listen(sock_fd, 3) < 0) { // <-  escucha las conexiones entrantes
        close(sock_fd);
        perror("Failed to listen connections");
        return;
    }

    printf("Papu's server listen on :8080\n");
    struct sockaddr_in cli_address;
    socklen_t addrlen;

    for(;;) {
        addrlen = sizeof(cli_address);

        int con_sock_fd = accept(sock_fd, (struct sockaddr*)&cli_address, &addrlen); // <-  accept bloquea hasta tomar la conexión entrante
        if (con_sock_fd < 0) {
            if (errno == EINTR) continue;
            if (errno == ECONNABORTED) continue;
            if (errno == EINVAL) break; // <-  EINVAL porque el kernel quitó al socket principal `sock_fd` de estado LISTEN tras el shutdown
            perror("Failed to accept connection");
            break;
        }

        // uint8_t buff[1024] = {'\0'}; ->  ¿También funcionaria con uint8_t?
        char buff[1024] = {'\0'};
        int n = read(con_sock_fd, buff, sizeof(buff) - 1);
        if (n > 0) {
            buff[n] = '\0';
            printf("[client] %s\n", buff);
        }

        // char *response = "Hello from Papu's C server"; ->  Esto también funcionará
        char str_response[] = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nhelo\n";

        // Al tratar strings o char[] como en cualquier lenguaje se pasa directamente la referencia de memoria de ese arreglo,
        // Pero en realidad lo que se hace es &str_response[0], darle el puntero al primer elemento de la cadena
        // y solo digamos que intermanete se hace un *char + 1, la cual recorre a la siguiente direccion (aritmetica de punteros).
        write(con_sock_fd, str_response, strlen(str_response));
        close(con_sock_fd); // <-- close ya hace el four-way handshake por debajo
    }
    printf("Close main socket\n");
    close(sock_fd);
}

void signal_handler(int sig) {
	int saved_errno = errno;
	printf("signal %d recieved, cleaning before exit\n", sig);
	if (sock_fd >= 0) {
	    shutdown(sock_fd, SHUT_RDWR);
		printf("Stop comunication\n");
	}
	errno = saved_errno;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    tcp_server(ADDR, PORT);
}

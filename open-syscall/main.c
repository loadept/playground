#include <fcntl.h>
#include <stddef.h>
#include <unistd.h> // read(), write(), close()
#include <stdio.h>

int main() {
    int fd = open("open-syscall/main.c", O_RDONLY);
    if (fd < 0) {
        perror("Error");
        return 1;
    }

    char buff[1024];
    for (;;) {
        int n = read(fd, buff, 10 - 1);
        if (n < 0) {
            perror("Error");
            return 1;
        }
        if (n == 0) {
            break;
        }

        buff[n] = '\0';
        printf("%s", buff);
    }

    printf("\n");
}

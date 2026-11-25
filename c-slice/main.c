#include <stdint.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h> // read(), write(), close()
#include <stdio.h>
#include "slice.h"

int main() {
    slice my_slice = {0};

    char data[] = {'h', 'o', 'l', 'a'};
    if (slice_append(&my_slice, data, sizeof(data)) < 0) {
        printf("error\n");
        return 1;
    }

    int fd = open("/dev/stdout", O_WRONLY, 0644);
    if (fd < 0) {
        perror("Error");
        return 1;
    }

    int len = my_slice.len;
    write(fd, my_slice.data, len - 1);
    close(fd);
    slice_free(&my_slice);
}

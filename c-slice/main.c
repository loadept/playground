#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read(), write(), close()
#include <stdio.h>

#define DEF_ALLOC 1024

// I try to create Go slice in C
typedef struct {
    void *data;
    size_t len;
    size_t cap;
} slice;

int append(slice *s, const void *src, size_t n) {
    if (s == NULL) {
        return -1;
    }

    if (s == NULL || s->data == NULL) {
        size_t initial_cap = (n > DEF_ALLOC) ? n : DEF_ALLOC; // 1024 bytes default
        void *mem = malloc(initial_cap);
        if (mem == NULL) {
            return -1;
        }
        s->data = mem;
        s->len = 0;
        s->cap = initial_cap;
    }

    if (n > 0 && src != NULL) {
        void *dest = (char *)s->data + s->len;
        memcpy(dest, src, n);
        s->len += n;
    }
    return 0;
}

int main() {
    slice my_slice = {0};

    char data[] = {'h', 'o', 'l', 'a', '\0'};
    int num = 19;
    if (append(&my_slice, data, sizeof(data)) < 0) {
        printf("error\n");
        return 1;
    }
    if (append(&my_slice, &num, sizeof(num)) < 0 ) {
        printf("error\n");
        return 1;
    }

    int fd = open("file.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (fd < 0) {
        perror("Error");
        return 1;
    }

    int len = my_slice.len;
    write(fd, my_slice.data, len);
    printf("written: %d bytes\n", len);
    // then, i show this file with hexdump or od commands
}

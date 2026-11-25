#include <stdlib.h>
#include <string.h>
#include "slice.h"

#ifndef DEF_ALLEC
#define DEF_ALLOC 1024
#endif

int slice_append(slice *s, const void *src, size_t n) {
    if (s == NULL || src == NULL) {
        return -1;
    }
    if (n == 0) {
        return 0;
    }
    if (n > SIZE_MAX - s->len) {
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
    if (s->len + n > s->cap) {
        size_t new_cap = (s->cap == 0) ? 1 : s->cap;
        while (new_cap < s->len + n) {
            if (new_cap > SIZE_MAX / 2) {
                new_cap = s->len + n;
                break;
            }
            new_cap *= 2;
        }

        void *re_mem = realloc(s->data, new_cap);
        if (re_mem == NULL) {
            return -1;
        }
        s->data = re_mem;
        s->cap = new_cap;
    }

    void *dest = (char *)s->data + s->len;
    memcpy(dest, src, n);
    s->len += n;

    return 0;
}

void slice_free(slice *s) {
    if (s == NULL) {
        return;
    }
    free(s->data);
    s->data = NULL;
    s->len = 0;
    s->cap = 0;
}

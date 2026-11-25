#ifndef SLICE_H
#define SLICE_H

#include <stddef.h>
#include <stdint.h>

// I try to create Go slice in C
typedef struct {
    void *data;
    size_t len;
    size_t cap;
} slice;

int slice_append(slice *s, const void *src, size_t n);
void slice_free(slice *s);

#endif

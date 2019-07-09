#ifndef XZDEC_H
#define XZDEC_H

#include "xz.h"
#include "nbpatch.h"

#define IN_BUF_MAX  1024
#define OUT_BUF_MAX 4096
#define RET_OK      0
#define RET_ERROR   1

typedef struct xzReadHandler {
    uint8_t in_buf [IN_BUF_MAX];
    size_t avail_size;
    size_t read_pos;
} xzReadHandler;

bool xz_init(struct xz_dec **s, struct xz_buf *b);
bool xz_end(struct xz_dec *s);
size_t xz_read(xzReadHandler *handler, struct xz_buf *b,struct xz_dec *s, unsigned long in_file, unsigned char *buf, int buf_len);
#endif /*XZDEC_H*/

#ifndef KFIFO_H
#define KFIFO_H 1

#include "plat_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct kfifo {
    unsigned char *buffer;     /* the buffer holding the data */
    unsigned int size;         /* the size of the allocated buffer */
    unsigned int in;           /* data is added at offset (in % size) */
    unsigned int out;          /* data is extracted from off. (out % size) */
};

void kfifo_init(struct kfifo *k, unsigned char *buff, unsigned int len);
unsigned int kfifo_put(struct kfifo *k, unsigned char *buff, unsigned int len);
unsigned int kfifo_get(struct kfifo *k, unsigned char *buff, unsigned int len);
unsigned int kfifo_peek(struct kfifo *k, unsigned int len_want, unsigned char **buff1, unsigned char **buff2, unsigned int *len1, unsigned int *len2);
unsigned int kfifo_peek_to_buf(struct kfifo *fifo, unsigned char *buff, unsigned int len);
unsigned int kfifo_len(struct kfifo *fifo);
unsigned int kfifo_available(struct kfifo *fifo);

#if defined(__cplusplus)
}
#endif

#endif /* KFIFO_H */

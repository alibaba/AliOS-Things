/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifdef RHINO_CONFIG_IPC

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t aos_ipc_strlen(const char *str)
{
    uint32_t len = 0;

    while (str[len ++] != '\0');

    return len - 1;
}

int aos_ipc_strcmp(const char *src, const char *dst)
{
    uint32_t i = 0;
    char diff = 0;

    while (src[i] != '\0' && src[i] == dst[i]) {
        i ++;
    }

    diff = src[i] - dst[i];

    return diff != 0 ? (diff > 0 ? 1 : -1) : 0;
}

void aos_ipc_strcpy(char *dst, const char *src)
{
    while ((*dst ++ = *src ++) != '\0');
}

typedef long        ipc_word_t;
#define IPC_LSIZE   sizeof(ipc_word_t)
#define IPC_LMASK   (IPC_LSIZE - 1)

void *aos_ipc_memcpy(void *dest, const void *src, size_t count)
{
    char *d = (char *)dest;
    const char *s = (const char *)src;
    int len;

    if (count == 0 || dest == src) {
        return dest;
    }

    if (((ipc_word_t)d | (ipc_word_t)s) & IPC_LMASK) {
        /* src and/or dest do not align on word boundary */
        if ((((ipc_word_t)d ^ (ipc_word_t)s) & IPC_LMASK) || (count < IPC_LSIZE)) {
            len = count; /* copy the rest of the buffer with the byte mover */
        } else {  /* move the ptrs up to a word boundary */
            len = IPC_LSIZE - ((ipc_word_t)d & IPC_LMASK);
        }
        count -= len;
        for (; len > 0; len --) {
            *d ++ = *s ++;
        }
    }
    for (len = count / IPC_LSIZE; len > 0; len --) {
        /* fast copy with length of word*/
        *(ipc_word_t *)d = *(ipc_word_t *)s;
        d += IPC_LSIZE;
        s += IPC_LSIZE;
    }
    for (len = count & IPC_LMASK; len > 0; len --) {
        *d ++ = *s ++;
    }

    return dest;
}

#ifdef __cplusplus
}
#endif

#endif

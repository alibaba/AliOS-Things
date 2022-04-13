/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "dechunk.h"
#include <string.h>
#include <stdlib.h>

#define CHUNK_INIT() \
do { \
    g_is_chunkbegin = 0; \
    g_chunk_len = 0; \
    g_chunk_read = 0; \
} while (0)

#define DC_MIN(x, y) ((x) > (y) ? (y) : (x))

static char     *g_buff_out     = NULL;
static char     *g_buff_pt      = NULL;

static size_t   g_buff_outlen   = 0;
static size_t   g_chunk_len     = 0;
static size_t   g_chunk_read    = 0;

static int      g_is_running    = 0;
static int      g_is_first      = 1;
static int      g_is_chunkbegin = 0;

void *memstr(void *src, size_t src_len, char *sub)
{
    if (NULL == src || NULL == sub || src_len < strlen(sub))
        return NULL;

    char *p     = src;
    char *q     = sub;
    size_t indx    = src_len;
    size_t sub_len = strlen(sub);

    while (indx > 0) {
        int i = 0;

        while (i < sub_len) {
            char cp = *(p + i);
            char cq = *(q + i);
            // case ignore
            if (cq >= 'A' && cq <= 'Z')
                cq |= 0x20;

            if (cp >= 'A' && cp <= 'Z')
                cp |= 0x20;

            if (cq != cp)
                break;

            i++;
        }

        if (i == sub_len)
            return p;

        p++;
        indx--;
    }

    return NULL;
}

int dechunk_init()
{
    if (g_is_running)
        return DCE_ISRUNNING;

    g_buff_out      = NULL;
    g_buff_pt       = NULL;

    g_buff_outlen   = 0;
    g_chunk_len     = 0;
    g_chunk_read    = 0;

    g_is_running    = 1;
    g_is_first      = 1;
    g_is_chunkbegin = 0;

    return DCE_OK;
}

int dechunk(void *input, size_t inlen)
{
    if (!g_is_running)
        return DCE_LOCK;

    if (NULL == input || inlen == 0)
        return DCE_ARGUMENT;

    void *data_start = input;
    size_t data_len = inlen;

    if (g_is_first) {
        data_start = memstr(data_start, data_len, "\r\n\r\n");
        if (NULL == data_start)
            return DCE_FORMAT;

        data_start += 4;
        data_len -= (data_start - input);

        g_is_first = 0;
    }

    if (!g_is_chunkbegin) {
        char *stmp = data_start;
        int itmp = 0;

        sscanf(stmp, "%x", &itmp);
        // exclude the terminate "\r\n"
        itmp = (itmp > 0 ? itmp - 2 : itmp);
        data_start = memstr(stmp, data_len, "\r\n");
        data_start += 2;    // strlen("\r\n")

        data_len        -=  (data_start - (void *)stmp);
        g_chunk_len     =   itmp;
        g_buff_outlen   +=  g_chunk_len;
        g_is_chunkbegin =   1;
        g_chunk_read    =   0;

        if (g_chunk_len > 0 && 0 != g_buff_outlen) {
            if (NULL == g_buff_out) {
                g_buff_out = (char *)malloc(g_buff_outlen);
                g_buff_pt = g_buff_out;
            } else {
                g_buff_out = realloc(g_buff_out, g_buff_outlen);
            }

            if (NULL == g_buff_out)
                return DCE_MEM;

        }
    }

    if (g_chunk_read < g_chunk_len) {
        size_t cpsize = DC_MIN(g_chunk_len - g_chunk_read, data_len);
        memcpy(g_buff_pt, data_start, cpsize);

        g_buff_pt       += cpsize;
        g_chunk_read    += cpsize;
        data_len        -= (cpsize + 2);
        data_start      += (cpsize + 2);

        if (g_chunk_read >= g_chunk_len) {
            CHUNK_INIT();
            if (data_len > 0)
                return dechunk(data_start, data_len);

        }
    } else {
        CHUNK_INIT();
    }

    return DCE_OK;
}

int dechunk_free()
{
    free(g_buff_out);

    g_buff_out      = NULL;
    g_buff_pt       = NULL;

    g_buff_outlen   = 0;
    g_chunk_len     = 0;
    g_chunk_read    = 0;

    g_is_running    = 0;
    g_is_first      = 1;
    g_is_chunkbegin = 0;

    return DCE_OK;
}

int dechunk_getbuff(void **buff, size_t *buf_size)
{
    *buff = g_buff_out;
    *buf_size = g_buff_outlen;

    return DCE_OK;
}

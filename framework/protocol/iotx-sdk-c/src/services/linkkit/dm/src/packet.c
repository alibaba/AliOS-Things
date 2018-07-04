#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "packet.h"
#include "iot_import.h"

int packet_init(linkkit_pkt_t *pkt, char *buf, int buf_len)
{
    memset(pkt, 0, sizeof(linkkit_pkt_t));

    pkt->buf = buf;
    pkt->end = buf + buf_len;
    pkt->ptr = buf;

    if (pkt->ptr >= pkt->end)
        return -1;

    return 0;
}


int packet_printf(linkkit_pkt_t *pkt, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    HAL_Vsnprintf(pkt->ptr, pkt->end - pkt->ptr, fmt, ap);
    va_end(ap);

    pkt->ptr += strlen(pkt->ptr);
    if (pkt->ptr >= pkt->end)
        return -1;
    return 0;
}

int packet_strncat(linkkit_pkt_t *pkt, char *str, int n)
{
    int left = pkt->end - pkt->ptr;
    if (left < n + 1)   /* 1 for null terminator */
        return -1;

    strncat(pkt->ptr, str, n);
    pkt->ptr += n;

    pkt->got_data = 1;

    return 0;
}


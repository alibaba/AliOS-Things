
#include <aos/kernel.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "define.h"
#include "osal.h"
#include "log.h"

/* reverse byte order */
static uint32_t reverse_32bit(uint32_t data)
{
    return ((data >> 24) & 0xff) | ((data >> 8) & 0xff00) | ((data << 8) & 0xff0000) | ((data << 24) & 0xff000000);
}

static inline uint16_t reverse_16bit(uint16_t data)
{
    return (data >> 8) | (data << 8);
}

int os_is_big_endian(void)
{
    uint32_t data = 0xFF000000;

    if (0xFF == *(uint8_t *) & data) {
        return 1;                                    /* big endian */
    }

    return 0;                                         /* little endian */
}

void umesh_produce_random(uint8_t *random, uint32_t len)
{
    int i = 0;
    int time = aos_now_ms();
    aos_srand(time);
    for (i = 0; i < len; i ++) {
        random[i] = aos_rand() % 0xFF;
    }
}

int log_hex(const char *tag, const uint8_t *data, uint16_t len)
{
    int i;
    if (data == NULL || len == 0) {
        return UMESH_ERR_NULL_POINTER;
    }
    if (tag) {
        printf(tag);
        printf(":");
    }
    for (i = 0; i < len; i ++) {
        printf("%02x ", data[i]);
    }
    printf("\r\n");
    return 0;
}

/* host byte order to little endian */
uint16_t umesh_htole16(uint16_t data)
{
    if (os_is_big_endian()) {
        return reverse_16bit(data);
    }

    return data;
}

/* little endian to host byte order */
uint16_t umesh_le16toh(uint16_t data)
{
    return umesh_htole16(data);
}

/* host byte order to little endian */
uint32_t umesh_htole32(uint32_t data)
{
    if (os_is_big_endian()) {
        return reverse_32bit(data);
    }

    return data;
}

/* host byte order to little endian */
uint32_t umesh_le32toh(uint32_t data)
{
    if (os_is_big_endian()) {
        return reverse_32bit(data);
    }

    return data;
}

//////////

/* host byte order to big endian */
uint16_t umesh_htobe16(uint16_t data)
{
    if (!os_is_big_endian()) {
        return reverse_16bit(data);
    }

    return data;
}

/* big endian to host byte order */
uint16_t umesh_be16toh(uint16_t data)
{
    return umesh_htobe16(data);
}

/* host byte order to big endian */
uint32_t umesh_htobe32(uint32_t data)
{
    if (!os_is_big_endian()) {
        return reverse_32bit(data);
    }

    return data;
}

/* host byte order to big endian */
uint32_t umesh_be32toh(uint32_t data)
{
    if (!os_is_big_endian()) {
        return reverse_32bit(data);
    }

    return data;
}

int umesh_get_ucast_iv(uint8_t *r1, uint8_t *r2, uint8_t *out)
{
    int i;
    if (r1 == NULL || r2 == NULL || out == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    for (i = 0; i < UMESH_AES_KEY_LEN; i++) {
        out[i] = r1[i] ^ r2[i];
    }
    return 0;
}

int umesh_get_bcast_iv(uint8_t *r1, uint8_t *r2, uint8_t *out)
{
    int i;
    if (r1 == NULL || r2 == NULL || out == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    for (i = 0; i < UMESH_AES_KEY_LEN; i++) {
        out[i] = r1[i % IEEE80211_MAC_ADDR_LEN] ^ r2[i % 4];
    }
    return 0;
}
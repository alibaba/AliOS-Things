
#include <aos/kernel.h>
#include <stdint.h>
#include "define.h"
#include "osal.h"
#include "log.h"

/* reverse byte order */
static uint32_t reverse_32bit(uint32_t data)
{
    return ((data >> 24) &0xff) |  ((data >> 8) &0xff00) | ((data << 8) & 0xff0000) | ((data << 24) & 0xff000000);
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
        random[i] = aos_rand()%0xFF;
    }
}

int log_hex(const char *tag,const uint8_t *data, uint16_t len)
{
    int i;
    if(data == NULL || len == 0) {
        return UMESH_ERR_NULL_POINTER;
    }
    if(tag)
        printf(tag);
        printf(":");
    for (i = 0; i < len; i ++) {
        printf("%02x ",data[i]);
    }
    printf("\r\n");
    return 0;
}

int umesh_hex_to_str(uint8_t *src, int len, char *dst)
{
    int i = 0;
    if(src == NULL || len <= 0|| dst == NULL) {
        return UMESH_ERR_NULL_POINTER;
    }
    for(i = 0;i < len; i++) {
        snprintf(dst + 3*i,3, "%02x ",src +i);
    }
    return i*3;
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
#ifndef _DRIVER_AUDIO_IF_H_
#define _DRIVER_AUDIO_IF_H_
#include "sys_rtos.h"
#include "rtos_pub.h"

#define AUDIO_BUFF_LEN 	       (6144)

#define DRIVER_AUDIO_DEBUG

#ifdef DRIVER_AUDIO_DEBUG
#define DRIVER_AUDIO_PRT       os_printf
#else
#define DRIVER_AUDIO_PRT       os_null_printf
#endif

typedef struct _driver_ringbuff_s
{
    uint16_t buffer_len;
    uint16_t buffer_fill;
    uint16_t   wptr;
    uint16_t   rptr;
    uint8_t   *buffp;
} driver_ringbuff_t;

typedef struct
{
    uint8_t *data_buff;
    driver_ringbuff_t   aud_rb;
    uint32_t  channels;
} AUDIO_CTRL_BLK;
#endif
// eof


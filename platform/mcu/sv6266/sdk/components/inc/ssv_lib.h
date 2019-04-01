#ifndef _SSV_LIB_H_
#define _SSV_LIB_H_

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define os_memcpy memcpy
#define os_memset memset
#define os_strlen strlen


struct list_q {
    struct list_q   *next;
    struct list_q   *prev;
    unsigned int    qlen;
};

struct data_packet{
    struct list_q q;
    void *pbuf;
    int32_t next_hw; //next hw module
#if(DATA_QUEUE_DEBUG==1)
    int8_t idx;
#endif
};

struct data_queue{
    struct list_q qHead;
    uint16_t max_queue_len;
    bool out_lock; // FALSE: unlock, TRUE:lock
    bool int_lock; // FALSE: unlock, TRUE:lock
    uint8_t qID;
    bool out_auto_lock;// FALSE: disable, TRUE:enable
    uint8_t out_auto_lock_count;
    uint32_t count; //Monitor if the traffic doesn't stop
    uint32_t out_last_lock_time; //This variable is available when the out_lock is TRUE
    //u32 int_last_lock_time; //This variable is available when the int_lock is TRUE
};


#endif /* _SSV_LIB_H_ */


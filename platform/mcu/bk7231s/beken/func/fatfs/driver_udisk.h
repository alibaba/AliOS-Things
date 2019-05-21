#ifndef __DRIVER_USB_H__
#define __DRIVER_USB_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "include.h"
#include "ffconf.h"


#if CFG_USE_USB_HOST
#define USB_RET_OK			              0
#define USB_RET_ERROR 		              1
#define USB_RET_CONNECT 	              2
#define USB_RET_DISCONNECT                3
#define USB_RET_READ_OK 	              4
#define USB_RET_WRITE_OK 	              5

typedef struct __driver_udisk_s
{
    uint32        total_block;
    uint16		  block_size;
    uint16		  InitFlag;
} driver_udisk_t;

extern uint8 udisk_init(void);
extern void udisk_uninit(void);
extern uint32 udisk_get_size(void);
extern uint8 udisk_is_attached(void);
extern int udisk_rd_blk_sync(uint32 first_block, uint32 block_num, uint8 *dest );
extern int udisk_wr_blk_sync(uint32 first_block, uint32 block_num, uint8 *dest);
#endif


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif      /* __DRIVER_USB_H__ */

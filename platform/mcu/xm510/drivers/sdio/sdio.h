/*
 * File      : sdio.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __SDIO_H__
#define __SDIO_H__

#include "mmcsd_host.h"
#include "mmcsd_card.h"
#include "sdio_func_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Card Common Control Registers (CCCR)
 */

#define SDIO_REG_CCCR_CCCR_REV      0x00

#define  SDIO_CCCR_REV_1_00 0   /* CCCR/FBR Version 1.00 */
#define  SDIO_CCCR_REV_1_10 1   /* CCCR/FBR Version 1.10 */
#define  SDIO_CCCR_REV_1_20 2   /* CCCR/FBR Version 1.20 */
#define  SDIO_CCCR_REV_3_00 3   /* CCCR/FBR Version 2.00 */

#define  SDIO_SDIO_REV_1_00 0   /* SDIO Spec Version 1.00 */
#define  SDIO_SDIO_REV_1_10 1   /* SDIO Spec Version 1.10 */
#define  SDIO_SDIO_REV_1_20 2   /* SDIO Spec Version 1.20 */
#define  SDIO_SDIO_REV_2_00 3   /* SDIO Spec Version 2.00 */

#define SDIO_REG_CCCR_SD_REV        0x01

#define  SDIO_SD_REV_1_01   0   /* SD Physical Spec Version 1.01 */
#define  SDIO_SD_REV_1_10   1   /* SD Physical Spec Version 1.10 */
#define  SDIO_SD_REV_2_00   2   /* SD Physical Spec Version 2.00 */

#define SDIO_REG_CCCR_IO_EN         0x02
#define SDIO_REG_CCCR_IO_RDY        0x03

#define SDIO_REG_CCCR_INT_EN        0x04    /* Function/Master Interrupt Enable */
#define SDIO_REG_CCCR_INT_PEND      0x05    /* Function Interrupt Pending */

#define SDIO_REG_CCCR_IO_ABORT      0x06    /* function abort/card reset */

#define SDIO_REG_CCCR_BUS_IF        0x07    /* bus interface controls */

#define  SDIO_BUS_WIDTH_1BIT        0x00
#define  SDIO_BUS_WIDTH_4BIT        0x02
#define  SDIO_BUS_ECSI              0x20    /* Enable continuous SPI interrupt */
#define  SDIO_BUS_SCSI              0x40    /* Support continuous SPI interrupt */

#define  SDIO_BUS_ASYNC_INT         0x20

#define  SDIO_BUS_CD_DISABLE        0x80   /* disable pull-up on DAT3 (pin 1) */

#define SDIO_REG_CCCR_CARD_CAPS     0x08

#define  SDIO_CCCR_CAP_SDC  0x01    /* can do CMD52 while data transfer */
#define  SDIO_CCCR_CAP_SMB  0x02    /* can do multi-block xfers (CMD53) */
#define  SDIO_CCCR_CAP_SRW  0x04    /* supports read-wait protocol */
#define  SDIO_CCCR_CAP_SBS  0x08    /* supports suspend/resume */
#define  SDIO_CCCR_CAP_S4MI 0x10    /* interrupt during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_E4MI 0x20    /* enable ints during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_LSC  0x40    /* low speed card */
#define  SDIO_CCCR_CAP_4BLS 0x80    /* 4 bit low speed card */

#define SDIO_REG_CCCR_CIS_PTR       0x09    /* common CIS pointer (3 bytes) */

/* Following 4 regs are valid only if SBS is set */
#define SDIO_REG_CCCR_BUS_SUSPEND   0x0c
#define SDIO_REG_CCCR_FUNC_SEL      0x0d
#define SDIO_REG_CCCR_EXEC_FLAG     0x0e
#define SDIO_REG_CCCR_READY_FLAG    0x0f

#define SDIO_REG_CCCR_FN0_BLKSIZE   0x10 /* 2bytes, 0x10~0x11 */

#define SDIO_REG_CCCR_POWER_CTRL    0x12

#define  SDIO_POWER_SMPC            0x01    /* Supports Master Power Control */
#define  SDIO_POWER_EMPC            0x02    /* Enable Master Power Control */

#define SDIO_REG_CCCR_SPEED         0x13

#define  SDIO_SPEED_SHS             0x01    /* Supports High-Speed mode */
#define  SDIO_SPEED_EHS             0x02    /* Enable High-Speed mode */

/*
 * Function Basic Registers (FBR)
 */

#define SDIO_REG_FBR_BASE(f)    ((f) * 0x100) /* base of function f's FBRs */

#define SDIO_REG_FBR_STD_FUNC_IF    0x00

#define  SDIO_FBR_SUPPORTS_CSA      0x40    /* supports Code Storage Area */
#define  SDIO_FBR_ENABLE_CSA        0x80    /* enable Code Storage Area */

#define SDIO_REG_FBR_STD_IF_EXT     0x01

#define SDIO_REG_FBR_POWER          0x02

#define  SDIO_FBR_POWER_SPS         0x01    /* Supports Power Selection */
#define  SDIO_FBR_POWER_EPS         0x02    /* Enable (low) Power Selection */

#define SDIO_REG_FBR_CIS            0x09    /* CIS pointer (3 bytes) */


#define SDIO_REG_FBR_CSA            0x0C    /* CSA pointer (3 bytes) */

#define SDIO_REG_FBR_CSA_DATA       0x0F

#define SDIO_REG_FBR_BLKSIZE        0x10    /* block size (2 bytes) */

/* SDIO CIS Tuple code */
#define CISTPL_NULL      0x00
#define CISTPL_CHECKSUM  0x10
#define CISTPL_VERS_1    0x15
#define CISTPL_ALTSTR    0x16
#define CISTPL_MANFID    0x20
#define CISTPL_FUNCID    0x21
#define CISTPL_FUNCE     0x22
#define CISTPL_SDIO_STD  0x91
#define CISTPL_SDIO_EXT  0x92
#define CISTPL_END       0xff

/* SDIO device id */
#define SDIO_ANY_FUNC_ID    0xff
#define SDIO_ANY_MAN_ID     0xffff
#define SDIO_ANY_PROD_ID    0xffff

struct rt_sdio_device_id
{
    uint8_t   func_code;
    uint16_t  manufacturer;
    uint16_t  product;
};

struct rt_sdio_driver
{
    char *name;
    int32_t (*probe)(struct rt_mmcsd_card *card);
    int32_t (*remove)(struct rt_mmcsd_card *card);
    struct rt_sdio_device_id *id;
};

int32_t sdio_io_send_op_cond(struct rt_mmcsd_host *host,
                                uint32_t           ocr,
                                uint32_t          *cmd5_resp);
int32_t sdio_io_rw_direct(struct rt_mmcsd_card *card,
                             int32_t            rw,
                             uint32_t           fn,
                             uint32_t           reg_addr,
                             uint8_t           *pdata,
                             uint8_t            raw);
int32_t sdio_io_rw_extended(struct rt_mmcsd_card *card,
                               int32_t            rw,
                               uint32_t           fn,
                               uint32_t           addr,
                               int32_t            op_code,
                               uint8_t           *buf,
                               uint32_t           blocks,
                               uint32_t           blksize);
int32_t sdio_io_rw_extended_block(struct rt_sdio_function *func,
                              int32_t               rw,
                              uint32_t              addr,
                              int32_t               op_code,
                              uint8_t              *buf,
                              uint32_t              len);
uint8_t sdio_io_readb(struct rt_sdio_function *func, 
                         uint32_t              reg,
                         int32_t              *err);
int32_t sdio_io_writeb(struct rt_sdio_function *func, 
                          uint32_t              reg,
                          uint8_t               data);
uint16_t sdio_io_readw(struct rt_sdio_function *func,
                          uint32_t              addr,
                          int32_t              *err);
int32_t sdio_io_writew(struct rt_sdio_function *func,
                          uint16_t              data,
                          uint32_t              addr);
uint32_t sdio_io_readl(struct rt_sdio_function *func,
                          uint32_t              addr,
                          int32_t              *err);
int32_t sdio_io_writel(struct rt_sdio_function *func,
                          uint32_t              data,
                          uint32_t              addr);
int32_t sdio_io_read_multi_fifo_b(struct rt_sdio_function *func, 
                                     uint32_t              addr,
                                     uint8_t              *buf,
                                     uint32_t              len);
int32_t sdio_io_write_multi_fifo_b(struct rt_sdio_function *func, 
                                      uint32_t              addr,
                                      uint8_t              *buf,
                                      uint32_t              len);
int32_t sdio_io_read_multi_incr_b(struct rt_sdio_function *func, 
                                     uint32_t              addr,
                                     uint8_t              *buf,
                                     uint32_t              len);
int32_t sdio_io_write_multi_incr_b(struct rt_sdio_function *func, 
                                      uint32_t              addr,
                                      uint8_t              *buf,
                                      uint32_t              len);
int32_t init_sdio(struct rt_mmcsd_host *host, uint32_t ocr);
int32_t sdio_attach_irq(struct rt_sdio_function *func,
                           rt_sdio_irq_handler_t   *handler);
int32_t sdio_detach_irq(struct rt_sdio_function *func);
void sdio_irq_wakeup(struct rt_mmcsd_host *host);
int32_t sdio_enable_func(struct rt_sdio_function *func);
int32_t sdio_disable_func(struct rt_sdio_function *func);
void sdio_set_drvdata(struct rt_sdio_function *func, void *data);
void* sdio_get_drvdata(struct rt_sdio_function *func);
int32_t sdio_set_block_size(struct rt_sdio_function *func,
                               uint32_t              blksize);
int32_t sdio_register_driver(struct rt_sdio_driver *driver);
int32_t sdio_unregister_driver(struct rt_sdio_driver *driver);
void rt_sdio_init(void);

#ifdef __cplusplus
}
#endif

#endif


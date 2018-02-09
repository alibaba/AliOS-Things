/**
 * File: efc_si.h
 * Brief: bsp APIs of Sanechips efc
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author:
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _ZX29_EFC_H
#define _ZX29_EFC_H

/****************************************************************************
*                             Include files
****************************************************************************/
#include <stdint.h>
#include <drv_eflash.h>
#include <addrspace.h>
#include <soc.h>
#include <io.h>


/****************************************************************************
*                              Macros
****************************************************************************/
/***** CRPM *****/
#define SYS_TOP_CRPM_ADDR                   (TOP_CRPM_BASEADDR)
#define TOP_CRPM_EFLASH_CLK                 (SYS_TOP_CRPM_ADDR+0x6c)
/***** PMU ******/
#define SYS_PMU_SBY_ADDR                    (PMU_CTRL_BASEADDR)
/*****TOP_CRM+0x70******/
#define EFLASH_RESET_ADDR                   (SYS_TOP_CRPM_ADDR + 0x70)

#define EFC1_BASE_ADDR                      (EFLASH1_CTRL_BASEADDR)
#define EFC2_BASE_ADDR                      (EFLASH2_CTRL_BASEADDR)
#define EFLASH1_MAIN_START                  (EFLASH1_BASEADDR)
#define EFLASH2_MAIN_START                  (EFLASH2_BASEADDR)
#define EFLASH1_INFO_START                  (EFLASH1_INFO_BASEADDR)
#define EFLASH2_INFO_START                  (EFLASH2_INFO_BASEADDR)

#define EFLASH1_MAIN_SIZE                   (1216*1024)
#define EFLASH2_MAIN_SIZE                   (256*1024)
#define EFLASH1_PAGE_SIZE                   (2048)
#define EFLASH2_PAGE_SIZE                   (512)
#define EFLASH1_INFO_SIZE                   (2*2048)
#define EFLASH2_INFO_SIZE                   (2*512)
#define EFLASH1_FAST_SIZE                   (1024)
#define EFLASH2_FAST_SIZE                   (256)
#define EFLASH1_ALLIGN_SIZE                 (16)
#define EFLASH2_ALLIGN_SIZE                 (4)
/****reg seting*****/
#define EFLASH_RESET_SHIFT                  (0)
/*START/STATUS [0X28]*/
#define EFC_START_BUSY_FLAG                 BIT(0)
#define EFC_WRITE_ENABLE                    BIT(1)
#define EFC_ERR_ADDR                        BIT(3)
#define EFC_OTP_LOCKED                      BIT(6)
#define EFC_DATA_REQ                        BIT(8)
#define EFC_DATA_ACK                        BIT(30)
#define EFC_TIMING_UPT                      (31)
/*TIMING2  [0X30]*/
#define EFC_TIMING_ACR                      (16)
#define EFC_TIMING_AC                       (20)
#define EFC_TIMING_FASTPS                   (24)
/*LOCK  [0X34]*/
#define EFC_RECALL_PIN                      BIT(0)
/*INT_STATUS  [0X3C]*/
#define EFC_INT_TRANS_DONE                  BIT(0)
#define EFC_INT_DATA_REQ                    BIT(1)
/*BUF_CTRL    [0X80]*/
#define EFC_CLR_DSP_IBUF                    (0)
#define EFC_CLR_DSP_DBUF                    (1)
#define EFC_CLR_CK802_IBUF                  BIT(2)
#define EFC_CLR_CK802_DBUF                  BIT(3)
#define EFC_CLR_M0_BUF                      (4)
/*EFC_REGION_CFG  [0X108]*/
#define EFC_AP_ACCESS_PRIORITY              (0)
#define EFC_SECURE_CTRL                     (2)
#define EFC_SECURE_ENABLE                   (5)
/*EFLASH1_MPU_SEC_ENABLE  [0X154]*/
#define EFC1_MPU_SEC_ENABLE                 (0)
/*EFLASH2_CFG  [0X184]*/
#define EFC2_MPU_SEC_ENABLE                 (0)
/*EFLASH_CLKEN    [0X06C]*/
#define EFC1_CFG_SW_PCLKEN                  0
#define EFC1_CTRL_SW_CLKEN                  2
#define EFC2_CTRL_SW_CLKEN                  4
#define EFC1_CFG_CLK_REQEN                  1
#define EFC1_CTRL_CLK_REQEN                 3
#define EFC2_CLK_REQEN                      5

#define OP_EN_PASSWORD          (0x12345AA5)
#define EFC_DEV_NUM             (2)

/*trim position */
#define EFLASH1_TRIM_ADDR       (EFLASH1_INFO_START+EFLASH1_PAGE_SIZE)//eflash1 information_region 2nd page start
#define EFLASH2_TRIM_ADDR       (EFLASH2_INFO_START+EFLASH2_PAGE_SIZE)//eflash2 information_region 2nd page start
#define EFLASH_TRIM_NUM         3

/*otp position*/
#define EFLASH1_PAGE0_OTP                     (EFLASH1_INFO_START+EFLASH1_PAGE_SIZE-1)//eflash1 information_region 1st page last byte
#define EFLASH1_PAGE1_OTP                     (EFLASH1_INFO_START+EFLASH1_PAGE_SIZE*2-1)//eflash1 information_region 2nd page last byte
#define EFLASH2_PAGE0_OTP                     (EFLASH2_INFO_START+EFLASH2_PAGE_SIZE-1)//eflash2 information_region 1st page last byte
#define EFLASH2_PAGE1_OTP                     (EFLASH2_INFO_START+EFLASH2_PAGE_SIZE*2-1)//eflash2 information_region 2nd page last byte
#define OTP_LOCK_FLAG       (0)

#define ROUNDDOWN(a, b) ((a) & ~((b)-1))

#define IS_EFC1_MAIN_ADDR(addr)     ((addr >= EFLASH1_MAIN_START)&&(addr < EFLASH1_MAIN_START+EFLASH1_MAIN_SIZE))
#define IS_EFC1_INFO_ADDR(addr)     ((addr >= EFLASH1_INFO_START)&&(addr < EFLASH1_INFO_START+EFLASH1_INFO_SIZE))
#define IS_EFC2_MAIN_ADDR(addr)     ((addr >= EFLASH2_MAIN_START)&&(addr < EFLASH2_MAIN_START+EFLASH2_MAIN_SIZE))
#define IS_EFC2_INFO_ADDR(addr)     ((addr >= EFLASH2_INFO_START)&&(addr < EFLASH2_INFO_START+EFLASH2_INFO_SIZE))

#define IS_EFC_ADDR(addr) (IS_EFC1_MAIN_ADDR(addr) || IS_EFC1_INFO_ADDR(addr) \
                           || IS_EFC2_MAIN_ADDR(addr) || IS_EFC2_INFO_ADDR(addr))
/****************************************************************************
*                                             Types
****************************************************************************/

enum efc_si_dev_num {
    EFLASH_DEV1         = 0,
    EFLASH_DEV2         = 1,
    MAX_EFLASHDEV_NUM   = 2
};

enum efc_si_infopage_num {
    INFO_PAGE0          = 0,
    INFO_PAGE1          = 1,
    MAX_INFOPAGE_NUM    = 2
};

enum efc_si_core_type {
    CORE_DSP        = 8,
    CORE_CK802      = 9,
    CORE_M0         = 10,
    MAX_EFC_CORE    = 11
};

enum efc_si_opera_code {
    OP_CODE_SET_CONFIG   = 0,
    OP_CODE_SETUP       = 1,
    OP_CODE_PROGRAM     = 2,
    OP_CODE_PAGE_ERASE  = 3,
    OP_CODE_MACRO_ERASE = 4,
    OP_CODE_STANDBY     = 5
};

typedef struct {
    __IOM uint32_t version;                   /* 0x00 */
    __IOM uint32_t sec_flag;                  /* 0x04        [4:0]region0-4 flag ,[31]default flag,0:non-sec  1:sec*/
    __IOM uint32_t password;                  /* 0x08 */
    __IOM uint32_t op_code;                   /* 0x0c         [2:0]   000:set config  001:setup  010:program  011:page erase  100:macro erase    101:standby*/
    __IOM uint32_t op_addr;                   /* 0x10 */
    __IOM uint32_t op_len;                    /* 0x14 */
    __IOM uint32_t data[4];                   /* 0x18-0x24 */
    __IOM uint32_t start_status;              /* 0x28         [0]read as busy flag, when not busy write 1 will start operation, write 0 has no affection;
    __IOM                                                     [1]write enable   [2]command not support  [3]addr out of range  [4]master non-secure  [5]unwriteable zone
    __IOM                                                     [6]otp locked    [7]error standby req,already in standby mode  [8]data request in fast program
    __IOM                                                     [31]write 1 will refresh work clock domain timing parameter.write 0 has no affection.*/
    __IOM uint32_t timing1;                   /* 0x2c */
    __IOM uint32_t timing2;                   /* 0x30 */
    __IOM uint32_t lock;                      /* 0x34        [0]this bit controll recall pin status  [1]ctrl opera priority  [3:2]page0-1 lock reg,0 not lock,can be programmed*/
    __IOM uint32_t int_mask;                  /* 0x38        [0]transfer done int mask    [1]data req int mask,  write 1 to mask*/
    __IOM uint32_t int_status;                /* 0x3c        [0]transfer done int  [1]data req int,  write 1 to clear*/
    __IOM uint32_t debug;                     /* 0x40        [7:0]Operation state machine  [9:8]Power state machine*/
    __IOM uint32_t reserved0[15];
    __IOM uint32_t buf_ctrl;                  /* 0x80         [5:0]buf clear reg, bit0:DSP_I, bit1:DSP_D, bit2:CK_I, bit3:CK_D, bit4:M0, bit5:DMA   write 1 to clear.
    __IOM                                             [10:8]buffer prefetch reg, bit8:DSP_I, bit9:CK_I, bit10:M0     write 1 to enable*/
    __IOM uint32_t pri_cfg[7];                /* 0x84-0x9c     [3:0]default priority [7:4]highest priority [8]priority take effect flag*/
    __IOM uint32_t reserved1[24];             /* 0xA0-0xFC */
    __IOM uint32_t region0_cfg;               /* 0x100      [1:0]access priority  [2]1-secure aera, 0-nonsecure aera  [3]1-none executive  [5]read lock status, write 1 to lock*/
    __IOM uint32_t reserved2;
    __IOM uint32_t region1_cfg0;              /* 0x108      [1:0]access priority    [2]1-secure aera, 0-nonsecure aera    [3]1-none executive  [5]read lock status, write 1 to lock
    __IOM                                             [31:10]start address of region1*/
    __IOM uint32_t region1_cfg1;              /* 0x10c      [31:10]end address of region1*/
    __IOM uint32_t region2_cfg0;              /* 0x110 */
    __IOM uint32_t region2_cfg1;              /* 0x114 */
    __IOM uint32_t region3_cfg0;              /* 0x118 */
    __IOM uint32_t region3_cfg1;              /* 0x11c */
    __IOM uint32_t region4_cfg0;              /* 0x120 */
    __IOM uint32_t region4_cfg1;              /* 0x124 */
} efc_si_regs;

typedef struct {
    uint32_t idx;
    uint32_t fast_write_size;
    uint32_t write_allign_size;
    uint32_t page_size;
    uint32_t main_size;
    uint32_t main_base;
    uint32_t info_size;
    uint32_t info_base;
    uint32_t irq_line;
    uint32_t copy_flag;
    uint32_t poll_mode;
    eflash_event_cb_t cb;
    eflash_status_t status;
    efc_si_regs *reg;
    uint8_t *page_buffer;
} efc_si_dev_data;

#endif

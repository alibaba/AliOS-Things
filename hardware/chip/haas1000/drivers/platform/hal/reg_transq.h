/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_TRANSQ_H__
#define __REG_TRANSQ_H__

#include "plat_types.h"

#define TRANSQ_SLOT_NUM                 32

struct TRANSQ_T
{
    __IO uint32_t CTRL;                 // 0x000
    __IO uint32_t RMT_INTMASK;          // 0x004
    __IO uint32_t RMT_INTSET;           // 0x008
    __IO uint32_t LDONE_INTMASK;        // 0x00C
    union {
        __I  uint32_t LDONE_RIS;        // 0x010
        __O  uint32_t LDONE_INTCLR;     // 0x010
    } LDONE_ISC;
    __IO uint32_t LERR_INTMASK;         // 0x014
    union {
        __I  uint32_t LERR_RIS;         // 0x018
        __O  uint32_t LERR_INTCLR;      // 0x018
    } LERR_ISC;
    __IO uint32_t RESERVED_01C;         // 0x01C
    __I  uint32_t LDONE_MIS;            // 0x020
    __I  uint32_t LERR_MIS;             // 0x024
    __IO uint32_t RESERVED_028[2];      // 0x028
    struct {
        __IO uint32_t ADDR;             // 0x030 + N * 8
        __IO uint32_t LEN;              // 0x038 + N * 8
    } WSLOT[TRANSQ_SLOT_NUM];
    __IO uint32_t RESERVED_130[0x34];   // 0x130
    union {
        __I  uint32_t RMT_RIS;          // 0x200
        __O  uint32_t RMT_INTCLR;       // 0x200
    } RMT_ISC;
    __IO uint32_t RMT_MIS;              // 0x204
    __IO uint32_t RESERVED_208[10];     // 0x208
    struct {
        __I  uint32_t ADDR;             // 0x230 + N * 8
        __I  uint32_t LEN;              // 0x238 + N * 8
    } RSLOT[TRANSQ_SLOT_NUM];
};

// CTRL
#define CTRL_REMOTE_IRQ_EN          (1 << 0)
#define CTRL_LOCAL_DONE_IRQ_EN      (1 << 1)
#define CTRL_LOCAL_ERR_IRQ_EN       (1 << 2)

#define TRANSQ_BIT(n)               (1 << (n))

#endif

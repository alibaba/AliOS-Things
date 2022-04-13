/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SDMMCIP_REG_HW_H
#define __SDMMCIP_REG_HW_H

#include "plat_types.h"

#define SDMMCIP_REG_CTRL     0x000
#define SDMMCIP_REG_PWREN        0x004
#define SDMMCIP_REG_CLKDIV       0x008
#define SDMMCIP_REG_CLKSRC       0x00C
#define SDMMCIP_REG_CLKENA       0x010
#define SDMMCIP_REG_TMOUT        0x014
#define SDMMCIP_REG_CTYPE        0x018
#define SDMMCIP_REG_BLKSIZ       0x01C
#define SDMMCIP_REG_BYTCNT       0x020
#define SDMMCIP_REG_INTMASK      0x024
#define SDMMCIP_REG_CMDARG       0x028
#define SDMMCIP_REG_CMD      0x02C
#define SDMMCIP_REG_RESP0        0x030
#define SDMMCIP_REG_RESP1        0x034
#define SDMMCIP_REG_RESP2        0x038
#define SDMMCIP_REG_RESP3        0x03C
#define SDMMCIP_REG_MINTSTS      0x040
#define SDMMCIP_REG_RINTSTS      0x044
#define SDMMCIP_REG_STATUS       0x048
#define SDMMCIP_REG_FIFOTH       0x04C
#define SDMMCIP_REG_CDETECT      0x050
#define SDMMCIP_REG_WRTPRT       0x054
#define SDMMCIP_REG_GPIO     0x058
#define SDMMCIP_REG_TCMCNT       0x05C
#define SDMMCIP_REG_TBBCNT       0x060
#define SDMMCIP_REG_DEBNCE       0x064
#define SDMMCIP_REG_USRID        0x068
#define SDMMCIP_REG_VERID        0x06C
#define SDMMCIP_REG_HCON     0x070
#define SDMMCIP_REG_UHS_REG      0x074
#define SDMMCIP_REG_RESET_CARD      0x078
#define SDMMCIP_REG_BMOD     0x080
#define SDMMCIP_REG_PLDMND       0x084
#define SDMMCIP_REG_DBADDR       0x088
#define SDMMCIP_REG_IDSTS        0x08C
#define SDMMCIP_REG_IDINTEN      0x090
#define SDMMCIP_REG_DSCADDR      0x094
#define SDMMCIP_REG_BUFADDR      0x098
#define SDMMCIP_REG_DATA     0x200

/* Interrupt Mask register */
#define SDMMCIP_REG_INTMSK_ALL   0xffffffff
#define SDMMCIP_REG_INTMSK_CD    (1 << 0)
#define SDMMCIP_REG_INTMSK_RE    (1 << 1)
#define SDMMCIP_REG_INTMSK_CDONE (1 << 2)
#define SDMMCIP_REG_INTMSK_DTO   (1 << 3)
#define SDMMCIP_REG_INTMSK_TXDR  (1 << 4)
#define SDMMCIP_REG_INTMSK_RXDR  (1 << 5)
#define SDMMCIP_REG_INTMSK_DCRC  (1 << 7)
#define SDMMCIP_REG_INTMSK_RTO   (1 << 8)
#define SDMMCIP_REG_INTMSK_DRTO  (1 << 9)
#define SDMMCIP_REG_INTMSK_HTO   (1 << 10)
#define SDMMCIP_REG_INTMSK_FRUN  (1 << 11)
#define SDMMCIP_REG_INTMSK_HLE   (1 << 12)
#define SDMMCIP_REG_INTMSK_SBE   (1 << 13)
#define SDMMCIP_REG_INTMSK_ACD   (1 << 14)
#define SDMMCIP_REG_INTMSK_EBE   (1 << 15)

/* Raw interrupt Regsiter */
#define SDMMCIP_REG_DATA_ERR (SDMMCIP_REG_INTMSK_EBE | SDMMCIP_REG_INTMSK_SBE | SDMMCIP_REG_INTMSK_HLE |\
            SDMMCIP_REG_INTMSK_FRUN | SDMMCIP_REG_INTMSK_EBE | SDMMCIP_REG_INTMSK_DCRC)
#define SDMMCIP_REG_DATA_TOUT    (SDMMCIP_REG_INTMSK_HTO | SDMMCIP_REG_INTMSK_DRTO)
/* CTRL register */
#define SDMMCIP_REG_CTRL_RESET   (1 << 0)
#define SDMMCIP_REG_CTRL_FIFO_RESET  (1 << 1)
#define SDMMCIP_REG_CTRL_DMA_RESET   (1 << 2)
#define SDMMCIP_REG_INT_EN       (1 << 4)
#define SDMMCIP_REG_DMA_EN       (1 << 5)
#define SDMMCIP_REG_CTRL_SEND_AS_CCSD    (1 << 10)
#define SDMMCIP_REG_IDMAC_EN     (1 << 25)
#define SDMMCIP_REG_RESET_ALL        (SDMMCIP_REG_CTRL_RESET | SDMMCIP_REG_CTRL_FIFO_RESET |\
                SDMMCIP_REG_CTRL_DMA_RESET)

/* CMD register */
#define SDMMCIP_REG_CMD_RESP_EXP (1 << 6)
#define SDMMCIP_REG_CMD_RESP_LENGTH  (1 << 7)
#define SDMMCIP_REG_CMD_CHECK_CRC    (1 << 8)
#define SDMMCIP_REG_CMD_DATA_EXP (1 << 9)
#define SDMMCIP_REG_CMD_RW       (1 << 10)
#define SDMMCIP_REG_CMD_SEND_STOP    (1 << 12)
#define SDMMCIP_REG_CMD_ABORT_STOP   (1 << 14)
#define SDMMCIP_REG_CMD_PRV_DAT_WAIT (1 << 13)
#define SDMMCIP_REG_CMD_UPD_CLK  (1 << 21)
#define SDMMCIP_REG_CMD_USE_HOLD_REG (1 << 29)
#define SDMMCIP_REG_CMD_START        (1 << 31)

/* CLKENA register */
#define SDMMCIP_REG_CLKEN_ENABLE (1 << 0)
#define SDMMCIP_REG_CLKEN_LOW_PWR    (1 << 16)

/* Card-type registe */
#define SDMMCIP_REG_CTYPE_1BIT   0
#define SDMMCIP_REG_CTYPE_4BIT   (1 << 0)
#define SDMMCIP_REG_CTYPE_8BIT   (1 << 16)

/* Status Register */
#define SDMMCIP_REG_BUSY     (1 << 9)
#define SDMMCIP_REG_FIFO_FULL        (1 << 3)
#define SDMMCIP_REG_FIFO_EMPTY       (1 << 2)
#define SDMMCIP_REG_FIFO_COUNT_SHIFT     (17)
#define SDMMCIP_REG_FIFO_COUNT_MASK      (0x1fff << SDMMCIP_REG_FIFO_COUNT_SHIFT)

/* FIFOTH Register */
#define MSIZE(x)        ((x) << 28)
#define RX_WMARK(x)     ((x) << 16)
#define TX_WMARK(x)     (x)
#define RX_WMARK_SHIFT      16
#define RX_WMARK_MASK       (0xfff << RX_WMARK_SHIFT)

#define SDMMCIP_REG_IDMAC_OWN        (1 << 31)
#define SDMMCIP_REG_IDMAC_CH     (1 << 4)
#define SDMMCIP_REG_IDMAC_FS     (1 << 3)
#define SDMMCIP_REG_IDMAC_LD     (1 << 2)

/*  Bus Mode Register */
#define SDMMCIP_REG_BMOD_IDMAC_RESET (1 << 0)
#define SDMMCIP_REG_BMOD_IDMAC_FB    (1 << 1)
#define SDMMCIP_REG_BMOD_IDMAC_EN    (1 << 7)

/* UHS register */
#define SDMMCIP_REG_DDR_MODE (1 << 16)

/* quirks */
#define SDMMCIP_REG_QUIRK_DISABLE_SMU        (1 << 0)

/* FIFO Register */
#define SDMMCIP_REG_FIFO_OFFSET 0x200

#endif  /* __SDMMCIP_REG_HW_H */

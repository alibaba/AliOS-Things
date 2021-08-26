/***************************************************************************
 *
 * Copyright 2015-2020 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __REG_SEC_ENG_H__
#define __REG_SEC_ENG_H__

#include "plat_types.h"
#include "plat_addr_map.h"

#define SE_ADEC_BASE                        (SEC_ENG_BASE + 0x0400)
#define SE_DMACFG_BASE                      (SEC_ENG_BASE + 0x0800)
#define SE_ACB_BASE                         (SEC_ENG_BASE + 0x0C00)
#define SE_CRYPT_BASE                       (SEC_ENG_BASE + 0x1000)
#define SE_ECP_BASE                         (SEC_ENG_BASE + 0x1400)
#define SE_HASH_BASE                        (SEC_ENG_BASE + 0x1800)
#define SE_SCRATCH_BASE                     (SEC_ENG_BASE + 0x1C00)
#define SE_ZMODP_BASE                       (SEC_ENG_BASE + 0x2000)
#define SE_MCT_BASE                         (SEC_ENG_BASE + 0x2400)
#define SE_OTP_BASE                         (SEC_ENG_BASE + 0x2800)
#define SE_EBG_BASE                         (SEC_ENG_BASE + 0x2C00)

#define SE_DMA_RX_FIFO_DEPTH                32
#define SE_DMA_TX_FIFO_DEPTH                16

struct SE_ADEC_T {
    __IO uint32_t ADEC_CTRL;                // 0x000
    __IO uint32_t ADEC_CTRL2;               // 0x004
    __IO uint32_t RESERVED_00C;             // 0x008
    __IO uint32_t ADEC_INT;                 // 0x00C
    __IO uint32_t ADEC_INT_MSK;             // 0x010
    __IO uint32_t ADEC_ERR_ADR;             // 0x014
    __IO uint32_t RESERVED_018;             // 0x018
};

struct SE_DMACFG_T {
    __IO uint32_t DMA_CTRL;                 // 0x000
    __IO uint32_t DMA_TDL;                  // 0x004
    __IO uint32_t DMA_RDL;                  // 0x008
    __IO uint32_t DMA_STATUS;               // 0x00C
    __IO uint32_t DMA_TXFIFO;               // 0x010
    __IO uint32_t DMA_RXFIFO;               // 0x014
    __IO uint32_t DMA_FIFOCLR;              // 0x018
};

struct SE_ACB_T {
    __IO uint32_t ACB_CTRL;                 // 0x000
};

struct SE_CRYPT_T {
    __IO uint32_t AES_CFG;                  // 0x000
    __IO uint32_t AES_CTRL;                 // 0x004
    __IO uint32_t AES_CMD;                  // 0x008
    __IO uint32_t AES_STATUS;               // 0x00C
    __IO uint32_t AES_INTRPT;               // 0x010
    __IO uint32_t AES_INTRPT_SRC_EN;        // 0x014
    __IO uint32_t AES_STREAM_SIZE;          // 0x018
    __IO uint32_t DES_CFG;                  // 0x01C
    __IO uint32_t DES_CTRL;                 // 0x020
    __IO uint32_t DES_CMD;                  // 0x024
    __IO uint32_t DES_STATUS;               // 0x028
    __IO uint32_t DES_INTRPT;               // 0x02C
    __IO uint32_t DES_INTRPT_SRC_EN;        // 0x030
    __IO uint32_t DES_STREAM_SIZE;          // 0x034
    __IO uint32_t RC4_CFG;                  // 0x038
    __IO uint32_t RC4_CTRL;                 // 0x03C
    __IO uint32_t RC4_CMD;                  // 0x040
    __IO uint32_t RC4_STATUS;               // 0x044
    __IO uint32_t RC4_INTRPT;               // 0x048
    __IO uint32_t RC4_INTRPT_SRC_EN;        // 0x04C
    __IO uint32_t RC4_STREAM_SIZE;          // 0x050
    __IO uint32_t RC4_INDEX;                // 0x054
    __IO uint32_t KEY2[8];                  // 0x058
    __IO uint32_t KEY1[8];                  // 0x078
    __IO uint32_t IV[4];                    // 0x098
    __IO uint32_t ENGINE_SELECT;            // 0x0A8
};

struct SE_ECP_T {
    __IO uint32_t ECP_CFG;                  // 0x000
    __IO uint32_t ECP_CTRL;                 // 0x004
    __IO uint32_t ECP_CMD;                  // 0x008
    __IO uint32_t ECP_INTRPT;               // 0x00C
    __IO uint32_t ECP_STATUS;               // 0x010
    __IO uint32_t ECP_INTRPT_MASK;          // 0x014
    __IO uint32_t ECP_SRAM[136];            // 0x018
};

struct SE_HASH_T {
    __IO uint32_t HASH_CFG;                 // 0x000
    __IO uint32_t HASH_CTRL;                // 0x004
    __IO uint32_t HASH_CMD;                 // 0x008
    __IO uint32_t HASH_STATUS;              // 0x00C
    __IO uint32_t HASH_SEG_SIZE;            // 0x010
    __IO uint32_t RESERVED_014;             // 0x014
    __IO uint32_t HASH_MSG_SIZE_L;          // 0x018
    __IO uint32_t HASH_MSG_SIZE_H;          // 0x01C
    __IO uint32_t HASH_CTX[8];              // 0x020
    __IO uint32_t HASH_CTX_H[8];            // 0x040
    __IO uint32_t HMAC_CTX[8];              // 0x060
    __IO uint32_t HMAC_CTX_H[8];            // 0x080
    __IO uint32_t HMAC_KEY_LEN;             // 0x0A0
    __IO uint32_t HMAC_KEY[32];             // 0x0A4
};

struct SE_SCRATCH_T {
    __IO uint32_t SP_CTRL;                  // 0x000
    __IO uint32_t SP_STATUS;                // 0x004
    __IO uint32_t SP_CPU_WR_ADDR;           // 0x008
    __IO uint32_t SP_CPU_WR_DATA;           // 0x00C
    __IO uint32_t SP_CPU_RD_ADDR;           // 0x010
    __IO uint32_t SP_CPU_RD_DATA;           // 0x014
    __IO uint32_t SP_SET_WR_ADDR;           // 0x018
    __IO uint32_t SP_SET_RD_ADDR;           // 0x01C
};

struct SE_ZMODP_T {
    __IO uint32_t ZMODP_CFG;                // 0x000
    __IO uint32_t ZMODP_CTRL;               // 0x004
    __IO uint32_t ZMODP_CMD;                // 0x008
    __IO uint32_t ZMODP_STATUS;             // 0x00C
    __IO uint32_t ZMODP_INTRPT_SRC;         // 0x010
    __IO uint32_t ZMODP_INTRPT_SRC_EN;      // 0x014
    __IO uint32_t ZMODP_MEM[196];           // 0x018
};

struct SE_MCT_T {
    __IO uint32_t MCT_CTRL;                 // 0x000
    __IO uint32_t MCT_CMD;                  // 0x004
    __IO uint32_t MCT_STATUS;               // 0x008
    __IO uint32_t MCT_INTRPT;               // 0x00C
    __IO uint32_t MCT_INTRPT_MASK;          // 0x010
    __IO uint32_t MCT_VAL[5];               // 0x014
};

struct SE_OTP_T {
    __IO uint32_t OTP[16];                  // 0x000
    __IO uint32_t RESERVED_040[18];         // 0x040
    __IO uint32_t CRYPT_KEY_CNTR;           // 0x088
    __IO uint32_t EBG_RNG_CTRL;             // 0x08C
};

struct SE_EBG_T {
    __IO uint32_t EBG_CTRL;                 // 0x000
    __IO uint32_t EBG_ENTROPY;              // 0x004
};

// ADEC_CTRL
#define ADEC_CTRL_CLK_EN_15_0_SHIFT         16
#define ADEC_CTRL_CLK_EN_15_0_MASK          (0xFFFF << ADEC_CTRL_CLK_EN_15_0_SHIFT)
#define ADEC_CTRL_CLK_EN_15_0(n)            BITFIELD_VAL(ADEC_CTRL_CLK_EN_15_0, n)
#define ADEC_CTRL_RST_15_0_SHIFT            0
#define ADEC_CTRL_RST_15_0_MASK             (0xFFFF << ADEC_CTRL_RST_15_0_SHIFT)
#define ADEC_CTRL_RST_15_0(n)               BITFIELD_VAL(ADEC_CTRL_RST_15_0, n)

// ADEC_CTRL2
#define ADEC_CTRL2_CLK_EN_20_16_SHIFT       16
#define ADEC_CTRL2_CLK_EN_20_16_MASK        (0x1F << ADEC_CTRL2_CLK_EN_20_16_SHIFT)
#define ADEC_CTRL2_CLK_EN_20_16(n)          BITFIELD_VAL(ADEC_CTRL2_CLK_EN_20_16, n)
#define ADEC_CTRL2_RST_20_16_SHIFT          0
#define ADEC_CTRL2_RST_20_16_MASK           (0x1F << ADEC_CTRL2_RST_20_16_SHIFT)
#define ADEC_CTRL2_RST_20_16(n)             BITFIELD_VAL(ADEC_CTRL2_RST_20_16, n)

// DMA_CTRL
#define DMA_CTRL_RX_DMA_SINGLE_DIS          (1 << 3)
#define DMA_CTRL_TX_DMA_SINGLE_DIS          (1 << 2)
#define DMA_CTRL_RX_DMA_EN                  (1 << 1)
#define DMA_CTRL_TX_DMA_EN                  (1 << 0)

// DMA_FIFOCLR
#define DMA_FIFOCLR_RXFIFO_CLR              (1 << 1)
#define DMA_FIFOCLR_TXFIFO_CLR              (1 << 0)

// DMA_STATUS
#define DMA_STATUS_RX_VALID_CNT_SHIFT       10
#define DMA_STATUS_RX_VALID_CNT_MASK        (0x1F << DMA_STATUS_RX_VALID_CNT_SHIFT)
#define DMA_STATUS_RX_VALID_CNT(n)          BITFIELD_VAL(DMA_STATUS_RX_VALID_CNT, n)
#define DMA_STATUS_RX_FULL                  (1 << 9)
#define DMA_STATUS_RX_EMPTY                 (1 << 7)
#define DMA_STATUS_TX_VALID_CNT_SHIFT       2
#define DMA_STATUS_TX_VALID_CNT_MASK        (0x1F << DMA_STATUS_TX_VALID_CNT_SHIFT)
#define DMA_STATUS_TX_VALID_CNT(n)          BITFIELD_VAL(DMA_STATUS_TX_VALID_CNT, n)
#define DMA_STATUS_TX_FULL                  (1 << 1)
#define DMA_STATUS_TX_EMPTY                 (1 << 0)

// ACB_CTRL
#define ACB_CTRL_RSVD_31_11_SHIFT           11
#define ACB_CTRL_RSVD_31_11_MASK            (0x1FFFFF << ACB_CTRL_RSVD_31_11_SHIFT)
#define ACB_CTRL_RSVD_31_11(n)              BITFIELD_VAL(ACB_CTRL_RSVD_31_11, n)
#define ACB_CTRL_GROUPB_SELECT_SHIFT        8
#define ACB_CTRL_GROUPB_SELECT_MASK         (0x7 << ACB_CTRL_GROUPB_SELECT_SHIFT)
#define ACB_CTRL_GROUPB_SELECT(n)           BITFIELD_VAL(ACB_CTRL_GROUPB_SELECT, n)
#define ACB_CTRL_RSVD_7                     (1 << 7)
#define ACB_CTRL_GROUPA_SELECT_SHIFT        4
#define ACB_CTRL_GROUPA_SELECT_MASK         (0x7 << ACB_CTRL_GROUPA_SELECT_SHIFT)
#define ACB_CTRL_GROUPA_SELECT(n)           BITFIELD_VAL(ACB_CTRL_GROUPA_SELECT, n)
#define ACB_CTRL_RSVD_3                     (1 << 3)
#define ACB_CTRL_WR_CB_CTRL                 (1 << 2)
#define ACB_CTRL_RSVD_1                     (1 << 1)
#define ACB_CTRL_RD_CB_CTRL                 (1 << 0)

// AES_CFG
#define AES_CFG_RSVD_31_17_SHIFT            17
#define AES_CFG_RSVD_31_17_MASK             (0x7FFF << AES_CFG_RSVD_31_17_SHIFT)
#define AES_CFG_RSVD_31_17(n)               BITFIELD_VAL(AES_CFG_RSVD_31_17, n)
#define AES_CFG_CTS_MODE                    (1 << 16)
#define AES_CFG_RSVD                        (1 << 15)
#define AES_CFG_MODULAR_SHIFT               8
#define AES_CFG_MODULAR_MASK                (0x7F << AES_CFG_MODULAR_SHIFT)
#define AES_CFG_MODULAR(n)                  BITFIELD_VAL(AES_CFG_MODULAR, n)
#define AES_CFG_RSVD_7                      (1 << 7)
#define AES_CFG_RKEY                        (1 << 6)
#define AES_CFG_MODE_SHIFT                  3
#define AES_CFG_MODE_MASK                   (0x7 << AES_CFG_MODE_SHIFT)
#define AES_CFG_MODE(n)                     BITFIELD_VAL(AES_CFG_MODE, n)
#define AES_CFG_KEYLEN_SHIFT                1
#define AES_CFG_KEYLEN_MASK                 (0x3 << AES_CFG_KEYLEN_SHIFT)
#define AES_CFG_KEYLEN(n)                   BITFIELD_VAL(AES_CFG_KEYLEN, n)
#define AES_CFG_DECRYPT                     (1 << 0)

// AES_CTRL
#define AES_CTRL_RSVD_31_3_SHIFT            3
#define AES_CTRL_RSVD_31_3_MASK             (0x1FFFFFFF << AES_CTRL_RSVD_31_3_SHIFT)
#define AES_CTRL_RSVD_31_3(n)               BITFIELD_VAL(AES_CTRL_RSVD_31_3, n)
#define AES_CTRL_OUTPUTBLOCK                (1 << 2)
#define AES_CTRL_RESUME                     (1 << 1)
#define AES_CTRL_RESET                      (1 << 0)

// AES_CMD
#define AES_CMD_RSVD_31_1_SHIFT             1
#define AES_CMD_RSVD_31_1_MASK              (0x7FFFFFFF << AES_CMD_RSVD_31_1_SHIFT)
#define AES_CMD_RSVD_31_1(n)                BITFIELD_VAL(AES_CMD_RSVD_31_1, n)
#define AES_CMD_START                       (1 << 0)

// AES_STATUS
#define AES_STATUS_RSVD_31_5_SHIFT          5
#define AES_STATUS_RSVD_31_5_MASK           (0x7FFFFFF << AES_STATUS_RSVD_31_5_SHIFT)
#define AES_STATUS_RSVD_31_5(n)             BITFIELD_VAL(AES_STATUS_RSVD_31_5, n)
#define AES_STATUS_STATE_SHIFT              1
#define AES_STATUS_STATE_MASK               (0xF << AES_STATUS_STATE_SHIFT)
#define AES_STATUS_STATE(n)                 BITFIELD_VAL(AES_STATUS_STATE, n)
#define AES_STATUS_BUSY                     (1 << 0)

// AES_INTRPT
#define AES_INTRPT_RSVD_31_3_SHIFT          3
#define AES_INTRPT_RSVD_31_3_MASK           (0x1FFFFFFF << AES_INTRPT_RSVD_31_3_SHIFT)
#define AES_INTRPT_RSVD_31_3(n)             BITFIELD_VAL(AES_INTRPT_RSVD_31_3, n)
#define AES_INTRPT_ERROR2                   (1 << 2)
#define AES_INTRPT_ERROR1                   (1 << 1)
#define AES_INTRPT_DONE                     (1 << 0)

// AES_INTRPT_SRC_EN
#define AES_INTRPT_SRC_EN_RSVD_31_3_SHIFT   3
#define AES_INTRPT_SRC_EN_RSVD_31_3_MASK    (0x1FFFFFFF << AES_INTRPT_SRC_EN_RSVD_31_3_SHIFT)
#define AES_INTRPT_SRC_EN_RSVD_31_3(n)      BITFIELD_VAL(AES_INTRPT_SRC_EN_RSVD_31_3, n)
#define AES_INTRPT_SRC_EN_ERROR2            (1 << 2)
#define AES_INTRPT_SRC_EN_ERROR1            (1 << 1)
#define AES_INTRPT_SRC_EN_DONE              (1 << 0)

// ENGINE_SELECT
#define ENGINE_SELECT_RSVD_31_2_SHIFT       2
#define ENGINE_SELECT_RSVD_31_2_MASK        (0x3FFFFFFF << ENGINE_SELECT_RSVD_31_2_SHIFT)
#define ENGINE_SELECT_RSVD_31_2(n)          BITFIELD_VAL(ENGINE_SELECT_RSVD_31_2, n)
#define ENGINE_SELECT_SELECT_SHIFT          0
#define ENGINE_SELECT_SELECT_MASK           (0x3 << ENGINE_SELECT_SELECT_SHIFT)
#define ENGINE_SELECT_SELECT(n)             BITFIELD_VAL(ENGINE_SELECT_SELECT, n)

// HASH_CFG
#define HASH_CFG_RSVD_31_5_SHIFT            5
#define HASH_CFG_RSVD_31_5_MASK             (0x7FFFFFF << HASH_CFG_RSVD_31_5_SHIFT)
#define HASH_CFG_RSVD_31_5(n)               BITFIELD_VAL(HASH_CFG_RSVD_31_5, n)
#define HASH_CFG_OUTPUT_DATA_MODE           (1 << 4)
#define HASH_CFG_HASH_MODE                  (1 << 3)
#define HASH_CFG_ALG_SELECT_SHIFT           0
#define HASH_CFG_ALG_SELECT_MASK            (0x7 << HASH_CFG_ALG_SELECT_SHIFT)
#define HASH_CFG_ALG_SELECT(n)              BITFIELD_VAL(HASH_CFG_ALG_SELECT, n)

// HASH_CTRL
#define HASH_CTRL_RSVD_31_4_SHIFT           4
#define HASH_CTRL_RSVD_31_4_MASK            (0xFFFFFFF << HASH_CTRL_RSVD_31_4_SHIFT)
#define HASH_CTRL_RSVD_31_4(n)              BITFIELD_VAL(HASH_CTRL_RSVD_31_4, n)
#define HASH_CTRL_RESET                     (1 << 3)
#define HASH_CTRL_HW_PADDING                (1 << 2)
#define HASH_CTRL_HASH_OP_MODE_SHIFT        0
#define HASH_CTRL_HASH_OP_MODE_MASK         (0x3 << HASH_CTRL_HASH_OP_MODE_SHIFT)
#define HASH_CTRL_HASH_OP_MODE(n)           BITFIELD_VAL(HASH_CTRL_HASH_OP_MODE, n)

// HASH_CMD
#define HASH_CMD_RSVD_31_1_SHIFT            1
#define HASH_CMD_RSVD_31_1_MASK             (0x7FFFFFFF << HASH_CMD_RSVD_31_1_SHIFT)
#define HASH_CMD_RSVD_31_1(n)               BITFIELD_VAL(HASH_CMD_RSVD_31_1, n)
#define HASH_CMD_START                      (1 << 0)

// HASH_STATUS
#define HASH_STATUS_RSVD_31_4_SHIFT         4
#define HASH_STATUS_RSVD_31_4_MASK          (0xFFFFFFF << HASH_STATUS_RSVD_31_4_SHIFT)
#define HASH_STATUS_RSVD_31_4(n)            BITFIELD_VAL(HASH_STATUS_RSVD_31_4, n)
#define HASH_STATUS_HMAC_SM_BUSY            (1 << 3)
#define HASH_STATUS_XFER_SM_BUSY            (1 << 2)
#define HASH_STATUS_HASH_BUSY               (1 << 1)
#define HASH_STATUS_HASH_DONE               (1 << 0)

// HMAC_KEY_LEN
#define HMAC_KEY_LEN_RSVD_31_8_SHIFT        8
#define HMAC_KEY_LEN_RSVD_31_8_MASK         (0xFFFFFF << HMAC_KEY_LEN_RSVD_31_8_SHIFT)
#define HMAC_KEY_LEN_RSVD_31_8(n)           BITFIELD_VAL(HMAC_KEY_LEN_RSVD_31_8, n)
#define HMAC_KEY_LEN_LEN_SHIFT              0
#define HMAC_KEY_LEN_LEN_MASK               (0xFF << HMAC_KEY_LEN_LEN_SHIFT)
#define HMAC_KEY_LEN_LEN(n)                 BITFIELD_VAL(HMAC_KEY_LEN_LEN, n)

// CRYPT_KEY_CNTR
#define CRYPT_KEY_CNTR_KEY_SEL              (1 << 2)
#define CRYPT_KEY_CNTR_DEV1_ENCRYPT         (1 << 1)
#define CRYPT_KEY_CNTR_DEV0_ENCRYPT         (1 << 0)

// EBG_RNG_CTRL
#define EBG_RNG_CTRL_RNG_SEL                (1 << 16)
#define EBG_RNG_CTRL_SEED_SHIFT             0
#define EBG_RNG_CTRL_SEED_MASK              (0xFFFF << EBG_RNG_CTRL_SEED_SHIFT)
#define EBG_RNG_CTRL_SEED(n)                BITFIELD_VAL(EBG_RNG_CTRL_SEED, n)



#endif


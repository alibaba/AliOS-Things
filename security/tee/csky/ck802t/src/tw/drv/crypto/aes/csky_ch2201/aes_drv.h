/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 **/

#ifndef _AES_DRV_H_
#define _AES_DRV_H_

#include "tee_types.h"
#include "hal_memmap.h"

typedef struct _aes_reg_t
{
    uint32_t Din[4];  /* Data input 0~127         */
    uint32_t KEY[8];  /* Key 0~255                */
    uint32_t IV[4];   /* Initial Vector: 0~127    */
    uint32_t CTRL;    /* AES Control Register     */
    uint32_t STATE;   /* AES State Register       */
    uint32_t Dout[4]; /* Data Output 0~31         */
} aes_reg_t, *paes_reg_t;

/******* Define IT Bits *********/
#define AES_IT_DATAINT 0x4
#define AES_IT_KEYINT 0x2
#define AES_IT_BUSY 0x1
#define AES_IT_ALL 0x7

/****** Enum of AES Edian ******/
typedef enum _aes_endian_t
{
    AES_ENDIAN_LITTLE,
    AES_ENDIAN_BIG
} aes_endian_t;

/***** Enum of AES Opcode ******/
typedef enum _aes_opcode_t
{
    AES_OPCODE_ENCRYPT,
    AES_OPCODE_DECRYPT,
    AES_OPCODE_KEYEXPAND,
} aes_opcode_t;

/***** Enum of AES Key Length *****/
typedef enum _aes_keylength_t
{
    AES_KEYLEN_128,
    AES_KEYLEN_192,
    AES_KEYLEN_256
} aes_keylength_t;

/***** Enum of AES Mode *******/
typedef enum _aes_mode_t
{
    AES_MODE_EBC,
    AES_MODE_CBC
} aes_mode_t;

#endif /* _AES_DRV_H_ */

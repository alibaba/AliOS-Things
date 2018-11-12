/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "ota_crc.h"

/*******************************************************************************/

static unsigned short UpdateCRC16(unsigned short crcIn, unsigned char byte)
{
    unsigned int crc = crcIn;
    unsigned int in = byte | 0x100;

    do {
        crc <<= 1;
        in <<= 1;
        if (in & 0x100) {
            ++crc;
        }
        if (crc & 0x10000) {
            crc ^= 0x1021;
        }
    } while (!(in & 0x10000));
    return crc & 0xffffu;
}

void ota_CRC16_Init( CRC16_Ctx *inCtx )
{
    inCtx->crc = 0;
}


void ota_CRC16_Update( CRC16_Ctx *inCtx, const void *inSrc, size_t inLen )
{
    const unsigned char *src = (const unsigned char *) inSrc;
    const unsigned char *srcEnd = src + inLen;
    while ( src < srcEnd ) {
        inCtx->crc = UpdateCRC16(inCtx->crc, *src++);
    }
}


void ota_CRC16_Final( CRC16_Ctx *inCtx, unsigned short *outResult )
{
    inCtx->crc = UpdateCRC16(inCtx->crc, 0);
    inCtx->crc = UpdateCRC16(inCtx->crc, 0);
    *outResult = inCtx->crc & 0xffffu;
}


static unsigned int UpdateCRC32(unsigned int data)
{
    register unsigned int crc = data;
    register unsigned int polynomial = 0xedb88320;

    unsigned int i;
    for (i = 8 ; i > 0; i--)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ polynomial;
        else
            crc >>= 1;
    }
    return crc;
}

void ota_CRC32_Init( CRC32_Ctx *inCtx )
{
    inCtx->crc = 0;
}

void ota_CRC32_Update( CRC32_Ctx *inCtx, const void *inSrc, size_t inLen )
{
    const unsigned char *src = (const unsigned char *) inSrc;
    while (inLen--)
        inCtx->crc = UpdateCRC32((inCtx->crc ^ *src++) & 0xff) ^ (inCtx->crc >> 8);
}

void ota_CRC32_Final( CRC32_Ctx *inCtx, unsigned int *outResult )
{
    *outResult = inCtx->crc & 0xffffffffu;
}


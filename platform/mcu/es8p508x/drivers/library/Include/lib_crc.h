/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/
#ifndef __LIBCRC_H
#define __LIBCRC_H

#include "ES8P508x.h"
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CRC_EmptayCheck_addr    0x10000010
#define CRC_FlashVerify_addr    0x10000014
#define CRC_UserCal_addr        0x10000018
#define CRC_CheckReset_addr     0x1000001C

typedef uint32_t (*CRC_EM)(void* address, uint32_t data_len);
typedef uint32_t (*CRC_FL)(void* address, uint32_t data_len, uint32_t type);
typedef uint32_t (*CRC_US)(void* address, uint32_t data_len, uint32_t type);
typedef uint32_t (*CRC_CH)(void);
	

typedef enum
{
    CRC_XOROUT_NORMAL = 0x0,   
    CRC_XOROUT_ANTI = (1U<<14),   
} CRC_XOROUT;

typedef enum
{
    CRC_REFOUT_NORMAL = 0x0,   
    CRC_REFOUT_REVERSE = (1U<<13),   
} CRC_REFOUT;

typedef enum
{
    CRC_REFIN_NORMAL = 0x0,  
    CRC_REFIN_REVERSE = (1U<<12),   
} CRC_REFIN;


typedef enum
{
    CRC_MOD_CRC32 = 0x0,	     
    CRC_MOD_CRC16 = (2U<<8),   
    CRC_MOD_CRC16_CCITT = (3U<<8),  
} CRC_MOD_TYPE;

typedef enum
{
    CRC_INIT_DATA_ALL_0 = 0x0,	     
    CRC_INIT_DATA_ALL_1 = (1U<<3),   
} CRC_INIT_DATA;

typedef enum
{
    CRC_HS_TYPE_DISABLE = 0x0,	    
    CRC_HS_TYPE_ENABLE = (1U<<2),   
} CRC_HS_TYPE;

typedef struct 
{
	CRC_XOROUT xorout;
	CRC_REFOUT refout;
	CRC_REFIN  refin;
	CRC_MOD_TYPE mode;
	CRC_INIT_DATA init_data_type;
	CRC_HS_TYPE hs_type;
} CRC_CONValueStruType;

uint32_t CRC_getTypeValue(CRC_CONValueStruType con_value);
uint32_t CRC_EmptayCheck(void* address, uint32_t data_len);
uint32_t CRC_FlashVerify(void* address, uint32_t data_len, uint32_t type);
uint32_t CRC_UserCal(void* address, uint32_t data_len, uint32_t type);
uint32_t CRC_CheckReset(void);

#ifdef __cplusplus
}
#endif

#endif
/*************************END OF FILE**********************/

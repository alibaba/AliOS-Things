/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIB_DMA_H__
#define __LIB_DMA_H__

#include "p131.h"
#include "type.h"

typedef enum {
    DMA_Channel_AES     = 0,
    DMA_Channel_UART0TX = 1,
    DMA_Channel_UART0RX = 2,
    DMA_Channel_UART1TX = 3,
    DMA_Channel_UART1RX = 4,
    DMA_Channel_UART2TX = 5,
    DMA_Channel_UART2RX = 6,
    DMA_Channel_MODEMTX = 9,
    DMA_Channel_MODEMRX = 10,
    DMA_Channel_UART3TX = 11,
    DMA_Channel_UART3RX = 12,
    DMA_Channel_SPI0TX  = 13,
    DMA_Channel_SPI0RX  = 14,
    DMA_Channel_SPI1TX  = 15,
    DMA_Channel_SPI1RX  = 16,
    DMA_Channel_SPI2TX  = 17,
    DMA_Channel_SPI2RX  = 18,
    DMA_Channel_SPI3TX  = 19,
    DMA_Channel_SPI3RX  = 20,
    DMA_Channel_I2C0TX  = 21,
    DMA_Channel_I2C0RX  = 22,
    DMA_Channel_I2C1TX  = 23,
    DMA_Channel_I2C1RX  = 24,
    DMA_Channel_ECC     = 25,
    DMA_Channel_TRNG    = 26,
    DMA_Channel_CRC     = 27,
} DMA_Channel_Typedef;

typedef union {
    struct {
        uint32_t cycle_ctrl    :3;
        uint32_t next_useburst :1;
        uint32_t n_minus_1     :10;
        uint32_t R_power       :4;
        uint32_t src_prot_ctrl :3;
        uint32_t dst_prot_ctrl :3;
        uint32_t src_size      :2;
        uint32_t src_inc       :2;
        uint32_t dst_size      :2;
        uint32_t dst_inc       :2;
    };
    uint32_t Word;
} Ctrl_Typedef;

typedef struct {
    void *src;
    void *dst;
    Ctrl_Typedef ctrl;
    uint32_t use;
} DMA_Descriptor_Typedef;

typedef enum {
    DMA_DATA_INC_BYTE     = 0x0,
    DMA_DATA_INC_HALFWORD = 0x1,
    DMA_DATA_INC_WORD     = 0x2,
    DMA_DATA_INC_NONE     = 0x3,
} DMA_DataInc_Typedef;

typedef enum {
    DMA_DATA_SIZE_BYTE     = 0x0,
    DMA_DATA_SIZE_HALFWORD = 0x1,
    DMA_DATA_SIZE_WORD     = 0x2,
} DMA_DataSize_Typedef;

typedef enum {
    DMA_CYCLE_CTRL_NONE               = 0x0,
    DMA_CYCLE_CTRL_BASIC              = 0x1,
    DMA_CYCLE_CTRL_AUTO               = 0x2,
    DMA_CYCLE_CTRL_PINGPONG           = 0x3,
    DMA_CYCLE_CTRL_MEM_SCATTER_GATHER = 0x4,
    DMA_CYCLE_CTRL_MEM_SCATTER_GATHER_ALT = 0x5,
    DMA_CYCLE_CTRL_PER_SCATTER_GATHER = 0x6,
    DMA_CYCLE_CTRL_PER_SCATTER_GATHER_ALT = 0x7,
} DMA_CycleCtrl_Typedef;

typedef enum {
    DMA_R_POWER_1    = 0x0,
    DMA_R_POWER_2    = 0x1,
    DMA_R_POWER_4    = 0x2,
    DMA_R_POWER_8    = 0x3,
    DMA_R_POWER_16   = 0x4,
    DMA_R_POWER_32   = 0x5,
    DMA_R_POWER_64   = 0x6,
    DMA_R_POWER_128  = 0x7,
    DMA_R_POWER_256  = 0x8,
    DMA_R_POWER_512  = 0x9,
    DMA_R_POWER_1024 = 0xA,
} DMA_ArbiterConfig_Typedef;


typedef struct {
    void                *src;
    void                *dst;
    uint16_t            size;
    DMA_DataSize_Typedef data_width;
    DMA_DataInc_Typedef src_inc;
    DMA_DataInc_Typedef dst_inc;
    DMA_ArbiterConfig_Typedef R_power;
    TYPE_FUNCEN         primary;
    TYPE_FUNCEN         burst;
    TYPE_FUNCEN         high_prio;
    DMA_Channel_Typedef channel;
    DMA_CycleCtrl_Typedef CycleCtrl;
} DMA_InitStruType;

typedef struct
{
  void                      *src;
  void                      *dst;
  DMA_DataInc_Typedef       dst_inc;
  DMA_DataInc_Typedef       src_inc;
  DMA_DataSize_Typedef      data_width;
  DMA_ArbiterConfig_Typedef R_power;
  uint16_t                  size;
  TYPE_FUNCEN               peripheral;
} DMA_CfgDescrSGAlt_TypeDef;


void DMA_Init(DMA_Channel_Typedef DMA_Channel, DMA_InitStruType *DMA_InitStruct);
void DMA_ChannelEnabled(DMA_Channel_Typedef channel);
void DMA_ChannelDisable(DMA_Channel_Typedef channel);
FlagStatus DMA_GetChannelStatus(DMA_Channel_Typedef channel);
void DMA_SoftRequest(DMA_Channel_Typedef channel);
FlagStatus DMA_GetFlagStatus(DMA_Channel_Typedef channel);
void DMA_ClearITPendingBit(DMA_Channel_Typedef channel);
void DMA_MaskRequest(DMA_Channel_Typedef channel, TYPE_FUNCEN state);
FlagStatus DMA_GetMaskRequestStype(DMA_Channel_Typedef channel);
FlagStatus DMA_GetErrStype(DMA_Channel_Typedef channel);
void DMA_ClearErr(DMA_Channel_Typedef channel);
void DMA_ClearPriority(DMA_Channel_Typedef channel);

#endif


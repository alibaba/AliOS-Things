/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIBSPI_H__
#define __LIBSPI_H__

#include "p131.h"
#include "type.h"

typedef enum {
    SPI_RiseSendFallRec = 0x0,
    SPI_FallSendRiseRec = 0x1,
    SPI_RiseRecFallSend = 0x2,
    SPI_FallRecRiseSend = 0x3,
} SPI_TYPE_DFS;

typedef enum {
    SPI_Mode_Master = 0x0,
    SPI_Mode_Slave  = 0x1,
} SPI_TYPE_MODE;

typedef struct {
    uint32_t SPI_Freq;
    SPI_TYPE_DFS SPI_Df;
    SPI_TYPE_MODE SPI_Mode;
    uint8_t SPI_DW;
    TYPE_FUNCEN SPI_DelayRec;
    TYPE_FUNCEN SPI_DelaySend;
    uint8_t SPI_SendDelayPeroid;
} SPI_InitStruType;

typedef enum {
    SPI_Flag_TB    = (1 << 0),
    SPI_Flag_RB    = (1 << 1),
    SPI_Flag_TE    = (1 << 2),
    SPI_Flag_RO    = (1 << 3),
    SPI_Flag_ID    = (1 << 4),
    SPI_Flag_NSSIF = (1 << 5),
    SPI_Flag_TBWE  = (1 << 6),
    SPI_Flag_NSS   = (1 << 7),
    SPI_Flag_TBEF0 = (1 << 8),
    SPI_Flag_TBEF1 = (1 << 9),
    SPI_Flag_TBEF2 = (1 << 10),
    SPI_Flag_TBEF3 = (1 << 11),
    SPI_Flag_RBFF0 = (1 << 12),
    SPI_Flag_RBFF1 = (1 << 13),
    SPI_Flag_RBFF2 = (1 << 14),
    SPI_Flag_RBFF3 = (1 << 15),
    SPI_Flag_IDLE  = (1 << 16),
    SPI_Flag_TMS   = (1 << 17),
} SPI_TYPE_FLAG;

typedef enum {
    SPI_IType_BYTE     = 0x0,
    SPI_IType_HALFWORD = 0x1,
    SPI_IType_WORD     = 0x2,
} SPI_TYPE_TRBIM;

typedef enum {
    SPI_IT_TB   = (1 << 0),
    SPI_IT_RB   = (1 << 1),
    SPI_IT_TE   = (1 << 2),
    SPI_IT_RO   = (1 << 3),
    SPI_IT_ID   = (1 << 4),
    SPI_IT_NSS  = (1 << 5),
    SPI_IT_TBWE = (1 << 6),
} SPI_TYPE_IT;

typedef enum {
    SPI_CLR_TE   = (1 << 2),
    SPI_CLR_RO   = (1 << 3),
    SPI_CLR_ID   = (1 << 4),
    SPI_CLR_NSS  = (1 << 5),
    SPI_CLR_TBWE = (1 << 6),
} SPI_CLR_IF;

typedef enum {
    SPI_STA_NSS   = (1 << 7),
    SPI_STA_TBEF0 = (1 << 8),
    SPI_STA_TBEF1 = (1 << 9),
    SPI_STA_TBEF2 = (1 << 10),
    SPI_STA_TBEF3 = (1 << 11),
    SPI_STA_RBFF0 = (1 << 12),
    SPI_STA_RBFF1 = (1 << 13),
    SPI_STA_RBFF2 = (1 << 14),
    SPI_STA_RBFF3 = (1 << 15),
    SPI_STA_IDLE  = (1 << 16),
} SPI_TYPE_STA;


/* SPI0 */
#define SPI0_Enable()       (SPI0->CON.EN  = 1)
#define SPI0_Disable()      (SPI0->CON.EN  = 0)
#define SPI0_RecEnable()    (SPI0->CON.REN = 1)
#define SPI0_RecDisable()   (SPI0->CON.REN = 0)
#define SPI0_Rst()          (SPI0->CON.RST = 1)

/* SPI1 */
#define SPI1_Enable()       (SPI1->CON.EN  = 1)
#define SPI1_Disable()      (SPI1->CON.EN  = 0)
#define SPI1_RecEnable()    (SPI1->CON.REN = 1)
#define SPI1_RecDisable()   (SPI1->CON.REN = 0)
#define SPI1_Rst()          (SPI1->CON.RST = 1)

/* SPI2 */
#define SPI2_Enable()       (SPI2->CON.EN  = 1)
#define SPI2_Disable()      (SPI2->CON.EN  = 0)
#define SPI2_RecEnable()    (SPI2->CON.REN = 1)
#define SPI2_RecDisable()   (SPI2->CON.REN = 0)
#define SPI2_Rst()          (SPI2->CON.RST = 1)

/* SPI3 */
#define SPI3_Enable()       (SPI3->CON.EN  = 1)
#define SPI3_Disable()      (SPI3->CON.EN  = 0)
#define SPI3_RecEnable()    (SPI3->CON.REN = 1)
#define SPI3_RecDisable()   (SPI3->CON.REN = 0)
#define SPI3_Rst()          (SPI3->CON.RST = 1)


/* SPI function */
void SPI_Init(SPI_TypeDef *SPIx, SPI_InitStruType *SPI_InitStruct);
void SPI_ITConfig(SPI_TypeDef *SPIx, SPI_TYPE_IT SPI_IE, TYPE_FUNCEN NewState);
void SPI_DataFormatConfig(SPI_TypeDef *SPIx, SPI_TYPE_DFS Type);
void SPI_SendByte(SPI_TypeDef *SPIx, uint8_t Temp);
void SPI_SendHalfWord(SPI_TypeDef *SPIx, uint16_t Temp);
void SPI_SendWord(SPI_TypeDef *SPIx, uint32_t Temp);
uint8_t SPI_RecByte(SPI_TypeDef *SPIx);
uint16_t SPI_RecHalfWord(SPI_TypeDef *SPIx);
uint32_t SPI_RecWord(SPI_TypeDef *SPIx);
void SPI_TBIMConfig(SPI_TypeDef *SPIx, SPI_TYPE_TRBIM Type);
void SPI_RBIMConfig(SPI_TypeDef *SPIx, SPI_TYPE_TRBIM Type);
FlagStatus SPI_GetFlagStatus(SPI_TypeDef *SPIx, SPI_TYPE_FLAG Flag);
ITStatus SPI_GetITStatus(SPI_TypeDef *SPIx, SPI_TYPE_IT Flag);
FlagStatus SPI_GetStatus(SPI_TypeDef *SPIx, SPI_TYPE_STA Flag);
void SPI_ClearITPendingBit(SPI_TypeDef *SPIx, SPI_CLR_IF Flag);
void Clear_TBW(SPI_TypeDef *SPIx);
void Clear_RBR(SPI_TypeDef *SPIx);
void SPI_configTXDMAEN(SPI_TypeDef *SPIx,TYPE_FUNCEN state);

void SPI_configRXDMAEN(SPI_TypeDef *SPIx,TYPE_FUNCEN state);

#endif  /* end of file */

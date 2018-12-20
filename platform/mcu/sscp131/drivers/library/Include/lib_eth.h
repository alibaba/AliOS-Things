/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIB_ETH_H
#define __LIB_ETH_H


#include "p131.h"
#include "type.h"

#define ETH_DMATxDesc_OWN   0x01
#define ETH_TX_RESET    1
#define ETH_DMARxDesc_OWN 0x01
#define ETH_RX_RESET    1
#define ETH_SUCCESS 1
#define ETH_ERROR   0

/* ETH_MAC_CONF1 */
#define RST_SOFT         (1 << 31)
#define RST_SIM          (1 << 30)
#define RST_RXMAC                (1 << 19)
#define RST_TXMAC        (1 << 18)
#define RST_RXFUNC           (1 << 17)
#define RST_TXFUNC       (1 << 16)
#define LOOP_BACK        (1 <<  8)
#define RX_FLOW_CTL      (1 <<  5)
#define TX_FLOW_CTL      (1 <<  4)
#define SYNC_RXEN        (1 <<  3)
#define ETH_RXEN             (1 <<  2)
#define SYNC_TXEN        (1 <<  1)
#define ETH_TXEN             (1 <<  0)

/* ETH_MAC_CONF2 */
#define NIBBLE_MODE      (1 <<  8)
#define PAD_CRC                  (1 <<  2)
#define CRC_EN                   (1 <<  1)
#define FULL_DUPLEX      (1 <<  0)

/* ETH_IPFON */
#define NIBBLE_MODE      (1 <<  8)
#define FULL_DUPLEX      (1 <<  0)

/* ETH_FIFO_CONF0 */
#define ENFABTX          (1 << 12)
#define ENSYSTX          (1 << 11)
#define ENFABRX          (1 << 10)
#define ENSYSRX          (1 <<  9)
#define ENWATMK          (1 <<  8)

/* MAC DMA Packet set */
#define PKT_EMPTY        (1 << 31)
#define PKT_PAD          (1 << 18)
#define PKT_CRC                  (1 << 17)

/* Phy Basic Control Parameter */
#define BC_SOFT_RESET    (1 <<  15)
#define BC_AUTONEGO_EN   (1 <<  12)

/* Phy Basic Status Parameter */
#define BS_AUTONEGO_COMP (1 <<  5)
#define BS_LINK_UP       (1 <<  2)

/* Phy Interrupt Mask Parameter */
#define IM_INT7          (1 <<  7)
#define IM_INT6          (1 <<  6)
#define IM_INT5          (1 <<  5)
#define IM_INT4          (1 <<  4)
#define IM_INT3          (1 <<  3)
#define IM_INT2          (1 <<  2)
#define IM_INT1          (1 <<  1)


/***************************/
/*      Phy primitive      */
/***************************/

/* SMI Register map */
typedef enum 
{
    PHY_BasicControl               = 0,
    PHY_BasicStatus                = 1,
    PHY_PhyIdentifier1             = 2,
    PHY_PhyIdentifier2             = 3,
    PHY_AutoNegoAdvertisement      = 4,
    PHY_AutoNegoLinkPartnerAbility = 5,
    PHY_AutoNegoExpansion          = 6,
    PHY_ModeControlStatus          = 17,
    PHY_SpecialMode                = 18,
    PHY_SymbolErrorCountr          = 26,
    PHY_ControlStatusIndication    = 27,
    PHY_InterruptSource            = 29,
    PHY_InterruptMask              = 30,
    PHY_PhySpecialControlStatus    = 31,
} PHY_Reg_Index;

/* Phy Address Parameter */
typedef enum 
{
    PHY_Address0,
    PHY_Address1,
    PHY_Address2,
    PHY_Address3,
    PHY_Address4,
    PHY_Address5,
} PHY_Addr_Index;

/* BASIC CONTROL REGISTER */
typedef union
{
    struct
    {
            uint16_t RESERVED0: 8;
            uint16_t DuplexMode: 1;
            uint16_t RestartAN: 1;
            uint16_t Isolate: 1;
            uint16_t PowerDown: 1;
            uint16_t ANEnable: 1;
            uint16_t SpeedSel: 1;
      uint16_t LoopBack: 1;
            uint16_t SoftRst: 1;
    };
    uint16_t HalfWord;
} PHY_BC_Typedef;

/* BASIC STATUS REGISTER */
typedef union
{
    struct
    {
            uint16_t ExtCapAbi: 1;
            uint16_t JabDetect: 1;
            uint16_t LinkStatus: 1;
            uint16_t ANAbi: 1;
            uint16_t RemoteFault: 1;
            uint16_t ANComplete: 1;
            uint16_t RESERVED0: 2;
            uint16_t ExtStatus: 1;
            uint16_t BT2_100_HD: 1;
            uint16_t BT2_100_FD: 1;
            uint16_t BT_10_HD: 1;
            uint16_t BT_10_FD: 1;
            uint16_t BTX_100_HD: 1;
            uint16_t BTX_100_FD: 1;
            uint16_t BT4_100: 1;
    };
    uint16_t HalfWord;
} PHY_BS_Typedef;

/* PHY IDENTIFIER 1 REGISTER */
typedef union
{
    uint16_t PIDNum;
} PHY_PID1_Typedef;

/* PHY IDENTIFIER 2 REGISTER */
typedef union
{
    struct
    {
            uint16_t RevNum: 4;
            uint16_t ModelNum: 6;
            uint16_t PIDNum: 6;
    };
    uint16_t HalfWord;
} PHY_PID2_Typedef;

/* AUTO NEGOTIATION ADVERTISEMENT REGISTER */
typedef union
{
    struct
    {
            uint16_t SelField: 5;
            uint16_t BT_10: 1;
            uint16_t BT_10_FD: 1;
            uint16_t BTX_100: 1;
            uint16_t BTX_100_FD: 1;
            uint16_t RESERVED0: 1;
            uint16_t PauseOp: 2;
            uint16_t RESERVED1: 1;
            uint16_t RemoteFault: 1;
            uint16_t RESERVED2: 2;
    };
    uint16_t HalfWord;
} PHY_ANADV_Typedef;

/* AUTO NEGOTIATION LINK PARTNER ABILITY REGISTER */
typedef union
{
    struct
    {
            uint16_t SelField: 5;
            uint16_t BT_10: 1;
            uint16_t BT_10_FD: 1;
            uint16_t BTX_100: 1;
            uint16_t BTX_100_FD: 1;
            uint16_t BT4_100: 1;
            uint16_t PauseOp: 1;
            uint16_t RESERVED1: 2;
            uint16_t RemoteFault: 1;
            uint16_t ACK: 1;
            uint16_t NextPage: 1;
    };
    uint16_t HalfWord;
} PHY_ANLPA_Typedef;

/* AUTO NEGOTIATION EXPANSION REGISTER */
typedef union
{
    struct
    {
            uint16_t LinkPartANAble: 1;
            uint16_t PageRec: 1;
            uint16_t NextPageAble: 1;
            uint16_t LinkPartNextPageAble: 1;
            uint16_t ParallelDetFault: 1;
            uint16_t RESERVED0: 11;
    };
    uint16_t HalfWord;
} PHY_ANEXP_Typedef;

/* MODE CONTROL/STATUS REGISTER */
typedef union
{
    struct
    {
            uint16_t RESERVED0: 1;
            uint16_t EnergyOn: 1;
            uint16_t RESERVED1: 4;
            uint16_t AltInt: 1;
            uint16_t RESERVED2: 2;
            uint16_t FarLoopBack: 1;
            uint16_t RESERVED3: 3;
            uint16_t EDPwrDown: 1;
            uint16_t RESERVED4: 2;
    };
    uint16_t HalfWord;
} PHY_MCS_Typedef;

/* SPECIAL MODES REGISTER */
typedef union
{
    struct
    {
            uint16_t PHYAD: 5;
            uint16_t Mode: 3;
            uint16_t RESERVED0: 8;
    };
    uint16_t HalfWord;
} PHY_SM_Typedef;

/* SYMBOL ERROR COUNTER REGISTER */
typedef union
{
    uint16_t SymErrCnt;
} PHY_SEC_Typedef;

/* SPECIAL CONTROL/STATUS INDICATIONS REGISTER */
typedef union
{
     struct
    {
            uint16_t RESERVED0: 4;
            uint16_t XPol: 1;
            uint16_t RESERVED1: 6;
            uint16_t SQEOff: 1;
            uint16_t RESERVED2: 1;
            uint16_t CHSel: 1;
            uint16_t RESERVED3: 1;
            uint16_t AMDIXCTtrl: 1;
    };
    uint16_t HalfWord;
} PHY_SCSI_Typedef;

/* INTERRUPT SOURCE FLAG REGISTER */
typedef union
{
     struct
    {
            uint16_t RESERVED0: 1;
            uint16_t INT1: 1;
            uint16_t INT2: 1;
            uint16_t INT3: 1;
            uint16_t INT4: 1;
            uint16_t INT5: 1;
            uint16_t INT6: 1;
            uint16_t INT7: 1;
            uint16_t RESERVED1: 8;
    };
    uint16_t HalfWord;
} PHY_INTSF_Typedef;

/* INTERRUPT MASK REGISTER */
typedef union
{
     struct
    {
            uint16_t RESERVED0: 1;
            uint16_t MB: 7;
            uint16_t RESERVED1: 8;
    };
    uint16_t HalfWord;
} PHY_INTM_Typedef;

/* PHY SPECIAL CONTROL/STATUS REGISTER */
typedef union
{
     struct
    {
            uint16_t RESERVED0: 2;
            uint16_t SI: 3;
            uint16_t RESERVED1: 7;
            uint16_t AUTODONE: 1;
            uint16_t RESERVED2: 3;
    };
    uint16_t HalfWord;
} PHY_PSCS_Typedef;

typedef struct
{
    PHY_BC_Typedef BC;
    PHY_BS_Typedef BS;
    PHY_PID1_Typedef PID1;
    PHY_PID2_Typedef PID2;
    PHY_ANADV_Typedef ANADV;
    PHY_ANLPA_Typedef ANLPA;
    PHY_ANEXP_Typedef ANEXP;
    PHY_MCS_Typedef MCS;
    PHY_SM_Typedef SM;
    PHY_SEC_Typedef SEC;
    PHY_SCSI_Typedef SCSI;
    PHY_INTSF_Typedef INTSF;
    PHY_INTM_Typedef INTM;
    PHY_PSCS_Typedef PSCS;
} PHY_TypeDef;

/***************************/
/*      Mac primitive      */
/***************************/

#pragma pack(1)
/* Mac Data request */
typedef struct
{
    uint8_t DstAddr[6]; //destination address
    uint8_t SrcAddr[6]; //source address
    uint8_t *MSDU;   //mac_service_data_unit
    uint32_t FCS;    //frame_check_sequence
} MACDATAREQ, *pMACDATAREQ;

/* Mac Data indication */
typedef struct
{
    uint8_t DstAddr[6]; //destination address
    uint8_t SrcAddr[6]; //source address
    uint8_t *MSDU;   //mac_service_data_unit
    uint32_t FCS;    //frame_check_sequence
    uint8_t Status;  //reception status
} MACDATAIDC, *pMACDATAIDC;


/* Size of packet, Overrides and Empty Flag */
typedef union
{
     struct
    {
            uint32_t PKTSIZE: 12;
            uint32_t RESERVED0: 4;
            uint32_t FTPPEN: 1;
            uint32_t FTPPGENFCS: 1;
            uint32_t FTPPPADMODE: 2;
            uint32_t FTCFRM: 1;
            uint32_t RESERVED1: 10;
            uint32_t PKTVOID: 1;
    };
    uint32_t Word;
} MAC_PKT_SET_Typedef;


//Mac Dma descriptors
typedef struct _MAC_DMA_TypeDef_
{
    uint32_t PktStrAddx;
  MAC_PKT_SET_Typedef PktSet;
    uint32_t PktNxtDes;
} MAC_DMA_TypeDef,*pMAC_DMA_TypeDef;

typedef struct{
  uint32_t length;
  uint32_t buffer;
  __IO MAC_DMA_TypeDef *descriptor;
}FrameTypeDef;

#pragma pack()


extern PHY_TypeDef PHY;
extern MAC_DMA_TypeDef MACDMA_Tx;
extern MAC_DMA_TypeDef MACDMA_Rx;
extern uint8_t DMATxBuf[0x600];
extern uint8_t DMARxBuf[0x600];

/* ETH ssc1667 MAC functions */
void ssc1667MacInit (void);
void ssc1667MacDmaTxConfig(uint16_t length);
void ssc1667MacDmaRxConfig(void);
void ssc1667MacDataReq(uint8_t  *pMacReqBuf);
void ssc1667MacDataIdc(uint8_t  *pMacIdcBuf);

/* ETH LAN8720 PHY functions */
void     lan8720WritePhyReg(PHY_Addr_Index phyadx, PHY_Reg_Index idx, uint16_t data);
uint16_t lan8720ReadPhyReg(PHY_Addr_Index phyadx, PHY_Reg_Index idx);
void     lan8720PhyReset(PHY_Addr_Index phyadx);
uint32_t ETH_GetCurrentTxBuffer(void);
int8_t eth_init(void);
void lan8720Init(void);
FrameTypeDef ETH_Rx_Packet(void);
uint8_t ETH_Tx_Packet(uint16_t FrameLength);
void desc_buf_init(void);


#endif  /* end of file */

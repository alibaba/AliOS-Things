/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#include "lib_eth.h"
#include "system_p131.h"
#include "stdlib.h"
#include <string.h>

extern void lwip_pkt_handle(void);

PHY_TypeDef PHY;
MAC_DMA_TypeDef MACDMA_Tx;
MAC_DMA_TypeDef MACDMA_Rx;
uint8_t DMATxBuf[0x600];
uint8_t DMARxBuf[0x600];

MAC_DMA_TypeDef ETH_TX_DESC[5];
MAC_DMA_TypeDef ETH_RX_DESC[16];
uint8_t ETHTxBuf[5][0x600];
uint8_t ETHRxBuf[16][0x600];

MAC_DMA_TypeDef *DMATxDescToSet, *DMARxDescToGet;

extern uint32_t intrETH;

void ssc1667MacInit (void)
{
    ETH->MGCONF.RST_MGMT = 1;           // Reset MG
        
    ETH->MAC_CONF1.Word  = (ETH_RXEN|ETH_TXEN); // 0x000 
      ETH->MAC_CONF2.PREAMBLE_LENGTH = 7;
    ETH->MAC_CONF2.Word |= (NIBBLE_MODE|FULL_DUPLEX|PAD_CRC); // 0x004
    ETH->IPFG.Word   = 0x40605060;  // 0x008 
    ETH->HFDUP.Word  = 0x00a1f037;  // 0x00C 
    ETH->MFLEN.MAXIMUM_FRAME_LENGTH = 0x600;  // 0x010 
      ETH->ITFCON.PHY_MODE = 1;       // Speed 1-100M 0-10M
        ETH->FIFCONF0 = (ENFABTX|ENSYSTX|ENFABRX|ENSYSRX|ENWATMK); // 0x048 
        ETH->MGCONF.Word = 7;     // MGCLK = 150MHz/28
}

/*added by billzhang*/

/***************************************************/
/***************************************************/
/***************************************************/

void ETH_MACAddressConfig(uint8_t *mac_addr)
{
    ETH->STAADR1.octet_1 = mac_addr[0];
    ETH->STAADR1.octet_2 = mac_addr[1];
    ETH->STAADR1.octet_3 = mac_addr[2];
    ETH->STAADR1.octet_4 = mac_addr[3];
    ETH->STAADR2.octet_5 = mac_addr[4];
    ETH->STAADR2.octet_6 = mac_addr[5];
}

void  INT_ETH(void)
{
        ETH->DMARXSTA |= (1 << 0);
        while(DMARxDescToGet->PktSet.PKTSIZE == 0);
        if(DMARxDescToGet->PktSet.PKTSIZE > 0)
        {   
            lwip_pkt_handle();
        }
        else
        {
            DMARxDescToGet->PktSet.PKTVOID = ETH_RX_RESET;
            DMARxDescToGet=(MAC_DMA_TypeDef *)(DMARxDescToGet->PktNxtDes);
            ETH->DMARXCTL    = 1; // enable DMA rx
        }
}   

void desc_buf_init(void)
{
    int i, j;
    

    for(i = 0; i < 5; i++)
        for(j = 0; j < 0x600; j++)
            ETHTxBuf[i][j] = 0x00;

    for(i = 0; i < 16; i++)
        for(j = 0; j < 0x600; j++)
            ETHRxBuf[i][j] = 0x00;  

    for(i = 0; i < 5; i++) {
        ETH_TX_DESC[i].PktStrAddx = (uint32_t)ETHTxBuf[i]; 
            //set tx packet info in DMA descriptors
        ETH_TX_DESC[i].PktSet.PKTVOID = ETH_TX_RESET;
        ETH_TX_DESC[i].PktSet.FTCFRM = 1;
        ETH_TX_DESC[i].PktSet.FTPPPADMODE = 1; // enable PAD
        ETH_TX_DESC[i].PktSet.FTPPGENFCS = 1;  // enable CRC
        ETH_TX_DESC[i].PktSet.FTPPEN = 1;
        ETH_TX_DESC[i].PktSet.PKTSIZE = 1500; // packet size(dst + src + length + data),no CRC
        if(i < 4)
            ETH_TX_DESC[i].PktNxtDes = (uint32_t)&ETH_TX_DESC[i+1]; 
        else
            ETH_TX_DESC[i].PktNxtDes = (uint32_t)&ETH_TX_DESC[0]; 
    }
    ETH->DMATXDES = (uint32_t)&ETH_TX_DESC[0];

    for(i = 0; i < 16; i++) {
        ETH_RX_DESC[i].PktStrAddx = (uint32_t)ETHRxBuf[i]; 
            //set tx packet info in DMA descriptors
        ETH_RX_DESC[i].PktSet.PKTVOID = ETH_RX_RESET;
        if(i < 15)
            ETH_RX_DESC[i].PktNxtDes = (uint32_t)&ETH_RX_DESC[i+1]; 
        else
            ETH_RX_DESC[i].PktNxtDes = (uint32_t)&ETH_RX_DESC[0]; 
    }
    ETH->DMARXDES = (uint32_t)&ETH_RX_DESC[0];
    
    DMATxDescToSet = (MAC_DMA_TypeDef *)&ETH_TX_DESC[0];
    DMARxDescToGet = (MAC_DMA_TypeDef *)&ETH_RX_DESC[0];
    
}

int8_t eth_init(void)
{
    ssc1667MacInit();
    lan8720Init();
    //enable rx interrupt£¬we only use rx interrupt
    ETH->DMAIMR = (1 << 4);
    
    return 0;
}

FrameTypeDef ETH_Rx_Packet(void)
{ 
    uint32_t framelength=0;
    FrameTypeDef frame={0,0};  
    
    if((DMARxDescToGet->PktSet.PKTVOID & ETH_DMARxDesc_OWN) == (uint32_t)ETH_RX_RESET)
    {   
        frame.length = ETH_ERROR;

        ETH->DMARXCTL    = 1; // enable DMA rx
        
        return frame;
    }  
    
    if((DMARxDescToGet->PktSet.PKTVOID & ETH_DMARxDesc_OWN) != (uint32_t)ETH_RX_RESET)  
    {       
        framelength = DMARxDescToGet->PktSet.PKTSIZE-4;
        frame.buffer = DMARxDescToGet->PktStrAddx;
    }
    else 
        framelength=ETH_ERROR;
    frame.length = framelength; 
    frame.descriptor = DMARxDescToGet;

    DMARxDescToGet=(MAC_DMA_TypeDef *)(DMARxDescToGet->PktNxtDes); 
    
    return frame;  
}

uint8_t ETH_Tx_Packet(uint16_t FrameLength)
{   
    if((DMATxDescToSet->PktSet.PKTVOID & ETH_DMATxDesc_OWN) != (uint32_t)ETH_TX_RESET)
        return ETH_ERROR;
    DMATxDescToSet->PktSet.PKTSIZE = FrameLength;
    DMATxDescToSet->PktSet.PKTVOID = ~ETH_TX_RESET;
    
    ETH->DMATXCTL    = 1; // enable DMA tx

    DMATxDescToSet=(MAC_DMA_TypeDef *)(DMATxDescToSet->PktNxtDes);    
    return ETH_SUCCESS;   
}

uint32_t ETH_GetCurrentTxBuffer(void)
{  
  return DMATxDescToSet->PktStrAddx;
}

/******************************************************/
/******************************************************/
/******************************************************/

void ssc1667MacDmaTxConfig(uint16_t length)
{
    //set tx data buffer start address in DMA descriptors
    MACDMA_Tx.PktStrAddx = (uint32_t)DMATxBuf; 
    //set tx packet info in DMA descriptors
    MACDMA_Tx.PktSet.PKTVOID = 0;
  MACDMA_Tx.PktSet.FTCFRM = 1;
    MACDMA_Tx.PktSet.FTPPPADMODE = 1; // enable PAD
    MACDMA_Tx.PktSet.FTPPGENFCS = 1;  // enable CRC
    MACDMA_Tx.PktSet.FTPPEN = 1;
    MACDMA_Tx.PktSet.PKTSIZE = length; // packet size(dst + src + length + data),no CRC
    MACDMA_Tx.PktNxtDes = (uint32_t)&MACDMA_Tx; 
    //set tx packet info in DMA transmit descriptors
    ETH->DMATXDES = (uint32_t)&MACDMA_Tx;
    
    ETH->DMATXCTL = 1; //TX enable
}

void ssc1667MacDmaRxConfig(void)
{
    //set rx data buffer start address in DMA descriptors
    MACDMA_Rx.PktStrAddx = (uint32_t)DMARxBuf;
    //set rx packet info in DMA descriptors
    MACDMA_Rx.PktSet.PKTVOID = 0;
    MACDMA_Rx.PktNxtDes = (uint32_t)&MACDMA_Rx;//set rx packet info in DMA descriptors
    //set tx packet info in DMA transmit descriptors
    ETH->DMARXDES = (uint32_t)&MACDMA_Rx;
}

void ssc1667MacDataReq(uint8_t *pMacReqBuf)
{
    uint16_t DataLen;
    pMACDATAREQ pMacDataReq= (pMACDATAREQ)pMacReqBuf;
    
    DataLen = (uint16_t)((pMacDataReq->MSDU[0] << 8) | (pMacDataReq->MSDU[1])); //get frame length
    
    if(DataLen<=1500)
    {
        memcpy(DMATxBuf,pMacDataReq->DstAddr,6); //copy dest addr to DMATxBuf
        memcpy(DMATxBuf+6,pMacDataReq->SrcAddr,6); //copy src addr to DMATxBuf
        memcpy(DMATxBuf+12,pMacDataReq->MSDU,DataLen+2); //copy msdu to DMATxBuf
        
        ssc1667MacDmaTxConfig(DataLen+6+6+2);
        //wait tx finish
        while(!intrETH);
        intrETH = 0;
        ETH->DMAIMR = (1<<0);
        ETH->DMATXSTA = 0x1;
    }
}

void ssc1667MacDataIdc(uint8_t  *pMacIdcBuf)
{
    
}

void lan8720WritePhyReg(PHY_Addr_Index phyadx, PHY_Reg_Index idx, uint16_t data)
{
    while(ETH->MGFIG.BUSY);                     // Chaek Bus Busy
            ETH->MGADR.PHY_ADX      = phyadx;       // Set MG I/F PHY Address
            ETH->MGADR.REG_ADX      = idx;      // Set MG I/F Register Address
            ETH->MGWDATA.WDATA      = data;     // Set MG I/F Wdata
        while(ETH->MGFIG.BUSY);                     // Wait Write Command Finish
}

uint16_t lan8720ReadPhyReg(PHY_Addr_Index phyadx, PHY_Reg_Index idx)
{
    while(ETH->MGFIG.BUSY);                     // Chaek Bus Busy
            ETH->MGADR.PHY_ADX      = phyadx;   // Set MG I/F PHY Address
            ETH->MGADR.REG_ADX      = idx;      // Set MG I/F Register Address
            ETH->MGCMD.READ_CYCLE = 1;          // Set MG I/F Read Command
        while(ETH->MGFIG.BUSY);
            ETH->MGCMD.READ_CYCLE = 0;          // Clear MG I/F Read Command
        return ETH->MGRDATA.RDATA;              // Wait Read Command Finish
}

void lan8720PhyReset(PHY_Addr_Index phyadx)
{
        PHY.BC.HalfWord = BC_SOFT_RESET;  //soft reset
        lan8720WritePhyReg(phyadx,PHY_BasicControl,PHY.BC.HalfWord);
        //wait the soft reset finish
        while(!((lan8720ReadPhyReg(phyadx,PHY_BasicControl) & BC_SOFT_RESET)));
}

void lan8720Init(void)
{
    //phy soft reset and set AUTONEGO
    lan8720PhyReset(PHY_Address0);
    lan8720WritePhyReg(PHY_Address0, PHY_BasicControl, BC_AUTONEGO_EN);

    while(!(lan8720ReadPhyReg(PHY_Address0,PHY_BasicStatus) & BS_AUTONEGO_COMP)); //Wait auto-negotiation complete
    while(!(lan8720ReadPhyReg(PHY_Address0,PHY_BasicStatus) & BS_LINK_UP)); //Wait link up
}

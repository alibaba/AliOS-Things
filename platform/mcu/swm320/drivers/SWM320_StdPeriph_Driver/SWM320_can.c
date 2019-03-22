/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#include "SWM320.h"
#include "SWM320_can.h"

void CAN_Init(CAN_TypeDef * CANx, CAN_InitStructure * initStruct)
{
    switch((uint32_t)CANx)
    {
    case ((uint32_t)CAN):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_CAN_Pos);
        break;
    }

    CAN_Close(CANx);

    CANx->CR &= ~(CAN_CR_LOM_Msk | CAN_CR_STM_Msk | CAN_CR_AFM_Msk);
    CANx->CR |= (initStruct->Mode << CAN_CR_LOM_Pos) |
                (initStruct->FilterMode << CAN_CR_AFM_Pos);

    CANx->FILTER.AMR[3] = initStruct->FilterMask32b & 0xFF;
    CANx->FILTER.AMR[2] = (initStruct->FilterMask32b >>  8) & 0xFF;
    CANx->FILTER.AMR[1] = (initStruct->FilterMask32b >> 16) & 0xFF;
    CANx->FILTER.AMR[0] = (initStruct->FilterMask32b >> 24) & 0xFF;

    CANx->FILTER.ACR[3] = initStruct->FilterCheck32b & 0xFF;
    CANx->FILTER.ACR[2] = (initStruct->FilterCheck32b >>  8) & 0xFF;
    CANx->FILTER.ACR[1] = (initStruct->FilterCheck32b >> 16) & 0xFF;
    CANx->FILTER.ACR[0] = (initStruct->FilterCheck32b >> 24) & 0xFF;

    CANx->BT1 = (0 << CAN_BT1_SAM_Pos) |
                (initStruct->CAN_BS1 << CAN_BT1_TSEG1_Pos) |
                (initStruct->CAN_BS2 << CAN_BT1_TSEG2_Pos);

    CANx->BT0 = (initStruct->CAN_SJW << CAN_BT0_SJW_Pos) |
                ((SystemCoreClock/2/initStruct->Baudrate/(1 + (initStruct->CAN_BS1 + 1) + (initStruct->CAN_BS2 + 1)) - 1) << CAN_BT0_BRP_Pos);

    CANx->RXERR = 0;
    CANx->TXERR = 0;

    CANx->IE = (initStruct->RXNotEmptyIEn << CAN_IE_RXDA_Pos)    |
               (initStruct->RXOverflowIEn << CAN_IE_RXOV_Pos)    |
               (initStruct->ArbitrLostIEn << CAN_IE_ARBLOST_Pos) |
               (initStruct->ErrPassiveIEn << CAN_IE_ERRPASS_Pos);

    switch((uint32_t)CANx)
    {
    case ((uint32_t)CAN):
        if(initStruct->RXNotEmptyIEn | initStruct->RXOverflowIEn | initStruct->ArbitrLostIEn | initStruct->ErrPassiveIEn)
        {
            NVIC_EnableIRQ(CAN_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(CAN_IRQn);
        }
        break;
    }
}

void CAN_Open(CAN_TypeDef * CANx)
{
    CANx->CR &= ~(0x01 << CAN_CR_RST_Pos);
}

void CAN_Close(CAN_TypeDef * CANx)
{
    CANx->CR |= (0x01 << CAN_CR_RST_Pos);
}

void CAN_Transmit(CAN_TypeDef * CANx, uint32_t format, uint32_t id, uint8_t data[], uint32_t size, uint32_t once)
{
    uint32_t i;

    if(format == CAN_FRAME_STD)
    {
        CANx->TXFRAME.INFO = (0 << CAN_INFO_FF_Pos)  |
                             (0 << CAN_INFO_RTR_Pos) |
                             (size << CAN_INFO_DLC_Pos);

        CANx->TXFRAME.DATA[0] = id >> 3;
        CANx->TXFRAME.DATA[1] = id << 5;

        for(i = 0; i < size; i++)
        {
            CANx->TXFRAME.DATA[i+2] = data[i];
        }
    }
    else //if(format == CAN_FRAME_EXT)
    {
        CANx->TXFRAME.INFO = (1 << CAN_INFO_FF_Pos)  |
                             (0 << CAN_INFO_RTR_Pos) |
                             (size << CAN_INFO_DLC_Pos);

        CANx->TXFRAME.DATA[0] = id >> 21;
        CANx->TXFRAME.DATA[1] = id >> 13;
        CANx->TXFRAME.DATA[2] = id >>  5;
        CANx->TXFRAME.DATA[3] = id <<  3;

        for(i = 0; i < size; i++)
        {
            CANx->TXFRAME.DATA[i+4] = data[i];
        }
    }

    if(CANx->CR & CAN_CR_STM_Msk)
    {
        CANx->CMD = (1 << CAN_CMD_SRR_Pos);
    }
    else
    {
        if(once == 0)
        {
            CANx->CMD = (1 << CAN_CMD_TXREQ_Pos);
        }
        else
        {
            CANx->CMD = (1 << CAN_CMD_TXREQ_Pos) | (1 << CAN_CMD_ABTTX_Pos);
        }
    }
}

void CAN_TransmitRequest(CAN_TypeDef * CANx, uint32_t format, uint32_t id, uint32_t once)
{
    if(format == CAN_FRAME_STD)
    {
        CANx->TXFRAME.INFO = (0 << CAN_INFO_FF_Pos)  |
                             (1 << CAN_INFO_RTR_Pos) |
                             (0 << CAN_INFO_DLC_Pos);

        CANx->TXFRAME.DATA[0] = id >> 3;
        CANx->TXFRAME.DATA[1] = id << 5;
    }
    else //if(format == CAN_FRAME_EXT)
    {
        CANx->TXFRAME.INFO = (1 << CAN_INFO_FF_Pos)  |
                             (1 << CAN_INFO_RTR_Pos) |
                             (0 << CAN_INFO_DLC_Pos);

        CANx->TXFRAME.DATA[0] = id >> 21;
        CANx->TXFRAME.DATA[1] = id >> 13;
        CANx->TXFRAME.DATA[2] = id >>  5;
        CANx->TXFRAME.DATA[3] = id <<  3;
    }

    if(once == 0)
    {
        CANx->CMD = (1 << CAN_CMD_TXREQ_Pos);
    }
    else
    {
        CANx->CMD = (1 << CAN_CMD_TXREQ_Pos) | (1 << CAN_CMD_ABTTX_Pos);
    }
}

void CAN_Receive(CAN_TypeDef * CANx, CAN_RXMessage *msg)
{
    uint32_t i;
    msg->format = (CANx->RXFRAME.INFO & CAN_INFO_FF_Msk) >> CAN_INFO_FF_Pos;

    msg->remote = (CANx->RXFRAME.INFO & CAN_INFO_RTR_Msk) >> CAN_INFO_RTR_Pos;
    msg->size = (CANx->RXFRAME.INFO & CAN_INFO_DLC_Msk) >> CAN_INFO_DLC_Pos;

    if(msg->format == CAN_FRAME_STD)
    {
        msg->id = (CANx->RXFRAME.DATA[0] << 3) | (CANx->RXFRAME.DATA[1] >> 5);

        for(i = 0; i < msg->size; i++)
        {
            msg->data[i] = CANx->RXFRAME.DATA[i+2];
        }
    }
    else //if(msg->format == CAN_FRAME_EXT)
    {
        msg->id = (CANx->RXFRAME.DATA[0] << 21) | (CANx->RXFRAME.DATA[1] << 13) | (CANx->RXFRAME.DATA[2] << 5) | (CANx->RXFRAME.DATA[3] >> 3);

        for(i = 0; i < msg->size; i++)
        {
            msg->data[i] = CANx->RXFRAME.DATA[i+4];
        }
    }

    CANx->CMD = (1 << CAN_CMD_RRB_Pos);
}

uint32_t CAN_TXComplete(CAN_TypeDef * CANx)
{
    return (CANx->SR & CAN_SR_TXBR_Msk) ? 1 : 0;
}

uint32_t CAN_TXSuccess(CAN_TypeDef * CANx)
{
    return (CANx->SR & CAN_SR_TXOK_Msk) ? 1 : 0;
}

void CAN_AbortTransmit(CAN_TypeDef * CANx)
{
    CANx->CMD = (1 << CAN_CMD_ABTTX_Pos);
}

uint32_t CAN_TXBufferReady(CAN_TypeDef * CANx)
{
    return (CANx->SR & CAN_SR_TXBR_Msk) ? 1 : 0;
}

uint32_t CAN_RXDataAvailable(CAN_TypeDef * CANx)
{
    return (CANx->SR & CAN_SR_RXDA_Msk) ? 1 : 0;
}

void CAN_SetBaudrate(CAN_TypeDef * CANx, uint32_t baudrate, uint32_t CAN_BS1, uint32_t CAN_BS2, uint32_t CAN_SJW)
{
    CANx->BT1 = (0 << CAN_BT1_SAM_Pos) |
                (CAN_BS1 << CAN_BT1_TSEG1_Pos) |
                (CAN_BS2 << CAN_BT1_TSEG2_Pos);

    CANx->BT0 = (CAN_SJW << CAN_BT0_SJW_Pos) |
                ((SystemCoreClock/2/baudrate/(1 + (CAN_BS1 + 1) + (CAN_BS2 + 1)) - 1) << CAN_BT0_BRP_Pos);
}

void CAN_SetFilter32b(CAN_TypeDef * CANx, uint32_t check, uint32_t mask)
{
    CANx->CR &= ~CAN_CR_AFM_Msk;
    CANx->CR |= (CAN_FILTER_32b << CAN_CR_AFM_Pos);

    CANx->FILTER.AMR[0] =  mask & 0xFF;
    CANx->FILTER.AMR[1] = (mask >>  8) & 0xFF;
    CANx->FILTER.AMR[2] = (mask >> 16) & 0xFF;
    CANx->FILTER.AMR[3] = (mask >> 24) & 0xFF;

    CANx->FILTER.ACR[0] =  check & 0xFF;
    CANx->FILTER.ACR[1] = (check >>  8) & 0xFF;
    CANx->FILTER.ACR[2] = (check >> 16) & 0xFF;
    CANx->FILTER.ACR[3] = (check >> 24) & 0xFF;
}

void CAN_SetFilter16b(CAN_TypeDef * CANx, uint16_t check1, uint16_t mask1, uint16_t check2, uint16_t mask2)
{
    CANx->CR &= ~CAN_CR_AFM_Msk;
    CANx->CR |= (CAN_FILTER_16b << CAN_CR_AFM_Pos);

    CANx->FILTER.AMR[0] =  mask1 & 0xFF;
    CANx->FILTER.AMR[1] = (mask1 >>  8) & 0xFF;
    CANx->FILTER.AMR[2] =  mask2 & 0xFF;
    CANx->FILTER.AMR[3] = (mask2 >>  8) & 0xFF;

    CANx->FILTER.ACR[0] =  check1 & 0xFF;
    CANx->FILTER.ACR[1] = (check1 >>  8) & 0xFF;
    CANx->FILTER.ACR[2] =  check2 & 0xFF;
    CANx->FILTER.ACR[3] = (check2 >>  8) & 0xFF;
}

void CAN_INTRXNotEmptyEn(CAN_TypeDef * CANx)
{
    CANx->IE |= (1 << CAN_IE_RXDA_Pos);
}

void CAN_INTRXNotEmptyDis(CAN_TypeDef * CANx)
{
    CANx->IE &= ~(1 << CAN_IE_RXDA_Pos);
}

uint32_t CAN_INTRXNotEmptyStat(CAN_TypeDef * CANx)
{
    return (CANx->IF & CAN_IF_RXDA_Msk) ? 1 : 0;
}

void CAN_INTTXBufEmptyEn(CAN_TypeDef * CANx)
{
    CANx->IE |= (1 << CAN_IE_TXBR_Pos);
}

void CAN_INTTXBufEmptyDis(CAN_TypeDef * CANx)
{
    CANx->IE &= ~(1 << CAN_IE_TXBR_Pos);
}

uint32_t CAN_INTTXBufEmptyStat(CAN_TypeDef * CANx)
{
    return (CANx->IF & CAN_IF_TXBR_Msk) ? 1 : 0;
}

void CAN_INTErrWarningEn(CAN_TypeDef * CANx)
{
    CANx->IE |= (1 << CAN_IE_ERRWARN_Pos);
}

void CAN_INTErrWarningDis(CAN_TypeDef * CANx)
{
    CANx->IE &= ~(1 << CAN_IE_ERRWARN_Pos);
}

uint32_t CAN_INTErrWarningStat(CAN_TypeDef * CANx)
{
    return (CANx->IF & CAN_IF_ERRWARN_Msk) ? 1 : 0;
}

void CAN_INTRXOverflowEn(CAN_TypeDef * CANx)
{
    CANx->IE |= (1 << CAN_IE_RXOV_Pos);
}

void CAN_INTRXOverflowDis(CAN_TypeDef * CANx)
{
    CANx->IE &= ~(1 << CAN_IE_RXOV_Pos);
}

uint32_t CAN_INTRXOverflowStat(CAN_TypeDef * CANx)
{
    return (CANx->IF & CAN_IF_RXOV_Msk) ? 1 : 0;
}

void CAN_INTRXOverflowClear(CAN_TypeDef * CANx)
{
    CANx->CMD = (1 << CAN_CMD_CLROV_Pos);
}

void CAN_INTWakeupEn(CAN_TypeDef * CANx)
{
    CANx->IE |= (1 << CAN_IE_WKUP_Pos);
}

void CAN_INTWakeupDis(CAN_TypeDef * CANx)
{
    CANx->IE &= ~(1 << CAN_IE_WKUP_Pos);
}

uint32_t CAN_INTWakeupStat(CAN_TypeDef * CANx)
{
    return (CANx->IF & CAN_IF_WKUP_Msk) ? 1 : 0;
}

void CAN_INTErrPassiveEn(CAN_TypeDef * CANx)
{
    CANx->IE |= (1 << CAN_IE_ERRPASS_Pos);
}

void CAN_INTErrPassiveDis(CAN_TypeDef * CANx)
{
    CANx->IE &= ~(1 << CAN_IE_ERRPASS_Pos);
}

uint32_t CAN_INTErrPassiveStat(CAN_TypeDef * CANx)
{
    return (CANx->IF & CAN_IF_ERRPASS_Msk) ? 1 : 0;
}

void CAN_INTArbitrLostEn(CAN_TypeDef * CANx)
{
    CANx->IE |= (1 << CAN_IE_ARBLOST_Pos);
}

void CAN_INTArbitrLostDis(CAN_TypeDef * CANx)
{
    CANx->IE &= ~(1 << CAN_IE_ARBLOST_Pos);
}

uint32_t CAN_INTArbitrLostStat(CAN_TypeDef * CANx)
{
    return (CANx->IF & CAN_IF_ARBLOST_Msk) ? 1 : 0;
}

void CAN_INTBusErrorEn(CAN_TypeDef * CANx)
{
    CANx->IE |= (1 << CAN_IE_BUSERR_Pos);
}

void CAN_INTBusErrorDis(CAN_TypeDef * CANx)
{
    CANx->IE &= ~(1 << CAN_IE_BUSERR_Pos);
}

uint32_t CAN_INTBusErrorStat(CAN_TypeDef * CANx)
{
    return (CANx->IF & CAN_IF_BUSERR_Msk) ? 1 : 0;
}

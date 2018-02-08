/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"

extern _LONG_CALL_
HAL_Status HalSsiInitRtl8195a(VOID *Adaptor);

extern _LONG_CALL_ 
u32 HalGetCpuClk(VOID);


VOID _SsiReadInterruptRtl8195a(VOID *Adapter)
{
    SSI_DBG_ENTRANCE("_SsiReadInterrupt()\n");
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    u32 ReceiveLevel;
    volatile u32 Readable = HalSsiReadableRtl8195a(Adapter);
    u8  Index = pHalSsiAdapter->Index;

//    while (Readable) {
    if (Readable) {
        ReceiveLevel = HalSsiGetRxFifoLevelRtl8195a(Adapter);

        while (ReceiveLevel--) {
            if (pHalSsiAdapter->RxData != NULL) {
                if ((pHalSsiAdapter->DataFrameSize+1) > 8) {
                    // 16~9 bits mode
                    *((u16*)(pHalSsiAdapter->RxData)) = (u16)(HAL_SSI_READ32(Index, REG_DW_SSI_DR));
                    pHalSsiAdapter->RxData = (VOID*)(((u16*)pHalSsiAdapter->RxData) + 1);
                }
                else {
                    // 8~4 bits mode
                    *((u8*)(pHalSsiAdapter->RxData)) = (u8)(HAL_SSI_READ32(Index, REG_DW_SSI_DR));
                    pHalSsiAdapter->RxData = (VOID*)(((u8*)pHalSsiAdapter->RxData) + 1);
                }
            }
            else {
                // for Master mode, doing TX also will got RX data, so drop the dummy data
                HAL_SSI_READ32(Index, REG_DW_SSI_DR);
            }
            
            if (pHalSsiAdapter->RxLength > 0) {
                pHalSsiAdapter->RxLength--;
            }
#if 0
            else if (pHalSsiAdapter->RxLengthRemainder > 0) {
                pHalSsiAdapter->RxLengthRemainder--;
            }

            // Fixed length receive Complete. (RxLength & RxLengthRemainder == 0)
            if ((pHalSsiAdapter->RxLength == 0) && (pHalSsiAdapter->RxLengthRemainder == 0)) {
                break;
            }
#endif            
            if (pHalSsiAdapter->RxLength == 0) {
                break;
            }
        }

//        if (pHalSsiAdapter->RxLength == 0) {
//            break;
//        }
        
        if (((pHalSsiAdapter->InterruptMask & BIT_IMR_TXEIM)!=0) && (pHalSsiAdapter->TxLength > 0)) {
            _SsiWriteInterruptRtl8195a(pHalSsiAdapter);
        }

//        Readable = HalSsiReadableRtl8195a(Adapter);
    }

    if ((pHalSsiAdapter->RxLength > 0) && 
        (pHalSsiAdapter->RxLength < (pHalSsiAdapter->RxThresholdLevel+1))) {
        SSI_DBG_INT_READ("Setting Rx FIFO Threshold Level to 1\n");
        pHalSsiAdapter->RxThresholdLevel = 0;
        HalSsiSetRxFifoThresholdLevelRtl8195a((VOID*)pHalSsiAdapter);
    }

    if (pHalSsiAdapter->RxLength == 0) {
        DBG_SSI_INFO("_SsiReadInterruptRtl8195a: RX_Done\r\n");
        pHalSsiAdapter->InterruptMask &= ~(BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_RXUIM);
        HalSsiSetInterruptMaskRtl8195a((VOID*)pHalSsiAdapter);
//        if (pHalSsiAdapter->RxData != NULL) {
            if (pHalSsiAdapter->RxCompCallback != NULL) {
                pHalSsiAdapter->RxCompCallback(pHalSsiAdapter->RxCompCbPara);
            }
//        }
    }

}


VOID _SsiWriteInterruptRtl8195a(VOID *Adapter)
{
    SSI_DBG_ENTRANCE("_SsiWriteInterrupt()\n");
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    u32 Writeable     = HalSsiWriteableRtl8195a(Adapter);
//    u32 TxWriteMax    = SSI_TX_FIFO_DEPTH - pHalSsiAdapter->TxThresholdLevel;
    u32 TxWriteMax;
    u8  Index         = pHalSsiAdapter->Index;
    u32 i;
    volatile u32 bus_busy;

    if (pHalSsiAdapter->TxLength == 0) {
        pHalSsiAdapter->InterruptMask &= ~(BIT_IMR_TXEIM);
        HalSsiSetInterruptMaskRtl8195a((VOID*)pHalSsiAdapter);
        for (i=0;i<1000000;i++) {            
            bus_busy = HAL_SSI_READ32(Index, REG_DW_SSI_SR) & BIT_SR_BUSY;
            if (!bus_busy) {
                break;  // break the for loop
            }
        }

        // If it's not a dummy TX for master read SPI, then call the TX_done callback
        if (pHalSsiAdapter->TxData != NULL) {
            if (pHalSsiAdapter->TxIdleCallback != NULL) {
                pHalSsiAdapter->TxIdleCallback(pHalSsiAdapter->TxIdleCbPara);
            }
        }
        
        return;
    }

    if (Writeable) {
        TxWriteMax = SSI_TX_FIFO_DEPTH - HalSsiGetTxFifoLevelRtl8195a(Adapter);
        /* Disable Tx FIFO Empty IRQ */
        pHalSsiAdapter->InterruptMask &= ~ BIT_IMR_TXEIM;
        HalSsiSetInterruptMaskRtl8195a((VOID*)pHalSsiAdapter);
        //Clear RX FIFO first, to prevent RX Overflow
        if ((pHalSsiAdapter->RxLength > 0) && (pHalSsiAdapter->TxData != NULL)) {
            _SsiReadInterruptRtl8195a(pHalSsiAdapter);
        }

        while (TxWriteMax--) {
            if ((pHalSsiAdapter->DataFrameSize+1) > 8) {
                // 16~9 bits mode
                if (pHalSsiAdapter->TxData != NULL) {
                    HAL_SSI_WRITE16(Index, REG_DW_SSI_DR, *((u16*)(pHalSsiAdapter->TxData)));
                    pHalSsiAdapter->TxData = (VOID*)(((u16*)pHalSsiAdapter->TxData) + 1);
                }
                else {
                    // For master mode: Push a dummy to TX FIFO for Read
                    if (pHalSsiAdapter->Role == SSI_MASTER) {
                        HAL_SSI_WRITE16(Index, REG_DW_SSI_DR, (u16)SSI_DUMMY_DATA);    // Dummy byte
                    }
                }
            }
            else {
                // 8~4 bits mode
                if (pHalSsiAdapter->TxData != NULL) {
                    HAL_SSI_WRITE8(Index, REG_DW_SSI_DR, *((u8*)(pHalSsiAdapter->TxData)));
                    pHalSsiAdapter->TxData = (VOID*)(((u8*)pHalSsiAdapter->TxData) + 1);
                }
                else {
                    // For master mode: Push a dummy to TX FIFO for Read
                    if (pHalSsiAdapter->Role == SSI_MASTER) {
                        HAL_SSI_WRITE8(Index, REG_DW_SSI_DR, (u8)SSI_DUMMY_DATA);    // Dummy byte
                    }
                }
            }

            pHalSsiAdapter->TxLength--;

            if (pHalSsiAdapter->TxLength == 0)
                break;
        }

        /* Enable Tx FIFO Empty IRQ */
        pHalSsiAdapter->InterruptMask |= BIT_IMR_TXEIM;
        HalSsiSetInterruptMaskRtl8195a((VOID*)pHalSsiAdapter);
    }

    if (pHalSsiAdapter->TxLength == 0) {
        DBG_SSI_INFO("_SsiWriteInterruptRtl8195a: TX_Done\r\n");
        HalSsiTxFIFOThresholdRtl8195a((VOID*)pHalSsiAdapter, 0);    // trigger TX interrupt when TX FIFO is totally empty
//        pHalSsiAdapter->InterruptMask &= ~(BIT_IMR_TXOIM | BIT_IMR_TXEIM);
        pHalSsiAdapter->InterruptMask &= ~(BIT_IMR_TXOIM);
        HalSsiSetInterruptMaskRtl8195a((VOID*)pHalSsiAdapter);
        // If it's not a dummy TX for master read SPI, then call the TX_done callback
        if (pHalSsiAdapter->TxData != NULL) {
            if (pHalSsiAdapter->TxCompCallback != NULL) {
                pHalSsiAdapter->TxCompCallback(pHalSsiAdapter->TxCompCbPara);
            }
        }
    }
}


u32 _SsiIrqHandleRtl8195a(VOID *Adaptor)
{
    SSI_DBG_ENTRANCE("_SsiIrqHandle()\n");
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Adaptor;
    u32 InterruptStatus = HalSsiGetInterruptStatusRtl8195a(Adaptor);
    u8  Index = pHalSsiAdaptor->Index;

    if (InterruptStatus & BIT_ISR_TXOIS) {
        DBG_SSI_ERR("[INT][SSI%d] Transmit FIFO Overflow Interrupt\n", Index);
        HAL_SSI_READ32(Index, REG_DW_SSI_TXOICR);
    }

    if (InterruptStatus & BIT_ISR_RXUIS) {
        SSI_DBG_INT_HNDLR("[INT][SSI%d] Receive FIFO Underflow Interrupt\n", Index);
        HAL_SSI_READ32(Index, REG_DW_SSI_RXUICR);
    }

    if (InterruptStatus & BIT_ISR_RXOIS) {
        DBG_SSI_ERR("[INT][SSI%d] Receive FIFO Overflow Interrupt\n", Index);
        HAL_SSI_READ32(Index, REG_DW_SSI_RXOICR);
    }

    if (InterruptStatus & BIT_ISR_MSTIS) {
        SSI_DBG_INT_HNDLR("[INT][SSI%d] Multi-Master Contention Interrupt\n", Index);
        /* Another master is actively transferring data */
        /* TODO: Do reading data... */
        HAL_SSI_READ32(Index, REG_DW_SSI_MSTICR);
    }

    if ((InterruptStatus & BIT_ISR_RXFIS) ) {
        SSI_DBG_INT_HNDLR("[INT][SSI%d] Receive FIFO Full Interrupt\n", Index);
        _SsiReadInterruptRtl8195a(Adaptor);
    }

    if ((InterruptStatus & BIT_ISR_TXEIS) ||
        (((pHalSsiAdaptor->InterruptMask & BIT_IMR_TXEIM)!=0) && (pHalSsiAdaptor->TxLength > 0))) {
        /* Tx FIFO is empty, need to transfer data */
        SSI_DBG_INT_HNDLR("[INT][SSI%d] Transmit FIFO Empty Interrupt\n", Index);
        _SsiWriteInterruptRtl8195a(Adaptor);
    }

    return 0;
}

HAL_Status HalSsiInitRtl8195a_Patch(VOID *Adaptor)
{
    SSI_DBG_ENTRANCE("HalSsiInitRtl8195a()\n");
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Adaptor;
    volatile IRQn_Type IrqNum;
    u32  IRQ_UNKNOWN  = 999;
    u32  Ctrlr0Value  = 0;
    u32  Ctrlr1Value  = 0;
    u32  SerValue     = 0;
    u32  BaudrValue   = 0;
    u32  TxftlrValue  = 0;
    u32  RxftlrValue  = 0;
    u32  MwcrValue    = 0;
    u8   MicrowireTransferMode = pHalSsiAdaptor->MicrowireTransferMode;
    u8   DataFrameFormat = pHalSsiAdaptor->DataFrameFormat;
    u8   TransferMode = pHalSsiAdaptor->TransferMode;
    u8   Index = pHalSsiAdaptor->Index;
    u8   Role  = pHalSsiAdaptor->Role;

    if (Index > 2) {
        DBG_SSI_ERR("HalSsiInitRtl8195a: Invalid SSI Idx %d\r\n", Index);
        return HAL_ERR_PARA;
    }
    HalSsiPinmuxEnableRtl8195a_Patch(pHalSsiAdaptor);    
    HalSsiDisableRtl8195a(Adaptor);

    /* REG_DW_SSI_CTRLR0 */
    Ctrlr0Value |= BIT_CTRLR0_DFS(pHalSsiAdaptor->DataFrameSize);
    Ctrlr0Value |= BIT_CTRLR0_FRF(pHalSsiAdaptor->DataFrameFormat);
    Ctrlr0Value |= BIT_CTRLR0_SCPH(pHalSsiAdaptor->SclkPhase);
    Ctrlr0Value |= BIT_CTRLR0_SCPOL(pHalSsiAdaptor->SclkPolarity);
    Ctrlr0Value |= BIT_CTRLR0_TMOD(pHalSsiAdaptor->TransferMode);
    Ctrlr0Value |= BIT_CTRLR0_CFS(pHalSsiAdaptor->ControlFrameSize);

    if (Role == SSI_SLAVE)
        Ctrlr0Value |= BIT_CTRLR0_SLV_OE(pHalSsiAdaptor->SlaveOutputEnable);

    SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_CTRLR0 Value: %X\n", Index, Ctrlr0Value);

    HAL_SSI_WRITE32(Index, REG_DW_SSI_CTRLR0, Ctrlr0Value);

    SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_CTRLR0(%X) = %X\n", Index,
            SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_CTRLR0,
            HAL_SSI_READ32(Index, REG_DW_SSI_CTRLR0));

    /* REG_DW_SSI_TXFTLR */
    TxftlrValue = BIT_TXFTLR_TFT(pHalSsiAdaptor->TxThresholdLevel);
    SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_TXFTLR Value: %X\n", Index, TxftlrValue);

    HAL_SSI_WRITE32(Index, REG_DW_SSI_TXFTLR, TxftlrValue);

    SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_TXFTLR(%X) = %X\n", Index,
            SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_TXFTLR,
            HAL_SSI_READ32(Index, REG_DW_SSI_TXFTLR));

    /* REG_DW_SSI_RXFTLR */
    RxftlrValue = BIT_RXFTLR_RFT(pHalSsiAdaptor->RxThresholdLevel);
    SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_RXFTLR Value: %X\n", Index, RxftlrValue);

    HAL_SSI_WRITE32(Index, REG_DW_SSI_RXFTLR, RxftlrValue);

    SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_RXFTLR(%X) = %X\n", Index,
            SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_RXFTLR,
            HAL_SSI_READ32(Index, REG_DW_SSI_RXFTLR));
    /**
     * Master Only
     * REG_DW_SSI_CTRLR1, REG_DW_SSI_SER, REG_DW_SSI_BAUDR
     */
    if (Role & SSI_MASTER) {
        if ((TransferMode == TMOD_RO) || (TransferMode == TMOD_EEPROM_R) ||
                ((DataFrameFormat == FRF_NS_MICROWIRE) && (MicrowireTransferMode == MW_TMOD_SEQUENTIAL))) {
            Ctrlr1Value = BIT_CTRLR1_NDF(pHalSsiAdaptor->DataFrameNumber);
            SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_CTRLR1 Value: %X\n", Index, Ctrlr1Value);

            HAL_SSI_WRITE32(Index, REG_DW_SSI_CTRLR1, Ctrlr1Value);

            SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_CTRLR1(%X) = %X\n", Index,
                    SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_CTRLR1,
                    HAL_SSI_READ32(Index, REG_DW_SSI_CTRLR1));
        }

        SerValue = BIT_SER_SER(1 << (pHalSsiAdaptor->SlaveSelectEnable));
        SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_SER Value: %X\n", Index, SerValue);

        //HAL_SSI_WRITE32(Index, REG_DW_SSI_SER, SerValue);
        HalSsiSetSlaveEnableRegisterRtl8195a(Adaptor, pHalSsiAdaptor->SlaveSelectEnable);

        SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_SER(%X) = %X\n", Index,
                SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_SER,
                HalSsiGetSlaveEnableRegisterRtl8195a(Adaptor));

        BaudrValue = BIT_BAUDR_SCKDV(pHalSsiAdaptor->ClockDivider);
        SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_BAUDR Value: %X\n", Index, BaudrValue);

        HAL_SSI_WRITE32(Index, REG_DW_SSI_BAUDR, BaudrValue);

        SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_BAUDR(%X) = %X\n", Index,
                SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_BAUDR,
                HAL_SSI_READ32(Index, REG_DW_SSI_BAUDR));
    }

    // Microwire
    MwcrValue |= BIT_MWCR_MWMOD(pHalSsiAdaptor->MicrowireTransferMode);
    MwcrValue |= BIT_MWCR_MDD(pHalSsiAdaptor->MicrowireDirection);
    MwcrValue |= BIT_MWCR_MHS(pHalSsiAdaptor->MicrowireHandshaking);
    SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_MWCR Value: %X\n", Index, MwcrValue);

    HAL_SSI_WRITE32(Index, REG_DW_SSI_MWCR, MwcrValue);

    SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_MWCR(%X) = %X\n", Index,
            SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_MWCR,
            HAL_SSI_READ32(Index, REG_DW_SSI_MWCR));

    SSI_DBG_INIT("SSI%d TransferMechanism: %d\n", Index, pHalSsiAdaptor->TransferMechanism);
    if (pHalSsiAdaptor->TransferMechanism == SSI_DTM_INTERRUPT)
    {
        SSI_DBG_INIT("SSI%d Interrupt initialize, Interrupt: %X\n", Index, pHalSsiAdaptor->InterruptMask);
        switch (Index) {
            case 0:
                IrqNum = SPI0_IRQ;
                break;
            case 1:
                IrqNum = SPI1_IRQ;
                break;
            case 2:
                IrqNum = SPI2_IRQ;
                break;
            default:
                IrqNum = IRQ_UNKNOWN;
                break;
        }

        if (likely(IrqNum != IRQ_UNKNOWN)) {
            /* REG_DW_SSI_IMR */
            HalSsiSetInterruptMaskRtl8195a(Adaptor);

            pHalSsiAdaptor->IrqHandle.Data     = (u32)pHalSsiAdaptor;
            pHalSsiAdaptor->IrqHandle.IrqFun   = (IRQ_FUN)_SsiIrqHandleRtl8195a;
            pHalSsiAdaptor->IrqHandle.IrqNum   = (IRQn_Type)IrqNum;
            pHalSsiAdaptor->IrqHandle.Priority = pHalSsiAdaptor->InterruptPriority;

            InterruptRegister(&pHalSsiAdaptor->IrqHandle);
            InterruptEn(&pHalSsiAdaptor->IrqHandle);
        }
        else {
            SSI_DBG_INIT("Unknown SSI Index.\n");
            pHalSsiAdaptor->TransferMechanism = SSI_DTM_BASIC;
        }
    }

    HalSsiEnableRtl8195a(Adaptor);

    return HAL_OK;
}

HAL_Status HalSsiPinmuxEnableRtl8195a_Patch(VOID *Adaptor)
{
    SSI_DBG_ENTRANCE("HalSsiPinmuxEnableRtl8195a()\n");
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Adaptor;
    volatile HAL_Status Result;
    u32 PinmuxSelect = pHalSsiAdaptor->PinmuxSelect;
    u8  Index = pHalSsiAdaptor->Index;

    SSI_DBG_PINMUX("[1] SSI%d REG_SSI_MUX_CTRL(%X) = %X\n", Index,
            PERI_ON_BASE + REG_SPI_MUX_CTRL,
            HAL_READ32(PERI_ON_BASE, REG_SPI_MUX_CTRL));

    switch (Index)
    {
        case 0:
        {
            ACTCK_SPI0_CCTRL(ON);
            SLPCK_SPI0_CCTRL(ON);
            PinCtrl(SPI0, PinmuxSelect, ON);
            SPI0_FCTRL(ON);
            Result = HAL_OK;
            break;
        }
        case 1:
        {
            ACTCK_SPI1_CCTRL(ON);
            SLPCK_SPI1_CCTRL(ON);
            PinCtrl(SPI1, PinmuxSelect, ON);
            SPI1_FCTRL(ON);
            Result = HAL_OK;
            break;
        }
        case 2:
        {
            ACTCK_SPI2_CCTRL(ON);
            SLPCK_SPI2_CCTRL(ON);
            PinCtrl(SPI2, PinmuxSelect, ON);
            SPI2_FCTRL(ON);
            Result = HAL_OK;
            break;
        }
        default:
        {
            DBG_SSI_ERR("Invalid SSI Index %d!\n", Index);
            Result = HAL_ERR_PARA;
            break;
        }
    }

    SSI_DBG_PINMUX("[2] SSI%d REG_SSI_MUX_CTRL(%X) = %X\n", Index,
            PERI_ON_BASE + REG_SPI_MUX_CTRL,
            HAL_READ32(PERI_ON_BASE, REG_SPI_MUX_CTRL));

    return Result;
}

HAL_Status HalSsiPinmuxDisableRtl8195a(VOID *Adaptor)
{
    SSI_DBG_ENTRANCE("HalSsiPinmuxEnableRtl8195a()\n");
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Adaptor;
    volatile HAL_Status Result;
    u32 PinmuxSelect = pHalSsiAdaptor->PinmuxSelect;
    u8  Index = pHalSsiAdaptor->Index;

    SSI_DBG_PINMUX("[1] SSI%d REG_SSI_MUX_CTRL(%X) = %X\n", Index,
            PERI_ON_BASE + REG_SPI_MUX_CTRL,
            HAL_READ32(PERI_ON_BASE, REG_SPI_MUX_CTRL));

    switch (Index)
    {
        case 0:
        {
            ACTCK_SPI0_CCTRL(OFF);
            SLPCK_SPI0_CCTRL(OFF);
            PinCtrl(SPI0, PinmuxSelect, OFF);
            SPI0_FCTRL(OFF);
            Result = HAL_OK;
            break;
        }
        case 1:
        {
            ACTCK_SPI1_CCTRL(OFF);
            SLPCK_SPI1_CCTRL(OFF);
            PinCtrl(SPI1, PinmuxSelect, OFF);
            SPI1_FCTRL(OFF);
            Result = HAL_OK;
            break;
        }
        case 2:
        {
            ACTCK_SPI2_CCTRL(OFF);
            SLPCK_SPI2_CCTRL(OFF);
            PinCtrl(SPI2, PinmuxSelect, OFF);
            SPI2_FCTRL(OFF);
            Result = HAL_OK;
            break;
        }
        default:
        {
            DBG_SSI_ERR("Invalid SSI Index %d!\n", Index);
            Result = HAL_ERR_PARA;
            break;
        }
    }

    SSI_DBG_PINMUX("[2] SSI%d REG_SSI_MUX_CTRL(%X) = %X\n", Index,
            PERI_ON_BASE + REG_SPI_MUX_CTRL,
            HAL_READ32(PERI_ON_BASE, REG_SPI_MUX_CTRL));

    return Result;
}

HAL_Status HalSsiDeInitRtl8195a(VOID *Adapter)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    u8 Index;
    volatile HAL_Status Result;

    Index = pHalSsiAdapter->Index;

    if(Index > 2){
        DBG_SSI_ERR("Invalid SSI Index %d!\n", Index);
        return HAL_ERR_PARA;
    }

    HalSsiInterruptDisableRtl8195a(pHalSsiAdapter);  
    HalSsiDisableRtl8195a(pHalSsiAdapter);
    
    Result = HalSsiPinmuxDisableRtl8195a(pHalSsiAdapter);   
    if(Result != HAL_OK){
        DBG_SSI_ERR("Pinmux Disable Error\n");
        return Result;
    }

    return Result;
}

HAL_Status HalSsiClockOnRtl8195a(VOID *Adapter)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Adapter;
    volatile HAL_Status Result;
    u8 Index = pHalSsiAdaptor->Index;

    switch (Index)
    {
        case 0:
        {
            ACTCK_SPI0_CCTRL(ON);
            SLPCK_SPI0_CCTRL(ON);
            Result = HAL_OK;
            break;
        }
        case 1:
        {
            ACTCK_SPI1_CCTRL(ON);
            SLPCK_SPI1_CCTRL(ON);
            Result = HAL_OK;
            break;
        }
        case 2:
        {
            ACTCK_SPI2_CCTRL(ON);
            SLPCK_SPI2_CCTRL(ON);
            Result = HAL_OK;
            break;
        }
        default:
        {
            DBG_SSI_ERR("Invalid SSI Index %d!\n", Index);
            Result = HAL_ERR_PARA;
            break;
        }
    }
    return Result;
}

HAL_Status HalSsiClockOffRtl8195a(VOID *Adapter)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Adapter;
    volatile HAL_Status Result;
    u8 Index = pHalSsiAdaptor->Index;

    switch (Index)
    {
        case 0:
        {
            ACTCK_SPI0_CCTRL(OFF);
            SLPCK_SPI0_CCTRL(OFF);
            Result = HAL_OK;
            break;
        }
        case 1:
        {
            ACTCK_SPI1_CCTRL(OFF);
            SLPCK_SPI1_CCTRL(OFF);
            Result = HAL_OK;
            break;
        }
        case 2:
        {
            ACTCK_SPI2_CCTRL(OFF);
            SLPCK_SPI2_CCTRL(OFF);
            Result = HAL_OK;
            break;
        }
        default:
        {
            DBG_SSI_ERR("Invalid SSI Index %d!\n", Index);
            Result = HAL_ERR_PARA;
            break;
        }
    }
    return Result;

}

HAL_Status HalSsiSetFormatRtl8195a(VOID *Adaptor)
{
    SSI_DBG_ENTRANCE("HalSsiSetFormatRtl8195a()\n");
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Adaptor;
    u32  Ctrlr0Value  = 0;
    u32 TxftlrValue = 0;
    u32 RxftlrValue = 0;
    u8   Index = pHalSsiAdaptor->Index;
    u8   Role  = pHalSsiAdaptor->Role;
	u32 Spi_mode = 0;

    if (Index > 2) {
        DBG_SSI_ERR("HalSsiSetFormatRtl8195a: Invalid SSI Idx %d\r\n", Index);
        return HAL_ERR_PARA;
    }
    HalSsiDisableRtl8195a(Adaptor);

    Ctrlr0Value &= ~(BIT_CTRLR0_SCPH(1) | BIT_CTRLR0_SCPOL(1) | BIT_CTRLR0_DFS(0xF));

    /* REG_DW_SSI_CTRLR0 */
    Ctrlr0Value |= BIT_CTRLR0_DFS(pHalSsiAdaptor->DataFrameSize);
    Ctrlr0Value |= BIT_CTRLR0_SCPH(pHalSsiAdaptor->SclkPhase);
    Ctrlr0Value |= BIT_CTRLR0_SCPOL(pHalSsiAdaptor->SclkPolarity);

    if (Role == SSI_SLAVE)
        Ctrlr0Value |= BIT_CTRLR0_SLV_OE(pHalSsiAdaptor->SlaveOutputEnable);

    SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_CTRLR0 Value: %X\n", Index, Ctrlr0Value);

    HAL_SSI_WRITE32(Index, REG_DW_SSI_CTRLR0, Ctrlr0Value);

    Spi_mode = (HAL_SSI_READ32(Index, REG_DW_SSI_CTRLR0) >>6) & 0x3;
    SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_CTRLR0(%X) = %X, SPI Mode = %X\n", Index,
            SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_CTRLR0,
            HAL_SSI_READ32(Index, REG_DW_SSI_CTRLR0), Spi_mode);
    //The tx threshold and rx threshold value will be reset after the spi changes its role
    /* REG_DW_SSI_TXFTLR */
    TxftlrValue = BIT_TXFTLR_TFT(pHalSsiAdaptor->TxThresholdLevel);
    SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_TXFTLR Value: %X\n", Index, TxftlrValue);

    HAL_SSI_WRITE32(Index, REG_DW_SSI_TXFTLR, TxftlrValue);

    SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_TXFTLR(%X) = %X\n", Index,
            SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_TXFTLR,
            HAL_SSI_READ32(Index, REG_DW_SSI_TXFTLR));

    /* REG_DW_SSI_RXFTLR */
    RxftlrValue = BIT_RXFTLR_RFT(pHalSsiAdaptor->RxThresholdLevel);
    SSI_DBG_INIT("[1] Set SSI%d REG_DW_SSI_RXFTLR Value: %X\n", Index, RxftlrValue);

    HAL_SSI_WRITE32(Index, REG_DW_SSI_RXFTLR, RxftlrValue);

    SSI_DBG_INIT("[2] SSI%d REG_DW_SSI_RXFTLR(%X) = %X\n", Index,
            SSI0_REG_BASE + (SSI_REG_OFF * Index) + REG_DW_SSI_RXFTLR,
            HAL_SSI_READ32(Index, REG_DW_SSI_RXFTLR));

    HalSsiEnableRtl8195a(Adaptor);

    return HAL_OK;
}

VOID HalSsiSetSclkRtl8195a(VOID *Adapter, u32 ClkRate)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    u32 ssi_clk;
    u32 ClockDivider;
    u32 SsiEn;
    u32 RegValue;
    u32 SystemClock;
    u8  spi_idx = pHalSsiAdapter->Index;

    // Set SCLK Freq only available for Master mode
    // For Slave mode, the baud rate is depends on the Master side, max rate = ssi_clk/2
    // Fsclk_out = Fssi_clk/SCKDV
    SystemClock = HalGetCpuClk();
    
    if (spi_idx == 1) {
        ssi_clk = SystemClock >> 1;
        RegValue = HAL_READ32(SYSTEM_CTRL_BASE, 0x250);
        if (ClkRate > (ssi_clk/2)) {
            // Use High speed clock: Fixed Freq.
            RegValue |= BIT18;
            ssi_clk = (200000000*5/6) >> 1;            
        }
        else {
            // Use Normal speed clock: CPU_Clk/2
            RegValue &= ~BIT18;
        }
        HAL_WRITE32(SYSTEM_CTRL_BASE, 0x250, RegValue);
    }
    else {
        ssi_clk = SystemClock >> 2;
    }

    if (pHalSsiAdapter->Role == SSI_MASTER) {
        if (ClkRate > (ssi_clk/2)) {
            DBG_SSI_ERR("spi_frequency: Freq %d is too high, available highest Freq=%d\r\n", ClkRate, (ssi_clk/2));
            ClockDivider = 2;
        } 
        else {
            ClockDivider = ssi_clk/ClkRate + 1;
            if ((ssi_clk%ClkRate) > (ClkRate/2)) {
                ClockDivider++;
            }
            if (ClockDivider >= 0xFFFF) {
                // devider is 16 bits
                ClockDivider = 0xFFFE;
            }
            ClockDivider &= 0xFFFE;     // bit 0 always is 0
        }
        DBG_SSI_INFO("spi_frequency: Set SCLK Freq=%d\r\n", (ssi_clk/ClockDivider));
        pHalSsiAdapter->ClockDivider = ClockDivider;
        
        SsiEn = HAL_SSI_READ32(spi_idx, REG_DW_SSI_SSIENR); // Backup SSI_EN register
        
        // Disable SSI first, so we can modify the Clock Divider
        RegValue = SsiEn & BIT_INVC_SSIENR_SSI_EN;
        HAL_SSI_WRITE32(spi_idx, REG_DW_SSI_SSIENR, RegValue);
        HAL_SSI_WRITE32(spi_idx, REG_DW_SSI_BAUDR, (pHalSsiAdapter->ClockDivider & 0xFFFF));
        // recover the SSI_EN setting
        HAL_SSI_WRITE32(spi_idx, REG_DW_SSI_SSIENR, SsiEn);
    }
    else {
        if (ClkRate > (ssi_clk/10)) {
            DBG_SSI_ERR("spi_frequency: Freq %d is too high, available highest Freq=%d\r\n", ClkRate, (ssi_clk/10));
        }
    }    
}

HAL_Status HalSsiIntReadRtl8195a(VOID *Adapter, VOID *RxData, u32 Length)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    u32 RxFifoThresholdLevel;
    u8  Index = pHalSsiAdapter->Index;

    DBG_SSI_INFO("HalSsiIntReadRtl8195a: Idx=%d, RxData=0x%x, Len=0x%x\r\n", Index, RxData, Length);
//    if (HalSsiBusyRtl8195a(Adapter)) {
        // As a Slave mode, if the peer(Master) side is power off, the BUSY flag is always on
//        DBG_SSI_WARN("HalSsiIntReadRtl8195a: SSI%d is busy\n", Index);
//        return HAL_BUSY;
//    }

    if (Length == 0) {
        SSI_DBG_INT_READ("SSI%d RxData addr: 0x%X, Length: %d\n", Index, RxData, Length);
        return HAL_ERR_PARA;
    }

    if (Length > (pHalSsiAdapter->DefaultRxThresholdLevel)) {
        RxFifoThresholdLevel = pHalSsiAdapter->DefaultRxThresholdLevel;
    }
    else {
        RxFifoThresholdLevel = 0;        
    }

    if (pHalSsiAdapter->RxThresholdLevel != RxFifoThresholdLevel) {
        DBG_SSI_INFO("Setting Rx FIFO Threshold Level to %d\n", RxFifoThresholdLevel);
        pHalSsiAdapter->RxThresholdLevel = RxFifoThresholdLevel;
        HalSsiSetRxFifoThresholdLevelRtl8195a((VOID*)pHalSsiAdapter);
    }

    if ((pHalSsiAdapter->DataFrameSize+1) > 8) {
        // 16~9 bits mode
        pHalSsiAdapter->RxLength = Length >> 1; // 2 bytes(16 bit) every transfer
    }
    else {
        // 8~4 bits mode
        pHalSsiAdapter->RxLength = Length; // 1 byte(8 bit) every transfer
    }

    pHalSsiAdapter->RxData = RxData;
    DBG_SSI_INFO("SSI%d RxData addr: 0x%X, Length: %d\n", Index,
            pHalSsiAdapter->RxData, pHalSsiAdapter->RxLength);

    pHalSsiAdapter->InterruptMask |= BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_RXUIM;
    HalSsiSetInterruptMaskRtl8195a((VOID*)pHalSsiAdapter);

    return HAL_OK;
}

// Configure Transmit FIFO Threshold Level
VOID HalSsiTxFIFOThresholdRtl8195a(VOID *Adaptor, u32 txftl)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Adaptor;
    u8   Index = pHalSsiAdaptor->Index;
    u32  TxftlrValue;

    TxftlrValue = BIT_TXFTLR_TFT(txftl);

    HAL_SSI_WRITE32(Index, REG_DW_SSI_TXFTLR, TxftlrValue);
}


HAL_Status
HalSsiIntWriteRtl8195a(
    IN VOID *Adapter,      // PHAL_SSI_ADAPTOR
    IN u8 *pTxData,     // the Buffer to be send
    IN u32 Length      // the length of data to be send
)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;

    DBG_SSI_INFO("HalSsiIntWriteRtl8195a: Idx=%d, RxData=0x%x, Len=0x%x\r\n", pHalSsiAdapter->Index, pTxData, Length);
    if ((Length == 0)) {
        DBG_SSI_ERR("HalSsiIntSendRtl8195a: Err: pTxData=0x%x,  Length=%d\n", pTxData, Length);
        return HAL_ERR_PARA;
    }

    if ((pHalSsiAdapter->DataFrameSize+1) > 8) {
        // 16~9 bits mode
        pHalSsiAdapter->TxLength = Length >> 1; // 2 bytes(16 bit) every transfer
    }
    else {
        // 8~4 bits mode
        pHalSsiAdapter->TxLength = Length; // 1 byte(8 bit) every transfer
    }

    HalSsiTxFIFOThresholdRtl8195a(Adapter, pHalSsiAdapter->TxThresholdLevel);
    pHalSsiAdapter->TxData = (void*)pTxData;
    pHalSsiAdapter->InterruptMask |= BIT_IMR_TXOIM | BIT_IMR_TXEIM;
    HalSsiSetInterruptMaskRtl8195a((VOID*)pHalSsiAdapter);

    return HAL_OK;
}

HAL_Status HalSsiEnterCriticalRtl8195a(VOID *Data)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Data;

    InterruptDis(&pHalSsiAdaptor->IrqHandle);
#ifdef CONFIG_GDMA_EN
    PSSI_DMA_CONFIG pDmaConfig = &pHalSsiAdaptor->DmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;

    if(NULL != pDmaConfig){
        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pTxHalGdmaAdapter;
        if(pHalGdmaAdapter->ChEn != 0){
            InterruptDis(&pDmaConfig->TxGdmaIrqHandle);
        }
        
        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pRxHalGdmaAdapter;
        if(pHalGdmaAdapter->ChEn != 0){
            InterruptDis(&pDmaConfig->RxGdmaIrqHandle);
        }
    }
#endif
    return HAL_OK;
}

HAL_Status HalSsiExitCriticalRtl8195a(VOID *Data)
{
    PHAL_SSI_ADAPTOR pHalSsiAdaptor = (PHAL_SSI_ADAPTOR) Data;

    InterruptEn(&pHalSsiAdaptor->IrqHandle);
#ifdef CONFIG_GDMA_EN
    PSSI_DMA_CONFIG pDmaConfig = &pHalSsiAdaptor->DmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;

    if(NULL != pDmaConfig){
        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pTxHalGdmaAdapter;
        if(pHalGdmaAdapter->ChEn != 0){
            InterruptEn(&pDmaConfig->TxGdmaIrqHandle);
        }
        
        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pRxHalGdmaAdapter;
        if(pHalGdmaAdapter->ChEn != 0){
            InterruptEn(&pDmaConfig->RxGdmaIrqHandle);
        }
    }
#endif
    return HAL_OK;
}

HAL_Status
HalSsiIsTimeoutRtl8195a(
    u32 StartCount,
    u32 TimeoutCnt
)
{
    HAL_Status Status;
    u32 CurrentCount, ExpireCount;

    CurrentCount = HalTimerOp.HalTimerReadCount(1);

    if (StartCount < CurrentCount) {
        ExpireCount =  (0xFFFFFFFF - CurrentCount) + StartCount;
    }
    else {
        ExpireCount = StartCount - CurrentCount;
    }

    if (TimeoutCnt < ExpireCount) {
        Status = HAL_TIMEOUT;
    }
    else {
        Status = HAL_OK;
    }

    return Status;
}

HAL_Status HalSsiStopRecvRtl8195a(VOID *Data)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Data;
    PSSI_DMA_CONFIG pDmaConfig = &pHalSsiAdapter->DmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP pHalGdmaOp;
    u32 DMAStopAddr = 0;
    u32 ReceivedCnt;
    u8 Index;
    u8 DmaMode = 0;
    
    Index = pHalSsiAdapter->Index;
    pHalSsiAdapter->InterruptMask &= ~(BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_RXUIM);
    HalSsiSetInterruptMaskRtl8195a(pHalSsiAdapter);

    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pRxHalGdmaAdapter;

    if(NULL != pHalGdmaAdapter){
        pHalGdmaOp = (PHAL_GDMA_OP)pDmaConfig->pHalGdmaOp;
        if ((NULL != pHalGdmaOp) && (HalGdmaQueryChEnRtl8195a((VOID*)pHalGdmaAdapter))){
            DmaMode = 1;
            pHalGdmaOp->HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
            pHalGdmaOp->HalGdmaChCleanAutoSrc((VOID*)pHalGdmaAdapter);
            pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));
            while((HAL_GDMAX_READ32(pHalGdmaAdapter->GdmaIndex, REG_GDMA_CH_EN) & (pHalGdmaAdapter->ChEn)) != 0 ){

            }
            DMAStopAddr = HalGdmaQueryDArRtl8195a((VOID*)pHalGdmaAdapter);      
            ReceivedCnt = DMAStopAddr - (u32)(pHalSsiAdapter->RxData);
            pHalSsiAdapter->RxLength-= ReceivedCnt;
            pHalSsiAdapter->RxData = (u8 *)(pHalSsiAdapter->RxData) + ReceivedCnt;
        }
    }

    while(HalSsiGetRxFifoLevelRtl8195a(pHalSsiAdapter) != 0){
        if ((pHalSsiAdapter->DataFrameSize+1) > 8) {
            // 16~9 bits mode
            *((u16*)(pHalSsiAdapter->RxData)) = (u16)(HAL_SSI_READ32(Index, REG_DW_SSI_DR));
            pHalSsiAdapter->RxData = (VOID*)(((u16*)pHalSsiAdapter->RxData) + 1);
            if(DmaMode){     
                pHalSsiAdapter->RxLength--;
            }
        }        
        else {
            // 8~4 bits mode
            *((u8*)(pHalSsiAdapter->RxData)) = (u8)(HAL_SSI_READ32(Index, REG_DW_SSI_DR));
            pHalSsiAdapter->RxData = (VOID*)(((u8*)pHalSsiAdapter->RxData) + 1);
        }
        pHalSsiAdapter->RxLength--;
    }

    return HAL_OK;
       
}

#ifdef CONFIG_GDMA_EN
/**
 * GDMA IRQ Handler
 */
VOID SsiTxGdmaIrqHandle (VOID *Data)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Data;
    PSSI_DMA_CONFIG pDmaConfig = &pHalSsiAdapter->DmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP pHalGdmaOp;

    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pTxHalGdmaAdapter;
    pHalGdmaOp = (PHAL_GDMA_OP)pDmaConfig->pHalGdmaOp;
    /* Clear Pending ISR */
    pHalGdmaOp->HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    pHalGdmaOp->HalGdmaChCleanAutoDst((VOID*)pHalGdmaAdapter);

    pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));
    // Call user TX complete callback
    pHalSsiAdapter->TxLength = 0;
    HalSsiTxFIFOThresholdRtl8195a((VOID*)pHalSsiAdapter, 0);    // trigger TX interrupt when TX FIFO is totally empty
    pHalSsiAdapter->InterruptMask |= (BIT_IMR_TXEIM);
    HalSsiSetInterruptMaskRtl8195a((VOID*)pHalSsiAdapter);

    if (NULL != pHalSsiAdapter->TxCompCallback) {
        pHalSsiAdapter->TxCompCallback(pHalSsiAdapter->TxCompCbPara);
    }

#if 0
    /* Set SSI DMA Disable */
    HAL_SSI_WRITE32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR, \
        (HAL_SSI_READ32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR) & ~SSI_TXDMA_ENABLE));
#endif

}

VOID SsiRxGdmaIrqHandle (VOID *Data)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Data;
    PSSI_DMA_CONFIG pDmaConfig = &pHalSsiAdapter->DmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP pHalGdmaOp;


    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pRxHalGdmaAdapter;
    pHalGdmaOp = (PHAL_GDMA_OP)pDmaConfig->pHalGdmaOp;
    
    pHalGdmaOp->HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    pHalGdmaOp->HalGdmaChCleanAutoSrc((VOID*)pHalGdmaAdapter);

    HAL_SSI_WRITE32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR, \
        (HAL_SSI_READ32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR) & ~SSI_RXDMA_ENABLE));
    pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));

    if (NULL != pHalSsiAdapter->RxCompCallback) {
        pHalSsiAdapter->RxCompCallback(pHalSsiAdapter->RxCompCbPara);
    }

}

VOID
HalSsiTxGdmaLoadDefRtl8195a(
        IN VOID *Adapter
)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    PSSI_DMA_CONFIG pDmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    u8 *pDst;
    u8 DmaIdx;
    u8 DmaCh;
    u8 DstPer;
    u8 ssi_idx;
    u32 DmaChEn;
    IRQn_Type IrqNum;

    if ((NULL == pHalSsiAdapter)) {
        return;
    }
    pDmaConfig = &pHalSsiAdapter->DmaConfig;

    ssi_idx = pHalSsiAdapter->Index;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pTxHalGdmaAdapter;
    if (NULL == pHalGdmaAdapter) {
        DBG_SSI_ERR("HalSsiTxGdmaLoadDefRtl8195a: HalGdmaAdapter is NULL\r\n");
        return;
    }
    _memset((void *)pHalGdmaAdapter, 0, sizeof(HAL_GDMA_ADAPTER));

    pHalSsiAdapter->DmaControl |= SSI_TXDMA_ENABLE;
    if ((pHalSsiAdapter->DataFrameSize+1) > 8) {
        // 16~9 bits mode
        pHalSsiAdapter->DmaTxDataLevel = 48;  // When TX FIFO entity number <=48 then DMA Request asserted
    }
    else {
        // 8~4 bits mode
        pHalSsiAdapter->DmaTxDataLevel = 56;  // When TX FIFO entity number <=56 then DMA Request asserted
    }

    switch (ssi_idx) {
        case 0:
            pDst = (u8*) (SSI0_REG_BASE + REG_DW_SSI_DR);
            DmaIdx = 0;
            DmaCh = 1;
            DmaChEn = GdmaCh1;
            IrqNum = GDMA0_CHANNEL1_IRQ;
            DstPer = GDMA_HANDSHAKE_SSI0_TX;
            break;

        case 1:
            pDst = (u8*) (SSI1_REG_BASE + REG_DW_SSI_DR);
            DmaIdx = 1;
            DmaCh = 1;
            DmaChEn = GdmaCh1;
            IrqNum = GDMA1_CHANNEL1_IRQ;
            DstPer = GDMA_HANDSHAKE_SSI1_TX;
            break;

        case 2:
            pDst = (u8*) (SSI2_REG_BASE + REG_DW_SSI_DR);
            DmaIdx = 0;     // SPI2 TX only can use GDMA0
            DmaCh = 3;
            DmaChEn = GdmaCh3;
            IrqNum = GDMA0_CHANNEL3_IRQ;
            DstPer = GDMA_HANDSHAKE_SSI2_TX;
            break;

        default:
            return;
    }

    pHalGdmaAdapter->GdmaCtl.TtFc      = TTFCMemToPeri;
    pHalGdmaAdapter->GdmaCtl.Done      = 1;
    pHalGdmaAdapter->GdmaCfg.ReloadDst = 1;
    pHalGdmaAdapter->MuliBlockCunt     = 0;
    pHalGdmaAdapter->MaxMuliBlock      = 1;
    pHalGdmaAdapter->GdmaCfg.DestPer   = DstPer;
    pHalGdmaAdapter->ChDar = (u32)pDst;
    pHalGdmaAdapter->GdmaIndex   = DmaIdx;
    pHalGdmaAdapter->ChNum       = DmaCh;
    pHalGdmaAdapter->ChEn        = DmaChEn;
    pHalGdmaAdapter->GdmaIsrType = (TransferType|ErrType);
    pHalGdmaAdapter->IsrCtrl     = ENABLE;
    pHalGdmaAdapter->GdmaOnOff   = ON;

    pHalGdmaAdapter->GdmaCtl.IntEn      = 1;
    pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeOne;
    pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeFour;
    pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
    pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthOneByte;
    pHalGdmaAdapter->GdmaCtl.Dinc = NoChange;
    pHalGdmaAdapter->GdmaCtl.Sinc = IncType;

    pDmaConfig->TxGdmaIrqHandle.Data = (u32)pHalSsiAdapter;
    pDmaConfig->TxGdmaIrqHandle.IrqNum = IrqNum;
    pDmaConfig->TxGdmaIrqHandle.IrqFun = (IRQ_FUN)SsiTxGdmaIrqHandle;
    pDmaConfig->TxGdmaIrqHandle.Priority = 10;
}


VOID
HalSsiRxGdmaLoadDefRtl8195a(
    IN VOID *Adapter
)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    PSSI_DMA_CONFIG pDmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    u8  *pSrc;
    u8 DmaIdx;
    u8 DmaCh;
    u8 SrcPer;
    u8 ssi_idx;
    u32 DmaChEn;
    IRQn_Type IrqNum;

    if ((NULL == pHalSsiAdapter)) {
        return;
    }

    pDmaConfig = &pHalSsiAdapter->DmaConfig;

    ssi_idx = pHalSsiAdapter->Index;

    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pRxHalGdmaAdapter;
    if (NULL == pHalGdmaAdapter) {
        return;
    }

    _memset((void *)pHalGdmaAdapter, 0, sizeof(HAL_GDMA_ADAPTER));

    pHalSsiAdapter->DmaControl |= SSI_RXDMA_ENABLE;
    if ((pHalSsiAdapter->DataFrameSize+1) > 8) {
        // 16~9 bits mode
        pHalSsiAdapter->DmaRxDataLevel = 7;  // RX FIFO stored bytes > (DMARDLR(7) + 1) then request DMA transfer
    }
    else {
        // 8~4 bits mode
        pHalSsiAdapter->DmaRxDataLevel = 3;  // RX FIFO stored bytes > (DMARDLR(3) + 1) then request DMA transfer
    }
    switch (ssi_idx) {
        case 0:
            pSrc = (u8*) (SSI0_REG_BASE + REG_DW_SSI_DR);
            DmaIdx = 0;
            DmaCh = 2;
            DmaChEn = GdmaCh2;
            SrcPer = GDMA_HANDSHAKE_SSI0_RX;
            IrqNum = GDMA0_CHANNEL2_IRQ;
            break;

        case 1:
            pSrc = (u8*) (SSI1_REG_BASE + REG_DW_SSI_DR);
            DmaIdx = 1;
            DmaCh = 2;
            DmaChEn = GdmaCh2;
            SrcPer = GDMA_HANDSHAKE_SSI1_RX;
            IrqNum = GDMA1_CHANNEL2_IRQ;
            break;

        case 2:
            pSrc = (u8*) (SSI2_REG_BASE + REG_DW_SSI_DR);
            DmaIdx = 1; // SSI2 RX only can use GDMA1
            DmaCh = 3;
            DmaChEn = GdmaCh3;
            SrcPer = GDMA_HANDSHAKE_SSI2_RX;
            IrqNum = GDMA1_CHANNEL3_IRQ;
            break;

        default:
            return;
    }

    pHalGdmaAdapter->GdmaCtl.TtFc      = TTFCPeriToMem;
    pHalGdmaAdapter->GdmaCtl.Done      = 1;
    pHalGdmaAdapter->GdmaCfg.ReloadSrc = 1;
    pHalGdmaAdapter->GdmaCfg.SrcPer    = SrcPer;
    pHalGdmaAdapter->MuliBlockCunt     = 0;
    pHalGdmaAdapter->MaxMuliBlock      = 1;
    pHalGdmaAdapter->ChSar = (u32)pSrc;
    pHalGdmaAdapter->GdmaIndex   = DmaIdx;
    pHalGdmaAdapter->ChNum       = DmaCh;
    pHalGdmaAdapter->ChEn        = DmaChEn;
    pHalGdmaAdapter->GdmaIsrType = (TransferType|ErrType);
    pHalGdmaAdapter->IsrCtrl     = ENABLE;
    pHalGdmaAdapter->GdmaOnOff   = ON;

    pHalGdmaAdapter->GdmaCtl.IntEn      = 1;
    pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeEight;
    pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeFour;
    pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthTwoBytes;
    pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes;
    pHalGdmaAdapter->GdmaCtl.Dinc = IncType;
    pHalGdmaAdapter->GdmaCtl.Sinc = NoChange;

    pDmaConfig->RxGdmaIrqHandle.Data = (u32)pHalSsiAdapter;
    pDmaConfig->RxGdmaIrqHandle.IrqNum = IrqNum;
    pDmaConfig->RxGdmaIrqHandle.IrqFun = (IRQ_FUN)SsiRxGdmaIrqHandle;
    pDmaConfig->RxGdmaIrqHandle.Priority = 11;
}

VOID
HalSsiDmaInitRtl8195a(
    IN VOID *Adapter
)
{
    u32 RegValue;
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    PSSI_DMA_CONFIG pDmaConfig;
    PHAL_GDMA_ADAPTER pTxHalGdmaAdapter;
    PHAL_GDMA_ADAPTER pRxHalGdmaAdapter;
    u8 ssi_idx;
    u32 hdk_tx_bit;
    u32 hdk_rx_bit;
    u32 DmatdlrValue = 0;
    u32 DmardlrValue = 0;

    pDmaConfig = &pHalSsiAdapter->DmaConfig;
    pTxHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pTxHalGdmaAdapter;
    pRxHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pRxHalGdmaAdapter;
    ssi_idx = pHalSsiAdapter->Index;

    // Set REG_PESOC_SOC_CTRL[28:16] to configure the GDMA handshake connection
    // SSI2 handshake connection is hardware fixed
    if (ssi_idx != 2) {
        hdk_tx_bit = 16+pTxHalGdmaAdapter->GdmaCfg.DestPer;
        hdk_rx_bit = 16+pRxHalGdmaAdapter->GdmaCfg.SrcPer;
    }
    else {
        hdk_tx_bit = 0;
        hdk_rx_bit = 0;
    }

    HalSsiDisableRtl8195a(pHalSsiAdapter);
    
    RegValue = HAL_READ32(PERI_ON_BASE, REG_PESOC_SOC_CTRL);
    if (pHalSsiAdapter->DmaControl & SSI_TXDMA_ENABLE) {
        // TX DMA is enabled
        if (pTxHalGdmaAdapter->GdmaIndex ==0) {
            ACTCK_GDMA0_CCTRL(ON);
            GDMA0_FCTRL(ON);
            if (hdk_tx_bit != 0) {
                RegValue &= ~(1<<hdk_tx_bit);
            }
        }
        else {
            ACTCK_GDMA1_CCTRL(ON);
            GDMA1_FCTRL(ON);
            if (hdk_tx_bit != 0) {
                RegValue |= (1<<hdk_tx_bit);
            }
        }
        /* Set TX FIFO water level to trigger Tx DMA transfer */
        DmatdlrValue = BIT_DMATDLR_DMATDL(pHalSsiAdapter->DmaTxDataLevel);
        HAL_SSI_WRITE32(ssi_idx, REG_DW_SSI_DMATDLR, DmatdlrValue);

        /* Set SSI DMA Enable */
        HAL_SSI_WRITE32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR, \
            (HAL_SSI_READ32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR) | SSI_TXDMA_ENABLE));
    }

    if (pHalSsiAdapter->DmaControl & SSI_RXDMA_ENABLE) {
        // RX DMA is enabled
        if (pRxHalGdmaAdapter->GdmaIndex ==0) {
            ACTCK_GDMA0_CCTRL(ON);
            GDMA0_FCTRL(ON);
            if (hdk_rx_bit != 0) {
                RegValue &= ~(1<<hdk_rx_bit);
            }
        }
        else {
            ACTCK_GDMA1_CCTRL(ON);
            GDMA1_FCTRL(ON);
            if (hdk_rx_bit != 0) {
                RegValue |= (1<<hdk_rx_bit);
            }
        }
        /* Set RX FIFO water level to trigger Rx DMA transfer */
        DmardlrValue = BIT_DMARDLR_DMARDL(pHalSsiAdapter->DmaRxDataLevel);
        HAL_SSI_WRITE32(ssi_idx, REG_DW_SSI_DMARDLR, DmardlrValue);        
        // the RX DMA will be enabled at read start.
    }

    HAL_WRITE32(PERI_ON_BASE, REG_PESOC_SOC_CTRL, RegValue);

    HalSsiEnableRtl8195a(pHalSsiAdapter);
}

HAL_Status
HalSsiDmaSendRtl8195a(
    IN VOID *Adapter,      // PHAL_SSI_ADAPTOR
    IN u8 *pTxData,     // the Buffer to be send
    IN u32 Length      // the length of data to be send
)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    PSSI_DMA_CONFIG pDmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;


    if ((pTxData == NULL) || (Length == 0)) {
        DBG_SSI_ERR("HalSsiDmaSendRtl8195a: Err: pTxData=0x%x,  Length=%d\n", pTxData, Length);
        return HAL_ERR_PARA;
    }
    pDmaConfig = &pHalSsiAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pTxHalGdmaAdapter;

    pHalSsiAdapter->TxLength = Length;
    pHalSsiAdapter->TxData = (void*)pTxData;

    // Cofigure GDMA transfer
    if ((pHalSsiAdapter->DataFrameSize+1) > 8) {
        // 16~9 bits mode
        if (((Length & 0x03)==0) &&
            (((u32)(pTxData) & 0x03)==0)) {
            // 4-bytes aligned, move 4 bytes each transfer
            pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeFour;
            //pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
            pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthTwoBytes;
            //pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeEight;
            pHalGdmaAdapter->GdmaCtl.DestMsize   = MsizeFour;
            pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthTwoBytes;
            //pHalGdmaAdapter->GdmaCtl.BlockSize = Length >> 2;
            pHalGdmaAdapter->GdmaCtl.BlockSize = Length >> 1;
        }
        else if (((Length & 0x01)==0) &&
            (((u32)(pTxData) & 0x01)==0)) {
            // 2-bytes aligned, move 2 bytes each transfer
            //pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeEight;
            pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeFour;
            pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthTwoBytes;
            //pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeEight;
            pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeFour;
            pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthTwoBytes;
            pHalGdmaAdapter->GdmaCtl.BlockSize = Length >> 1;
        }
        else {
            DBG_SSI_ERR("HalSsiDmaSendRtl8195a: Aligment Err: pTxData=0x%x,  Length=%d\n", pTxData, Length);
            return HAL_ERR_PARA;
        }
    }
    else {
        // 8~4 bits mode
        if (((Length & 0x03)==0) &&
            (((u32)(pTxData) & 0x03)==0)) {
            // 4-bytes aligned, move 4 bytes each transfer
            pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeOne;
            pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
            pHalGdmaAdapter->GdmaCtl.BlockSize = Length >> 2;
        }
        else {
            pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeFour;
            pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthOneByte;
            pHalGdmaAdapter->GdmaCtl.BlockSize = Length;
        }
        pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthOneByte;
    }

    DBG_SSI_INFO("TX SrcMsize=%d SrcTrWidth=%d DestMsize=%d DstTrWidth=%d BlockSize=%d\n", \
        pHalGdmaAdapter->GdmaCtl.SrcMsize, pHalGdmaAdapter->GdmaCtl.SrcTrWidth, \
        pHalGdmaAdapter->GdmaCtl.DestMsize, pHalGdmaAdapter->GdmaCtl.DstTrWidth, \
        pHalGdmaAdapter->GdmaCtl.BlockSize);

#if 0
    /* Set SSI DMA Enable */
    // TODO: protect the enable DMA register, it may collision with the DMA disable in the GDMA done ISR
    HAL_SSI_WRITE32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR, \
        (HAL_SSI_READ32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR) | SSI_TXDMA_ENABLE));
#endif

    return HAL_OK;
}


HAL_Status
HalSsiDmaSendMultiBlockRtl8195a(VOID *Adapter, u8 *pTxData, u32 Length)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    PSSI_DMA_CONFIG pDmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;  
    PSSI_DMA_MULTIBLK pSsiDmaMultiBlk;  
    
    u32 BlockNumber = 1;
    u32 SingleBlockBytes;
    u32 BlockIndex = 0;


    pDmaConfig = &pHalSsiAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pTxHalGdmaAdapter;
    pSsiDmaMultiBlk = (PSSI_DMA_MULTIBLK) &pHalSsiAdapter->DmaTxMultiBlk;
    SingleBlockBytes = MAX_DMA_BLOCK_SIZE * (1 << (pHalGdmaAdapter->GdmaCtl.SrcTrWidth)) ;
    
    BlockNumber = Length / MAX_DMA_BLOCK_SIZE;
    if(Length % MAX_DMA_BLOCK_SIZE)
        BlockNumber++;
    
    if(BlockNumber > 16){
        DBG_SSI_ERR("HalSsiDmaSendMultiBlockRtl8195a: Data length is too long\n");
        return HAL_ERR_PARA;
    }
    DBG_SSI_INFO("TX BlockNumber = %x, Length = %x\n", BlockNumber, Length);

    
    pHalGdmaAdapter->MaxMuliBlock = BlockNumber;
    pHalGdmaAdapter->Rsvd4to7 = 1;
    pHalGdmaAdapter->Llpctrl = 1;
    pHalGdmaAdapter->GdmaCtl.LlpSrcEn = 1;

    for(BlockIndex = 0; BlockIndex < BlockNumber; BlockIndex++){
        pSsiDmaMultiBlk->GdmaChLli[BlockIndex].Sarx = (u32) (pTxData + SingleBlockBytes * BlockIndex);
        pSsiDmaMultiBlk->GdmaChLli[BlockIndex].Darx = (u32) (pHalGdmaAdapter->ChDar);
        
        //DBG_SSI_INFO("GdmaChLli[%x].Sarx = %x\n", BlockIndex, pSsiDmaMultiBlk->GdmaChLli[BlockIndex].Sarx);        
        //DBG_SSI_INFO("GdmaChLli[%x] = %x\n", BlockIndex, &(pSsiDmaMultiBlk->GdmaChLli[BlockIndex]));        
        pSsiDmaMultiBlk->Lli[BlockIndex].pLliEle = (GDMA_CH_LLI_ELE*) &(pSsiDmaMultiBlk->GdmaChLli[BlockIndex]);
        if(BlockIndex == BlockNumber - 1){
            pSsiDmaMultiBlk->Lli[BlockIndex].pNextLli = NULL;
            pSsiDmaMultiBlk->BlockSizeList[BlockIndex].pNextBlockSiz = NULL;
            pSsiDmaMultiBlk->BlockSizeList[BlockIndex].BlockSize = Length - BlockIndex*MAX_DMA_BLOCK_SIZE;
        }
        else{
            pSsiDmaMultiBlk->Lli[BlockIndex].pNextLli = &(pSsiDmaMultiBlk->Lli[BlockIndex + 1]); 
            pSsiDmaMultiBlk->BlockSizeList[BlockIndex].BlockSize = MAX_DMA_BLOCK_SIZE;
            pSsiDmaMultiBlk->BlockSizeList[BlockIndex].pNextBlockSiz = &(pSsiDmaMultiBlk->BlockSizeList[BlockIndex + 1]);

        }
        //DBG_SSI_INFO("Lli[%x] = %x\n", BlockIndex, &(pSsiDmaMultiBlk->Lli[BlockIndex]));   
        //DBG_SSI_INFO("pSsiDmaMultiBlk->BlockSizeList[%x]= %x\n", BlockIndex, pSsiDmaMultiBlk->BlockSizeList[BlockIndex].BlockSize);        
    }

    pHalGdmaAdapter->pBlockSizeList = (struct BLOCK_SIZE_LIST*) &(pSsiDmaMultiBlk->BlockSizeList);
    pHalGdmaAdapter->pLlix = (struct GDMA_CH_LLI*) &(pSsiDmaMultiBlk->Lli);

    return HAL_OK;
}

HAL_Status
HalSsiDmaRecvRtl8195a(
    IN VOID *Adapter,      // PHAL_SSI_ADAPTOR
    IN u8  *pRxData,  ///< Rx buffer
    IN u32 Length      // buffer length
)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    PSSI_DMA_CONFIG pDmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;

    if ((pRxData == NULL) || (Length == 0)) {
        DBG_SSI_ERR("HalSsiDmaRecvRtl8195a: Null Err: pRxData=0x%x, Length=%d\n",  pRxData, Length);
        return HAL_ERR_PARA;
    }
    
    pDmaConfig = &pHalSsiAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pRxHalGdmaAdapter;

    pHalSsiAdapter->RxLength = Length;
    pHalSsiAdapter->RxData = (void*)pRxData;

    // Cofigure GDMA transfer
    if ((pHalSsiAdapter->DataFrameSize+1) > 8) {
        // 16~9 bits mode
        pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeEight;
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthTwoBytes;
        pHalGdmaAdapter->GdmaCtl.BlockSize = Length >> 1;

        if (((Length & 0x03)==0) &&
            (((u32)(pRxData) & 0x03)==0)) {
            // 4-bytes aligned, move 4 bytes each transfer
            pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeFour;
            pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes;
        }
        else if (((Length & 0x01)==0) &&
            (((u32)(pRxData) & 0x01)==0)) {
            // 2-bytes aligned, move 2 bytes each transfer
            pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeEight;
            pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthTwoBytes;
        }
        else {
            DBG_SSI_ERR("HalSsiDmaRecvRtl8195a: Aligment Err: pTxData=0x%x,  Length=%d\n", pRxData, Length);
            return HAL_ERR_PARA;
        }
    }
    else {
        // 8~4 bits mode
        pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthOneByte;
        pHalGdmaAdapter->GdmaCtl.BlockSize = Length;
        if (((Length & 0x03)==0) &&
            (((u32)(pRxData) & 0x03)==0)) {
            // 4-bytes aligned, move 4 bytes each transfer
            pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeOne;
            pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes;
        }
        else {
            pHalGdmaAdapter->GdmaCtl.DestMsize  = MsizeFour;
            pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthOneByte;
        }
    }
    
    DBG_SSI_INFO("RX SrcMsize=%d SrcTrWidth=%d DestMsize=%d DstTrWidth=%d BlockSize=%d\n", \
        pHalGdmaAdapter->GdmaCtl.SrcMsize, pHalGdmaAdapter->GdmaCtl.SrcTrWidth, \
        pHalGdmaAdapter->GdmaCtl.DestMsize, pHalGdmaAdapter->GdmaCtl.DstTrWidth, \
        pHalGdmaAdapter->GdmaCtl.BlockSize);

    /* Set SSI DMA Enable */
    // TODO: protect the enable DMA register, it may collision with the DMA disable in the GDMA TX done ISR
    HAL_SSI_WRITE32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR, \
        (HAL_SSI_READ32(pHalSsiAdapter->Index, REG_DW_SSI_DMACR) | SSI_RXDMA_ENABLE));

    return HAL_OK;
}

HAL_Status
HalSsiDmaRecvMultiBlockRtl8195a(VOID *Adapter, u8 *pRxData, u32 Length)
{
    PHAL_SSI_ADAPTOR pHalSsiAdapter = (PHAL_SSI_ADAPTOR) Adapter;
    PSSI_DMA_CONFIG pDmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PSSI_DMA_MULTIBLK pSsiDmaMultiBlk;
    
    
    u32 BlockNumber = 1;
    u32 SingleBlockBytes;
    u32 BlockIndex = 0;


    pDmaConfig = &pHalSsiAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pDmaConfig->pRxHalGdmaAdapter;
    pSsiDmaMultiBlk = (PSSI_DMA_MULTIBLK) &pHalSsiAdapter->DmaRxMultiBlk;
    SingleBlockBytes = MAX_DMA_BLOCK_SIZE * (1 << (pHalGdmaAdapter->GdmaCtl.SrcTrWidth)) ;
    
    BlockNumber = Length / MAX_DMA_BLOCK_SIZE;
    if(Length % MAX_DMA_BLOCK_SIZE)
        BlockNumber++;
    
    if(BlockNumber > 16){
        DBG_SSI_ERR("HalSsiDmaRecvMultiBlockRtl8195a: Data length is too long\n");
        return HAL_ERR_PARA;
    }
    DBG_SSI_INFO("RX BlockNumber = %x, Length = %x\n", BlockNumber, Length);

    
    pHalGdmaAdapter->MaxMuliBlock = BlockNumber;
    pHalGdmaAdapter->Rsvd4to7 = 1;
    pHalGdmaAdapter->Llpctrl = 1;
    pHalGdmaAdapter->GdmaCtl.LlpDstEn = 1;

    for(BlockIndex = 0; BlockIndex < BlockNumber; BlockIndex++){
        pSsiDmaMultiBlk->GdmaChLli[BlockIndex].Sarx = (u32) (pHalGdmaAdapter->ChSar);
        pSsiDmaMultiBlk->GdmaChLli[BlockIndex].Darx = (u32) (pRxData + SingleBlockBytes * BlockIndex);
        
        //DBG_SSI_INFO("GdmaChLli[%x].Darx = %x\n", BlockIndex, pSsiDmaMultiBlk->GdmaChLli[BlockIndex].Darx);        
        //DBG_SSI_INFO("GdmaChLli[%x] = %x\n", BlockIndex, &(pSsiDmaMultiBlk->GdmaChLli[BlockIndex]));        
        pSsiDmaMultiBlk->Lli[BlockIndex].pLliEle = (GDMA_CH_LLI_ELE*) &(pSsiDmaMultiBlk->GdmaChLli[BlockIndex]);
        if(BlockIndex == BlockNumber - 1){
            pSsiDmaMultiBlk->Lli[BlockIndex].pNextLli = NULL;
            pSsiDmaMultiBlk->BlockSizeList[BlockIndex].pNextBlockSiz = NULL;
            pSsiDmaMultiBlk->BlockSizeList[BlockIndex].BlockSize = Length - BlockIndex*MAX_DMA_BLOCK_SIZE;
        }
        else{
            pSsiDmaMultiBlk->Lli[BlockIndex].pNextLli = &(pSsiDmaMultiBlk->Lli[BlockIndex + 1]); 
            pSsiDmaMultiBlk->BlockSizeList[BlockIndex].BlockSize = MAX_DMA_BLOCK_SIZE;
            pSsiDmaMultiBlk->BlockSizeList[BlockIndex].pNextBlockSiz = &(pSsiDmaMultiBlk->BlockSizeList[BlockIndex + 1]);

        }
        //DBG_SSI_INFO("Lli[%x] = %x\n", BlockIndex, &(pSsiDmaMultiBlk->Lli[BlockIndex]));           
        //DBG_SSI_INFO("pSsiDmaMultiBlk->BlockSizeList[%x]= %x\n", BlockIndex, pSsiDmaMultiBlk->BlockSizeList[BlockIndex].BlockSize);        
    }

    pHalGdmaAdapter->pBlockSizeList = (struct BLOCK_SIZE_LIST*) &(pSsiDmaMultiBlk->BlockSizeList);
    pHalGdmaAdapter->pLlix = (struct GDMA_CH_LLI*) &(pSsiDmaMultiBlk->Lli);

    return HAL_OK;
}

#endif // end of "#ifdef CONFIG_GDMA_EN"

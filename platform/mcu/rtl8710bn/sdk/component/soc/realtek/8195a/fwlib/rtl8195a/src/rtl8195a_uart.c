/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"
#include "rtl8195a_uart.h"
#include "hal_uart.h"
#include "hal_gdma.h"

u8
HalRuartGetChipVerRtl8195a(VOID)
{
    u8 chip_ver;
    
    chip_ver = (HAL_READ32(SYSTEM_CTRL_BASE, 0x01F0) >> 4) & 0x0f;
    return chip_ver;
}

/**
 * Reset RUART Tx FIFO.
 *
 * Reset RUART Receiver and Rx FIFO wrapper function.
 * It will check LINE_STATUS_REG until reset action completion.
 *
 * @return BOOL
 */
HAL_Status
HalRuartResetTxFifoRtl8195a(
    IN VOID *Data  ///< RUART Adapter
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex = pHalRuartAdapter->UartIndex;
    u32 rx_trigger_lv;
    volatile u32 RegValue;
    u32 timeout;

    // Backup the RX FIFO trigger Level setting
    rx_trigger_lv = HAL_RUART_READ32(UartIndex, RUART_FIFO_CTL_REG_OFF);
    rx_trigger_lv &= 0xC0;   // only keep the bit[7:6]

    /* Step 2: Enable clear_txfifo */
    RegValue = (FIFO_CTL_DEFAULT_WITH_FIFO_DMA | RUART_FIFO_CTL_REG_CLEAR_TXFIFO) & (~0xC0);
    RegValue |= rx_trigger_lv;
    HAL_RUART_WRITE32(UartIndex, RUART_FIFO_CTL_REG_OFF, RegValue);

    // Wait TSR empty
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF);
    timeout = 100;  // wait 10 ms
    while (((RegValue & RUART_LINE_STATUS_REG_TEMT)==0) && (timeout > 0)) {
        HalDelayUs(100);
        RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF);
        timeout--;
    }

    return HAL_OK;
}

// Reset RX FIFO
HAL_Status
HalRuartResetRxFifoRtl8195a_Patch(
    IN VOID *Data  ///< RUART Adapter
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex;
    volatile u32 RegValue;
    u32 rx_trigger_lv;

    UartIndex = pHalRuartAdapter->UartIndex;

    /* Step 1: Enable Reset_rcv */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue |= RUART_STS_REG_RESET_RCV;
    HAL_RUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);

    // Backup the RX FIFO trigger Level setting
    rx_trigger_lv = HAL_RUART_READ32(UartIndex, RUART_FIFO_CTL_REG_OFF);
    rx_trigger_lv &= 0xC0;   // only keep the bit[7:6]

    /* Step 2: Enable clear_rxfifo */
    RegValue = (FIFO_CTL_DEFAULT_WITH_FIFO_DMA | RUART_FIFO_CTL_REG_CLEAR_RXFIFO) & (~0xC0);
    RegValue |= rx_trigger_lv;
    HAL_RUART_WRITE32(UartIndex, RUART_FIFO_CTL_REG_OFF, RegValue);

    /* Step 3: Disable Reset_rcv */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue &= ~(RUART_STS_REG_RESET_RCV);
    HAL_RUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);
    
    return HAL_OK;
}

// Reset both TX and RX FIFO
HAL_Status
HalRuartResetTRxFifoRtl8195a(
    IN VOID *Data  ///< RUART Adapter
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex;
    volatile u32 RegValue;
    u32 timeout;
    u32 rx_trigger_lv;

    UartIndex = pHalRuartAdapter->UartIndex;

    /* Step 1: Enable Reset_rcv */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue |= RUART_STS_REG_RESET_RCV;
    HAL_RUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);

    // Backup the RX FIFO trigger Level setting
    rx_trigger_lv = HAL_RUART_READ32(UartIndex, RUART_FIFO_CTL_REG_OFF);
    rx_trigger_lv &= 0xC0;   // only keep the bit[7:6]

    /* Step 2: Enable clear_txfifo & clear_rxfifo */
    RegValue = (FIFO_CTL_DEFAULT_WITH_FIFO_DMA | RUART_FIFO_CTL_REG_CLEAR_TXFIFO | RUART_FIFO_CTL_REG_CLEAR_RXFIFO) & (~0xC0);
    RegValue |= rx_trigger_lv;
    HAL_RUART_WRITE32(UartIndex, RUART_FIFO_CTL_REG_OFF, RegValue);

    // Wait THR & TSR empty
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF);
    timeout = 100;  // wait 10 ms
    while (((RegValue & RUART_LINE_STATUS_REG_TEMT)==0) && (timeout > 0)) {
        HalDelayUs(100);
        RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF);
        timeout--;
    }

    /* Step 3: Disable Reset_rcv */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue &= ~(RUART_STS_REG_RESET_RCV);
    HAL_RUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);
    
    return HAL_OK;
}

HAL_Status 
HalRuartGenBaudRateRtl8195a(
    IN RUART_SPEED_SETTING *pBaudSetting
)
{
    u32 baud_rate;
    u32 min_divisor=0;
    u32 min_err=0xffffffff;
    u32 uart_ovsr;
    u32 uart_ovsr_mod;
    u32 min_uart_ovsr;  // ovsr with mini err
    u32 min_uart_ovsr_mod;
    u32 uart_clock;
    u32 divisor_temp;
    u32 max_jitter_temp;
    u32 err_temp;
    u32 uart_ovsr_target;
    u32 uart_ovsrs_actual;
    u32 ovsr_adj;
    u32 adj_bits;
    u32 div_res;
    u32 uart_ovsrs_actual_mod;

    baud_rate = pBaudSetting->BaudRate;
    if (baud_rate >= 1000000) {
        baud_rate /= 100;
        uart_clock = pBaudSetting->sclk;
    } else {
        baud_rate /= 2;
        uart_clock = pBaudSetting->sclk*50;   // UART clock is 1/2 CPU clock
    }

    div_res = pBaudSetting->divisor_resolution;
    while ((min_err > pBaudSetting->max_err) && (div_res > 0)) {
        uart_ovsr = pBaudSetting->Ovsr_max;
        while(uart_ovsr >= pBaudSetting->Ovsr_min) {        
            divisor_temp = ((uart_clock/baud_rate)/uart_ovsr);
            max_jitter_temp = 0;    
            if (divisor_temp > 0) {
                max_jitter_temp = 100000/uart_ovsr;
                if (max_jitter_temp >= pBaudSetting->jitter_lim) {
                    err_temp = 100;
                } else {
                    err_temp = (uart_clock/divisor_temp)/((uart_ovsr/100)*100);
                    if (err_temp > baud_rate) {
                        err_temp = (err_temp - baud_rate)*1000 / baud_rate;
                    } else {
                        err_temp = (baud_rate - err_temp)*1000 / baud_rate;
                    }

                    if (err_temp < min_err) {
                        min_err = err_temp;
                        min_divisor = divisor_temp;
                        min_uart_ovsr = uart_ovsr/100;
                        min_uart_ovsr_mod = uart_ovsr%100;
                    } else if (err_temp == min_err) {
                        uart_ovsr_mod = uart_ovsr%100;
                        // we perfer OVSR bigger and adj bits smaller
                        if (((uart_ovsr/100) >= min_uart_ovsr) && (uart_ovsr_mod < min_uart_ovsr_mod)) {
                            min_err = err_temp;
                            min_divisor = divisor_temp;
                            min_uart_ovsr = uart_ovsr/100;
                            min_uart_ovsr_mod = uart_ovsr_mod;
                        }
                    }
                }
            }
            uart_ovsr -= div_res;
        }
        div_res = div_res >> 1;
    }

    if (min_divisor == 0) {    
        min_divisor = 1;
    }
    uart_ovsr_target = (uart_clock/baud_rate)/min_divisor;

    ovsr_adj = 0;
    adj_bits = 0;
    uart_ovsrs_actual = uart_ovsr_target/100;
    uart_ovsrs_actual_mod = uart_ovsr_target%100;
    if (uart_ovsrs_actual_mod > 0) {
        adj_bits = (uart_ovsrs_actual_mod*pBaudSetting->Ovsr_adj_max_bits)/100;
        if ((uart_ovsrs_actual_mod - ((adj_bits*100)/pBaudSetting->Ovsr_adj_max_bits)) > 4) {
            adj_bits++;
        }
        
        if (adj_bits > (pBaudSetting->Ovsr_adj_max_bits-1)) {
            DBG_UART_WARN("HalRuartGenBaudRateRtl8195a: adj_bits=%d\r\n", adj_bits);
            adj_bits = pBaudSetting->Ovsr_adj_max_bits-1;
        }
    }
    ovsr_adj = pBaudSetting->Ovsr_adj_map[adj_bits];
//    DBG_8195A("baud_rate=%d uart_clock=%d uart_ovsr_target=%d min_divisor=%d adj_bits=%d\r\n", baud_rate, uart_clock, uart_ovsr_target, min_divisor, adj_bits);

    pBaudSetting->Ovsr = uart_ovsrs_actual;
    pBaudSetting->Div = min_divisor;
    pBaudSetting->Ovsr_adj = ovsr_adj;
    pBaudSetting->Ovsr_adj_bits = adj_bits;
    
    DBG_UART_INFO("HalRuartGenBaudRateRtl8195a: BaudRate=%d ovsr=%d divisor=%d ovsr_adj=0x%x\r\n", 
        pBaudSetting->BaudRate, uart_ovsrs_actual, min_divisor, ovsr_adj);

    return HAL_OK;
}

HAL_Status 
HalRuartDumpBaudRateTableRtl8195a(
    IN VOID *Data
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    RUART_SPEED_SETTING RuartSpeedSetting;
    u32 Divisor;
    u32 Ovsr;
    u32 Ovsr_adj;
    u32 i;
    u32 j;
    u32 adj;

    RuartSpeedSetting.max_err = 3;
    RuartSpeedSetting.Ovsr_min = UART_OVSR_POOL_MIN;
    RuartSpeedSetting.Ovsr_max = UART_OVSR_POOL_MAX;
    RuartSpeedSetting.divisor_resolution = DIVISOR_RESOLUTION;
    RuartSpeedSetting.jitter_lim = JITTER_LIMIT;
    RuartSpeedSetting.sclk = UART_SCLK;
    
    if (pHalRuartAdapter->pDefaultBaudRateTbl != NULL) {
        // for debugging
        DBG_8195A("==== 10 Bit ====\r\n");
        i = 0;
        RuartSpeedSetting.Ovsr_adj_map = pHalRuartAdapter->pDefOvsrAdjTbl_10;
        RuartSpeedSetting.Ovsr_adj_max_bits = 10;
        while (pHalRuartAdapter->pDefaultBaudRateTbl[i] < 0xffffffff) {
            RuartSpeedSetting.BaudRate = pHalRuartAdapter->pDefaultBaudRateTbl[i];
            if (HalRuartGenBaudRateRtl8195a(&RuartSpeedSetting) == HAL_OK) {
                Divisor = RuartSpeedSetting.Div;
                Ovsr = RuartSpeedSetting.Ovsr;
                Ovsr_adj = RuartSpeedSetting.Ovsr_adj;
                adj = 0;
                for (j=0;j<10;j++) {
                    if (Ovsr_adj & (1<<j)) {
                        adj++;
                    }
                }

                DBG_8195A("BAUD: %d\t\tOVSR: %d\t\tDIV: %d\t\tADJ: 0x%04x\t%d\r\n", pHalRuartAdapter->pDefaultBaudRateTbl[i], Ovsr, Divisor, Ovsr_adj, adj);
            }
            i++;
        }


        DBG_8195A("==== 9 Bit ====\r\n");
        i = 0;
        RuartSpeedSetting.Ovsr_adj_map = pHalRuartAdapter->pDefOvsrAdjTbl_9;
        RuartSpeedSetting.Ovsr_adj_max_bits = 9;
        while (pHalRuartAdapter->pDefaultBaudRateTbl[i] < 0xffffffff) {
            RuartSpeedSetting.BaudRate = pHalRuartAdapter->pDefaultBaudRateTbl[i];
            if (HalRuartGenBaudRateRtl8195a(&RuartSpeedSetting) == HAL_OK) {
                Divisor = RuartSpeedSetting.Div;
                Ovsr = RuartSpeedSetting.Ovsr;
                Ovsr_adj = RuartSpeedSetting.Ovsr_adj;
                adj = 0;
                for (j=0;j<10;j++) {
                    if (Ovsr_adj & (1<<j)) {
                        adj++;
                    }
                }

                DBG_8195A("BAUD: %d\t\tOVSR: %d\t\tDIV: %d\t\tADJ: 0x%04x\t%d\r\n", pHalRuartAdapter->pDefaultBaudRateTbl[i], Ovsr, Divisor, Ovsr_adj, adj);
            }
            i++;
        }

        DBG_8195A("==== 8 Bit ====\r\n");
        i = 0;
        RuartSpeedSetting.Ovsr_adj_map = pHalRuartAdapter->pDefOvsrAdjTbl_8;
        RuartSpeedSetting.Ovsr_adj_max_bits = 8;
        while (pHalRuartAdapter->pDefaultBaudRateTbl[i] < 0xffffffff) {
            RuartSpeedSetting.BaudRate = pHalRuartAdapter->pDefaultBaudRateTbl[i];
            if (HalRuartGenBaudRateRtl8195a(&RuartSpeedSetting) == HAL_OK) {
                Divisor = RuartSpeedSetting.Div;
                Ovsr = RuartSpeedSetting.Ovsr;
                Ovsr_adj = RuartSpeedSetting.Ovsr_adj;
                adj = 0;
                for (j=0;j<10;j++) {
                    if (Ovsr_adj & (1<<j)) {
                        adj++;
                    }
                }

                DBG_8195A("BAUD: %d\t\tOVSR: %d\t\tDIV: %d\t\tADJ: 0x%04x\t%d\r\n", pHalRuartAdapter->pDefaultBaudRateTbl[i], Ovsr, Divisor, Ovsr_adj, adj);
            }
            i++;
        }
    }
	return HAL_OK;
}

HAL_Status 
HalRuartSetBaudRateRtl8195a(
    IN VOID *Data
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    RUART_SPEED_SETTING RuartSpeedSetting;
    u32 RegValue;
    u32 Dll, Dlm;
    u8 UartIndex;
    u32 Divisor;
    u32 Ovsr;
    u32 Ovsr_adj;
    u32 i;
    u32 cpu_clk;
    u32 baud_rate_temp;
    u32 err;
    u8 is_defined_baud;
    u8 word_bits;
    u8 adj_bits;
    
#if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT)        
    u8 chip_ver;

    // get chip version
    chip_ver = HalRuartGetChipVerRtl8195a();
#endif

    if (pHalRuartAdapter->WordLen == RUART_WLS_8BITS) {
        word_bits = 8+1;    // 1 start bit + 8 data bit
    } else {
        word_bits = 7+1;
    }
    
    if (pHalRuartAdapter->Parity == RUART_PARITY_ENABLE) {
        word_bits++;        // 1 parity bit    
    }

    is_defined_baud = 0;
        
    if (pHalRuartAdapter->pDefaultBaudRateTbl != NULL) {
        i = 0;
        while (pHalRuartAdapter->pDefaultBaudRateTbl[i] < 0xffffffff) {
            if (pHalRuartAdapter->pDefaultBaudRateTbl[i] == pHalRuartAdapter->BaudRate) {
                Divisor = pHalRuartAdapter->pDefaultDivTbl[i];
                Ovsr = pHalRuartAdapter->pDefaultOvsrRTbl[i];
                switch (word_bits) {
                    case 9:
                        adj_bits = pHalRuartAdapter->pDefOvsrAdjBitTbl_9[i];                        
                        Ovsr_adj = pHalRuartAdapter->pDefOvsrAdjTbl_9[adj_bits];
                        break;
                    case 10:
                        adj_bits = pHalRuartAdapter->pDefOvsrAdjBitTbl_10[i];
                        Ovsr_adj = pHalRuartAdapter->pDefOvsrAdjTbl_10[adj_bits];
                        break;
                    case 8:
                        adj_bits = pHalRuartAdapter->pDefOvsrAdjBitTbl_8[i];
                        Ovsr_adj = pHalRuartAdapter->pDefOvsrAdjTbl_8[adj_bits];
                        break;

                    default:
                        adj_bits = pHalRuartAdapter->pDefOvsrAdjBitTbl_9[i];
                        Ovsr_adj = pHalRuartAdapter->pDefOvsrAdjTbl_9[adj_bits];
                        break;
                }
                // Verify again
                cpu_clk = UART_SCLK;
                baud_rate_temp = cpu_clk/Ovsr/Divisor;
                if (baud_rate_temp > pHalRuartAdapter->BaudRate) {
                    err = baud_rate_temp - pHalRuartAdapter->BaudRate;
                } else {
                    err = pHalRuartAdapter->BaudRate - baud_rate_temp;
                }

                // Tolerance is 10%
                // If the err is too big, it may caused by "the baud rate table is not for this CPU clock"
                if (err < (pHalRuartAdapter->BaudRate/10)) {
                    is_defined_baud = 1;
                }
                break;  // break the while loop
            } else {
                i++;
            }
        }
    }

    if (is_defined_baud == 0) {
        
        switch (word_bits) {
            case 9:
                RuartSpeedSetting.Ovsr_adj_map = pHalRuartAdapter->pDefOvsrAdjTbl_9;
                break;

            case 10:
                RuartSpeedSetting.Ovsr_adj_map = pHalRuartAdapter->pDefOvsrAdjTbl_10;
                break;

            case 8:
                RuartSpeedSetting.Ovsr_adj_map = pHalRuartAdapter->pDefOvsrAdjTbl_8;
                break;

            default:
                word_bits = 9;
                RuartSpeedSetting.Ovsr_adj_map = pHalRuartAdapter->pDefOvsrAdjTbl_9;
                break;
        }
        DBG_UART_INFO("BaudRate(%d) not in the Lookup table \n", pHalRuartAdapter->BaudRate);        
        RuartSpeedSetting.Ovsr_adj_max_bits = word_bits;
        RuartSpeedSetting.max_err = 3;
        RuartSpeedSetting.Ovsr_min = UART_OVSR_POOL_MIN;
        RuartSpeedSetting.Ovsr_max = UART_OVSR_POOL_MAX;
        RuartSpeedSetting.divisor_resolution = DIVISOR_RESOLUTION;
        RuartSpeedSetting.jitter_lim = JITTER_LIMIT;
        RuartSpeedSetting.sclk = UART_SCLK;
        RuartSpeedSetting.BaudRate = pHalRuartAdapter->BaudRate;
#if defined(CONFIG_CHIP_A_CUT) || defined(CONFIG_CHIP_B_CUT)        
        if (chip_ver < 2) {
            // A or B Cut
            // workround: +2% bias
            RuartSpeedSetting.BaudRate = (pHalRuartAdapter->BaudRate * 102)/100;
        }
#endif        
        if (HalRuartGenBaudRateRtl8195a(&RuartSpeedSetting) == HAL_OK) {
            Divisor = RuartSpeedSetting.Div;
            Ovsr = RuartSpeedSetting.Ovsr;
            Ovsr_adj = RuartSpeedSetting.Ovsr_adj;
        } else {
            DBG_UART_ERR("Invalid BaudRate(%d), Force Baud Rateit as 9600\n", 
                pHalRuartAdapter->BaudRate);
            Divisor = 434;
            Ovsr = 20;
            Ovsr_adj = 0;
        }
    }

    UartIndex = pHalRuartAdapter->UartIndex;

    DBG_UART_INFO("HalRuartSetBaudRateRtl8195a: BaudRate:%d Divisor:%d Ovsr:%d Ovsr_ADj:0x%x\n", 
        pHalRuartAdapter->BaudRate, Divisor, Ovsr, Ovsr_adj);

    Dll = Divisor & 0xFF;
    Dlm = (Divisor & 0xFF00) >> 8;

    /* Set DLAB bit to 1 to access DLL/DLM */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);
    RegValue |= RUART_LINE_CTL_REG_DLAB_ENABLE;
    HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);

    HAL_RUART_WRITE32(UartIndex, RUART_DLL_OFF, Dll);
    HAL_RUART_WRITE32(UartIndex, RUART_DLM_OFF, Dlm);

    /**
      * Clean Rx break signal interrupt status at initial stage.
      */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    RegValue |= RUART_SP_REG_RXBREAK_INT_STATUS;
    HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);

    /* Set OVSR(xfactor) */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue &= ~(RUART_STS_REG_XFACTOR);
    RegValue |= (((Ovsr - 5) << 4) & RUART_STS_REG_XFACTOR);
    HAL_RUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);

    /* Set OVSR_ADJ[10:0] (xfactor_adj[26:16]) */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    RegValue &= ~(RUART_SP_REG_XFACTOR_ADJ);
    RegValue |= ((Ovsr_adj << 16) & RUART_SP_REG_XFACTOR_ADJ);
    HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);

    /* clear DLAB bit */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);
    RegValue &= ~(RUART_LINE_CTL_REG_DLAB_ENABLE);
    HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);
    pHalRuartAdapter->BaudRateUsing = pHalRuartAdapter->BaudRate;
    pHalRuartAdapter->WordLenUsing = pHalRuartAdapter->WordLen;
    pHalRuartAdapter->ParityUsing = pHalRuartAdapter->Parity;

    return HAL_OK;
}


HAL_Status 
HalRuartInitRtl8195a_Patch(
    IN VOID *Data  ///< RUART Adapter
)
{
    /* DBG_ENTRANCE; */
    u32 RegValue;
    u8 UartIndex;
    u8 PinmuxSelect;

    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;
    PinmuxSelect = pHalRuartAdapter->PinmuxSelect;

    if (UartIndex > 2) {
        DBG_UART_ERR(ANSI_COLOR_MAGENTA"HalRuartInitRtl8195a: Invalid UART Index\n"ANSI_COLOR_RESET);
        return HAL_ERR_PARA;
    }

    DBG_UART_INFO("HalRuartInitRtl8195a: [UART %d] PinSel=%d\n", UartIndex, PinmuxSelect);
    if(( PinmuxSelect == RUART0_MUX_TO_GPIOE ) && ((UartIndex == 0) || (UartIndex == 1))) {
        DBG_UART_WARN(ANSI_COLOR_MAGENTA"UART Pin may conflict with JTAG\r\n"ANSI_COLOR_RESET);
    }

    // switch Pin from EEPROM to UART0
    if(( PinmuxSelect == RUART0_MUX_TO_GPIOC ) && (UartIndex == 0)) {
        RegValue = HAL_READ32(SYSTEM_CTRL_BASE, 0xa4);
        if (RegValue & 0x10) {
            DBG_UART_WARN("UART Pin may conflict with EEPROM\n");
//          HAL_WRITE32(SYSTEM_CTRL_BASE, 0xa4, (RegValue & (~0x10)));
        }
    }

    switch (UartIndex) {
        case 0:
            /* UART 0 */
            ACTCK_UART0_CCTRL(ON);
            SLPCK_UART0_CCTRL(ON);
            PinCtrl(UART0, PinmuxSelect, ON);
            UART0_FCTRL(ON);
            UART0_BD_FCTRL(ON);
            break;

        case 1:
            /* UART 1 */
            ACTCK_UART1_CCTRL(ON);
            SLPCK_UART1_CCTRL(ON);
            PinCtrl(UART1, PinmuxSelect, ON);
            UART1_FCTRL(ON);
            UART1_BD_FCTRL(ON);
            break;

        case 2:
            /* UART 1 */
            ACTCK_UART2_CCTRL(ON);
            SLPCK_UART2_CCTRL(ON);
            PinCtrl(UART2, PinmuxSelect, ON);
            UART2_FCTRL(ON);
            UART2_BD_FCTRL(ON);
            break;

        default:
            DBG_UART_ERR("Invalid UART Index(%d)\n", UartIndex);
            return HAL_ERR_PARA;
    }

    /* Reset RX FIFO */
    HalRuartResetRxFifoRtl8195a(Data);
    DBG_UART_INFO(ANSI_COLOR_CYAN"HAL UART Init[UART %d]\n"ANSI_COLOR_RESET, UartIndex);

    /* Disable all interrupts */
    HAL_RUART_WRITE32(UartIndex, RUART_INTERRUPT_EN_REG_OFF, 0x00);

    /* Set Baudrate Division */
    if ((pHalRuartAdapter->BaudRateUsing != pHalRuartAdapter->BaudRate) ||
        (pHalRuartAdapter->WordLenUsing != pHalRuartAdapter->WordLen) ||
        (pHalRuartAdapter->ParityUsing != pHalRuartAdapter->Parity)) {
        HalRuartSetBaudRateRtl8195a(pHalRuartAdapter);
    }

    /**
      * Clean Rx break signal interrupt status at initial stage.
      */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    RegValue |= RUART_SP_REG_RXBREAK_INT_STATUS;
    HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);

//    DBG_UART_INFO("[R] UART%d INT_EN(0x04) = %x\n", UartIndex, pHalRuartAdapter->Interrupts);
    RegValue = ((pHalRuartAdapter->Interrupts) & 0xFF);
    HAL_RUART_WRITE32(UartIndex, RUART_INTERRUPT_EN_REG_OFF, RegValue);
//    DBG_UART_INFO("[W] UART%d INT_EN(0x04) = %x\n", UartIndex, RegValue);

    /* Configure FlowControl */
    if (pHalRuartAdapter->FlowControl == AUTOFLOW_ENABLE) {
        RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_CTL_REG_OFF);
        RegValue |= RUART_MCL_AUTOFLOW_ENABLE;
        HAL_RUART_WRITE32(UartIndex, RUART_MODEM_CTL_REG_OFF, RegValue);
    }

    /* RUART DMA Initialization */
//    HalRuartDmaInitRtl8195a(pHalRuartAdapter);

    DBG_UART_INFO("[R] UART%d LCR(0x%02X): %X\n", UartIndex, RUART_LINE_CTL_REG_OFF, HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF));
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);

    /* PARITY CONTROL */
    RegValue &= BIT_CLR_LCR_WLS;
    RegValue |= BIT_LCR_WLS(pHalRuartAdapter->WordLen);

    RegValue &= BIT_INVC_LCR_STB_EN;
    RegValue |= BIT_LCR_STB_EN(pHalRuartAdapter->StopBit);

    RegValue &= BIT_INVC_LCR_PARITY_EN;
    RegValue |= BIT_LCR_PARITY_EN(pHalRuartAdapter->Parity);

    /* PARITY TYPE SELECT */
    RegValue &= BIT_INVC_LCR_PARITY_TYPE;
    RegValue |= BIT_LCR_PARITY_TYPE(pHalRuartAdapter->ParityType);

    /* STICK PARITY CONTROL */
    RegValue &= BIT_INVC_LCR_STICK_PARITY_EN;
    RegValue |= BIT_LCR_STICK_PARITY_EN(pHalRuartAdapter->StickParity);

    HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);
    DBG_UART_INFO("[W] UART%d LCR(0x%02X): %X\n", UartIndex, RUART_LINE_CTL_REG_OFF, HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF));

    /* Need to assert RTS during initial stage. */
    if (pHalRuartAdapter->FlowControl == AUTOFLOW_ENABLE) {
        HalRuartRTSCtrlRtl8195a(Data, 1);
    }
    pHalRuartAdapter->State = HAL_UART_STATE_READY;

	return HAL_OK;
}

HAL_Status 
HalRuartEnableRtl8195a(
    IN VOID *Data
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex;

    // Enable IP Clock
    UartIndex = pHalRuartAdapter->UartIndex;
    switch (UartIndex) {
        case 0:
            /* UART 0 */
            ACTCK_UART0_CCTRL(ON);
            SLPCK_UART0_CCTRL(ON);
            break;

        case 1:
            /* UART 1 */
            ACTCK_UART1_CCTRL(ON);
            SLPCK_UART1_CCTRL(ON);
            break;

        case 2:
            /* UART 1 */
            ACTCK_UART2_CCTRL(ON);
            SLPCK_UART2_CCTRL(ON);
            break;

        default:
            DBG_UART_ERR("Invalid UART Index(%d)\n", UartIndex);
            return HAL_ERR_PARA;
    }

    return HAL_OK;
}

HAL_Status 
HalRuartDisableRtl8195a(
    IN VOID *Data
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex;

    // Gate IP Clock
    UartIndex = pHalRuartAdapter->UartIndex;
    switch (UartIndex) {
        case 0:
            /* UART 0 */
            ACTCK_UART0_CCTRL(OFF);
            SLPCK_UART0_CCTRL(OFF);
            break;

        case 1:
            /* UART 1 */
            ACTCK_UART1_CCTRL(OFF);
            SLPCK_UART1_CCTRL(OFF);
            break;

        case 2:
            /* UART 1 */
            ACTCK_UART2_CCTRL(OFF);
            SLPCK_UART2_CCTRL(OFF);
            break;

        default:
            DBG_UART_ERR("Invalid UART Index(%d)\n", UartIndex);
            return HAL_ERR_PARA;
    }

    return HAL_OK;
}

HAL_Status 
HalRuartFlowCtrlRtl8195a(
    IN VOID *Data
)
{
    u32 UartIndex;
    u32 RegValue;
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;

    RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_CTL_REG_OFF);
    if (!pHalRuartAdapter->FlowControl) {
        // No Auto Flow Control: no flow control or flow controled by software
        RegValue &= ~(RUART_MCL_AUTOFLOW_ENABLE);
    }
    else {
        RegValue |= RUART_MCL_AUTOFLOW_ENABLE;
    }
    HAL_RUART_WRITE32(UartIndex, RUART_MODEM_CTL_REG_OFF, RegValue);

    return HAL_OK;
}

u32
_UartTxDmaIrqHandle_Patch(
        IN VOID *Data
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    PUART_DMA_CONFIG pUartGdmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP pHalGdmaOp;
    u8  IsrTypeMap;

    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pTxHalGdmaAdapter;
    pHalGdmaOp = (PHAL_GDMA_OP)pUartGdmaConfig->pHalGdmaOp;

    // Clear Pending ISR
    IsrTypeMap = pHalGdmaOp->HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
    
    if (IsrTypeMap & BlockType) {   
        pHalGdmaAdapter->MuliBlockCunt++;       
    }

    if (pHalGdmaAdapter->MuliBlockCunt == pHalGdmaAdapter->MaxMuliBlock) {   
        // Clean Auto Reload Bit
        pHalGdmaOp->HalGdmaChCleanAutoDst((VOID*)pHalGdmaAdapter);
        pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));

        if ((HAL_UART_STATE_BUSY_TX == pHalRuartAdapter->State) ||
            (HAL_UART_STATE_BUSY_TX_RX == pHalRuartAdapter->State)) {
            if (pHalRuartAdapter->State == HAL_UART_STATE_BUSY_TX) {
                pHalRuartAdapter->State = HAL_UART_STATE_READY;
            }
            else {
                pHalRuartAdapter->State = HAL_UART_STATE_BUSY_RX;
            }

            // Call user TX complete callback
            if (NULL != pHalRuartAdapter->TxCompCallback) {
                pHalRuartAdapter->TxCompCallback(pHalRuartAdapter->TxCompCbPara);
            }
        }
    }
    return 0;
}

u32
_UartRxDmaIrqHandle_Patch(
        IN VOID *Data
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    PUART_DMA_CONFIG pUartGdmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP pHalGdmaOp;
    u8  IsrTypeMap;
    u8 LineStatus;
    u8 UartIndex;

    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pRxHalGdmaAdapter;
    pHalGdmaOp = (PHAL_GDMA_OP)pUartGdmaConfig->pHalGdmaOp;

    // Clear Pending ISR
    IsrTypeMap = pHalGdmaOp->HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);

    if (IsrTypeMap & BlockType) {   
        pHalGdmaAdapter->MuliBlockCunt++;
    }

    if ((pHalGdmaAdapter->MuliBlockCunt == pHalGdmaAdapter->MaxMuliBlock)) {   
        // Clean Auto Reload Bit
        pHalGdmaOp->HalGdmaChCleanAutoSrc((VOID*)pHalGdmaAdapter);        
        pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));

        // Check the Line Status
        UartIndex = pHalRuartAdapter->UartIndex;
        LineStatus = (u8)HAL_RUART_READ32(UartIndex, RUART_LINE_STATUS_REG_OFF);
        pHalRuartAdapter->Status |= LineStatus & RUART_LINE_STATUS_ERR;

        if (pHalRuartAdapter->State == HAL_UART_STATE_BUSY_RX) {
            pHalRuartAdapter->State = HAL_UART_STATE_READY;
        }
        else {
            pHalRuartAdapter->State = HAL_UART_STATE_BUSY_TX;
        }

        // Call User Rx complete callback
        if (pHalRuartAdapter->RxCompCallback != NULL) {
            pHalRuartAdapter->RxCompCallback (pHalRuartAdapter->RxCompCbPara);
        }
    }
    return 0;
}


/**
 * RUART send a data buffer by DMA(non-block) mode.
 *
 * RUART send data.
 *
 * @return VOID
 */
HAL_Status
HalRuartDmaSendRtl8195a_Patch(
    IN VOID *Data,      // PHAL_RUART_ADAPTER
    IN u8 *pTxData,     // the Buffer to be send
    IN u32 Length      // the length of data to be send
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)Data;
//    u8 UartIndex = pHalRuartAdapter->UartIndex;
    PUART_DMA_CONFIG pUartGdmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP pHalGdmaOp;
    HAL_UART_State State;

    State = pHalRuartAdapter->State;
    if ((State != HAL_UART_STATE_READY) && (State != HAL_UART_STATE_BUSY_RX)) {
        DBG_UART_WARN("HalRuartDmaSendRtl8195a: on Busy, State=%d\n", State);
        return HAL_BUSY;
    }

    if ((pTxData == NULL) || (Length == 0)) {
        pHalRuartAdapter->Status = HAL_UART_STATUS_ERR_PARA;
        DBG_UART_ERR("HalRuartDmaSendRtl8195a: Err: pTxData=0x%x,  Length=%d\n", pTxData, Length);
        return HAL_ERR_PARA;
    }

    if (State == HAL_UART_STATE_READY) {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_TX;
    }
    else {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_TX_RX;
    }

    pHalRuartAdapter->Status = HAL_UART_STATUS_OK;
    pHalRuartAdapter->pTxBuf = pTxData;
    pHalRuartAdapter->TxCount = Length;

    // Enable GDMA for TX
    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pTxHalGdmaAdapter;
    pHalGdmaOp = (PHAL_GDMA_OP)pUartGdmaConfig->pHalGdmaOp;

    if (((pHalRuartAdapter->TxCount & 0x03)==0) &&
        (((u32)(pHalRuartAdapter->pTxBuf) & 0x03)==0)) {
        // 4-bytes aligned, move 4 bytes each transfer
        pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeOne;
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
        pHalGdmaAdapter->GdmaCtl.BlockSize = pHalRuartAdapter->TxCount >> 2;
    }
    else{
        // move 1 byte each transfer
        pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthOneByte;
        pHalGdmaAdapter->GdmaCtl.BlockSize = pHalRuartAdapter->TxCount;
    }

    if (pHalGdmaAdapter->GdmaCtl.BlockSize > 4095) {
        // over Maximum block size 4096
        return HAL_ERR_PARA;
    }

    pHalGdmaAdapter->MuliBlockCunt = 0;
    pHalGdmaAdapter->MaxMuliBlock = 1;
    pHalGdmaAdapter->ChSar = (u32)(pHalRuartAdapter->pTxBuf);

    pHalGdmaAdapter->Rsvd4to7 = 0;
    pHalGdmaAdapter->Llpctrl = 0;
    pHalGdmaAdapter->GdmaCtl.LlpSrcEn = 0;

    pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));

    return HAL_OK;
}

/**
 * RUART Receive data by Interrupt (non-block) mode.
 *
 * RUART Receive data.
 * Receive one byte each time.
 *
 * @return u8
 */
HAL_Status
HalRuartDmaRecvRtl8195a_Patch(
    IN VOID *Data,  ///< RUART Adapter
    IN u8  *pRxData,  ///< Rx buffer
    IN u32 Length      // buffer length
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    HAL_UART_State State;
    PUART_DMA_CONFIG pUartGdmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP pHalGdmaOp;

    State = pHalRuartAdapter->State;
    if ((State != HAL_UART_STATE_READY) && (State != HAL_UART_STATE_BUSY_TX)) {
        DBG_UART_WARN("%s: on Busy, State=%d\n",  State);
        return HAL_BUSY;
    }

    if ((pRxData == NULL) || (Length == 0)) {
        pHalRuartAdapter->Status = HAL_UART_STATUS_ERR_PARA;
        DBG_UART_ERR("HalRuartDmaRecvRtl8195a: Err: pTxData=0x%x,  Length=%d\n",  pRxData, Length);
        return HAL_ERR_PARA;
    }

    if (State == HAL_UART_STATE_READY) {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_RX;
    }
    else {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_TX_RX;
    }

    pHalRuartAdapter->Status = HAL_UART_STATUS_OK;
    pHalRuartAdapter->pRxBuf = pRxData;
    pHalRuartAdapter->RxCount = Length;

    // Enable GDMA for RX
    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pRxHalGdmaAdapter;
    pHalGdmaOp = (PHAL_GDMA_OP)pUartGdmaConfig->pHalGdmaOp;

    if (((u32)(pHalRuartAdapter->pRxBuf) & 0x03)==0) {
        // 4-bytes aligned, move 4 bytes each DMA transaction
        pHalGdmaAdapter->GdmaCtl.DestMsize   = MsizeOne;
        pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes;
    }
    else{
        // move 1 byte each DMA transaction
        pHalGdmaAdapter->GdmaCtl.DestMsize   = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthOneByte;
    }
    pHalGdmaAdapter->GdmaCtl.BlockSize = pHalRuartAdapter->RxCount;
    if (pHalGdmaAdapter->GdmaCtl.BlockSize > 4095) {
        // over Maximum block size 4096
        DBG_UART_ERR("HalRuartDmaRecvRtl8195a: BlockSize too big(%d)\n", pHalGdmaAdapter->GdmaCtl.BlockSize);
        return HAL_ERR_PARA;
    }

    pHalGdmaAdapter->MuliBlockCunt = 0;
    pHalGdmaAdapter->MaxMuliBlock = 1;
    pHalGdmaAdapter->ChDar = (u32)(pHalRuartAdapter->pRxBuf);

    pHalGdmaAdapter->Rsvd4to7 = 0;
    pHalGdmaAdapter->Llpctrl = 0;
    pHalGdmaAdapter->GdmaCtl.LlpSrcEn = 0;

    pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChSeting((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));

    return HAL_OK;
}

/**
 * RUART send a data buffer by Multi-Block DMA(non-block) mode.
 *
 * RUART send data.
 *
 * @return VOID
 */
HAL_Status
HalRuartMultiBlkDmaSendRtl8195a(
    IN VOID *Data,      // PHAL_RUART_ADAPTER
    IN u8 *pTxData,     // the Buffer to be send
    IN u32 Length      // the length of data to be send
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)Data;
    PUART_DMA_CONFIG pUartGdmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP pHalGdmaOp;
    HAL_UART_State State;
    UART_DMA_MULTIBLK *pDmaBlkList;
    u32 BlockBytes;
    u32 TotalTr;
    u32 SrcAddr;
    u8 i;
    u8 BlkNum;

    State = pHalRuartAdapter->State;
    if ((State != HAL_UART_STATE_READY) && (State != HAL_UART_STATE_BUSY_RX)) {
        DBG_UART_WARN("HalRuartDmaSendRtl8195a: on Busy, State=%d\n", State);
        return HAL_BUSY;
    }

    if ((pTxData == NULL) || (Length == 0)) {
        pHalRuartAdapter->Status = HAL_UART_STATUS_ERR_PARA;
        DBG_UART_ERR("HalRuartMultiBlkDmaSendRtl8195a: Err: pTxData=0x%x,  Length=%d\n", pTxData, Length);
        return HAL_ERR_PARA;
    }

    if (State == HAL_UART_STATE_READY) {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_TX;
    }
    else {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_TX_RX;
    }

    pHalRuartAdapter->Status = HAL_UART_STATUS_OK;
    pHalRuartAdapter->pTxBuf = pTxData;
    pHalRuartAdapter->TxCount = Length;

    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pTxHalGdmaAdapter;
    pDmaBlkList = pUartGdmaConfig->pTxDmaBlkList;
    pHalGdmaOp = (PHAL_GDMA_OP)pUartGdmaConfig->pHalGdmaOp;

    if (((pHalRuartAdapter->TxCount & 0x03)==0) &&
        (((u32)(pHalRuartAdapter->pTxBuf) & 0x03)==0)) {
        // 4-bytes aligned, move 4 bytes each transfer
        pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeOne;
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthFourBytes;
        TotalTr = Length >> 2;     // 4-bytes each write
        BlockBytes = UART_DMA_BLOCK_SIZE << 2; // a block can transfer BlockSize*4 bytes
    }
    else{
        // move 1 byte each transfer
        pHalGdmaAdapter->GdmaCtl.SrcMsize   = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.SrcTrWidth = TrWidthOneByte;
        TotalTr = Length;     // 1-byte each write
        BlockBytes = UART_DMA_BLOCK_SIZE;
    }

    BlkNum = 0;
    SrcAddr = (u32)pTxData;
    for (i=0; i<UART_DMA_MBLK_NUM; i++) {
        pDmaBlkList->GdmaChLli[i].Sarx = SrcAddr;
        pDmaBlkList->GdmaChLli[i].Darx = (u32) (pHalGdmaAdapter->ChDar);
        pDmaBlkList->Lli[i].pLliEle = (GDMA_CH_LLI_ELE*) &(pDmaBlkList->GdmaChLli[i]);
        SrcAddr += BlockBytes;
        BlkNum++;
        if (TotalTr >= UART_DMA_BLOCK_SIZE) {
            pDmaBlkList->Lli[i].pNextLli = &(pDmaBlkList->Lli[i+1]); 
            pDmaBlkList->BlockSizeList[i].pNextBlockSiz = &(pDmaBlkList->BlockSizeList[i+1]);
            pDmaBlkList->BlockSizeList[i].BlockSize = UART_DMA_BLOCK_SIZE;
            TotalTr -= UART_DMA_BLOCK_SIZE;
        } else {
            pDmaBlkList->Lli[i].pNextLli = (struct GDMA_CH_LLI*)NULL; 
            pDmaBlkList->BlockSizeList[i].pNextBlockSiz = (struct BLOCK_SIZE_LIST*)NULL;
            pDmaBlkList->BlockSizeList[i].BlockSize = TotalTr;
            TotalTr = 0;
            break;
        }
    }

    if (TotalTr > 0) {
        // Cannot transfer all data in multiple-block DMA
        // Try to increase block number, but maximum block number is 16
        pDmaBlkList->Lli[UART_DMA_MBLK_NUM-1].pNextLli = (struct GDMA_CH_LLI*)NULL; 
        pDmaBlkList->BlockSizeList[UART_DMA_MBLK_NUM-1].pNextBlockSiz = (struct BLOCK_SIZE_LIST*)NULL;
        DBG_UART_ERR("HalRuartMultiBlkDmaSendRtl8195a: Cannot Transfer all data\n");
    }

    pHalGdmaAdapter->MuliBlockCunt = 0;
    pHalGdmaAdapter->MaxMuliBlock = BlkNum;
    pHalGdmaAdapter->Rsvd4to7 = 1;
    pHalGdmaAdapter->Llpctrl = 1;
    pHalGdmaAdapter->GdmaCtl.LlpSrcEn = 1;

    pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChBlockSeting((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));

    return HAL_OK;

}

/**
 * RUART Receive data by Multi-Block DMA (non-block) mode.
 *
 * RUART Receive data.
 *
 * @return u8
 */
HAL_Status
HalRuartMultiBlkDmaRecvRtl8195a(
    IN VOID *Data,  ///< RUART Adapter
    IN u8  *pRxData,  ///< Rx buffer
    IN u32 Length      // buffer length
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    HAL_UART_State State;
    PUART_DMA_CONFIG pUartGdmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    PHAL_GDMA_OP pHalGdmaOp;
    UART_DMA_MULTIBLK *pDmaBlkList;
    u32 TotalTr;
    u32 DstAddr;
    u8 i;
    u8 BlkNum;

    State = pHalRuartAdapter->State;
    if ((State != HAL_UART_STATE_READY) && (State != HAL_UART_STATE_BUSY_TX)) {
        DBG_UART_WARN("HalRuartMultiBlkDmaRecvRtl8195a: on Busy, State=%d\n",  State);
        return HAL_BUSY;
    }

    if ((pRxData == NULL) || (Length == 0)) {
        pHalRuartAdapter->Status = HAL_UART_STATUS_ERR_PARA;
        DBG_UART_ERR("HalRuartDmaRecvRtl8195a: Err: pTxData=0x%x,  Length=%d\n",  pRxData, Length);
        return HAL_ERR_PARA;
    }

    if (State == HAL_UART_STATE_READY) {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_RX;
    } else {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_TX_RX;
    }

    pHalRuartAdapter->Status = HAL_UART_STATUS_OK;
    pHalRuartAdapter->pRxBuf = pRxData;
    pHalRuartAdapter->RxCount = Length;

    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pRxHalGdmaAdapter;
    pDmaBlkList = pUartGdmaConfig->pRxDmaBlkList;
    pHalGdmaOp = (PHAL_GDMA_OP)pUartGdmaConfig->pHalGdmaOp;

    if (((u32)(pHalRuartAdapter->pRxBuf) & 0x03)==0) {
        // 4-bytes aligned, move 4 bytes each DMA transaction
        pHalGdmaAdapter->GdmaCtl.DestMsize   = MsizeOne;
        pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthFourBytes;
    } else{
        // move 1 byte each DMA transaction
        pHalGdmaAdapter->GdmaCtl.DestMsize   = MsizeFour;
        pHalGdmaAdapter->GdmaCtl.DstTrWidth = TrWidthOneByte;
    }

    TotalTr = Length;     // 1-byte each write, total transaction = data lenth
    BlkNum = 0;
    DstAddr = (u32)pRxData;
    for (i=0; i<UART_DMA_MBLK_NUM; i++) {
        pDmaBlkList->GdmaChLli[i].Sarx = (u32) (pHalGdmaAdapter->ChSar);
        pDmaBlkList->GdmaChLli[i].Darx = DstAddr;
        pDmaBlkList->Lli[i].pLliEle = (GDMA_CH_LLI_ELE*) &(pDmaBlkList->GdmaChLli[i]);
        DstAddr += UART_DMA_BLOCK_SIZE;
        BlkNum++;
        if (TotalTr >= UART_DMA_BLOCK_SIZE) {
            pDmaBlkList->Lli[i].pNextLli = &(pDmaBlkList->Lli[i+1]); 
            pDmaBlkList->BlockSizeList[i].pNextBlockSiz = &(pDmaBlkList->BlockSizeList[i+1]);
            pDmaBlkList->BlockSizeList[i].BlockSize = UART_DMA_BLOCK_SIZE;
            TotalTr -= UART_DMA_BLOCK_SIZE;
        } else {
            pDmaBlkList->Lli[i].pNextLli = (struct GDMA_CH_LLI*)NULL; 
            pDmaBlkList->BlockSizeList[i].pNextBlockSiz = (struct BLOCK_SIZE_LIST*)NULL;
            pDmaBlkList->BlockSizeList[i].BlockSize = TotalTr;
            TotalTr = 0;
            break;
        }
    }

    if (TotalTr > 0) {
        // Cannot transfer all data in multiple-block DMA
        // Try to increase block number, but maximum block number is 16
        pDmaBlkList->Lli[UART_DMA_MBLK_NUM-1].pNextLli = (struct GDMA_CH_LLI*)NULL; 
        pDmaBlkList->BlockSizeList[UART_DMA_MBLK_NUM-1].pNextBlockSiz = (struct BLOCK_SIZE_LIST*)NULL;
        DBG_UART_ERR("HalRuartMultiBlkDmaRecvRtl8195a: Cannot Transfer all data\n");
    }

    pHalGdmaAdapter->MuliBlockCunt = 0;
    pHalGdmaAdapter->MaxMuliBlock = BlkNum;
    pHalGdmaAdapter->Rsvd4to7 = 1;
    pHalGdmaAdapter->Llpctrl = 1;
    pHalGdmaAdapter->GdmaCtl.LlpDstEn = 1;

    pHalGdmaOp->HalGdmaOnOff((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChIsrEnAndDis((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChBlockSeting((VOID*)(pHalGdmaAdapter));
    pHalGdmaOp->HalGdmaChEn((VOID*)(pHalGdmaAdapter));

    return HAL_OK;
}

/**
 * Stop non-blocking UART TX
 *
 *
 * @return VOID
 */
HAL_Status
HalRuartStopRecvRtl8195a_Patch(
        IN VOID *Data      // PHAL_RUART_ADAPTER
        )
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)Data;
    PUART_DMA_CONFIG pUartGdmaConfig;
    HAL_UART_State State;
    u32 DMA_Dar;
    u32 RecvdCnt;

    State = pHalRuartAdapter->State;
    if ((State != HAL_UART_STATE_BUSY_RX) && (State != HAL_UART_STATE_BUSY_TX_RX)) {
        DBG_UART_WARN("HalRuartStopRecvRtl8195a: Not in TX state, State=%d\n",  State);
        return HAL_OK;
    }

    if (HAL_OK != RuartLock(pHalRuartAdapter)) {
        DBG_UART_WARN("HalRuartStopRecvRtl8195a:Unable to Lock, Statu=%d\n",  State);
        return HAL_BUSY;
    }

    // Disable Rx interrupt
    pHalRuartAdapter->Interrupts &= ~(RUART_IER_ERBI | RUART_IER_ELSI);
    HalRuartSetIMRRtl8195a (pHalRuartAdapter);

    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    if (NULL != pUartGdmaConfig) {
        PHAL_GDMA_ADAPTER pHalGdmaAdapter;
        PHAL_GDMA_OP pHalGdmaOp;
        u8  IsrTypeMap;

        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pRxHalGdmaAdapter;
        pHalGdmaOp = (PHAL_GDMA_OP)pUartGdmaConfig->pHalGdmaOp;

        if ((NULL != pHalGdmaAdapter) && (NULL != pHalGdmaOp) && 
            (HalGdmaQueryChEnRtl8195a((VOID*)pHalGdmaAdapter))) {
            // Clean Auto Reload Bit
            pHalGdmaOp->HalGdmaChCleanAutoDst((VOID*)pHalGdmaAdapter);
            // Clear Pending ISR
            IsrTypeMap = pHalGdmaOp->HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
            pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));

            DMA_Dar = HalGdmaQueryDArRtl8195a((VOID*)pHalGdmaAdapter);
            RecvdCnt = DMA_Dar - (u32)(pHalRuartAdapter->pRxBuf);
//            DBG_8195A("%s: got %d bytes\r\n", __FUNCTION__, RecvdCnt);
            pHalRuartAdapter->RxCount -= RecvdCnt;
            pHalRuartAdapter->pRxBuf += RecvdCnt;
        }
    }

    while (HalRuartGetCRtl8195a(pHalRuartAdapter, pHalRuartAdapter->pRxBuf) == HAL_OK) {
        pHalRuartAdapter->RxCount--;
        pHalRuartAdapter->pRxBuf++;
    }

    if (pHalRuartAdapter->State == HAL_UART_STATE_BUSY_RX) {
        pHalRuartAdapter->State = HAL_UART_STATE_READY;
    }
    else {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_TX;
    }

    RuartUnLock(pHalRuartAdapter);

    return HAL_OK;

}

/**
 * Stop non-blocking UART TX
 *
 *
 * @return VOID
 */
HAL_Status
HalRuartStopSendRtl8195a_Patch(
        IN VOID *Data      // PHAL_RUART_ADAPTER
        )
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)Data;
    PUART_DMA_CONFIG pUartGdmaConfig;
    HAL_UART_State State;
    u32 DMA_Sar;
    u32 TxedCnt;

    State = pHalRuartAdapter->State;
    if ((State != HAL_UART_STATE_BUSY_TX) && (State != HAL_UART_STATE_BUSY_TX_RX)) {
        DBG_UART_WARN("HalRuartDmaSendRtl8195a: Not in TX state, State=%d\n", State);
        return HAL_OK;
    }

    if (HAL_OK != RuartLock(pHalRuartAdapter)) {
        DBG_UART_WARN("HalRuartDmaSendRtl8195a:Unable to Lock, Statu=%d\n", State);
        return HAL_BUSY;
    }

    // Disable Tx FIFO empty interrupt
    pHalRuartAdapter->Interrupts &= ~RUART_IER_ETBEI;
    HalRuartSetIMRRtl8195a (pHalRuartAdapter);

    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    if (NULL != pUartGdmaConfig) {
        PHAL_GDMA_ADAPTER pHalGdmaAdapter;
        PHAL_GDMA_OP pHalGdmaOp;
        u8  IsrTypeMap;

        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pTxHalGdmaAdapter;
        pHalGdmaOp = (PHAL_GDMA_OP)pUartGdmaConfig->pHalGdmaOp;

        if ((NULL != pHalGdmaAdapter) && (NULL != pHalGdmaOp) && 
            (HalGdmaQueryChEnRtl8195a((VOID*)pHalGdmaAdapter))) {
            // Clean Auto Reload Bit
            pHalGdmaOp->HalGdmaChCleanAutoDst((VOID*)pHalGdmaAdapter);
            // Clear Pending ISR
            IsrTypeMap = pHalGdmaOp->HalGdmaChIsrClean((VOID*)pHalGdmaAdapter);
            pHalGdmaOp->HalGdmaChDis((VOID*)(pHalGdmaAdapter));

            DMA_Sar = HalGdmaQuerySArRtl8195a((VOID*)pHalGdmaAdapter);
            TxedCnt = DMA_Sar - (u32)(pHalRuartAdapter->pTxBuf);
//            DBG_8195A("%s: got %d bytes\r\n", __FUNCTION__, RecvdCnt);
            pHalRuartAdapter->TxCount -= TxedCnt;
            pHalRuartAdapter->pTxBuf += TxedCnt;
        }
    }

    if (State == HAL_UART_STATE_BUSY_TX) {
        pHalRuartAdapter->State = HAL_UART_STATE_READY;
    }
    else {
        pHalRuartAdapter->State = HAL_UART_STATE_BUSY_RX;
    }

    RuartUnLock(pHalRuartAdapter);

    return HAL_OK;

}

VOID
HalRuartEnterCriticalRtl8195a(
        IN VOID *Data  ///< RUART Adapter
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
#ifdef CONFIG_GDMA_EN
    PUART_DMA_CONFIG pUartGdmaConfig;
#endif

    InterruptDis(&pHalRuartAdapter->IrqHandle);

#ifdef CONFIG_GDMA_EN
    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    if (NULL != pUartGdmaConfig) {
        PHAL_GDMA_ADAPTER pHalGdmaAdapter;
        
        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pRxHalGdmaAdapter;
        if (pHalGdmaAdapter->ChEn != 0) {
            InterruptDis(&pUartGdmaConfig->RxGdmaIrqHandle);
        }
        
        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pTxHalGdmaAdapter;
        if (pHalGdmaAdapter->ChEn != 0) {
            InterruptDis(&pUartGdmaConfig->TxGdmaIrqHandle);
        }
    }
#endif    
}

VOID
HalRuartExitCriticalRtl8195a(
        IN VOID *Data  ///< RUART Adapter
        )
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
#ifdef CONFIG_GDMA_EN
    PUART_DMA_CONFIG pUartGdmaConfig;
#endif

    InterruptEn(&pHalRuartAdapter->IrqHandle);

#ifdef CONFIG_GDMA_EN
    pUartGdmaConfig = pHalRuartAdapter->DmaConfig;
    if (NULL != pUartGdmaConfig) {
        PHAL_GDMA_ADAPTER pHalGdmaAdapter;
        
        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pRxHalGdmaAdapter;
        if (pHalGdmaAdapter->ChEn != 0) {
            InterruptEn(&pUartGdmaConfig->RxGdmaIrqHandle);
        }
        
        pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pTxHalGdmaAdapter;
        if (pHalGdmaAdapter->ChEn != 0) {
            InterruptEn(&pUartGdmaConfig->TxGdmaIrqHandle);
        }
    }
#endif    
}

VOID
HalRuartDumpRegRtl8195a(
    IN VOID *Data
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;
    u8 UartIndex;
    u32 i;
    u32 RegValue;

    UartIndex = pHalRuartAdapter->UartIndex;

    /* Set DLAB bit to 1 to access DLL/DLM */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);
    RegValue |= RUART_LINE_CTL_REG_DLAB_ENABLE;
    HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);
    
    for (i=0;i<0x40;i++) {
        DBG_8195A("UART Reg[0x%x] = 0x%x\r\n", i, HAL_RUART_READ8(UartIndex, i));
    }
    
/* clear DLAB bit */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);
    RegValue &= ~(RUART_LINE_CTL_REG_DLAB_ENABLE);
    HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);
}

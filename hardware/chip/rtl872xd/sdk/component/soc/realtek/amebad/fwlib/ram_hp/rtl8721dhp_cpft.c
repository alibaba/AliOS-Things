/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
 
#include "ameba_soc.h"

#if (defined(CONFIG_CP))

BOOT_RAM_TEXT_SECTION
VOID CPTest_GPIOCtrl(u32 GPIO_Pin, u32 Val)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_Init(&GPIO_InitStruct);
	GPIO_WriteBit(GPIO_Pin, Val);
}

BOOT_RAM_TEXT_SECTION
VOID CPTest_Init(VOID)
{
	BKUP_Write(BKUP_REG2, 0x00);

	CPTest_GPIOCtrl(_PA_0, 0);
	CPTest_GPIOCtrl(_PA_1, 0);
	CPTest_GPIOCtrl(_PA_2, 0);
	CPTest_GPIOCtrl(_PA_3, 0);
	CPTest_GPIOCtrl(_PA_4, 0);
	CPTest_GPIOCtrl(_PA_5, 0);
	CPTest_GPIOCtrl(_PA_16, 0);
	DelayUs(1000);

	BKUP_Write(BKUP_REG2, 0x01);
	GPIO_WriteBit(_PA_0, 1);
}

BOOT_RAM_TEXT_SECTION
VOID CPTest_FlashCalibrationOk(VOID)
{
	BKUP_Write(BKUP_REG2, 0x03);
	GPIO_WriteBit(_PA_1, 1);
}

BOOT_RAM_TEXT_SECTION
VOID CPTest_EnterImg2Ok(VOID)
{
	BKUP_Write(BKUP_REG2, 0x07);
	GPIO_WriteBit(_PA_2, 1);
}

BOOT_RAM_TEXT_SECTION
void CPTest_UARTInit(u8 uart_idx, u8 pin_mux, u32 baud_rate)
{
	UART_InitTypeDef UART_InitStruct;

	switch (uart_idx) {
		case 0:
			RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);
			PinCtrl(PERIPHERAL_UART0, pin_mux, ON);
		break;

		case 1:
			RCC_PeriphClockCmd(APBPeriph_UART1, APBPeriph_UART1_CLOCK, ENABLE);
			PinCtrl(PERIPHERAL_UART1, pin_mux, ON);
		break;

		case 2:
			/* UART2 is LogUART */
			RCC_PeriphClockCmd(APBPeriph_LOGUART, APBPeriph_LOGUART_CLOCK, ENABLE);
			PinCtrl(PERIPHERAL_LOG_UART, pin_mux, ON);
		break;

		default:
		return;
	}
	
	UART_StructInit(&UART_InitStruct);

	UART_InitStruct.DmaModeCtrl = DISABLE;
	
	UART_InitStruct.WordLen = RUART_WLS_8BITS;
	UART_InitStruct.StopBit = RUART_STOP_BIT_1;     // word length select: 0 -> no stop bit, 1 -> 1 stop bit
	UART_InitStruct.Parity = DISABLE;      // parity check enable
	UART_InitStruct.ParityType = RUART_ODD_PARITY;
	UART_InitStruct.StickParity = DISABLE;

	UART_InitStruct.FlowControl = DISABLE;
	UART_InitStruct.RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_1BYTES;
	UART_InitStruct.RxErReportCtrl = UART_RX_EEROR_REPORT_ENABLE;
    
	UART_Init(UART_DEV_TABLE[uart_idx].UARTx, &UART_InitStruct);
	UART_SetBaud(UART_DEV_TABLE[uart_idx].UARTx, baud_rate);
	UART_INTConfig(UART_DEV_TABLE[uart_idx].UARTx, RUART_IER_ERBI | RUART_IER_ELSI, DISABLE);
	UART_RxCmd(UART_DEV_TABLE[uart_idx].UARTx, ENABLE);
}

BOOT_RAM_TEXT_SECTION
static VOID CPTest_UART(VOID)
{
	char rx_char = 0;		
	u32 timeout = 10000;
	
	CPTest_UARTInit(1, S0, 1500000);
	CPTest_UARTInit(0, S1, 1500000);

	BKUP_Write(BKUP_REG2, 0x1F);
	GPIO_WriteBit(_PA_4, 1);

	/* clear UART 1 RX */
	while (1) {
		if (UART_Readable(UART1_DEV)) {
			UART_CharGet(UART1_DEV, &rx_char);
		} else {
			break;
		}
	
	}

	/* clear UART 0 RX */
	while (1) {
		if (UART_Readable(UART0_DEV)) {
			UART_CharGet(UART0_DEV, &rx_char);
		} else {
			break;
		}
	}

	rx_char = 0;
	UART_CharPut(UART1_DEV, 0xAA);		
	do {
		if (UART_Readable(UART0_DEV)) {
			UART_CharGet(UART0_DEV, &rx_char);
			break;
		}

		timeout--;
	} while (timeout != 0);
	
	if (rx_char == 0xAA) {
		BKUP_Write(BKUP_REG2, 0x3F);
		GPIO_WriteBit(_PA_5, 1);
	}
}


BOOT_RAM_TEXT_SECTION
VOID CPTest_OSCCalibrationOk(VOID)
{
	int i = 5;
	BKUP_Write(BKUP_REG2, 0x0F);
	GPIO_WriteBit(_PA_3, 1);

	CPTest_UART();

	if (BKUP_Read(BKUP_REG2) == 0x3F) {
		while(i--){
			GPIO_WriteBit(_PA_16, 1);
			DelayUs(100);
			GPIO_WriteBit(_PA_16, 0);
			DelayUs(100);
		}
	} 
	
	DiagPrintf("\n");
	DiagPrintf("<<<<                          >>>>\n");
	DiagPrintf("<<<<    Flash boot SUCCESS    >>>>\n");
	DiagPrintf("<<<<                          >>>>\n");
	DiagPrintf("\n");
}
#endif

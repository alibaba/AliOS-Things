/**
  ******************************************************************************
  * @file    rtl8721d_usi.h
  * @author
  * @version V1.0.0
  * @date    2017-09-26
  * @brief   This file contains all the functions prototypes for the USI firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_USI_H_
#define _RTL8721D_USI_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup USI_Register_Definitions USI Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup USI_MODE_CTRL
 * @{
 *****************************************************************************/
#define USI_SERIAL_MODE						((u32)0x00000007)	     		/*BIT[2:0], USI serial mode control*/
#define USI_SERIAL_ALL_MODE_DISABLE			((u32)0x00000000)	     		/*BIT[2:0] = 000B, USI all serial mode disable*/
#define USI_SERIAL_UART_MODE				((u32)0x00000001)	     		/*BIT[2:0] = 001B, USI serial UART mode*/
#define USI_SERIAL_SPI_MODE					((u32)0x00000002)	     		/*BIT[2:0] = 010B, USI serial SPI mode*/
#define USI_SERIAL_I2C_MODE					((u32)0x00000004)	     		/*BIT[2:0] = 100B, USI serial I2C mode*/
#define USI_TX_BIT_SWAP						((u32)0x00000010)	     		/*BIT[4], USI TX bit swap */
#define USI_RX_BIT_SWAP						((u32)0x00000020)	     		/*BIT[5], USI RX bit swap */
#define USI_TX_BYTE_SWAP					((u32)0x00000040)	     		/*BIT[6], USI tx byte swap */
#define USI_RX_BYTE_SWAP					((u32)0x00000080)	     		/*BIT[7], USI rx byte swap */

/** @} */

/**************************************************************************//**
 * @defgroup SW_RESET
 * @{
 *****************************************************************************/
#define USI_SW_RESET_RSTB					((u32)0x00000001)	     		/*BIT[0], USI reset/release all logic*/
#define USI_SW_RESET_TX_RSTB				((u32)0x00000002)	     		/*BIT[1], USI reset TX path*/
#define USI_SW_RESET_RX_RSTB				((u32)0x00000004)	     		/*BIT[2], USI reset RX path*/
#define USI_SW_RESET_TXFIFO_RSTB			((u32)0x00000008)	     		/*BIT[3], USI reset tx FIFO*/
#define USI_SW_RESET_RXFIFO_RSTB			((u32)0x00000010)	     		/*BIT[4], USI reset rx FIFO*/

/** @} */

/**************************************************************************//**
 * @defgroup DMA_ENABLE
 * @{
 *****************************************************************************/
#define USI_TX_DMA_ENABLE					((u32)0x00000001)	     		/*BIT[0], USI tx path enable*/
#define USI_RX_DMA_ENABLE					((u32)0x00000002)	     		/*BIT[1], USI rx path enable*/

/** @} */

/**************************************************************************//**
 * @defgroup DMA_REQ_SIZE
 * @{
 *****************************************************************************/
#define USI_TX_DMA_BURST_SIZE				((u32)0x0000007F)	     		/*BIT[6:0], TX DMA burst size*/
#define USI_TX_DMA_SINGLE_SIZE				((u32)0x00007F00)	     		/*BIT[14:8], TX DMA single size*/
#define USI_RX_DMA_BURST_SIZE				((u32)0x007F0000)	     		/*BIT[22:16], RX DMA burst size*/
#define USI_RX_DMA_SINGLE_SIZE				((u32)0x7F000000)	     		/*BIT[30:24], RX DMA single size*/

/** @} */

/**************************************************************************//**
 * @defgroup TX_FIFO_WRITE
 * @{
 *****************************************************************************/
#define USI_TX_FIFO_WRITE_DATA				((u32)0x0000FFFF)	     		/*BIT[15:0], TX FIFO WRITE DATA mask*/
#define USI_TX_FIFO_DATA						((u32)0x000000FF)	     		/*BIT[7:0], TX DATA*/
#define USI_TX_FIFO_CMD_RW					((u32)0x00000001<<8)	   	/*BIT[8], this bit controls whether a read or write is performed*/
#define USI_TX_FIFO_CMD_STOP				((u32)0x00000001<<9)	   	/*BIT[9], this bit controls whether a STOP is issued after the byte is sent or received*/
#define USI_TX_FIFO_CMD_RESTART				((u32)0x00000001<<10)	   	/*BIT[10], this bit controls whether a RESTART is issued after the byte is sent or received*/
#define USI_TX_FIFO_NULL_DATA				((u32)0x00000001<<11)	   	/*BIT[11], NULL_DATA condition*/


/** @} */

/**************************************************************************//**
 * @defgroup TX_FIFO_CTRL
 * @{
 *****************************************************************************/
#define USI_TXFIFO_ALMOST_EMPTY_TH			((u32)0x0000007F)	     		/*BIT[6:0], Tx FIFO almost empty threshold, when Tx FIFO empty space >= 
																		txfifo_almost_empty_th, then txfifo_almost_empty will be "1" and report almost empty*/

/** @} */

/**************************************************************************//**
 * @defgroup TX_FIFO_STATUS
 * @{
 *****************************************************************************/
#define USI_TXFIFO_FULL						((u32)0x00000001)	     		/*BIT[0], TX FIFO FULL status*/
#define USI_TXFIFO_EMPTY					((u32)0x00000001<<1)	     	/*BIT[1], TX FIFO EMPTY status*/
#define USI_TXFIFO_ALMOST_EMPTY_COPY		((u32)0x00000001<<2)	     	/*BIT[2], TX FIFO ALMOST EMPTY status*/
#define USI_TXFIFO_EMPTY_SPACE				((u32)0x0000007F<<8)	     	/*BIT[14:8], Tx FIFO empty space(unpush unit number)*/

/** @} */

/**************************************************************************//**
 * @defgroup RX_FIFO_READ
 * @{
 *****************************************************************************/
#define USI_RX_FIFO_RD_DATA					((u32)0x0000FFFF)	     		/*BIT[15:0], read RX FIFO DATA mask*/
#define USI_RX_FIFO_DATA					((u32)0x000000FF)	     		/*BIT[7:0], TX DATA*/
#define USI_RX_FIFO_PARITY_ERR				((u32)0x00000001<<8)	   	/*BIT[8], parity error status  in current character*/
#define USI_RX_FIFO_STOP_ERR				((u32)0x00000001<<9)	   	/*BIT[9], stop error status in current character*/
#define USI_RX_FIFO_BREAK_FLAG				((u32)0x00000001<<10)	   	/*BIT[10], break event status in current character*/

/** @} */

/**************************************************************************//**
 * @defgroup TX_FIFO_CTRL
 * @{
 *****************************************************************************/
#define USI_RXFIFO_ALMOST_FULL_TH			((u32)0x0000007F)	     		/*BIT[6:0], Rx FIFO almost full threshold, when Rx FIFO valid cnt >= 
																	    rxfifo_almost_full_th, then rxfifo_almost_full will be "1" and report almost full*/

/** @} */

/**************************************************************************//**
 * @defgroup RX_FIFO_STATUS
 * @{
 *****************************************************************************/
#define USI_RXFIFO_FULL						((u32)0x00000001)	     		/*BIT[0], RX FIFO FULL status*/
#define USI_RXFIFO_EMPTY					((u32)0x00000001<<1)	     	/*BIT[1], RX FIFO EMPTY status*/
#define USI_RXFIFO_ALMOST_FULL_COPY		((u32)0x00000001<<2)	     	/*BIT[2], RX FIFO ALMOST FULL status*/
#define USI_RXFIFO_VALID_CNT				((u32)0x0000007F<<8)	     	/*BIT[14:8], Rx FIFO valid cnt(pushed unit number which can be read)*/

/** @} */

/**************************************************************************//**
 * @defgroup RX_FIFO_RD_CNT
 * @{
 *****************************************************************************/
#define USI_RX_FIFO_RD_CNT					((u32)0x0000FFFF)	     		/*BIT[15:0], mask for counting  the byte number of data read from Rx FIFO*/
#define USI_RX_FIFO_RD_CNT_CLR				((u32)0x00010000)	     		/*BIT[16], write 1 to clear rxfifo_rd_cnt*/

/** @} */

/**************************************************************************//**
 * @defgroup INTERRUPT_ENABLE
 * @{
 *****************************************************************************/
#define USI_TXFIFO_ALMOST_EMTY_INTR_EN		((u32)0x00000001)	     		/*BIT[0], txfifo_almost_empty_intr_en*/
#define USI_TXFIFO_OVERFLOW_INTR_EN		((u32)0x00000001<<1)	     	/*BIT[1], txfifo_overflow_intr_en*/
#define USI_TXFIFO_UNDERFLOW_INTR_EN		((u32)0x00000001<<2)	     	/*BIT[2], txfifo_underflow_intr_en*/
#define USI_RXFIFO_ALMOST_FULL_INTR_EN		((u32)0x00000001<<4)	     	/*BIT[4], rxfifo_almost_full_intr_en*/
#define USI_RXFIFO_OVERFLOW_INTR_EN		((u32)0x00000001<<5)	     	/*BIT[5], rxfifo_overflow_intr_en*/
#define USI_RXFIFO_UNDERFLOW_INTR_EN		((u32)0x00000001<<6)	     	/*BIT[6], rxfifo_underflow_intr_en*/
#define USI_RX_PARITY_ERR_INTR_EN			((u32)0x00000001<<8)	     	/*BIT[8], uart_rx_parity_err_intr_en*/
#define USI_RX_STOP_ERR_INTR_EN				((u32)0x00000001<<9)	     	/*BIT[9], uart_rx_stop_err_intr_en*/
#define USI_RX_BREAK_INTR_EN				((u32)0x00000001<<10)	     	/*BIT[10], uart_rx_break_intr_en*/
#define USI_RXFIFO_TM_OUT_INTR_EN			((u32)0x00000001<<11)	     	/*BIT[11], uart_rxfifo_timeout_intr_en*/
#define USI_RX_BAUDMON_DONE_INTR_EN		((u32)0x00000001<<12)	     	/*BIT[12], uart_rx_baudmon_done_intr_en*/
#define USI_TX_CTS_CHANGE_INTR_EN			((u32)0x00000001<<13)	     	/*BIT[13], uart_tx_cts_change_intr_en*/
#define USI_SPI_RX_DATA_FRM_ERR_INTER_EN	((u32)0x00000001<<16)	     	/*BIT[16], spi_rx_data_frame_err_intr_en*/
#define USI_I2C_RD_REQ_INTER_EN				((u32)0x00000001<<20)	     	/*BIT[20], i2c_rd_req_intr_en*/
#define USI_I2C_TX_ABRT_INTER_EN			((u32)0x00000001<<21)	     	/*BIT[21], i2c_tx_abrt_intr_en*/
#define USI_I2C_RX_DONE_INTER_EN			((u32)0x00000001<<22)	     	/*BIT[22], i2c_rx_done_intr_en*/
#define USI_I2C_ACTIVITY_INTER_EN			((u32)0x00000001<<23)	     	/*BIT[23], i2c_activity_intr_en*/
#define USI_I2C_STOP_DET_INTER_EN			((u32)0x00000001<<24)	     	/*BIT[24], i2c_stop_det_intr_en*/
#define USI_I2C_START_DET_INTER_EN			((u32)0x00000001<<25)	     	/*BIT[25], i2c_stop_det_intr_en*/
#define USI_I2C_GEN_CALL_INTER_EN			((u32)0x00000001<<26)	     	/*BIT[26], i2c_gen_call_intr_en*/
#define USI_I2C_LP_WAKE_INTER_EN			((u32)0x00000001<<27)	     	/*BIT[27], i2c_lp_wake_intr_en*/
#define USI_I2C_DMA_DONE_INTER_EN			((u32)0x00000001<<28)	     	/*BIT[28], i2c_dma_done_intr_en*/

/** @} */

/**************************************************************************//**
 * @defgroup INTERRUPT_STATUS
 * @{
 *****************************************************************************/
#define USI_TXFIFO_ALMOST_EMTY_INTS	((u32)0x00000001)	     		/*BIT[0], txfifo_almost_empty_intr*/
#define USI_TXFIFO_OVERFLOW_INTS		((u32)0x00000001<<1)	     	/*BIT[1], txfifo_overflow_intr*/
#define USI_TXFIFO_UNDERFLOW_INTS		((u32)0x00000001<<2)	     	/*BIT[2], txfifo_underflow_intr*/
#define USI_RXFIFO_ALMOST_FULL_INTS	((u32)0x00000001<<4)	     	/*BIT[4], rxfifo_almost_full_intr*/
#define USI_RXFIFO_OVERFLOW_INTS		((u32)0x00000001<<5)	     	/*BIT[5], rxfifo_overflow_intr*/
#define USI_RXFIFO_UNDERFLOW_INTS		((u32)0x00000001<<6)	     	/*BIT[6], rxfifo_underflow_intr*/
#define USI_RX_PARITY_ERR_INTS			((u32)0x00000001<<8)	     	/*BIT[8], uart_rx_parity_err_intr*/
#define USI_RX_STOP_ERR_INTS			((u32)0x00000001<<9)	     	/*BIT[9], uart_rx_stop_err_intr*/
#define USI_RX_BREAK_INTS				((u32)0x00000001<<10)	     	/*BIT[10], uart_rx_break_intr*/
#define USI_RXFIFO_TM_OUT_INTS			((u32)0x00000001<<11)	     	/*BIT[11], uart_rxfifo_timeout_intr*/
#define USI_RX_BAUDMON_DONE_INTS		((u32)0x00000001<<12)	     	/*BIT[12], uart_rx_baudmon_done_intr*/
#define USI_TX_CTS_CHANGE_INTS			((u32)0x00000001<<13)	     	/*BIT[13], uart_tx_cts_change_intr*/
#define USI_SPI_RX_DATA_FRM_ERR_INTS	((u32)0x00000001<<16)	     	/*BIT[16], spi_rx_data_frame_err_intr*/
#define USI_I2C_RD_REQ_INTS				((u32)0x00000001<<20)	     	/*BIT[20], i2c_rd_req_intr*/
#define USI_I2C_TX_ABRT_INTS				((u32)0x00000001<<21)	     	/*BIT[21], i2c_tx_abrt_intr*/
#define USI_I2C_RX_DONE_INTS			((u32)0x00000001<<22)	     	/*BIT[22], i2c_rx_done_intr*/
#define USI_I2C_ACTIVITY_INTS			((u32)0x00000001<<23)	     	/*BIT[23], i2c_activity_intr*/
#define USI_I2C_STOP_DET_INTS			((u32)0x00000001<<24)	     	/*BIT[24], i2c_stop_det_intr*/
#define USI_I2C_START_DET_INTS			((u32)0x00000001<<25)	     	/*BIT[25], i2c_stop_det_intr*/
#define USI_I2C_GEN_CALL_INTS			((u32)0x00000001<<26)	     	/*BIT[26], i2c_gen_call_intr*/
#define USI_I2C_LP_WAKE_INTS			((u32)0x00000001<<27)	     	/*BIT[27], i2c_lp_wake_intr*/
#define USI_I2C_DMA_DONE_INTS			((u32)0x00000001<<28)	     	/*BIT[28], i2c_dma_done_intr*/

/** @} */

/**************************************************************************//**
 * @defgroup INTERRUPT_RAW_STATUS
 * @{
 *****************************************************************************/
#define USI_TXFIFO_ALMOST_EMTY_RSTS	((u32)0x00000001)	     		/*BIT[0], txfifo_almost_empty*/
#define USI_TXFIFO_OVERFLOW_RSTS		((u32)0x00000001<<1)	     	/*BIT[1], txfifo_overflow*/
#define USI_TXFIFO_UNDERFLOW_RSTS		((u32)0x00000001<<2)	     	/*BIT[2], txfifo_underflow*/
#define USI_RXFIFO_ALMOST_FULL_RSTS	((u32)0x00000001<<4)	     	/*BIT[4], rxfifo_almost_full*/
#define USI_RXFIFO_OVERFLOW_RSTS		((u32)0x00000001<<5)	     	/*BIT[5], rxfifo_overflow*/
#define USI_RXFIFO_UNDERFLOW_RSTS		((u32)0x00000001<<6)	     	/*BIT[6], rxfifo_underflow*/
#define USI_RX_PARITY_ERR_RSTS			((u32)0x00000001<<8)	     	/*BIT[8], uart_rx_parity_err*/
#define USI_RX_STOP_ERR_RSTS			((u32)0x00000001<<9)	     	/*BIT[9], uart_rx_stop_err*/
#define USI_RX_BREAK_RSTS				((u32)0x00000001<<10)	     	/*BIT[10], uart_rx_break_intr*/
#define USI_RXFIFO_TM_OUT_RSTS			((u32)0x00000001<<11)	     	/*BIT[11], uart_rxfifo_timeout*/
#define USI_RX_BAUDMON_DONE_RSTS		((u32)0x00000001<<12)	     	/*BIT[12], uart_rx_baudmon_done*/
#define USI_TX_CTS_CHANGE_RSTS			((u32)0x00000001<<13)	     	/*BIT[13], uart_tx_cts_change*/
#define USI_SPI_RX_DATA_FRM_ERR_RSTS	((u32)0x00000001<<16)	     	/*BIT[16], spi_rx_data_frame_err*/
#define USI_I2C_RD_REQ_RSTS				((u32)0x00000001<<20)	     	/*BIT[20], i2c_rd_req*/
#define USI_I2C_TX_ABRT_RSTS			((u32)0x00000001<<21)	     	/*BIT[21], i2c_tx_abrt*/
#define USI_I2C_RX_DONE_RSTS			((u32)0x00000001<<22)	     	/*BIT[22], i2c_rx_done*/
#define USI_I2C_ACTIVITY_RSTS			((u32)0x00000001<<23)	     	/*BIT[23], i2c_activity*/
#define USI_I2C_STOP_DET_RSTS			((u32)0x00000001<<24)	     	/*BIT[24], i2c_stop_det*/
#define USI_I2C_START_DET_RSTS			((u32)0x00000001<<25)	     	/*BIT[25], i2c_stop_det*/
#define USI_I2C_GEN_CALL_RSTS			((u32)0x00000001<<26)	     	/*BIT[26], i2c_gen_call*/
#define USI_I2C_LP_WAKE_RSTS			((u32)0x00000001<<27)	     	/*BIT[27], i2c_lp_wake*/
#define USI_I2C_DMA_DONE_RSTS			((u32)0x00000001<<28)	     	/*BIT[28], i2c_dma_done*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_TX_ABRT_SOURCE
 * @{
 *****************************************************************************/
#define USI_I2C_ABRT_7B_ADDR_NOACK		((u32)0x00000001)	     		/*BIT[0], Master is in 7-bit addressing mode and the address sent was 
																    		not acknowledged by any slave. Role is Master*/
#define USI_I2C_ABRT_10ADDR1_NOACK		((u32)0x00000001<<1)	     	/*BIT[1], Master is in 10-bit address mode and the first 10-bit address 
																		byte was not acknowledged by any slave. Role is Master.*/
#define USI_I2C_ABRT_10ADDR2_NOACK		((u32)0x00000001<<2)	     	/*BIT[2], Master is in 10-bit address mode and the second byte of the 
																		10-bit address was not acknowledged by any slave. Role is Master.*/
#define USI_I2C_ABRT_TXDATA_NOACK		((u32)0x00000001<<3)	     	/*BIT[3], This is a master-mode only bit. Master has received an acknowledgement
																		for the address, but when it sent data byte(s) following the address, it did not
																		receive an acknowledge from the remote slave(s). Role is Master-Transmitter.*/
#define USI_I2C_ABRT_GCALL_NOACK		((u32)0x00000001<<4)	     	/*BIT[4], I2C in master mode sent a General Call and no slave on the bus acknowledged 
																		the General Call. Role is Master-Transmitter.*/
#define USI_I2C_ABRT_GCALL_READ			((u32)0x00000001<<5)	     	/*BIT[5], I2C in master mode sent a General Call but the user programmed the byte following
																		the General Call to be a read from the bus. Role is Master-Transmitter.*/
#define USI_I2C_ABRT_HS_ACKDET			((u32)0x00000001<<6)	     	/*BIT[6], Master is in High Speed mode and the High Speed Master code was acknowledged 
																		(wrong behavior). Role is Master.*/
#define USI_I2C_ABRT_SBYTE_ACKDET		((u32)0x00000001<<7)	     	/*BIT[7], Master has sent a START Byte and the START Byte was acknowledged (wrong behavior).
																		Role is Master.*/
#define USI_I2C_ABRT_HS_NORSTRT		((u32)0x00000001<<8)	     	/*BIT[8], The restart is disabled and the user is trying to use the master to transfer data in High 
																		Speed mode. Role is Master.*/
#define USI_I2C_ABRT_SBYTE_NORSTRT		((u32)0x00000001<<9)	     	/*BIT[9], The restart is disabled and the user is trying to send a START Byte. Role is Master.*/
#define USI_I2C_ABRT_10B_RD_NORSTRT	((u32)0x00000001<<10)	     	/*BIT[10], The restart is disabled and the master sends a read command in 10-bit addressing mode. 
																		Role is Master-Receiver.*/
#define USI_I2C_ABRT_MASTER_DIS			((u32)0x00000001<<11)	     	/*BIT[11], User tries to initiate a Master operation with the Master mode disabled. Role is Master.*/
#define USI_I2C_ARBT_LOST				((u32)0x00000001<<12)	     	/*BIT[12], Master has lost arbitration, or if i2c_tx_abrt_source [14] is also set, then the slave transmitter
																		has lost arbitration. Role is Master-Transmitter or Slave-Transmitter.*/
#define USI_I2C_ABRT_SLVFLUSH_TXFIFO	((u32)0x00000001<<13)	     	/*BIT[13], Slave has received a read command and some data exists in the TxFIFO so the slave issues a 
																		  TX_ABRT interrupt to flush old data in TxFIFO. Role is Slave-Transmitter*/
#define USI_I2C_ABRT_SLV_ARBLOST		((u32)0x00000001<<14)	     	/*BIT[14], Slave lost the bus while transmitting data to a remote master. i2c_tx_abrt_source [12] is set 
																		  at the same time. Role is Slave-Transmitter.*/
/** @} */

/**************************************************************************//**
 * @defgroup INTERRUPT_ALL_CLR
 * @{
 *****************************************************************************/
#define USI_INT_ALL_CLEAR				((u32)0x00000001)	     		/*BIT[0], Write "1" to this register to clear the combined interrupt, all individual interrupts, interrupt 
																		status register and raw interrupt status register. This bit does not clear hardware clearable 
																		interrupts but software dis-clearable interrupts, and the relate register (include txfifo_almost_empty, 
																		rxfifo_almost_full, uart_rxfifo_timeout)*/

/** @} */

/**************************************************************************//**
 * @defgroup INTERRUPT_STATUS_CLR
 * @{
 *****************************************************************************/
#define USI_TXFIFO_OVERFLOW_CLR		((u32)0x00000001<<1)	     	/*BIT[1], clear txfifo_overflow interrupt and related register*/
#define USI_TXFIFO_UNDERFLOW_CLR		((u32)0x00000001<<2)	     	/*BIT[2], clear txfifo_underflow interrupt and related register*/
#define USI_RXFIFO_OVERFLOW_CLR		((u32)0x00000001<<5)	     	/*BIT[5], clear rxfifo_overflow interrupt and related register*/
#define USI_RXFIFO_UNDERFLOW_CLR		((u32)0x00000001<<6)	     	/*BIT[6], clear rxfifo_underflow interrupt and related register*/
#define USI_RX_PARITY_ERR_CLR			((u32)0x00000001<<8)	     	/*BIT[8], clear uart_rx_parity_err interrupt and related register*/
#define USI_RX_STOP_ERR_CLR				((u32)0x00000001<<9)	     	/*BIT[9], clear uart_rx_stop_err interrupt and related register*/
#define USI_RX_BREAK_CLR				((u32)0x00000001<<10)	     	/*BIT[10], clear uart_rx_break interrupt and related register*/
#define USI_RX_BAUDMON_DONE_CLR		((u32)0x00000001<<12)	     	/*BIT[12], clear uart_rx_baudmon_done interrupt and related register*/
#define USI_TX_CTS_CHANGE_CLR			((u32)0x00000001<<13)	     	/*BIT[13], clear uart_tx_cts_change interrupt and related register*/
#define USI_SPI_RX_DATA_FRM_ERR_CLR	((u32)0x00000001<<16)	     	/*BIT[16], clear spi_rx_data_frame_err interrupt and related register*/
#define USI_I2C_RD_REQ_CLR				((u32)0x00000001<<20)	     	/*BIT[20], clear i2c_rd_req interrupt and related register*/
#define USI_I2C_TX_ABRT_CLR				((u32)0x00000001<<21)	     	/*BIT[21], clear i2c_tx_abrt interrupt and related register, this is also releases
																		the Tx FIFO from the flushed/reset state, allowing more writes to the Tx FIFO.*/
#define USI_I2C_RX_DONE_CLR				((u32)0x00000001<<22)	     	/*BIT[22], clear i2c_rx_done interrupt and related register*/
#define USI_I2C_ACTIVITY_CLR				((u32)0x00000001<<23)	     	/*BIT[23], clear i2c_activity interrupt and related register if the I2C is not active anymore. If the
																		I2C module is still active on the bus, the i2c_activity will continues to be set. It is automatically 
																		cleared by hardware if the module is disabled and if there is no further activity on the bus. */
#define USI_I2C_STOP_DET_CLR			((u32)0x00000001<<24)	     	/*BIT[24], clear i2c_stop_det interrupt and related register*/
#define USI_I2C_START_DET_CLR			((u32)0x00000001<<25)	     	/*BIT[25], clear i2c_start_det interrupt and related register*/
#define USI_I2C_GEN_CALL_CLR			((u32)0x00000001<<26)	     	/*BIT[26], clear i2c_gen_call interrupt and related register*/
#define USI_I2C_LP_WAKE_CLR				((u32)0x00000001<<27)	     	/*BIT[27], clear i2c_lp_wake interrupt and related register*/
#define USI_I2C_DMA_DONE_CLR			((u32)0x00000001<<28)	     	/*BIT[28], clear i2c_dma_done interrupt and related register*/

/** @} */

/**************************************************************************//**
 * @defgroup DEBUG_SEL
 * @{
 *****************************************************************************/
#define USI_DBG_SEL						((u32)0x0000001F)	     		/*BIT[4:0], USI DEBUG SELECT MASK */
#define USI_DBG_UART_CLKRST				((u32)0x00000000)	     		/*BIT[4:0]=0X00, dbg_uart_clkrst*/
#define USI_DBG_SPI_CLKRST				((u32)0x00000001)	     		/*BIT[4:0]=0X01, dbg_spi_clkrst*/
#define USI_DBG_I2C_CLKRST				((u32)0x00000002)	     		/*BIT[4:0]=0X02, dbg_i2c_clkrst*/
#define USI_DBG_TXFIFO_CLKRST			((u32)0x00000003)	     		/*BIT[4:0]=0X03, dbg_txfifo_clkrst*/
#define USI_DBG_RXFIFO_CLKRST			((u32)0x00000004)	     		/*BIT[4:0]=0X04, dbg_rxfifo_clkrst*/
#define USI_DBG_APBSLV					((u32)0x00000005)	     		/*BIT[4:0]=0X05, dbg_apbslv*/
#define USI_DBG_DMAIF					((u32)0x00000006)	     		/*BIT[4:0]=0X06, dbg_dmaif*/
#define USI_DBG_INTR						((u32)0x00000007)	     		/*BIT[4:0]=0X07, dbg_intr*/
#define USI_DBG_I2C_DMODE				((u32)0x00000008)	     		/*BIT[4:0]=0X08, dbg_i2c_dmode*/
#define USI_DBG_TRXFIFO					((u32)0x00000009)	     		/*BIT[4:0]=0X09, dbg_trxfifo*/
#define USI_DBG_UART_TXCTRL				((u32)0x0000000A)	     		/*BIT[4:0]=0X0A, dbg_uart_txctrl*/
#define USI_DBG_UART_RXCTRL				((u32)0x0000000B)	     		/*BIT[4:0]=0X0B, dbg_uart_rxctrl*/
#define USI_DBG_UART_RXMON				((u32)0x0000000C)	     		/*BIT[4:0]=0X0C, dbg_uart_rxmon*/
#define USI_DBG_UART_IRDA_DEC1			((u32)0x0000000D)	     		/*BIT[4:0]=0X0D, dbg_uart_irda_dec1*/
#define USI_DBG_UART_IRDA_DEC2			((u32)0x0000000E)	     		/*BIT[4:0]=0X0E, dbg_uart_irda_dec2*/
#define USI_DBG_UART_IRDA_DEC3			((u32)0x0000000F)	     		/*BIT[4:0]=0X0F, dbg_uart_irda_dec3*/
#define USI_DBG_UART_IRDA_DEC4			((u32)0x00000010)	     		/*BIT[4:0]=0X10, dbg_uart_irda_dec4*/
#define USI_DBG_UART_IRDA_ENC1			((u32)0x00000011)	     		/*BIT[4:0]=0X11, dbg_uart_irda_enc1*/
#define USI_DBG_UART_IRDA_ENC2			((u32)0x00000012)	     		/*BIT[4:0]=0X12, dbg_uart_irda_enc2*/
#define USI_DBG_UART_IRDA_ENC3			((u32)0x00000013)	     		/*BIT[4:0]=0X13, dbg_uart_irda_enc3*/
#define USI_DBG_UART_IRDA_ENC4			((u32)0x00000014)	     		/*BIT[4:0]=0X14, dbg_uart_irda_enc4*/
#define USI_DBG_UART_TOPCTRL			((u32)0x00000015)	     		/*BIT[4:0]=0X15, dbg_uart_topctrl*/
#define USI_DBG_SPI_MST_TXCTRL			((u32)0x00000016)	     		/*BIT[4:0]=0X16, dbg_spi_mst_txctrl*/
#define USI_DBG_SPI_MST_RXCTRL			((u32)0x00000017)	     		/*BIT[4:0]=0X17, dbg_spi_mst_rxctrl*/
#define USI_DBG_SPI_MST_CLKREQ			((u32)0x00000018)	     		/*BIT[4:0]=0X18, dbg_spi_mst_clkreq*/
#define USI_DBG_SPI_SLV_TXCTRL			((u32)0x00000019)	     		/*BIT[4:0]=0X19, dbg_spi_slv_txctrl*/
#define USI_DBG_SPI_SLV_RXCTRL			((u32)0x0000001A)	     		/*BIT[4:0]=0X1A, dbg_spi_slv_rxctrl*/
#define USI_DBG_I2C_FIFO					((u32)0x0000001B)	     		/*BIT[4:0]=0X1B, dbg_i2c_fifo*/
#define USI_DBG_I2C_BUS					((u32)0x0000001C)	     		/*BIT[4:0]=0X1C, dbg_i2c_bus*/
#define USI_DBG_I2C_INTR					((u32)0x0000001D)	     		/*BIT[4:0]=0X1D, dbg_i2c_intr*/

/** @} */

/**************************************************************************//**
 * @defgroup UART_CTRL
 * @{
 *****************************************************************************/
#define USI_UART_WORD_LEN_MD			((u32)0x00000001)	     		/*BIT[0], Word length selection , 0: data is 7 bit word length; 1: data is 8 bit word length.*/
#define USI_UART_STOP_BIT_MD			((u32)0x00000001<<1)    		/*BIT[1], This is bit specifies the number of Stop bits transmitted and received in each serial character. */
#define USI_UART_PARITY_ENABLE			((u32)0x00000001<<4)    		/*BIT[4], Parity Enable*/
#define USI_UART_PARITY_EVEN			((u32)0x00000001<<5)    		/*BIT[5], Even Parity select*/
#define USI_UART_STICK_PARITY_BIT		((u32)0x00000001<<6)    		/*BIT[6], Stick Parity bit.*/
#define USI_UART_LOOPBACK_MD_EN		((u32)0x00000001<<8)    		/*BIT[8], LoopBack mode*/
#define USI_UART_PIN_LOOPBACK_TEST_EN	((u32)0x00000001<<9)    		/*BIT[9], For uart IP txd/rxd/rts/cts pin loopback test*/
#define USI_UART_BREAK_CTRL				((u32)0x00000001<<12)    	/*BIT[12], Break Control bit*/

/** @} */

/**************************************************************************//**
 * @defgroup UART_IRDA_CTRL
 * @{
 *****************************************************************************/
#define USI_UART_IRDA_ENABLE			((u32)0x00000001)	     		/*BIT[0], UART IRDA ENABLE.*/
#define USI_UART_IRDA_TX_INV			((u32)0x00000001<<1)	     	/*BIT[1], Invert irda_tx_o when 1.*/
#define USI_UART_IRDA_RX_INV			((u32)0x00000001<<2)	     	/*BIT[2], Invert irda_rx_i when 1.*/

/** @} */


/**************************************************************************//**
 * @defgroup UART_IRDA_TX_PULSE_WD
 * @{
 *****************************************************************************/
#define USI_UART_IRDA_TX_PUL_LOW_BUND_VAL		((u32)0x00007FFF)		/*BIT[14:0], IrDA tx pulse low bound edge shift value*/
#define USI_UART_IRDA_TX_PUL_LOW_BUND_SHIFT		((u32)0x00000001 << 15)	/*BIT[15], IrDA tx pulse low bound edge shift direction*/
#define USI_UART_IRDA_TX_PUL_UP_BUND_VAL			((u32)0x00007FFF << 16)	/*BIT[30:16], IrDA tx pulse upper bound edge shift value*/
#define USI_UART_IRDA_TX_PUL_UP_BUND_SHIFT		((u32)0x00000001 << 31)	/*BIT[31], IrDA tx pulse upper bound edge shift direction*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_IRDA_RX_PULSE_WD
 * @{
 *****************************************************************************/
#define USI_UART_IRDA_RX_FILTER_ENABLE				((u32)0x00000001)		/*BIT[0], IrDA rx filter enable*/
#define USI_UART_IRDA_RX_FILTER_THRES				((u32)0x00007FFF << 1)	/*BIT[15:1], IrDA rx filter threshold field*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_TX_FRACTION_BAUD
 * @{
 *****************************************************************************/
#define USI_UART_TX_XFACTOR							((u32)0x000FFFFF)			/*BIT[19:0], USI UART Factor of Baud rate calculation*/
#define USI_UART_TX_XFACTOR_ADJ					((u32)0x000007FF<<20)		/*BIT[30:20], USI UART fractional factor of Baud rate calculation*/

/** @} */

/**************************************************************************//**
 * @defgroup UART_RX_BAUD_OSC
 * @{
 *****************************************************************************/
#define USI_UART_LPRX_OSC_CYCNUM_PERBIT			((u32)0x000FFFFF)		/*BIT[19:0], Average OSC clock cycle number of one bit, for integral baud rate(new rx path) osc clk
																				SW set the initial value, HW update it depend on the monitor result*/

/** @} */

/**************************************************************************//**
 * @defgroup UART_RX_BAUD_XTAL
 * @{
 *****************************************************************************/
#define USI_RUART_LPRX_XTAL_CYCNUM_PERBIT			((u32)0x000FFFFF)				/*BIT[19:0], Average fractional xtal clock cycle number of one bit, for integral baud rate(new rx path) xtal clk
																						SW set the initial value, HW update it depend on the monitor result*/

/** @} */

/**************************************************************************//**
 * @defgroup UART_RX_FRACTION_BAUD_CTRL
 * @{
 *****************************************************************************/
#define USI_UART_RX_XFACTOR_ADJ					((u32)0x000007FF)		/*BIT[10:0], USI UART RX fractional factor of Baud rate calculation*/

/** @} */

/**************************************************************************//**
 * @defgroup UART_BAUD_MON_CTRL
 * @{
 *****************************************************************************/
#define USI_UART_LP_RX_MON_ENABLE				((u32)0x00000001)     		/*BIT[0], 0x01, Enable low power rx monitor function*/
#define USI_UART_LP_RX_OSC_UPD_IN_XTAL		((u32)0x00000001 << 1)   	/*BIT[1], Control bit for osc monitor parameter update */
#define USI_UART_LP_RX_BIT_NUM_THRES			((u32)0x000000FF << 8)    	/*BIT[15:8], Bit Number threshold of one monitor period*/

/** @} */

/**************************************************************************//**
 * @defgroup UART_BAUD_MON_STATUS0
 * @{
 *****************************************************************************/
#define USI_UART_LP_RX_BAUDMON_VALID			((u32)0x00000001)    		/*BIT[0], Monitor VALID status*/
#define USI_UART_LP_RX_MON_TOTAL_BITS			((u32)0x000000FF << 8)    	/*BIT[15:8], Actualy monitor bit number */

/** @} */

/**************************************************************************//**
 * @defgroup UART_BAUD_MON_STATUS1
 * @{
 *****************************************************************************/
#define USI_UART_RX_BAUDMON_TOTAL_CYCLE		((u32)0x0FFFFFFF)    		/*BIT[27:0], actually monitored clk cycle*/

/** @} */

/**************************************************************************//**
 * @defgroup UART_FLOW_CTRL
 * @{
 *****************************************************************************/
#define USI_UART_RX_RTS							((u32)0x00000001)    		/*BIT[0], Request to Send (RTS) signal control*/
#define USI_UART_TX_CTS							((u32)0x00000001<<1)    		/*BIT[1], Complement of the CTS input or equals to RTS in loopback mode.*/
#define USI_UART_AUTO_FLOW_EN					((u32)0x00000001<<4)    		/*BIT[4], AutoFlow Enable .*/
#define USI_UART_RX_HOLD_THRD					((u32)0x0000007F<<8)    		/*BIT[14:8], when uart_auto_flowctrl_en = 1 and Rx FIFO unpushed space >= uart_rx_hold_thres,
																		    hardware will auto hold Rx path RTS (send uart_rx_rts_oe output to 1).*/


/** @} */

/**************************************************************************//**
 * @defgroup UART_RXFIFO_TO_TH
 * @{
 *****************************************************************************/
#define USI_UART_RXFIFO_TO_TH					((u32)0x0000FFFF)    		/*BIT[15:0], UART RXFIFO timeout threshold*/


/** @} */

/**************************************************************************//**
 * @defgroup UART_RXDMA_FLOW_CTRL
 * @{
 *****************************************************************************/
#define USI_UART_RXDMA_RXDMA_OWNER			((u32)0x00000001)    		/*BIT[0], UART RXFIFO FLOW OWNER*/
#define USI_UART_RXDMA_DUMMY_DATA				((u32)0x0000FF00)    		/*BIT[15:8], UART RXDMA dummy data*/
#define USI_UART_RXDMA_DUMMY_FLAG				((u32)0x00010000)    		/*BIT[16], UART RXDMA dummy flag*/

/** @} */

/**************************************************************************//**
 * @defgroup SPI_CTRL
 * @{
 *****************************************************************************/
#define USI_SPI_DAT_FRM_SIZE					((u32)0x0000000F)    		/*BIT[3:0], data frame size*/
#define USI_SPI_MASTER_MODE						((u32)0x00000001<<4)    		/*BIT[4], spi master mode.*/
#define USI_SPI_SS_TOG_PHASE					((u32)0x00000001<<5)    		/*BIT[5], only used by master mode, RSVD for slave mode. .*/
#define USI_SPI_CPH								((u32)0x00000001<<6)    		/*BIT[6], clock phase.*/
#define USI_SPI_CPOL								((u32)0x00000001<<7)    		/*BIT[7], clock polarity.*/
#define USI_SPI_RX_SAMPLE_DLY					((u32)0x000000FF<<8)    		/*BIT[15:8], Receive Data (rxd) Sample Delay*/
#define USI_SPI_MST_BAUD						((u32)0x0000FFFF<<16)  		/*BIT[15:8], spi_mst_clk Divider*/

/** @} */

/**************************************************************************//**
 * @defgroup SPI_TRANS_EN
 * @{
 *****************************************************************************/
#define USI_SPI_TX_ENABLE						((u32)0x00000001)    		/*BIT[0], SPI TX path enable/disable*/
#define USI_SPI_RX_ENABLE						((u32)0x00000001<<1)    		/*BIT[1], SPI RX path enable/disable*/
#define USI_SPI_RXONLY_NUM						((u32)0x0000FFFF<<16)    	/*BIT[16:31], Valid only in SPI Master Rx only (Tx disable) mode, otherwise has no affect.
																			Set this register and then write a dummy Tx data into Tx FIFO, SPI Master hardware will
																			automatically start Rx spi_rxonly_num+1 frames data from SPI Slave*/
/** @} */

/**************************************************************************//**
 * @defgroup SPI_TRANS_STATUS
 * @{
 *****************************************************************************/
#define USI_SPI_ACTIVITY							((u32)0x00000001)    		/*BIT[0], SPI activity/idle status*/
#define USI_SPI_MST_TX_ACTIVITY					((u32)0x00000001<<1)    		/*BIT[1], SPI MASTER TX PATH activity/idle status*/
#define USI_SPI_MST_RX_ACTIVITY					((u32)0x00000001<<2)    		/*BIT[2], SPI MASTER RX PATH activity/idle status*/
#define USI_SPI_SLV_TX_ACTIVITY					((u32)0x00000001<<3)    		/*BIT[3], SPI SLAVE TX PATH activity/idle status*/
#define USI_SPI_SLV_RX_ACTIVITY					((u32)0x00000001<<4)    		/*BIT[4], SPI SLAVE RX PATH activity/idle status*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_CTRL
 * @{
 *****************************************************************************/
#define USI_I2C_MODE							((u32)0x00000001)    		/*BIT[0], I2C master mode*/
#define USI_I2C_RESTART_EN						((u32)0x00000001<<1)    		/*BIT[1], Determine whether RESTART conditions may be sent when acting as a master.*/
#define USI_I2C_SPEED							((u32)0x00000003<<2)    		/*BIT[3:2], this setting is relevant only if operating in I2C master mode.*/
#define USI_I2C_HS_MAR							((u32)0x00000007<<4)    		/*BIT[6:4], the value of the I2C HS(high speed) mode master mode. */
#define USI_I2C_SLV_DISABLE						((u32)0x00000001<<8)    		/*BIT[8], This bit controls whether I2C has its slave (7-bit or 10-bit address) disabled*/
#define USI_I2C_10BITADDR_SLAVE					((u32)0x00000001<<9)    		/*BIT[9], When acting as a slave, this bit controls whether the I2C responds to 7- or 10-bit addresses.*/
#define USI_I2C_SAR								((u32)0x000003FF<<12)  		/*BIT[21:12], slave address when the I2C is operating as slave*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_SS_SCL_CNT
 * @{
 *****************************************************************************/
#define USI_I2C_SS_SCL_HCNT						((u32)0x0000FFFF)    		/*BIT[15:0], sets the SCL clock high-period count for standard speed. */
#define USI_I2C_SS_SCL_LCNT						((u32)0x0000FFFF<<16)    	/*BIT[31:16], sets the SCL clock low-period count for standard speed.   */

/** @} */

/**************************************************************************//**
 * @defgroup I2C_FS_SCL_CNT
 * @{
 *****************************************************************************/
#define USI_I2C_FS_SCL_HCNT						((u32)0x0000FFFF)    		/*BIT[15:0], sets the SCL clock high-period count for fast speed. */
#define USI_I2C_FS_SCL_LCNT						((u32)0x0000FFFF<<16)    	/*BIT[31:16], This register sets the SCL clock low-period count for fast speed.*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_FS_SCL_CNT
 * @{
 *****************************************************************************/
#define USI_I2C_HS_SCL_HCNT						((u32)0x0000FFFF)    		/*BIT[15:0], This register sets the SCL clock high-period count for high speed. */
#define USI_I2C_HS_SCL_LCNT						((u32)0x0000FFFF<<16)    	/*BIT[31:16], This register sets the SCL clock low-period count for high speed. */

/** @} */

/**************************************************************************//**
 * @defgroup I2C_SDA_TIMING
 * @{
 *****************************************************************************/
#define USI_I2C_SDA_SETUP						((u32)0x000000FF)    		/*BIT[7:0], Set the required SDA setup time in units of i2c_clk period. */
#define USI_I2C_SDA_HOLD						((u32)0x0000FFFF<<16)    	/*BIT[31:16], This register controls the amount of hold time on the SDA signal after 
																			a negative edge of SCL in both master and slave mode, in units of i2c_clk period.*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_DIG_FILTER
 * @{
 *****************************************************************************/
#define USI_I2C_DIG_FLTR_DEG					((u32)0x0000000F)    		/*BIT[3:0], DIG_FLTR_DEG is to define frequency range of filter */
#define USI_I2C_DIG_FLTR_SEL						((u32)0x00000001<<4)    		/*BIT[4], Enable filter*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_SLV_ACK_CTRL
 * @{
 *****************************************************************************/
#define USI_I2C_SLV_DATA_NACK_ONLY				((u32)0x00000001)    		/*BIT[0], Generate NACK*/
#define USI_I2C_ACK_GENERAL_CALL				((u32)0x00000001<<1)    		/*BIT[1], This register controls whether I2C responds with a ACK or NACK when it receives an I2C General Call address.*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_ENABLE
 * @{
 *****************************************************************************/
#define USI_I2C_ENABLE							((u32)0x00000001)    		/*BIT[0], Control whether the I2C is enabled.*/
#define USI_I2C_FORCE							((u32)0x00000001<<1)    		/*BIT[1], copy from I2C IP, used by i2c master*/
#define USI_I2C_SLEEP							((u32)0x00000001<<4)    		/*BIT[4], I2C clock contro*/
#define USI_I2C_CLK_IS_GATED					((u32)0x00000001<<8)    		/*BIT[8], Clock-gated I2C clock domain for address matching interrupts wake up.*/
#define USI_I2C_OUT_SMP_DLY					((u32)0x00000007<<9)    		/*BIT[11:9], I2C output delay sample*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_TAR
 * @{
 *****************************************************************************/
#define USI_I2C_IC_TAR							((u32)0x000003FF)    		/*BIT[9:0], target address for any master transaction.*/
#define USI_I2C_GC_OR_START						((u32)0x00000001<<10)    	/*BIT[10], If i2c_special is set to 1, then this bit indicates whether a General Call or START BYTE 
																		 command is to be performed by I2C. 0: General Call ¨C after issuing a General Call, only write may be performed. 1: START BYTE*/
#define USI_I2C_SPECIAL							((u32)0x00000001<<11)  		/*BIT[11], This bit indicates whether software performs a General Call or START BYTE command.*/
#define USI_I2C_10BITADDR_MST					((u32)0x00000001<<12)  		/*BIT[12], Control whether I2C starts its transfers in 7- or 10-bit addressing mode when acting as a master. */

/** @} */

/**************************************************************************//**
 * @defgroup I2C_DMA_CMD
 * @{
 *****************************************************************************/
#define USI_I2C_DMODE_ENABLE					((u32)0x00000001)    		/*BIT[0], 1: Set to enable dma mode, clear when transfer is done*/
#define USI_I2C_DMA_MODE						((u32)0x00000003<<1)    		/*BIT[2:1], DMA mode MASK*/
#define USI_I2C_DMA_LEGACY_MODE				((u32)0x00000000<<1)    		/*BIT[2:1]=0, DWC DMA legacy mode*/
#define USI_I2C_DMA_CTRL_REG_MODE				((u32)0x00000001<<1)    		/*BIT[2:1]=1, DMA with control register*/
#define USI_I2C_DMA_DESC_MODE					((u32)0x00000002<<1)    		/*BIT[2:1]=2, DMA with transfer descriptor*/
#define USI_I2C_DMODE_READ_CMD				((u32)0x00000001<<4)    		/*BIT[4], This bit controls a read operation is performed in DMA mode.*/
#define USI_I2C_DMODE_WRITE_CMD				((u32)0x00000000<<4)    		/*BIT[4], This bit controls a write operation is performed in DMA mode.*/
#define USI_I2C_DMODE_STOP						((u32)0x00000001<<5)    		/*BIT[5], This bit controls whether a STOP is issued after the byte is sent or received in DMA mode.*/
#define USI_I2C_DMODE_RESTART					((u32)0x00000001<<6)    		/*BIT[6], This bit controls whether a RESTART is issued after the byte is sent or received in DMA mode.*/
#define USI_I2C_DMA_DAT_LEN						((u32)0x0000FFFF<<16)    	/*BIT[31:16], DMA transfer data length*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_STATUS
 * @{
 *****************************************************************************/
#define USI_I2C_ENABLE_COPY						((u32)0x00000001)    		/*BIT[0], i2c_enable copy to here*/
#define USI_I2C_ACTIVITY_COMB					((u32)0x00000001<<4)    		/*BIT[4], I2C Activity Status, i2c_mst_activity or i2c_slv_activity is "1"*/
#define USI_I2C_MST_ACTIVITY						((u32)0x00000001<<5)    		/*BIT[5], Master FSM Activity Status. When the Master FSM is not in the IDLE state, this bit is set.*/
#define USI_I2C_SLV_ACTIVITY						((u32)0x00000001<<6)    		/*BIT[6], Slave FSM Activity Status. When the Slave FSM is not in the IDLE state, this bit is set.*/
#define USI_I2C_BUS_ACTIVITY						((u32)0x00000001<<7)    		/*BIT[7], BUS FSM Activity Status. When the BUS FSM is not in the IDLE state, this bit is set.*/
#define USI_I2C_SLV_DIS_WHL_BUSY				((u32)0x00000001<<8)    		/*BIT[8], Slave Disabled While Busy (Transmit, Receive)*/
#define USI_I2C_DMA_DIS_WHL_BUSY				((u32)0x00000003<<9)    		/*BIT[10:9], DMA disable While Busy mask*/
#define USI_I2C_DMA_DIS_WHL_BUSY_DMA			((u32)0x00000001<<9)    		/*BIT[9], I2C is disable while busy in DMA mode*/
#define USI_I2C_DMA_DIS_WHL_BUSY_DESC			((u32)0x00000002<<9)    		/*BIT[10], I2C is disable while busy in Descriptor mode*/
#define USI_I2C_DMA_TRANS_LEN					((u32)0x0000FFFF<<16)    	/*BIT[31:16], DMA mode transfer bytes*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_ANA_FILTER1
 * @{
 *****************************************************************************/
#define USI_I2C_ANA_DATA_DEG_RL				((u32)0x000FFFFF)    		/*BIT[19:0]*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_ANA_FILTER2
 * @{
 *****************************************************************************/
#define USI_I2C_ANA_DATA_DEG_RM				((u32)0x000FFFFF)    		/*BIT[19:0]*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_ANA_FILTER3
 * @{
 *****************************************************************************/
#define USI_I2C_ANA_CLK_DEG_RL					((u32)0x000FFFFF)		    	/*BIT[19:0]*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_ANA_FILTER4
 * @{
 *****************************************************************************/
#define USI_I2C_ANA_CLK_DEG_RM					((u32)0x000FFFFF)		    	/*BIT[19:0]*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_ANA_FILTER5
 * @{
 *****************************************************************************/
#define USI_I2C_ANA_DATA_DEG_CL				((u32)0x000FFFFF)		    	/*BIT[19:0]*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_ANA_FILTER6
 * @{
 *****************************************************************************/
#define USI_I2C_ANA_DATA_DEG_CM				((u32)0x0000001F)		    	/*BIT[4:0]*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_ANA_FILTER7
 * @{
 *****************************************************************************/
#define USI_I2C_ANA_CLK_DEG_CL				((u32)0x000FFFFF)		    		/*BIT[19:0]*/

/** @} */

/**************************************************************************//**
 * @defgroup I2C_ANA_FILTER8
 * @{
 *****************************************************************************/
#define USI_I2C_ANA_CLK_DEG_CM				((u32)0x0000001F)		    		/*BIT[4:0]*/

/** @} */

/** @} */

/**
  * @}
  */

/**
  * @}
  */ 

/* Other Definitions --------------------------------------------------------*/

/*----------------------global---------------------*/
typedef struct
{
	USI_TypeDef* USIx;
	u32 Tx_HandshakeInterface;
	u32 Rx_HandshakeInterface;
	IRQn_Type	IrqNum;
} USI_DevTable;

extern const USI_DevTable USI_DEV_TABLE[1];

#define MAX_USI_INDEX			(1)

#endif
/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/

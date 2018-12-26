/**
 *******************************************************************************
 * @file    uart.h
 * @author  Robert
 * @version V1.0.0
 * @date    26-June-2012
 * @brief   Fast Universal Asynchronous Receiver Transmitter
 * @maintainer Jerry
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define	CFG_BUART_EXFIFO_ADDR	(0x20018000)
#define	CFG_BUART_EXFIFO_SIZE	(32 * 1024)


/**
 * @brief
 *   The FuartSend() function send data from buffer referred by Buf in byte for the number of
 * BufLen,the Opt argument is reserved currently.The CALLER must be wait until the transmittion
 * is completion before return synchorously.
 *
 * @param	Buf		buffer where the transmitt data come from
 * @param	BufLen	buffer length in byte
 * @param	Opt		reserved
 * @return
 *   Upon successful completion,FuartSend() function shall return the actual transferring bytes.
 * Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   EBUSY Fuart is not available,please retry
 * @note
 */
int32_t FuartSend(uint8_t* Buf, uint32_t BufLen);

/**
 * @brief
 *   The FuartRecv() function receive incoming data into buffer referred by Buf in byte for the
 * number of BufLen,if the data is not available or not enough for requirement,waiting for data
 * comming for maximum timeout time indicated by Wait in millisecond,and if Wait is 0xffffFFFF,
 * it indicates that the CALLER will be wait for ever till data coming & enough data.
 *
 * @param	Buf		buffer where the received data put into
 * @param	BufLen	buffer length in byte
 * @param	Wait	maximum timeout if no or not enought quanity data than expecting
 * @return
 *   Upon successful completion,FuartRecv() function shall return the actual receiving data in
 * byte before timeout occurs.The actual length may be less than the request if timeout occurs.
 * Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   ETIMEOUT timeout occurs.
 * @note
 */
int32_t FuartRecv(uint8_t* Buf, uint32_t BufLen, uint32_t Wait);

/**
 * @brief
 *   The FuartRecvByte function receive incoming one byte into buf, if the data is not available
 * returns -1.
 *
 * @param	Buf		buffer where the received data put into
 * @return
 *   Upon successful completion,FuartRecvByte() function shall return 1 or (-1) for no data found.
 *
 * ERRORS
 *   None.
 * @note
 */
int32_t FuartRecvByte(uint8_t* Buf);

//FUART+BUART IO control command
enum
{
	//get F/BUART baude rate,ARG2=0
	UART_IOCTL_BAUDRATE_GET = 1,
	//set F/BUART buade rate,ARG2=BAUDRATE,{1,200-3,000,000/bps}
	UART_IOCTL_BAUDRATE_SET,		

	//get F/BUART RX interrupt trigger level,FIFO depth{1-4} in bytes repensented by{0-3},ARG2=0
	UART_IOCTL_RXFIFO_TRGRDEP_GET,
	//set F/BUART RX interrupt trigger level,FIFO depth{1-4} in bytes repensented by{0-3},ARG2={0-3} 
	UART_IOCTL_RXFIFO_TRGRDEP_SET,

	//clear F/BUART RX interrnal FIFO forcely,ARG2=0
	UART_IOCTL_RXFIFO_CLR,
    
	//dis/enable F/BUART RX receiption,ARG2{1,0},1 for enable,0 for disable    
	UART_IOCTL_DISENRX_SET,
	//dis/enable F/BUART TX receiption,ARG2{1,0},1 for enable,0 for disable
	UART_IOCTL_DISENTX_SET,

	//dis/enable F/BUART RX interrupt,ARG2{1,0},1 for enable,0 for disable
	UART_IOCTL_RXINT_SET,
	//dis/enable F/BUART TX interrupt,ARG2{1,0},1 for enable,0 for disable
	UART_IOCTL_TXINT_SET,

	//clean F/BUART RX data or error interrupt status
	UART_IOCTL_RXINT_CLR,
	//clean F/BUART TX data interrupt status
	UART_IOCTL_TXINT_CLR,

	//get F/BUART RX status register,5`bxxxxx,{frame_err,parity_err,overrun,dat_err,dat_int}
	UART_IOCTL_RXSTAT_GET,
	//get F/BUUART TX status register,2`bxx,{tx_busy,dat_int}
	UART_IOCTL_TXSTAT_GET,


	//*******************************for BT UART (BUART mode) only***************************
	//get BUART RX FIFO depth in bytes,ARG2=0
	BUART_IOCTL_RXFIFO_DEPTH_GET,
	//set BUART RX FIFO depth in bytes,ARG2={0-0x7FFF}
	BUART_IOCTL_RXFIFO_DEPTH_SET,	

	//get BUART RX FIFO interrupt trigger depth,ARG2=0
	BUART_IOCTL_RXFIFO_TRGR_DEPTH_GET,
	//set BUART RX FIFO interrupt trigger depth,ARG2={0-0x7FFF}
	BUART_IOCTL_RXFIFO_TRGR_DEPTH_SET,

	//get BUART TX FIFO depth in bytes,ARG2=0
	BUART_IOCTL_TXFIFO_DEPTH_GET,
	//set BUART TX FIFO depth in bytes,ARG2={0-0x7FFF}
	BUART_IOCTL_TXFIFO_DEPTH_SET,

	//issue set RX FIFO depth command when change FIFO depth dynamically,ARG2=0
	BUART_IOCTL_RXFIFO_SETCMD,
	//issue set TX FIFO depth command when change FIFO depth dynamically,ARG2=0
	BUART_IOCTL_TXFIFO_SETCMD,

	//get BUART data length in RX FIFO,ARG2=0
	BUART_IOCTL_RXFIFO_DATLEN_GET,
	//get BUART data length in TX FIFO,ARG2=0
	BUART_IOCTL_TXFIFO_FREELEN_GET,

	//set/unset RX RTS flow control,ARG2={0 for unset,1 for low active,2 for high active)
	BUART_IOCTL_RXRTS_FLOWCTL_SET,
	//set/unset TX CTS flow control,ARG2={0 for unset,1 for low active,2 for high active)
	BUART_IOCTL_TXCTS_FLOWCTL_SET,

	//disable/enable BUART mode or not,ARG2 = {0 for disable, 1 for enable(default))
	BUART_IOCTL_BTMODE_SET,		

	//clean BUART RX FIFO forcely, ARG2 = 0
	BUART_IOCTL_RXFIFO_CLR,
	
	//get F/BUART RX FIFO status register,2`bxx,{rx_fifo_full,rx_fifo_empty}
	UART_IOCTL_RXFIFO_STAT_GET,
	//get F/BUART TX FIFO status register,2`bxx,{tx_fifo_full,tx_fifo_empty}
	UART_IOCTL_TXFIFO_STAT_GET,
};


/**
 * @brief
 *   The FuartIOctl() function control uart action including but not limited to configration,
 * interrupt and other misc settings by the command of Cmd with parameter referred by Opt.
 *
 * @param	Cmd	I/O control command
 * @param	Opt	the control command argument if the command require
 *
 * @return
 *   Upon successful completion,FuartIOctl() function shall return the result like
 * UART_IOCTL_XXXX_GET(such as UART_IOCTL_BAUDRATE_GET) or return zero like
 * UART_IOCTL_XXXX_SET(such as UART_IOCTL_BAUDRATE_SET).
 * Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   ENOSYS The command is not available currently.
 *   EINVAL Invalid parameter,especially for UART_IOCTL_XXXX_SET.
 * @note
 */
int32_t FuartIOctl(int32_t Cmd, uint32_t Opt);


/**
 * @brief
 *   The FuartClrRxInt() function clears the Fuart Rx interrupt.
 *
 * @param	None.
 *
 * @return
 *   Upon successful completion,this function shall return a negative error
 *   code if error found.
 *
 * ERRORS
 *   ENOSYS The command is not available currently.
 *   EINVAL Invalid parameter,especially for UART_IOCTL_XXXX_SET.
 * @note
 */
/*int32_t FuartClrRxInt(void);*/

/**
* @brief
*   The FuartClrTxInt() function clears the Fuart Tx interrupt.
*
* @param	None.
*
* @return
*   Upon successful completion,this function shall return a negative error
*   code if error found.
*
* ERRORS
*   ENOSYS The command is not available currently.
*   EINVAL Invalid parameter,especially for UART_IOCTL_XXXX_SET.
* @note
*/
/*int32_t FuartClrTxInt(void);*/

#define FuartClrRxInt() FuartIOctl(UART_IOCTL_RXINT_CLR, 0)
#define FuartClrTxInt() FuartIOctl(UART_IOCTL_TXINT_CLR, 0)

/**
 * @brief
 *   The IsFuartRxIntSrc() function returns whether is the Rx interrupt found.
 *
 * @param	None.
 *
 * @return
 *   returns TRUE for Rx interrupt found, otherwise returns False.
 *
 * @note
 */
bool IsFuartRxIntSrc(void);

/**
 * @brief
 *   The IsFuartTxIntSrc() function returns whether is the Tx interrupt found.
 *
 * @param	None.
 *
 * @return
 *   returns TRUE for Rx interrupt found, otherwise returns False.
 *
 * @note
 */
bool IsFuartTxIntSrc(void);


/**
 * @brief
 *   The FuartStartByteSend() function sends one byte of data if available with
 *   Tx interrupt notification.
 *
 * @param	Data: the data to be send.
 *
 * @return
 *   returns TRUE for OK, otherwise returns False.
 *
 * @note
 */
bool FuartStartByteSend(uint8_t Data);

/**
 * @brief
 *   The FuartInit() function initialize the iosel'th referred by IoSel uart's basic configuration
 *   such as baudrate referred to BaudRate and frame length (5~8 bits) referred
 *   by DatumBits and weather or not need to parity(0 for NO, 1 for ODD, 2 for EVEN) referred by
 *   Parity and one frame stop bits (1 or 2).
 *
 * @param	BaudRate	96MHz主频下，取值（1200bps~3000000bps），48MHz主频下，最大1500000bps
 * @param	DatumBits	数据位数，取值（5,6,7,8）
 * @param	Parity		奇偶校验，取值（0：无校验，1：奇校验，2：偶校验）
 * @param	StopBits	停止位，取值（1：1bit停止位，2：2bit停止位）
 * @return
 *   Upon successful completion,BaudRate() function shall return 0.
 *   Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   EINVAL	Invalid parmeter,for example,BaudRate is too larger or small and others
 * @note
 */
int32_t FuartInit(uint32_t BaudRate, uint8_t DatumBits, uint8_t Parity, uint8_t StopBits);


//BUART API
/**
 * @brief
 *   The BuartSend() function send data from buffer referred by Buf in byte for the number of
 * BufLen,the Otp argument is reserved currently.The CALLER must be wait until the transmittion
 * is completion before return synchorously.
 *
 * @param	Buf		buffer where the transmitt data come from
 * @param	BufLen	buffer length in byte
 *
 * @return
 *   Upon successful completion,BuartSend() function shall return the actual transferring bytes.
 * Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   EBUSY Buart is not available,please retry
 * @note
 */
int32_t BuartSend(uint8_t* Buf, uint32_t BufLen);

/**
 * @brief
 *   The BuartRecv() function receive incoming data into buffer referred by Buf in byte for the
 * number of BufLen,if the data is not available or not enough for requirement,waiting for data
 * comming for maximum timeout time indicated by Wait in millisecond,and if Wait is 0xffffFFFF,
 * it indicates that the CALLER will be wait for ever till data coming & enough data.
 *
 * @param	Buf		buffer where the received data put into
 * @param	BufLen	buffer length in byte
 * @param	Wait	maximum timeout if no or not enought quanity data than expecting
 * @return

 *   Upon successful completion,BuartRecv() function shall return the actual receiving data in
 * byte before timeout occurs.The actual length may be less than the request if timeout occurs.
 * Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   ETIMEOUT timeout occurs.
 * @note
 */
int32_t BuartRecv(uint8_t* Buf, uint32_t BufLen, uint32_t Wait);

/**
 * @brief
 *   The BuartIOctl() function control BTUART action including but not limited to configration,
 * interrupt and other misc settings by the command of Cmd with parameter referred by Arg.
 *
 * @param	Cmd	I/O control command
 * @param	Arg	the control command's argument if the command require
 *
 * @return
 *   Upon successful completion,BuartIOctl() function shall return the result like
 * UART_IOCTL_XXXX_GET(such as UART_IOCTL_BAUDRATE_GET) or return zero like
 * UART_IOCTL_XXXX_SET(such as UART_IOCTL_BAUDRATE_SET).
 *	Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   ENOSYS The command is not available currently.
 *   EINVAL Invalid parameter,especially for UART_IOCTL_XXXX_SET.
 * @note
 */
int32_t BuartIOctl(int32_t Cmd, uint32_t Arg);

/**
 * @brief
 *   The BuartExFifoInit() function initialize the uart as BT-UART using the external memory as its
 * cyclic buffer called cyclic FIFO for RX/TX buffer.The external memory starts from PMEM offset
 * referred by FifoStartAddr from 0 to 0x3000,and it including two parts,one for RX FIFO with size
 * specified by RxFifoDepth,and the other for TX FIFO with size specified by TxFifoDepth.In order
 * to guarantee data statable & overflow,BT-UART support flow ctrol specified by FlowCtl,for
 * transfer & reception in duplx mode.For RX,if the incoming data length is greater than
 * RxFifoTrgDep + 1,it will trigger a interrupt if the associated interrupt is under enable.see more
 * details from IOCTL command.
 *
 * @param	FifoStartAddr	external FIFO offset address from PMEMORY
 * @param	RxFifoDepth		RX FIFO depth
 * @param	TxFifoDepth		TX FIFO depth
 * @param	RxFifoTrgDep	RX FIFO trigger interrupt threhold depth
 * @return
 *   Upon successful completion,BuartExFifoInit() function shall return 0.
 *   Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   EINVAL	Invalid parmeter,for example,RxFifoDepth + TxFifoDepth > PMEM size,
 *          or RxFifoTrgDep > RxFifoDepth and so on
 */
int32_t BuartExFifoInit(uint16_t FifoStartAddr, uint16_t RxFifoDepth, uint16_t TxFifoDepth, uint16_t RxFifoTrgDep);


/**
 * @brief
 *   The BuartInit() function initialize the uart's basic configuration such as baudrate referred
 * to BaudRate (1200~3000000 bps)and frame length (5~8 bits) referred by DatumBits and weather or
 * not need to parity(0 for NO, 1 for ODD, 2 for EVEN) referred by Parity and one frame stop bits
 * (1 or 2), the parameter IoSel is used as I/O multiple reuse, the last parameter is CTS/RTS flow
 * control reuse group selection.
 *
 * @param	BaudRate	96MHz主频下，取值（1200bps~3000000bps），48MHz主频下，最大1500000bps
 * @param	DatumBits	数据位数，取值（5,6,7,8）
 * @param	Parity		奇偶校验，取值（0：无校验，1：奇校验，2：偶校验）
 * @param	StopBits	停止位，取值（1：1bit停止位，2：2bit停止位）
 *
 * @return
 *   Upon successful completion,BaudRate() function shall return 0.
 *   Otherwise,a negative error code will be returned.
 * ERRORS
 *   EINVAL	Invalid parmeter,for example,BaudRate is too larger or small and others
 * @note
 */
int32_t BuartInit(uint32_t BaudRate, uint8_t DatumBits, uint8_t Parity, uint8_t StopBits);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__UART_H__

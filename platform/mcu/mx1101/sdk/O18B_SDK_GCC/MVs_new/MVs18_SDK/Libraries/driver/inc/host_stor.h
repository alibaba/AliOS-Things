/**
 *****************************************************************************
 * @file     host_stor.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    host mass-storage module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __HOST_STOR_H__
#define	__HOST_STOR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define		MAX_USB_SN_LEN		50


typedef struct _USB_INFO
{
	uint16_t	VID;
	uint16_t	PID;
	uint8_t	LenSN;	//length of SN
	uint8_t  	SN[MAX_USB_SN_LEN];	//Serial number, end with "\0\0"

} USB_INFO;
extern USB_INFO		gLibUsbInfo;

//遇到特定VID的设备，则不支持当作U盘用
//默认gMassStorInvalidVid为0x0000，将所有设备都当作U盘
//将gMassStorInvalidVid设置为0x05AC，这可以禁止将APPLE设备当作U盘来播放
extern uint16_t	gMassStorInvalidVid;


/**
 * @brief  usb host device init
 * @param  NONE
 * @return 1-成功，0-失败
 */
bool HostStorInit(void);

/**
 * @brief  read blocks
 * @param  BlockNum block number
 * @param  Buf 读缓冲区指针
 * @param  BlockCnt 要读的block个数
 * @return 1-成功，0-失败
 */
bool HostStorReadBlock(uint32_t BlockNum, void* Buf, uint8_t BlockCnt);

/**
 * @brief  write blocks
 * @param  BlockNum block number
 * @param  Buf 写缓冲区指针
 * @param  BlockCnt 要写的block个数
 * @return 1-成功，0-失败
 */
bool HostStorWriteBlock(uint32_t BlockNum, void* Buf, uint8_t BlockCnt);

/**
 * @brief  get storage device block size
 * @param  NONE
 * @return block size
 */
uint16_t HostStorGetBlockSize(void);

/**
 * @brief  get storage device last lba number
 * @param  NONE
 * @return last block number
 */
uint32_t HostStorGetLastLBA(void);


void SetDriverTerminateFuc(TerminateFunc func);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

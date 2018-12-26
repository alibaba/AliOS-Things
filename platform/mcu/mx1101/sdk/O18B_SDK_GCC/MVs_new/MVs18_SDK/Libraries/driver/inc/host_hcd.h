/**
 *****************************************************************************
 * @file     host_hcd.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    usb host module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __HOST_HCD_H__
#define	__HOST_HCD_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * Max packet size. Fixed, user should not modify.
 */
#define	HOST_FS_CONTROL_MPS		64
#define	HOST_FS_INT_IN_MPS		64
#define	HOST_FS_BULK_IN_MPS		64
#define	HOST_FS_BULK_OUT_MPS	64
#define	HOST_FS_ISO_OUT_MPS		192
#define	HOST_FS_ISO_IN_MPS		768


/**
 * Endpoint number. Fixed, user should not modify.
 * 作为HOST时，描述的端点号跟物理端点号可以不相同
 */
#define	HOST_CONTROL_EP			0
#define	HOST_BULK_IN_EP			1
#define	HOST_BULK_OUT_EP		2
#define	HOST_INT_IN_EP			3
#define	HOST_ISO_OUT_EP			4
#define	HOST_ISO_IN_EP			5


/**
 * pipe type
 */
#define PIPE_TYPE_CONTROL		0
#define PIPE_TYPE_ISO_IN		1
#define PIPE_TYPE_ISO_OUT		2
#define PIPE_TYPE_INT_IN		3
#define PIPE_TYPE_INT_OUT		4
#define PIPE_TYPE_BULK_IN		5
#define PIPE_TYPE_BULK_OUT		6


/**
 * define OTG endpoint descriptor, used for configuration endpoint information
 */
typedef struct _PIPE_INFO
{
	uint8_t	EndpointNum	;	//endpoint number
	uint8_t	PipeType;		//endpoint type: ctrl, bulk, interrupt, isochronous
	uint16_t	MaxPacketSize;	//max packet size

} PIPE_INFO;


/**
 * @brief  usb host init
 * @param  NONE
 * @return 1-成功，0-失败
 */
bool UsbHostOpen(void);

/**
 * @brief  选择当前USB端口
 * @param  NONE
 * @return NONE
 */
void UsbSetCurrentPort(uint8_t PortNum);

/**
 * @brief  使能USB1端口检查U盘连接
 * @param  HostEnable: 是否检测U盘连接
 * @param  DeviceEnable: 是否检测PC连接
 * @return NONE
 */
void Usb1SetDetectMode(bool HostEnable, bool DeviceEnable);

/**
 * @brief  使能USB2端口检查U盘连接
 * @param  HostEnable: 是否检测U盘连接
 * @param  DeviceEnable: 是否检测PC连接
 * @return NONE
 */
void Usb2SetDetectMode(bool HostEnable, bool DeviceEnable);

/**
 * @brief  检查USB1端口上是否有一个U盘设备连接
 * @param  NONE
 * @return 1-有U盘设备连接，0-无U盘设备连接
 */
bool UsbHost1IsLink(void);

/**
 * @brief  检查USB2端口上是否有一个U盘设备连接
 * @param  NONE
 * @return 1-有U盘设备连接，0-无U盘设备连接
 */
bool UsbHost2IsLink(void);

/**
 * @brief  USB连接检测函数
 * @brief  在定时器中断处理函数中每毫秒调用一次
 * @param  NONE
 * @return NONE
 */
void OTGLinkCheck(void);

/**
 * @brief  检查当前选择的USB端口上是否有一个U盘设备连接
 * @param  NONE
 * @return 1-有U盘设备连接，0-无U盘设备连接
 */
bool UsbHostIsLink(void);

/**
 * @brief  usb总线复位
 * @param  NONE
 * @return NONE
 */
void UsbHostPortReset(void);

/**
 * @brief  从控制端点完成一次控制输出传输
 * @param  SetupPacket SETUP包指针
 * @param  Buf OUT数据缓冲区指针
 * @param  Len OUT数据长度
 * @return 1-成功，0-失败
 */
bool UsbHostControlOutTransfer(uint8_t* SetupPacket, uint8_t* Buf, uint16_t Len);

/**
 * @brief  从控制端点完成一次控制输入传输
 * @param  SetupPacket SETUP包指针
 * @param  Buf IN数据缓冲区指针
 * @param  Len IN数据长度
 * @return 1-成功，0-失败
 */
bool UsbHostControlInTransfer(uint8_t* SetupPacket, uint8_t* Buf, uint16_t Len);

/**
 * @brief  接收一个数据包
 * @param  Pipe pipe指针
 * @param  Buf 接收数据缓冲区指针
 * @param  Len 接收数据长度
 * @param  TimeOut 接收数据超时时间，单位为毫秒
 * @return 实际接收到的数据长度
 */
uint16_t UsbHostRcvPacket(PIPE_INFO* Pipe, uint8_t* Buf, uint16_t Len, uint16_t TimeOut);

/**
 * @brief  发送一个数据包
 * @param  Pipe pipe指针
 * @param  Buf 发送数据缓冲区指针
 * @param  Len 发送数据长度
 * @param  TimeOut 接收数据超时时间，单位为毫秒
 * @return 1-成功，0-失败
 */
bool UsbHostSendPacket(PIPE_INFO* Pipe, uint8_t* Buf, uint16_t Len, uint16_t TimeOut);

/**
 * @brief  启动某个端点发送一个数据包
 * @brief  异步方式，启动后，不等待立即返回
 * @brief  硬件会自动不断发送OUT令牌，直到发送完一个数据包
 * @param  Pipe pipe指针
 * @param  Buf 发送数据缓冲区指针
 * @param  Len 发送数据长度
 * @return NONE
 */
void UsbHostStartSendPacket(PIPE_INFO* Pipe, uint8_t* Buf, uint16_t Len);

/**
 * @brief  判断发送是否完成
 * @param  Pipe pipe指针
 * @return 1-成功，0-失败
 */
bool UsbHostIsSendOK(PIPE_INFO* Pipe);

/**
 * @brief  启动某个端点接收一个数据包
 * @brief  异步方式，启动后，不等待立即返回
 * @brief  硬件会自动不断发送IN令牌，直到接收到一个数据包
 * @param  Pipe pipe指针
 * @return NONE
 */
void UsbHostStartRcvPacket(PIPE_INFO* Pipe);

/**
 * @brief  HOST模式下使能某个端点中断
 * @param  Pipe Pipe指针
 * @param  Func 中断回调函数指针
 * @return NONE
 */
void UsbHostEnableInt(PIPE_INFO* Pipe, FPCALLBACK Func);

/**
 * @brief  HOST模式下禁止某个端点中断
 * @param  Pipe Pipe指针
 * @return NONE
 */
void UsbHostDisableInt(PIPE_INFO* Pipe);

/**
 * @brief  设置外设的设备地址
 * @param  Address 外设的设备地址
 * @return 1-成功，0-失败
 */
bool UsbHostSetAddress(uint8_t Address);

/**
 * @brief  获取描述符
 * @param  Type 描述符类型
 * @param  Index index
 * @param  Buf 描述符接收缓冲区指针
 * @param  Size 允许的最大长度
 * @return 1-成功，0-失败
 */
bool UsbHostGetDescriptor(uint8_t Type, uint8_t Index, uint8_t* Buf, uint16_t Size);

/**
 * @brief  设置配置
 * @param  ConfigurationNum 配置号
 * @return 1-成功，0-失败
 */
bool UsbHostSetConfiguration(uint8_t ConfigurationNum);

/**
 * @brief  设置接口
 * @param  InterfaceNum 接口号
 * @return 1-成功，0-失败
 */
bool UsbHostSetInterface(uint8_t InterfaceNum);

/**
 * @brief  设置iPod充电电流
 * @param  Current 充电电流
 * @return 1-成功，0-失败
 */
bool UsbHostSetIpodChargeCurrent(int16_t Current);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

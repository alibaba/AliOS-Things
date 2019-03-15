/**
 *****************************************************************************
 * @file     device_hcd.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    device module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __DEVICE_HCD_H__
#define	__DEVICE_HCD_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

// Max packet size. Fixed, user should not modify.
#define	DEVICE_FS_CONTROL_MPS		64
#define	DEVICE_FS_INT_IN_MPS		64
#define	DEVICE_FS_BULK_IN_MPS		64
#define	DEVICE_FS_BULK_OUT_MPS		64
#define	DEVICE_FS_ISO_IN_MPS		192
#define	DEVICE_FS_ISO_OUT_MPS		192


// Endpoint number. Fixed, user should not modify.
#define	DEVICE_CONTROL_EP			0x00
#define	DEVICE_INT_IN_EP			0x81
#define	DEVICE_BULK_IN_EP			0x82
#define	DEVICE_BULK_OUT_EP			0x03
#define	DEVICE_ISO_IN_EP			0x84
#define	DEVICE_ISO_OUT_EP			0x05


// Bus event define
#define	USB_VBUS_ERROR				(1 << 7)
#define	USB_SESSION_REQ				(1 << 6)
#define	USB_DISCONNECT				(1 << 5)
#define	USB_CONNECT					(1 << 4)
#define	USB_SOF						(1 << 3)
#define	USB_RESET					(1 << 2)
#define	USB_RESUME					(1 << 1)
#define	USB_SUSPEND					(1 << 0)

#define READER_UNREADY              0 // 未就绪
#define READER_INIT                 1 // 初始化
#define READER_READY                2 // 就绪
#define READER_READ                 3 // 读数据
#define READER_WIRTE                4 // 写数据
#define READER_INQUIRY              5 // 查询读卡器信息
#define READER_READ_FORMAT_CAPACITY 6 // 查询格式化容量
#define READER_READ_CAPACITY        7 // 查询磁盘容量
#define READER_NOT_ALLOW_REMOVAL    8 // 不允许删除设备
#define READER_ALLOW_REMOVAL        9 // 允许删除设备
extern uint8_t GetSdReaderState(void);

/**
 * @brief  检查USB1端口上是否有一个PC连接
 * @param  NONE
 * @return 1-有PC连接，0-无PC连接
 */
bool UsbDevice1IsLink(void);

/**
 * @brief  检查USB2端口上是否有一个PC连接
 * @param  NONE
 * @return 1-有PC连接，0-无PC连接
 */
bool UsbDevice2IsLink(void);

/**
 * @brief  检查当前选择的USB端口上是否有一个PC连接
 * @param  NONE
 * @return 1-有PC连接，0-无PC连接
 */
bool UsbDeviceIsLink(void);

/**
 * @brief  Device initial
 * @param  NONE
 * @return NONE
 */
void UsbDeviceInit(void);

/**
 * @brief  Device software disconnect.
 * @param  NONE
 * @return NONE
 */
void UsbDeviceDisConnect(void);

/**
 * @brief  返回STALL应答
 * @param  EndpointNum 端点号
 * @return NONE
 */
void UsbDeviceSendStall(uint8_t EndpointNum);

/**
 * @brief  复位某个端点
 * @param  EndpointNum 端点号
 * @return NONE
 */
void UsbDeviceResetEndpoint(uint8_t EndpointNum);

/**
 * @brief  设置设备地址
 * @brief  总线复位后，设置设备地址为0，后续根据PC端发的SetAddress命令设置指定的设备地址
 * @param  Address 设备地址
 * @return NONE
 */
void UsbDeviceSetAddress(uint8_t Address);

/**
 * @brief  获取总线上的事件
 * @param  NONE
 * @return 总线事件编码，USB_RESET-总线复位事件，...
 */
uint8_t UsbDeviceGetBusEvent(void);

/**
 * @brief  从控制端点接收SETUP数据
 * @param  Buf 数据缓冲区指针
 * @param  MaxLen 最大数据长度，一般SETUP数据包长度都是8字节
 * @return 实际接收到的数据长度
 */
uint8_t UsbDeviceSetupRcv(uint8_t* Buf, uint8_t MaxLen);

/**
 * @brief  从控制端点发送数据
 * @param  Buf 数据缓冲区指针
 * @param  Len 数据长度
 * @return 1-成功，0-失败
 */
bool UsbDeviceControlSend(uint8_t* Buf, uint16_t Len);

/**
 * @brief  从控制端点接收OUT数据
 * @param  Buf 数据缓冲区指针
 * @param  MaxLen 最大数据长度
 * @return 实际接收到的数据长度
 */
uint8_t UsbDeviceControlRcv(uint8_t* Buf, uint16_t MaxLen);

/**
 * @brief  从INT IN端点发送数据
 * @param  Buf 数据缓冲区指针
 * @param  Len 数据长度，不能大于DEVICE_FS_BULK_IN_MPS字节
 * @return 1-成功，0-失败
 */
bool UsbDeviceIntSend(uint8_t* Buf, uint16_t Len);

/**
 * @brief  从BULK IN端点发送数据
 * @param  Buf 数据缓冲区指针
 * @param  Len 数据长度
 * @return 1-成功，0-失败
 */
bool UsbDeviceBulkSend(uint8_t* Buf, uint16_t Len);

/**
 * @brief  从BULK OUT端点接收数据
 * @param  Buf 数据缓冲区指针
 * @param  MaxLen 最大数据长度
 * @return 实际接收到的数据长度
 */
uint16_t UsbDeviceBulkRcv(uint8_t* Buf, uint16_t MaxLen);

/**
 * @brief  从ISO IN端点发送数据
 * @param  Buf 数据缓冲区指针
 * @param  Len 数据长度，不能大于DEVICE_FS_ISO_IN_MPS字节
 * @return 1-成功，0-失败
 */
bool UsbDeviceIsoSend(uint8_t* Buf, uint16_t Len);

/**
 * @brief  从ISO OUT端点接收数据
 * @param  Buf 数据缓冲区指针
 * @param  MaxLen 最大数据长度
 * @return 实际接收到的数据长度
 */
uint16_t UsbDeviceIsoRcv(uint8_t* Buf, uint16_t MaxLen);

/**
 * @brief  DEVICE模式下使能某个端点中断
 * @param  Pipe Pipe指针
 * @param  Func 中断回调函数指针
 * @return NONE
 */
void UsbDeviceEnableInt(uint8_t EpNum, FPCALLBACK Func);

/**
 * @brief  DEVICE模式下禁止某个端点中断
 * @param  EpNum 端点号
 * @return NONE
 */
void UsbDeviceDisableInt(uint8_t EpNum);

/**
 * @brief  使能SOF超时检测。默认SOF超时检测使能。
 * @param  
 * @return NONE
 */
void EnUsbSofTimeOut(bool En);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

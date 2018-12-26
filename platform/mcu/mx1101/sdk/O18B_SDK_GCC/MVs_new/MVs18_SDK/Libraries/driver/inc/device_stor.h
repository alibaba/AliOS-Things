/**
 *****************************************************************************
 * @file     device_stor.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    device mass-storage module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __DEVICE_STOR_H__
#define	__DEVICE_STOR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//下面三个字符串指针用于指定电脑设备管理器中显示的磁盘驱动器名称
extern uint8_t* gDeviceString_MassStor_Vendor;			//max length: 8bytes
extern uint8_t* gDeviceString_MassStor_Product;		//max length: 16bytes
extern uint8_t* gDeviceString_MassStor_Ver;			//max length: 4bytes


/**
 * @brief  读卡器功能初始化
 * @param  NONE
 * @return NONE
 */
void DeviceStorInit(void);

/**
 * @brief  读卡器模式命令处理总入口
 * @param  NONE
 * @return NONE
 */
void DeviceStorProcess(void);

/**
 * @brief  判断当前是否处于保护状态
 * @param  NONE
 * @return 1-保护，0-非保护
 */
bool DeviceStorIsPrevent(void);

/**
 * @brief  判断PC端是否执行了“弹出”操作
 * @param  NONE
 * @return 1-已弹出，0-未弹出
 */
bool DeviceStorIsStopped(void);

uint8_t GetSdReaderState(void);

void SetSdReaderState(uint8_t State);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

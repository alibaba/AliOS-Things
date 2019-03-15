//maintainer lilu
#ifndef __FS_MOUNT_H__
#define	__FS_MOUNT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//
//主要目的为实现多分区挂载策略
//
#define CONF_DRIVES (2)

//挂载文件系统
extern uint8_t FsMount(uint8_t LogicId, FS_CONTEXT * fs);

//切换当前默认工作的设备
extern uint8_t FsChDrive(uint8_t LogicId);

//获取当前默认工作的设备类型
extern uint8_t FsGetDriveType(void);

//获取可挂载的逻辑设备号
extern int8_t FsGetAvailableLogicId(void);

//获取当前工作的文件系统
extern FS_CONTEXT* FsGetCurContext(void);

//通过设备类型获取当前工作的文件系统
extern FS_CONTEXT* FsGetContextByDeviceType(uint8_t DeviceType);

//通过设备类型获取当前工作的文件系统逻辑号
extern int8_t FsGetLogicIdByDeviceType(uint8_t DeviceType);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //_FS_MOUNT_H_

//  maintainer: lujiangang
#ifndef __POWER_MONITOR_H__
#define __POWER_MONITOR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/*
**********************************************************
*					函数声明
**********************************************************
*/
//
//电能监视初始化
//实现系统启动过程中的低电压检测处理，以及配置充电设备接入检测IO等
//
void PowerMonitorInit(void);

//
//系统电源状态监控和处理
//系统启动后如果LDOIN大于开机电压，放在系统正常运行中监测LDOIN
//
void PowerMonitor(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

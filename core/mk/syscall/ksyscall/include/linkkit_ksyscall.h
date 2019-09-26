/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef LINKKIT_KSYSCALL_H
#define LINKKIT_KSYSCALL_H

int sys_IOT_Linkkit_Open_stub(void *arg);

int sys_IOT_Linkkit_Connect_stub(void *arg);

void sys_IOT_Linkkit_Yield_stub(void *arg);

int sys_IOT_Linkkit_Close_stub(void *arg);

int sys_IOT_Linkkit_Report_stub(void *arg);

int sys_IOT_Linkkit_Query_stub(void *arg);

int sys_IOT_Linkkit_TriggerEvent_stub(void *arg);

int sys_IOT_Linkkit_AnswerService_stub(void *arg);

void sys_IOT_SetLogLevel_stub(void *arg);

void sys_IOT_DumpMemoryStats_stub(void *arg);

int sys_IOT_SetupConnInfo_stub(void *arg);

int sys_IOT_Ioctl_stub(void *arg);

int sys_IOT_HAL_Ioctl_stub(void *arg);

int sys_IOT_RegisterCallback_stub(void *arg);

#endif /* LINKKIT_KSYSCALL_H */



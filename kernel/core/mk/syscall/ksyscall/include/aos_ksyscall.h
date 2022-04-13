/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef AOS_KSYSCALL_H
#define AOS_KSYSCALL_H

char *sys_aos_version_str_get_stub(void *arg);
void sys_aos_get_calendar_time_stub(void *arg);
void sys_aos_set_calendar_time_stub(void *arg);
void sys_aos_get_utc_offset_stub(void *arg);
void sys_aos_set_utc_offset_stub(void *arg);
void sys_aos_reboot_stub(void *arg);
void sys_aos_now_ms_stub(void *arg);
void sys_aos_get_hz_stub(void *arg);
void *sys_aos_ipc_copy_stub(void *arg);
int  sys_aos_print_stub(void *arg);
#endif /* AOS_KSYSCALL_H */


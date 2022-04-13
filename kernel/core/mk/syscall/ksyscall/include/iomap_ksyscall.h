/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef IOMAP_KSYSCALL_H
#define IOMAP_KSYSCALL_H

void sys_aos_mem_mmap_stub(void *arg);
void sys_aos_io_mmap_stub(void *arg);
void sys_aos_io_unmmap_stub(void *arg);
uintptr_t sys_aos_io_mva2pa_stub(void *arg);
uintptr_t sys_aos_io_mpa2va_stub(void *arg);
int sys_aos_get_sys_mem_info_stub(void *arg);

#endif /* IOMAP_KSYSCALL_H */


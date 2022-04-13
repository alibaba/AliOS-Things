/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef AOS_IO_MAP_H
#define AOS_IO_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_MEM      0
#define NORMAL_MEM      0x1

int32_t aos_mem_mmap(uintptr_t vaddr, uintptr_t paddr, size_t len, int attr);
int32_t aos_io_mmap(uintptr_t vaddr, uintptr_t paddr, size_t len);
int32_t aos_io_unmmap(uintptr_t vaddr);
uintptr_t aos_io_mva2pa(uintptr_t vaddr);
uintptr_t aos_io_mpa2va(uintptr_t paddr);

#ifdef __cplusplus
}
#endif

#endif /* AOS_IO_MAP_H */

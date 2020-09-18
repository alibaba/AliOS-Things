/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _COREDUMP_SECTION_H
#define _COREDUMP_SECTION_H
#define COREDUMP_SECTOR_SIZE             0x1000
#define COREDUMP_SECTOR_SIZE_MASK        0xFFF
#define COREDUMP_BUFFER_LEN              (COREDUMP_SECTOR_SIZE*2)
#define COREDUMP_NORFALSH_BUFFER_LEN     (COREDUMP_BUFFER_LEN)
#if defined(__cplusplus)
extern "C" {
#endif

void coredump_to_flash_init();
void core_dump_erase_section();
int32_t core_dump_write_large(const uint8_t* ptr,uint32_t len);
int32_t core_dump_write(const uint8_t* ptr,uint32_t len);

#if defined(__cplusplus)
}
#endif

#endif


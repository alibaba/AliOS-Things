/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _ASC_H_
#define _ASC_H_

#define TEE_ASC_SRAM    (0x0)
#define TEE_ASC_FLASH   (0x1)
#define TEE_SET_PERM    (0x2)
#define TEE_CLR_PERM    (0x3)

#define ASC_SRV_UUID    {                               \
    0x7465655f, 0x6173, 0x635f,                         \
    { 0x73, 0x72, 0x76, 0x5f, 0x75, 0x75, 0x69, 0x64 }  \
}

//#define ASC_TEST_SRAM_ADDR  (0x2000C000)
#define ASC_TEST_SRAM_SIZE  (64)

//#define ASC_TEST_FLASH_SIZE (512)
//#define ASC_TEST_FALSH_ADDR ((NTW_RO_ADDR) + (NTW_RO_SIZE) - (ASC_TEST_FLASH_SIZE))

extern int tee_asc_drv_test(void);

#endif /* _ASC_H_ */

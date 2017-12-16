/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _MEM_H_
#define _MEM_H_

#define TEE_CMD_TMP_MEM     (0x0)
#define TEE_CMD_REGS_SHM    (0x1)
#define TEE_CMD_ALOC_SHM    (0x2)

#define MEM_SRV_UUID    {                               \
    0x11111111, 0x2222, 0x3333,                         \
    { 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb }  \
}

extern int tee_tmp_mem_test();
extern int tee_regs_shm_test();
extern int tee_alloc_shm_test();

#endif /* _MEM_H_ */

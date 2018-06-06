/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef RHINO_SMP_SYNC_H
#define RHINO_SMP_SYNC_H

#define KRHINO_SPINLOCK_FREE_VAL        0xB33FFFFFu
#define KRHINO_SPINLOCK_MAGIC_VAL		0xB33F0000u
#define KRHINO_SPINLOCK_MAGIC_MASK		0xFFFF0000u
#define KRHINO_SPINLOCK_MAGIC_SHIFT		16
#define KRHINO_SPINLOCK_CNT_MASK		0x0000FF00u
#define KRHINO_SPINLOCK_CNT_SHIFT		8
#define KRHINO_SPINLOCK_VAL_MASK		0x000000FFu
#define KRHINO_SPINLOCK_VAL_SHIFT		0


static inline void osPortCompareSet(volatile uint32_t *addr, uint32_t compare, uint32_t *set) {
    __asm__ __volatile__(
        "WSR 	    %2,SCOMPARE1 \n"
        "ISYNC      \n"
        "S32C1I     %0, %1, 0	 \n"
        :"=r"(*set)
        :"r"(addr), "r"(compare), "0"(*set)
        );
}
#endif
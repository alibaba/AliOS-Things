#ifndef __ARMV7_H
#define __ARMV7_H

extern void ARMV7_FLUSH_DCACHE_ALL(void);
extern void ARMV7_FLUSH_ICACHE_ALL(void);
extern void ARMV7_FLUSH_CACHE_ALL(void);
extern void ARMV7_FLUSH_ICACHE_REGION(void *start, void *end);
extern void ARMV7_FLUSH_DCACHE_REGION(void *start, void *end);
extern void ARMV7_CLEAN_DCACHE_REGION(void *start, void *end);
extern void ARMV7_INVALID_DCACHE_REGION(void *start, void *end);
extern void ARMV7_INVALIDATE_DCACHE(void);
extern void ARMV7_FLUSH_TLB_ALL(void);
extern void ARMV7_MMU_TABLE_INIT(void);
extern int  ARMV7_GET_MMU_TABLE_BASE(void);
extern void ARMV7_PREDICTION_ENABLE(void);
extern void ARMV7_L1_ICACHE_ENABLE(void);
extern void ARMV7_L1_DCACHE_ENABLE(void);
extern void ARMV7_L2_CACHE_ENABLE(void);
extern void ARMV7_MMU_ENABLE(void);
extern void ARMV7_NEON_VFP_ENABLE(void);

extern void ARMV7_IRQ_ENABLE(void);
extern void ARMV7_IRQ_DISABLE(void);
extern void ARMV7_FIQ_ENABLE(void);
extern void ARMV7_FIQ_DISABLE(void);
extern void ARMV7_VECT_HIGH_ENABLE(void);
extern void ARMV7_VECT_HIGH_DISABLE(void);

#endif



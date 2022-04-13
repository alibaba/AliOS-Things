#ifndef __CACHE_H__
#define __CACHE_H__
#include <stddef.h>
#include <stdint.h>

void FlushDcacheAll(void);
void FlushIcacheAll(void);
void FlushCacheAll(void);
void FlushIcacheRegion(void *start, unsigned int size);
void FlushDcacheRegion(void *start, unsigned int size);
void InvalidDcacheRegion(void *start, unsigned int size);
void InvalidDcache(void);
void FlushTLB(void);
void flush_dcache(void);
void init_mmu(void);
void init_cache(void);
void InitNeonVFP(void);

//Page Table Attributes
#define SECATTR_MASK            ((0x1<<16)|(0x7<<12)|(0x3<<2))
#define LPGATTR_MASK            ((0x7<<12)|(0x3<<2))
#define SPGATTR_MASK            ((0x7<< 6)|(0x3<<2))

#define MRATTR_STRONG_ORDER     0x00        //Strongly Ordered
#define MRATTR_NORM_NC          0x01        //Normal, Non-Cacheable
#define MRATTR_NORM_WT          0x02        //Normal, Write Through, Cacheable
#define MRATTR_NORM_WB          0x03        //Normal, Write Back, Cacheable
#define MRATTR_DEVICE_SHA       0x04        //Normal, Write Back, Cacheable
#define MRATTR_STRONG_SHA       0x05

unsigned int SetMemoryRegionAttr(void *start, unsigned int size, unsigned int attr);

#define DESC_SEC       (0x2)
#define MEMWBWAYC      ((1<<12)|(3<<2))     /* write back, write allocate,cache enable */
#define MEMWBWANC      ((1<<12)|(1<<2))     /* write back, write allocate,cache disable */
#define MEMWB          (3<<2)  /* write back, no write allocate */
#define MEMWT          (2<<2)  /* write through, no write allocate */
#define SHAREDEVICE    (1<<2)  /* shared device */
#define STRONGORDER    (0<<2)  /* strong ordered */
#define XN             (1<<4)  /* eXecute Never */
#define AP_RW          (3<<10) /* supervisor=RW, user=RW */
#define AP_RO          (2<<10) /* supervisor=RW, user=RO */
#define SHARED         (1<<16) /* shareable */
#define NS             (1<<19) /* secuirty flag */

#define DOMAIN_FAULT   (0x0)
#define DOMAIN_CHK     (0x1)
#define DOMAIN_NOTCHK  (0x3)
#define DOMAIN0        (0x0<<5)
#define DOMAIN1        (0x1<<5)

#define DOMAIN0_ATTR   (DOMAIN_CHK<<0)
#define DOMAIN1_ATTR   (DOMAIN_FAULT<<2)

/* device mapping type */
#define DEVICE_MEM     (SHARED|AP_RW|DOMAIN0|SHAREDEVICE|DESC_SEC|XN)
/* normal memory mapping type */
#define NORMAL_MEM     (SHARED|AP_RW|DOMAIN0|MEMWBWAYC|DESC_SEC)

#define L1_COHERENT    0x00014c06
#define L1_NONCOHERENT 0x00000c1e
#define L1_DEVICE      0x00000c06

struct mem_zone_map_items {
    uint32_t vaddr_start;
    uint32_t vaddr_end;
    uint32_t paddr_start;
    uint32_t attr;
};

#endif //#ifndef __CACHE_H__

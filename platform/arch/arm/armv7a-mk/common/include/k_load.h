#ifndef K_LOAD_H
#define K_LOAD_H

#include "k_mmpage.h"

#define K_CONFIG_APP_MAX        16

/* segment type */
typedef enum {
    /* text, rodata... */
    SEG_EXE,
    /* data, bss... */
    SEG_DATA,
    /* for all type */
    SEG_EXE_DATA
} seg_type_t;

typedef struct {
    uint32_t   magicword;
    uintptr_t *text_start;
    uintptr_t *text_end;
    uintptr_t *data_start;
    uintptr_t *data_end;
    uintptr_t *zero_start;
    uintptr_t *zero_end;
} preamble_t;

/* physical memory pool */
typedef struct
{
    uintptr_t paddr_start;
    uintptr_t paddr_end;
    uintptr_t vp_offset; /* kernel vaddr = paddr + vp_offset */
    mmpage_info_t info;
} kload_pmm_t;

/* app infomation */
typedef struct
{
    uint32_t    appid;  /* same as appid */
    const char *name;
    void       *entry;
    preamble_t  preamble;
    uintptr_t   vaddr_pgtable;
    uintptr_t   paddr_pgtable;
} kload_app_t;

/* load infomation */
typedef struct
{
    kload_pmm_t phymem;
    kload_app_t apps[K_CONFIG_APP_MAX];
} kload_info_t;

int32_t k_load_exec(const char *path, const char *name);

#endif /* K_LOAD_H */


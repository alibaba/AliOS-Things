/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>

#include "k_api.h"
#include "k_arch.h"
#include "k_mmu_sd.h"

/*
Short-descriptor format:
    It uses 32-bit descriptor entries in the translation tables, and provides:
    Up to two levels of address lookup.
    32-bit input addresses.
    Output addresses of up to 40 bits.
    Support for PAs of more than 32 bits by use of supersections, with 16MB granularity.
    Support for No access, Client, and Manager domains.
    32-bit table entries.

Size:
    Supersections   Consist of 16MB blocks of memory.
    Sections        Consist of 1MB blocks of memory.
    Large pages     Consist of 64KB blocks of memory.
    Small pages     Consist of 4KB blocks of memory.
*/

/* from mmu_mem_attr_t to mmu section descriptor, for kernel mode */
const int32_t g_mmu_sec_attr_map_k[] =
{
    MMU_ATTR_SEC_NX_SO | DES_SEC_AP_KRW,            // MMU_ATTR_SO
    MMU_ATTR_SEC_NX_DEV | DES_SEC_AP_KRW,           // MMU_ATTR_DEV
    MMU_ATTR_SEC_NX_DEV_NSH | DES_SEC_AP_KRW,       // MMU_ATTR_DEV_NSH
    MMU_ATTR_SEC_NX_NC | DES_SEC_AP_KRW,            // MMU_ATTR_NON_CACHE

    MMU_ATTR_SEC_X_WB | DES_SEC_AP_KRO,             // MMU_ATTR_EXEC
    MMU_ATTR_SEC_NX_WB | DES_SEC_AP_KRW,            // MMU_ATTR_DATA
    MMU_ATTR_SEC_NX_WB | DES_SEC_AP_KRO,            // MMU_ATTR_DATA_RO
    MMU_ATTR_SEC_X_WB | DES_SEC_AP_KRW,             // MMU_ATTR_EXEC_DATA

    MMU_ATTR_SEC_X_WB | DES_SEC_AP_KRO | DES_SEC_S, // MMU_ATTR_EXEC_SH
    MMU_ATTR_SEC_NX_WB | DES_SEC_AP_KRW | DES_SEC_S,// MMU_ATTR_DATA_SH
    MMU_ATTR_SEC_NX_WB | DES_SEC_AP_KRO | DES_SEC_S,// MMU_ATTR_DATA_KRO_SH
    MMU_ATTR_SEC_X_WB | DES_SEC_AP_KRW | DES_SEC_S, // MMU_ATTR_EXEC_DATA_SH

    DES_SEC_BASE              // MMU_ATTR_BUTT
};

#if (K_CONFIG_MMU_TABLE_MAX > 1)
/* from mmu_mem_attr_t to mmu section descriptor, for user mode */
const int32_t g_mmu_sec_attr_map_u[] = 
{
    MMU_ATTR_SEC_NX_SO | DES_SEC_AP_URW,            // MMU_ATTR_SO
    MMU_ATTR_SEC_NX_DEV | DES_SEC_AP_URW,           // MMU_ATTR_DEV
    MMU_ATTR_SEC_NX_DEV_NSH | DES_SEC_AP_URW,       // MMU_ATTR_DEV_NSH
    MMU_ATTR_SEC_NX_NC | DES_SEC_AP_URW,            // MMU_ATTR_NON_CACHE

    MMU_ATTR_SEC_X_WB | DES_SEC_AP_URO,             // MMU_ATTR_EXEC
    MMU_ATTR_SEC_NX_WB | DES_SEC_AP_URW,            // MMU_ATTR_DATA
    MMU_ATTR_SEC_NX_WB | DES_SEC_AP_URO,            // MMU_ATTR_DATA_RO
    MMU_ATTR_SEC_X_WB | DES_SEC_AP_URW,             // MMU_ATTR_EXEC_DATA

    MMU_ATTR_SEC_X_WB | DES_SEC_AP_URO | DES_SEC_S, // MMU_ATTR_EXEC_SH
    MMU_ATTR_SEC_NX_WB | DES_SEC_AP_URW | DES_SEC_S,// MMU_ATTR_DATA_SH
    MMU_ATTR_SEC_NX_WB | DES_SEC_AP_URO | DES_SEC_S,// MMU_ATTR_DATA_URO_SH
    MMU_ATTR_SEC_X_WB | DES_SEC_AP_URW | DES_SEC_S, // MMU_ATTR_EXEC_DATA_SH

    DES_SEC_BASE              // MMU_ATTR_BUTT
};
#endif

#if (K_CONFIG_MMU_LVL2 == 1)
/* from mmu_mem_attr_t to mmu large page descriptor, for kernel mode */
const int32_t g_mmu_lpg_attr_map_k[] =
{
    MMU_ATTR_LPG_NX_SO | DES_LPG_AP_KRW,            // MMU_ATTR_SO
    MMU_ATTR_LPG_NX_DEV | DES_LPG_AP_KRW,           // MMU_ATTR_DEV
    MMU_ATTR_LPG_NX_DEV_NSH | DES_LPG_AP_KRW,       // MMU_ATTR_DEV_NSH
    MMU_ATTR_LPG_NX_NC | DES_LPG_AP_KRW,            // MMU_ATTR_NON_CACHE

    MMU_ATTR_LPG_X_WB | DES_LPG_AP_KRO,             // MMU_ATTR_EXEC
    MMU_ATTR_LPG_NX_WB | DES_LPG_AP_KRW,            // MMU_ATTR_DATA
    MMU_ATTR_LPG_NX_WB | DES_LPG_AP_KRO,            // MMU_ATTR_DATA_RO
    MMU_ATTR_LPG_X_WB | DES_LPG_AP_KRW,             // MMU_ATTR_EXEC_DATA

    MMU_ATTR_LPG_X_WB | DES_LPG_AP_KRO | DES_LPG_S, // MMU_ATTR_EXEC_SH
    MMU_ATTR_LPG_NX_WB | DES_LPG_AP_KRW | DES_LPG_S,// MMU_ATTR_DATA_SH
    MMU_ATTR_LPG_NX_WB | DES_LPG_AP_KRO | DES_LPG_S,// MMU_ATTR_DATA_KRO_SH
    MMU_ATTR_LPG_X_WB | DES_LPG_AP_KRW | DES_LPG_S, // MMU_ATTR_EXEC_DATA_SH

    DES_LPG_BASE            // MMU_ATTR_BUTT
};

#if (K_CONFIG_MMU_TABLE_MAX > 1)
/* from mmu_mem_attr_t to mmu large page descriptor, for user mode */
const int32_t g_mmu_lpg_attr_map_u[] = 
{
    MMU_ATTR_LPG_NX_SO | DES_LPG_AP_URW,            // MMU_ATTR_SO
    MMU_ATTR_LPG_NX_DEV | DES_LPG_AP_URW,           // MMU_ATTR_DEV
    MMU_ATTR_LPG_NX_DEV_NSH | DES_LPG_AP_URW,       // MMU_ATTR_DEV_NSH
    MMU_ATTR_LPG_NX_NC | DES_LPG_AP_URW,            // MMU_ATTR_NON_CACHE

    MMU_ATTR_LPG_X_WB | DES_LPG_AP_URO,             // MMU_ATTR_EXEC
    MMU_ATTR_LPG_NX_WB | DES_LPG_AP_URW,            // MMU_ATTR_DATA
    MMU_ATTR_LPG_NX_WB | DES_LPG_AP_URO,            // MMU_ATTR_DATA_RO
    MMU_ATTR_LPG_X_WB | DES_LPG_AP_URW,             // MMU_ATTR_EXEC_DATA

    MMU_ATTR_LPG_X_WB | DES_LPG_AP_URO | DES_LPG_S, // MMU_ATTR_EXEC_SH
    MMU_ATTR_LPG_NX_WB | DES_LPG_AP_URW | DES_LPG_S,// MMU_ATTR_DATA_SH
    MMU_ATTR_LPG_NX_WB | DES_LPG_AP_URO | DES_LPG_S,// MMU_ATTR_DATA_URO_SH
    MMU_ATTR_LPG_X_WB | DES_LPG_AP_URW | DES_LPG_S, // MMU_ATTR_EXEC_DATA_SH

    DES_LPG_BASE            // MMU_ATTR_BUTT
};
#endif

/* from mmu_mem_attr_t to mmu small page descriptor, for kernel mode */
const int32_t g_mmu_spg_attr_map_k[] =
{
    MMU_ATTR_SPG_NX_SO | DES_SPG_AP_KRW,            // MMU_ATTR_SO
    MMU_ATTR_SPG_NX_DEV | DES_SPG_AP_KRW,           // MMU_ATTR_DEV
    MMU_ATTR_SPG_NX_DEV_NSH | DES_SPG_AP_KRW,       // MMU_ATTR_DEV_NSH
    MMU_ATTR_SPG_NX_NC | DES_SPG_AP_KRW,            // MMU_ATTR_NON_CACHE

    MMU_ATTR_SPG_X_WB | DES_SPG_AP_KRO,             // MMU_ATTR_EXEC
    MMU_ATTR_SPG_NX_WB | DES_SPG_AP_KRW,            // MMU_ATTR_DATA
    MMU_ATTR_SPG_NX_WB | DES_SPG_AP_KRO,            // MMU_ATTR_DATA_RO
    MMU_ATTR_SPG_X_WB | DES_SPG_AP_KRW,             // MMU_ATTR_EXEC_DATA

    MMU_ATTR_SPG_X_WB | DES_SPG_AP_KRO | DES_SPG_S, // MMU_ATTR_EXEC_SH
    MMU_ATTR_SPG_NX_WB | DES_SPG_AP_KRW | DES_SPG_S,// MMU_ATTR_DATA_SH
    MMU_ATTR_SPG_NX_WB | DES_SPG_AP_KRO | DES_SPG_S,// MMU_ATTR_DATA_KRO_SH
    MMU_ATTR_SPG_X_WB | DES_SPG_AP_KRW | DES_SPG_S, // MMU_ATTR_EXEC_DATA_SH

    DES_SPG_BASE            // MMU_ATTR_BUTT
};

#if (K_CONFIG_MMU_TABLE_MAX > 1)
/* from mmu_mem_attr_t to mmu small page descriptor, for user mode */
const int32_t g_mmu_spg_attr_map_u[] = 
{
    MMU_ATTR_SPG_NX_SO | DES_SPG_AP_URW,            // MMU_ATTR_SO
    MMU_ATTR_SPG_NX_DEV | DES_SPG_AP_URW,           // MMU_ATTR_DEV
    MMU_ATTR_SPG_NX_DEV_NSH | DES_SPG_AP_URW,       // MMU_ATTR_DEV_NSH
    MMU_ATTR_SPG_NX_NC | DES_SPG_AP_URW,            // MMU_ATTR_NON_CACHE

    MMU_ATTR_SPG_X_WB | DES_SPG_AP_URO,             // MMU_ATTR_EXEC
    MMU_ATTR_SPG_NX_WB | DES_SPG_AP_URW,            // MMU_ATTR_DATA
    MMU_ATTR_SPG_NX_WB | DES_SPG_AP_URO,            // MMU_ATTR_DATA_RO
    MMU_ATTR_SPG_X_WB | DES_SPG_AP_URW,             // MMU_ATTR_EXEC_DATA

    MMU_ATTR_SPG_X_WB | DES_SPG_AP_URO | DES_SPG_S, // MMU_ATTR_EXEC_SH
    MMU_ATTR_SPG_NX_WB | DES_SPG_AP_URW | DES_SPG_S,// MMU_ATTR_DATA_SH
    MMU_ATTR_SPG_NX_WB | DES_SPG_AP_URO | DES_SPG_S,// MMU_ATTR_DATA_URO_SH
    MMU_ATTR_SPG_X_WB | DES_SPG_AP_URW | DES_SPG_S, // MMU_ATTR_EXEC_DATA_SH

    DES_SPG_BASE            // MMU_ATTR_BUTT
};
#endif

#endif

/* level1 page table, static management, 4K entrys. */
K_CONFIG_MMU_TABLE_REGION uint32_t g_pgtbl_lvl1[K_CONFIG_MMU_TABLE_MAX][MMU_SECTION_ENTRY_CNT]
    __attribute__((aligned(MMU_TALBE_SIZE_LVL1)));

#if (K_CONFIG_MMU_LVL2 == 1)
/* manage lvl2 pagetable resource, one item is 1KB */
typedef struct pagetab_pool{
    int   initflag;
    uintptr_t start;
    uintptr_t len;
    void *freelist;
} pagetab_pool_t;

static pagetab_pool_t   s_pgtbl_pool;
/* level2 page table, dynamic management, 1K entrys for 1 table. */
K_CONFIG_MMU_TABLE_REGION uint32_t g_pgtbl_lvl2[K_CONFIG_MMU_LVL2_TABLES][MMU_PAGE_ENTRY_CNT]
    __attribute__((aligned(MMU_TALBE_SIZE_LVL2)));

void os_lvl2tab_init()
{
    int loop;

    s_pgtbl_pool.start = (uintptr_t)g_pgtbl_lvl2;
    s_pgtbl_pool.len   = sizeof(g_pgtbl_lvl2);

    s_pgtbl_pool.freelist = NULL;

    for ( loop = 0; loop < K_CONFIG_MMU_LVL2_TABLES; loop++ )
    {
        g_pgtbl_lvl2[loop][0] = (uint32_t)s_pgtbl_pool.freelist;
        s_pgtbl_pool.freelist = (void *)&g_pgtbl_lvl2[loop][0];
    }
    s_pgtbl_pool.initflag = 1;
}

void *os_lvl2tab_alloc(void)
{
    uintptr_t *pgtable;

    if ( s_pgtbl_pool.initflag != 1 )
    {
        os_lvl2tab_init();
    }

    pgtable = s_pgtbl_pool.freelist;
    if (pgtable != NULL) {
        s_pgtbl_pool.freelist = (void *)(*pgtable);
        memset(pgtable, 0, MMU_TALBE_SIZE_LVL2);
    }

    return pgtable;
}

void os_lvl2tab_free(void *pgtbl)
{
    *(uint32_t *)pgtbl = (uint32_t)s_pgtbl_pool.freelist;
    s_pgtbl_pool.freelist = pgtbl;
}
#endif

/* Invalidate all unified TLB */
void os_tlb_invalidate(void)
{
    OS_DSB();
    OS_ISB();
    os_set_TLBIALL(0);
    os_set_BPIALL(0);
    OS_DSB();
    OS_ISB();
}

/* Invalidate all branch predictors */
void os_mmu_domain_set(uint32_t domain_set)
{
    os_set_DACR(domain_set);
    OS_DSB();
    OS_ISB();
}

uint32_t os_mmu_domain_get(void)
{
    return os_get_DACR();
}

void os_mmu_disable(void)
{
    uint32_t reg;
    reg  = os_get_SCTLR();
    /*
    SCTLR.M, bit[0]   MMU enable.
    0  PL1&0 stage 1 MMU disabled.
    1  PL1&0 stage 1 MMU enabled.
    */
    reg &= ~0x1;
    os_set_SCTLR(reg);
    OS_ISB();
}

void os_mmu_enable(void)
{
    uint32_t reg;

    reg  = os_get_SCTLR();
    /*
    SCTLR.M, bit[0]   MMU enable.
    0  PL1&0 stage 1 MMU disabled.
    1  PL1&0 stage 1 MMU enabled.
    */
    reg |= 0x1;
    os_set_SCTLR(reg);
    OS_ISB();
}

/* Get the translation table base form TTBR0
   AliOS-Things do not use TTBR1 */
uintptr_t os_mmu_ttb_get(void)
{
    uintptr_t reg;
    reg = os_get_TTBR0();
    return reg;
}

/* Set the translation table base form TTBR0
   AliOS-Things do not use TTBR1 */
void os_mmu_ttb_set(uintptr_t paddr_table)
{
    os_set_TTBR0(paddr_table);
    OS_ISB();
}

#if (K_CONFIG_MMU_TABLE_MAX > 1)
static uint32_t s_mmu_asid;

void os_mmu_asid_set(uint32_t asid)
{
    /* Address Space Identifier (ASID) */
    if ( asid < K_CONFIG_MMU_TABLE_MAX )
    {
        s_mmu_asid = asid;
        os_set_CONTEXTIDR(asid);
        OS_ISB();
    }
}

uint32_t os_mmu_asid_get()
{
    return s_mmu_asid;
}
#else
uint32_t os_mmu_asid_get()
{
    return 0;
}
#endif

/* Convert the mem_attr for users to mem_attr for first-level descriptor */
int32_t os_mmu_sec_attr_convert(mmu_mem_attr_t mem_attr, int32_t isKernel)
{
#if (K_CONFIG_MMU_TABLE_MAX > 1)
    int32_t des;

    if ( isKernel == 1 )
    {
        des = g_mmu_sec_attr_map_k[mem_attr];
    }
    else
    {
        des = g_mmu_sec_attr_map_u[mem_attr];
    }

    /* multi mmu table for multi APPs, using ASID for TLB performance */
    return des | DES_SEC_nG;
#else
    (void)isKernel;
    return g_mmu_sec_attr_map_k[mem_attr];
#endif
}

/* MMU Map the "vaddr" to "paddr" with mem_attr, Section or Supersection format,
   "vaddr" "paddr" "len" should be aligned with 1M */
int32_t os_mmap_section(uint32_t  pgtable[],
                        uintptr_t vaddr,
                        uintptr_t paddr,
                        uintptr_t len,
                        mmu_mem_attr_t  mem_attr)
{
    uint32_t entry;
    uint32_t *p_entry;
    int32_t entry_num;
    int32_t entry_flags;
    int32_t i, j;

    if ( len == 0 )
    {
        return 0;
    }

    p_entry     = &pgtable[vaddr >> MMU_SECTION_BITS];
    entry_num   = len >> MMU_SECTION_BITS;
    if ( pgtable == g_pgtbl_lvl1[0] )
    {
        entry_flags = os_mmu_sec_attr_convert(mem_attr, 1);
    }
    else
    {
        entry_flags = os_mmu_sec_attr_convert(mem_attr, 0);
    }

    for(i = 0; i < entry_num;)
    {
        /* checking the supersection conditions */
        if ( paddr % MMU_SUPERSECTION_SIZE == 0
          && vaddr % MMU_SUPERSECTION_SIZE == 0
          && i + MMU_SUPERSECTION_REPEAT <= entry_num )
        {
            /* add a supersection, Repeated 16 times */
            entry = paddr | DES_SUPSEC_FLAG | entry_flags;
            for(j = 0; j < MMU_SUPERSECTION_REPEAT; j++)
            {
                *p_entry = entry;
                k_dcache_clean((uintptr_t)p_entry, sizeof(*p_entry));
                p_entry++;
            }
            i += MMU_SUPERSECTION_REPEAT;
            vaddr += MMU_SUPERSECTION_SIZE;
            paddr += MMU_SUPERSECTION_SIZE;
        }
        else
        {
            /* add a section */
            entry = paddr | entry_flags;
            *p_entry = entry;
            k_dcache_clean((uintptr_t)p_entry, sizeof(*p_entry));
            p_entry++;
            i++;
            vaddr += MMU_SECTION_SIZE;
            paddr += MMU_SECTION_SIZE;
        }
    }

    os_tlb_invalidate();
    return 0;
}


#if (K_CONFIG_MMU_LVL2 == 1)
/* Convert the mem_attr for users to mem_attr for second-level large page descriptor */
int32_t os_mmu_lpg_attr_convert(mmu_mem_attr_t mem_attr, int32_t isKernel)
{
#if (K_CONFIG_MMU_TABLE_MAX > 1)
    int32_t des;

    if ( isKernel == 1 )
    {
        des = g_mmu_lpg_attr_map_k[mem_attr];
    }
    else
    {
        des = g_mmu_lpg_attr_map_u[mem_attr];
    }

    /* multi mmu table for multi APPs, using ASID for TLB performance */
    return des | DES_LPG_nG;
#else
    (void)isKernel;
    return g_mmu_lpg_attr_map_k[mem_attr];
#endif
}

/* Convert the mem_attr for users to mem_attr for second-level small page descriptor */
int32_t os_mmu_spg_attr_convert(mmu_mem_attr_t  mem_attr, int32_t isKernel)
{
#if (K_CONFIG_MMU_TABLE_MAX > 1)
    int32_t des;

    if ( isKernel == 1 )
    {
        des = g_mmu_spg_attr_map_k[mem_attr];
    }
    else
    {
        des = g_mmu_spg_attr_map_u[mem_attr];
    }

    /* multi mmu table for multi APPs, using ASID for TLB performance */
    return des | DES_SPG_nG;
#else
    (void)isKernel;
    return g_mmu_spg_attr_map_k[mem_attr];
#endif
}

/* Get pagetable for the "vaddr"
   alloc one pagetable if needed */
uint32_t *os_mmu_get_lvl2patable(uint32_t pgtable[], uintptr_t vaddr)
{
    uint32_t entry;
    uint32_t *pgtbl;

    /* pgtable entry from level1 */
    entry = pgtable[vaddr >> MMU_SECTION_BITS];
    /* Check if entry is pagetable, which ends with 0b01 */
    if ( (entry & 0x3) == DES_PGTBL_BASE )
    {
        /* pagetable exist, Get the address */
        pgtbl = (uint32_t *)((entry >> MMU_PAGETABLE_BITS) << MMU_PAGETABLE_BITS);
    }
    else
    {
        /* Alloc one pagetable, New the attribute bits */
        pgtbl = (uint32_t *)os_lvl2tab_alloc();
        if ( pgtbl == NULL )
        {
            return NULL;
        }
        entry = (uint32_t)pgtbl | DES_PGTBL_BASE;

        pgtable[vaddr >> MMU_SECTION_BITS] = entry;
        k_dcache_clean((uintptr_t)&pgtable[vaddr >> MMU_SECTION_BITS], sizeof(entry));
    }

    return pgtbl;
}

/* MMU Map the "vaddr" to "paddr" with mem_attr, Small Page or Large Page format,
   "vaddr" "paddr" "len" should be aligned with 4K */
int32_t os_mmap_page(uint32_t  pgtable[],
                     uintptr_t vaddr,
                     uintptr_t paddr,
                     uintptr_t len,
                     mmu_mem_attr_t  mem_attr)
{
    uint32_t entry;
    uint32_t *pgtbl;
    uint32_t *p_entry;
    int32_t entry_num;
    int32_t entry_flags;
    int32_t i, j;

    if ( len == 0 )
    {
        return 0;
    }

    /* get pgtable entry from level1 */
    pgtbl = os_mmu_get_lvl2patable(pgtable, vaddr);
    if ( pgtbl == NULL )
    {
        return -1;
    }

    /* Second-level table: Indexed by VA[19:12] */
    p_entry = &pgtbl[(vaddr % MMU_SECTION_SIZE) >> MMU_SMALLPAGE_BITS];
    entry_num   = len >> MMU_SMALLPAGE_BITS;
    for(i = 0; i < entry_num;)
    {
        /* checking the large page conditions */
        if ( paddr % MMU_LARGEPAGE_SIZE == 0
          && vaddr % MMU_LARGEPAGE_SIZE == 0
          && i + MMU_LARGEPAGE_REPEAT <= entry_num )
        {
            /* add a large page, Repeated 16 times */
            if ( pgtable == g_pgtbl_lvl1[0] )
            {
                entry_flags = os_mmu_lpg_attr_convert(mem_attr, 1);
            }
            else
            {
                entry_flags = os_mmu_lpg_attr_convert(mem_attr, 0);
            }
            entry = paddr | entry_flags;
            for(j = 0; j < MMU_LARGEPAGE_REPEAT; j++)
            {
                *p_entry = entry;
                k_dcache_clean((uintptr_t)p_entry, sizeof(*p_entry));
                p_entry++;
            }
            i += MMU_LARGEPAGE_REPEAT;
            vaddr += MMU_LARGEPAGE_SIZE;
            paddr += MMU_LARGEPAGE_SIZE;
        }
        else
        {
            /* add a small page */
            if ( pgtable == g_pgtbl_lvl1[0] )
            {
                entry_flags = os_mmu_spg_attr_convert(mem_attr, 1);
            }
            else
            {
                entry_flags = os_mmu_spg_attr_convert(mem_attr, 0);
            }
            entry = paddr | entry_flags;
            *p_entry = entry;
            k_dcache_clean((uintptr_t)p_entry, sizeof(*p_entry));
            p_entry++;
            i++;
            vaddr += MMU_SMALLPAGE_SIZE;
            paddr += MMU_SMALLPAGE_SIZE;
        }

        if ( vaddr % MMU_SECTION_SIZE == 0 )
        {
            /* need a new page table */
            pgtbl = os_mmu_get_lvl2patable(pgtable, vaddr);
            if ( pgtbl == NULL )
            {
                return -1;
            }
            p_entry = &pgtbl[0];
        }
    }

    os_tlb_invalidate();
    return 0;
}

#endif

/* traverse all page table, handle it, unmap it if needed */
int32_t os_mmu_traversal(uint32_t asid,
                         int32_t  is_unmap,
                         os_mmu_func_t handler)
{
    int32_t   isKernel; /* The map is for kernel or user space? */
    uint32_t  *pgtable;
    int32_t   lvl1_idx;
    uint32_t  lvl1_entry;
    uintptr_t lvl1_vaddr;
    uintptr_t paddr;
#if (K_CONFIG_MMU_LVL2 == 1)
    int32_t   lvl2_idx;
    uint32_t  lvl2_entry;
    uintptr_t lvl2_vaddr;
    uint32_t *lvl2_pgtable;
#endif

    if ( asid >= K_CONFIG_MMU_TABLE_MAX )
    {
        return -1;
    }
    
    pgtable = g_pgtbl_lvl1[asid];

    /* Traverse the page table */

    for(lvl1_idx = 0; lvl1_idx < MMU_SECTION_ENTRY_CNT; lvl1_idx++)
    {
        lvl1_entry = pgtable[lvl1_idx];
        lvl1_vaddr = lvl1_idx << MMU_SECTION_BITS;
        isKernel   = lvl1_entry == g_pgtbl_lvl1[0][lvl1_idx] ? 1: 0;

        if ( lvl1_idx % MMU_SUPERSECTION_REPEAT == 0 
          && (lvl1_entry & 0x40002) == DES_SUPSEC_BASE )
        {
            /* supersection */
            paddr = (lvl1_entry >> MMU_SUPERSECTION_BITS) << MMU_SUPERSECTION_BITS;
            if ( handler != NULL )
            {
                handler(lvl1_vaddr, paddr, MMU_SUPERSECTION_SIZE, isKernel);
            }
            lvl1_idx += MMU_SUPERSECTION_REPEAT - 1;
        }
        else if ( (lvl1_entry & 0x40002) == DES_SEC_BASE )
        {
            /* section */
            paddr = (lvl1_entry >> MMU_SECTION_BITS) << MMU_SECTION_BITS;
            if ( handler != NULL )
            {
                handler(lvl1_vaddr, paddr, MMU_SECTION_SIZE, isKernel);
            }
        }
#if (K_CONFIG_MMU_LVL2 == 1)

        else if ( (lvl1_entry & 0x3) == DES_PGTBL_BASE )
        {
            /* lvl2 page table */
            lvl2_pgtable = (uint32_t *)((lvl1_entry >> MMU_PAGETABLE_BITS) << MMU_PAGETABLE_BITS);
            for(lvl2_idx = 0; lvl2_idx < MMU_PAGE_ENTRY_CNT; lvl2_idx++)
            {
                lvl2_entry = lvl2_pgtable[lvl2_idx];
                lvl2_vaddr = lvl2_idx << MMU_SMALLPAGE_BITS;

                if ( lvl2_idx % MMU_LARGEPAGE_REPEAT == 0 
                  && (lvl2_entry & 0x3) == DES_LPG_BASE )
                {
                    /* Large page */
                    paddr = (lvl2_entry >> MMU_LARGEPAGE_BITS) << MMU_LARGEPAGE_BITS;
                    if ( handler != NULL )
                    {
                        handler(lvl1_vaddr + lvl2_vaddr, paddr, MMU_LARGEPAGE_SIZE, isKernel);
                    }
                    lvl2_idx += MMU_LARGEPAGE_REPEAT - 1;
                }
                else if ( (lvl2_entry & 0x2) == DES_SPG_BASE )
                {
                    /* Small page */
                    paddr = (lvl2_entry >> MMU_SMALLPAGE_BITS) << MMU_SMALLPAGE_BITS;
                    if ( handler != NULL )
                    {
                        handler(lvl1_vaddr + lvl2_vaddr, paddr, MMU_SMALLPAGE_SIZE, isKernel);
                    }
                }
                else
                {
                    /* Invalid */
                }
            }
            if ( is_unmap == 1 )
            {
                /* when unmap app, do not recycle kernel pages */
                if ( !isKernel )
                {
                    os_lvl2tab_free(lvl2_pgtable);
                }
            }
        }
#endif
        else
        {
            /* Invalid */
        }
    }

    if ( is_unmap == 1 )
    {
        /* Clear the page table */
        memset(pgtable, 0, MMU_TALBE_SIZE_LVL1);
        k_dcache_clean((uintptr_t)pgtable, MMU_TALBE_SIZE_LVL1);
        os_tlb_invalidate();
    }

    return 0;
}

void k_mmu_disable(void)
{
    memset(g_pgtbl_lvl1[0], 0, sizeof(g_pgtbl_lvl1[0]));
    os_tlb_invalidate();
    os_mmu_disable();
}

void k_mmu_enable(void)
{
    /* "Client" for domain0
       "No access" for others
       "Client" - Accesses are checked against the permission bits in the translation tables.*/
    os_mmu_domain_set(0x1);

    os_mmu_ttb_set((uintptr_t)g_pgtbl_lvl1[0]);

#if (K_CONFIG_MMU_TABLE_MAX > 1)
    /* ASID of kernel is 0 */
    os_mmu_asid_set(os_mmu_asid_get());
#endif

    os_mmu_enable();
}

#if (K_CONFIG_MMU_LVL2 == 1)
/* Level 1 + Level 2 mmap */
int32_t k_mmu_mmap(uint32_t  pgtable[],
                   uintptr_t vaddr, 
                   uintptr_t paddr, 
                   size_t    len, 
                   mmu_mem_attr_t  mem_attr)
{
    int32_t   ret;
    uintptr_t len_once;
    uintptr_t vaddr_aligned;
    uintptr_t paddr_aligned;
    size_t    len_aligned;

    if ( mem_attr >= MMU_ATTR_BUTT )
    {
        return -1;
    }
    
    if ( vaddr % MMU_SMALLPAGE_SIZE != paddr % MMU_SMALLPAGE_SIZE )
    {
        return -1;
    }

    if ( len == 0 )
    {
        return 0;
    }

    /* align the address and len */
    vaddr_aligned = ALIGN_DOWN(vaddr, MMU_SMALLPAGE_SIZE);
    paddr_aligned = ALIGN_DOWN(paddr, MMU_SMALLPAGE_SIZE);
    len_aligned   = ALIGN_UP(len + (vaddr - vaddr_aligned), MMU_SMALLPAGE_SIZE);

    /* checking the section conditions
       try to use section or supersection as much as possible */
    if (vaddr_aligned % MMU_SECTION_SIZE == paddr_aligned % MMU_SECTION_SIZE)
    {
        /* first, page map to the 1M aliged address */
        len_once = MMU_SECTION_SIZE - vaddr_aligned % MMU_SECTION_SIZE;
        if ( len_once != MMU_SECTION_SIZE && len_once <= len_aligned )
        {
            ret = os_mmap_page(pgtable, vaddr_aligned, paddr_aligned, len_once, mem_attr);
            if ( ret != 0 )
            {
                /* To be done: The completed map needs to be recycled */
                return ret;
            }
            vaddr_aligned += len_once;
            paddr_aligned += len_once;
            len_aligned   -= len_once;
        }

        /* second, section map */
        if ( len_aligned >= MMU_SECTION_SIZE )
        {
            len_once = len_aligned - len_aligned % MMU_SECTION_SIZE;
            ret = os_mmap_section(pgtable, vaddr_aligned, paddr_aligned, len_once, mem_attr);
            if ( ret != 0 )
            {
                /* To be done: The completed map needs to be recycled */
                return ret;
            }
            vaddr_aligned += len_once;
            paddr_aligned += len_once;
            len_aligned   -= len_once;
        }
    }

    if ( len_aligned > 0 )
    {
        /* third, page map the left */
        ret = os_mmap_page(pgtable, vaddr_aligned, paddr_aligned, len_aligned, mem_attr);
        if ( ret != 0 )
        {
            /* To be done: The completed map needs to be recycled */
            return ret;
        }
    }

    return 0;
}
#else
/* Only Level 1 mmap */
int32_t k_mmu_mmap(uint32_t  pgtable[],
                   uintptr_t vaddr,
                   uintptr_t paddr,
                   size_t    len,
                   mmu_mem_attr_t  mem_attr)
{
    uintptr_t vaddr_aligned;
    uintptr_t paddr_aligned;
    size_t    len_aligned;

    if ( mem_attr >= MMU_ATTR_BUTT )
    {
        return -1;
    }

    if ( vaddr % MMU_SECTION_SIZE != paddr % MMU_SECTION_SIZE )
    {
        return -1;
    }

    if ( len == 0 )
    {
        return 0;
    }

    /* align the address and len */
    vaddr_aligned = ALIGN_DOWN(vaddr, MMU_SECTION_SIZE);
    paddr_aligned = ALIGN_DOWN(paddr, MMU_SECTION_SIZE);
    len_aligned   = ALIGN_UP(len + (vaddr - vaddr_aligned), MMU_SECTION_SIZE);

    return os_mmap_section(pgtable, vaddr_aligned, paddr_aligned, len_aligned, mem_attr);
}
#endif

/* mmu map in kernel */
int32_t k_mmap(uintptr_t vaddr,
               uintptr_t paddr,
               size_t    len,
               mmu_mem_attr_t  mem_attr)
{
    int32_t ret;

    ret = k_mmu_mmap(g_pgtbl_lvl1[os_mmu_asid_get()], vaddr, paddr, len, mem_attr);
    return ret;
}

void os_mmu_print(uintptr_t vaddr, uintptr_t paddr, size_t len, int32_t isKenrel)
{
    if ( isKenrel == 1 )
    {
        printf("  [Kernel]: vaddr = %p, paddr = %p, len = 0x%x\n", (void*)vaddr, (void*)paddr, len);
    }
    else
    {
        printf("  [User]  : vaddr = %p, paddr = %p, len = 0x%x\n", (void*)vaddr, (void*)paddr, len);
    }
}

void k_mmu_show(void)
{
    int32_t ret;

    printf("[mmu show] SCTLR = 0x%x, ASID = %d, PA of TableBase = %p, VA of TableBase = %p\n", 
           os_get_SCTLR(), os_mmu_asid_get(), (void*)os_mmu_ttb_get(), (void*)g_pgtbl_lvl1[os_mmu_asid_get()]);
    ret = os_mmu_traversal(os_mmu_asid_get(), 0, os_mmu_print);
    if ( ret != 0 )
    {
        printf("[mmu show] return %d", ret);
    }
}

uintptr_t k_mmu_vir2phy(uintptr_t vaddr)
{
    uint32_t  par;
    uintptr_t paddr;

    /* Performing address translation operations */
    os_set_ATS1CPR(vaddr);
    par = os_get_PAR();

    /* PAR.F, bit[0], Indicates that the conversion completed successfully */
    if ( par & 1 )
    {
        /* translation do not completes successfully */
        return 0;
    }

    /* PAR.SS, bit[1],  Supersection. Indicates whether the result is a Supersection */
    if ( par & 2 )
    {
        paddr = ((par >> 24) << 24) + (vaddr & ((1 << 24) - 1));
    }
    else
    {
        paddr = ((par >> 12) << 12) + (vaddr & ((1 << 12) - 1));
    }

    return paddr;
}

#if (K_CONFIG_MMU_TABLE_MAX > 1)

/*
Each process maintains a separate first-level translation table. On a context switch:
• TTBR0 is updated to point to the first-level translation table for the new context
• TTBCR is updated if this change changes the size of the translation table
• the CONTEXTIDR is updated.
*/
void k_mmu_switch(uint32_t asid)
{
    if ( asid >= K_CONFIG_MMU_TABLE_MAX )
    {
        return;
    }

    os_mmu_asid_set(asid);
    os_mmu_ttb_set((uintptr_t)g_pgtbl_lvl1[asid]);

    OS_DSB();
    OS_ISB();
}

/* copy kernel maps to app maps */
int32_t k_mmu_copy_kernel_pgtbl(uint32_t asid)
{
    if ( asid >= K_CONFIG_MMU_TABLE_MAX )
    {
        return -1;
    }

    memcpy(g_pgtbl_lvl1[asid], g_pgtbl_lvl1[0], sizeof(g_pgtbl_lvl1[0]));
    k_dcache_clean((uintptr_t)g_pgtbl_lvl1[asid], sizeof(g_pgtbl_lvl1));
    os_tlb_invalidate();

    return 0;
}

int32_t k_mmu_munmap_all(uint32_t asid, 
                         os_mmu_func_t handler)
{
    if ( asid == 0 || asid >= K_CONFIG_MMU_TABLE_MAX )
    {
        return -1;
    }
    os_mmu_traversal(asid, 1, handler);
    return 0;
}

#endif


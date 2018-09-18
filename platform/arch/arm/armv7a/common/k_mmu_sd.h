#ifndef K_MMU_SD_H
#define K_MMU_SD_H

/********** Short-descriptor translation table format **********/
/*
Short-descriptor for Section:
Bit0
Bit1        1
Bit2        B
Bit3        C
Bit4        XN, The Execute-never bit. 
Bit5~8      Domain, Domain field. (DACR defines the permitted access for each domain)
Bit9
Bit10~11    AP[1:0], Access Permissions bits.
Bit12~14    TEX[2:0]
Bit15       AP[2], Access Permissions bits.
Bit16       S, The Shareable bit.
Bit17       nG, The not global bit.
Bit18       0, Descriptor is for a Section
Bit19       NS, Non-secure bit. 
Bit20~31    PA, Section base address
*/
/***************************************************************/

/* Secure + Global + Non-Shareable + AP(No access) + Domain 0 + Executable + strongly-ordered */
#define MMU_ENTRY_SEC_BASE                  0x2

/* XN, The Execute-never bit. */
#define MMU_ENTRY_SEC_XN                    (1<<4)
/* TEX[2:0] C B (TEX, C, and B encodings when TRE == 0): 
MT_STRONGLY_ORDERED:    Strongly-ordered
MT_SHARED_DEVICE:       Shareable Device
MT_NORMAL_NONCACHE:     Normal + Non-cacheable
MT_NORMAL_CACHE_WB:     Normal + Write-Back, Write-Allocate
MT_NORMAL_CACHE_WT:     Normal + Write-Through, no Write-Allocate
*/
#define MMU_ENTRY_SEC_MT_STRONGLY_ORDERED   ((0<<12) | (0<<3) | (0<<2))  
#define MMU_ENTRY_SEC_MT_SHARED_DEVICE      ((0<<12) | (0<<3) | (1<<2))
#define MMU_ENTRY_SEC_MT_NORMAL_NONCACHE    ((1<<12) | (0<<3) | (0<<2))
#define MMU_ENTRY_SEC_MT_NORMAL_CACHE_WB    ((1<<12) | (1<<3) | (1<<2))
#define MMU_ENTRY_SEC_MT_NORMAL_CACHE_WT    ((0<<12) | (1<<3) | (0<<2))
/* AP[2:0], define the access permissions (VMSAv7 MMU access permissions):
   000      No access 
   011      Full access
   111      Read-only   */
#define MMU_ENTRY_SEC_AP_RW                 ((0<<15) | (3<<10))
#define MMU_ENTRY_SEC_AP_RO                 ((1<<15) | (3<<10))
/* S, The Shareable bit */
#define MMU_ENTRY_SEC_S                     (1<<16)
/* nG, The not global bit. */
#define MMU_ENTRY_SEC_nG                    (1<<17)
/* NS, Non-secure bit. */
#define MMU_ENTRY_SEC_NS                    (1<<19)
/* Domain field, n = 0~15 */
#define MMU_ENTRY_SEC_DOMAIN(n)             ((n)<<5)

/********** mem_attr for Normal memory (e.g. ddr, sram ...) **********/
/* Secure + Global + Non-Shareable + Read-only + Domain 0 + Executable + Cache write back */
#define MMU_ATTR_X_RO_WB    \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_AP_RO | MMU_ENTRY_SEC_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Read-only + Domain 0 + Non-Executable + Cache write back */
#define MMU_ATTR_NX_RO_WB   \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_XN | MMU_ENTRY_SEC_AP_RO | MMU_ENTRY_SEC_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Full access + Domain 0 + Executable + Cache write back */
#define MMU_ATTR_X_RW_WB    \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_AP_RW | MMU_ENTRY_SEC_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Full access + Domain 0 + Non-Executable + Cache write back */
#define MMU_ATTR_NX_RW_WB   \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_XN  | MMU_ENTRY_SEC_AP_RW | MMU_ENTRY_SEC_MT_NORMAL_CACHE_WB)
/* Secure + Global + Non-Shareable + Full access + Domain 0 + Executable + Cache Write through */
#define MMU_ATTR_X_RW_WT    \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_AP_RW | MMU_ENTRY_SEC_MT_NORMAL_CACHE_WT)
/* Secure + Global + Non-Shareable + Full access + Domain 0 + Non-Executable + Cache Write through */
#define MMU_ATTR_NX_RW_WT    \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_XN  | MMU_ENTRY_SEC_AP_RW | MMU_ENTRY_SEC_MT_NORMAL_CACHE_WT)
/* Secure + Global + Non-Shareable + Full access + Domain 0 + Executable + Non-cacheable */
#define MMU_ATTR_X_RW_NC    \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_AP_RW | MMU_ENTRY_SEC_MT_NORMAL_NONCACHE)
/* Secure + Global + Non-Shareable + Full access + Domain 0 + Non-Executable + Non-cacheable */
#define MMU_ATTR_NX_RW_NC    \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_XN | MMU_ENTRY_SEC_AP_RW | MMU_ENTRY_SEC_MT_NORMAL_NONCACHE)
/*******************************************************/

/********** mem_attr for MMIO (e.g. drivers regs) **********/
/* Secure + Global + Non-Shareable + Full access + Domain 0 + Non-Executable + Strongly-ordered */
#define MMU_ATTR_NX_RW_SO   \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_XN | MMU_ENTRY_SEC_AP_RW | MMU_ENTRY_SEC_MT_STRONGLY_ORDERED)
/* Secure + Global + Non-Shareable + Read-only + Domain 0 + Non-Executable + Strongly-ordered */
#define MMU_ATTR_NX_RO_SO   \
        (MMU_ENTRY_SEC_BASE | MMU_ENTRY_SEC_XN | MMU_ENTRY_SEC_AP_RO | MMU_ENTRY_SEC_MT_STRONGLY_ORDERED)
/*******************************************************/

void k_cpu_mmu_disable(void);
void k_cpu_mmu_enable(void);
void *k_cpu_mmu_ttb_get();
void k_cpu_mmu_ttb_set(uint32_t table[]);
uint32_t k_cpu_mmu_domain_get();
void k_cpu_mmu_domain_set(uint32_t domain_set);
void k_cpu_mmu_section(uint32_t table[], 
                  uintptr_t vaddr_start, uintptr_t vaddr_end, uintptr_t paddr_start, uint32_t mem_attr);

#endif /* K_MMU_SD_H */

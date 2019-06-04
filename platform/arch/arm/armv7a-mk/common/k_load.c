/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "k_arch.h"
#include "k_mmu_sd.h"
#include "k_elf.h"

#define SECTION_ALIGN_SIZE          MMU_SMALLPAGE_SIZE

/* alloc form physical mem pool, block size is size, and align to size
   return virtual address in kernel space*/
#define os_phymem_alloc(size)       \
        os_mmpage_malloc(&g_kload_info.phymem.info, size, size)

#define os_phymem_free(ptr, size)   \
        os_mmpage_free(&g_kload_info.phymem.info, ptr, size)

/* convert physical address to virtual address */
#define os_phymem_phy2vir(address)  \
        os_mmpage_phy2vir(&g_kload_info.phymem.info, (uintptr_t)address)

/* convert virtual address to physical address */
#define os_phymem_vir2phy(address)  \
        os_mmpage_vir2phy(&g_kload_info.phymem.info, (uintptr_t)address)

/* check if the */
#define os_phymem_check_phy(paddr)  \
        (paddr - g_kload_info.phymem.info.phy_addr < g_kload_info.phymem.info.total_size)


kload_info_t g_kload_info;

/*
attach a physical memory to app:
1. alloc physical memory, match vaddr_start ~ vaddr_end
2. mmu map cover it
3. copy buff, from copy_from to app's vaddr_start
*/
int32_t os_load_mmap_copy(void      *pgtable,
                          uintptr_t  vaddr_start, 
                          uintptr_t  vaddr_end, 
                          void      *copy_from,
                          size_t     copy_len,
                          seg_type_t seg_type)
{
    int32_t   ret;
    void     *pPage;
    uintptr_t vaddr_once;
    uintptr_t paddr_once;
    /* mmap a page each time, len is 16M / 1M / 64K / 16K */
    size_t    len_once;
    uintptr_t vaddr_copy_start;
    uintptr_t vaddr_copy_end;
    mmu_mem_attr_t mem_attr;

    /* Address notice:
    1. viraddr from app perspective: vaddr_start / vaddr_end / vaddr_copy_start / vaddr_copy_end / vaddr_once
    2. viraddr from kernel perspective: copy_from / pPage
    3. phyaddr: paddr_once
    */

    mem_attr = seg_type == SEG_EXE ? MMU_ATTR_EXEC :
               seg_type == SEG_DATA ? MMU_ATTR_DATA : MMU_ATTR_EXEC_DATA;

    for ( vaddr_once = ALIGN_DOWN(vaddr_start, SECTION_ALIGN_SIZE) ; 
          vaddr_once < ALIGN_UP(vaddr_end, SECTION_ALIGN_SIZE) ; 
          vaddr_once += len_once )
    {
        len_once = 0;
        pPage    = NULL;

        /* try supersection */
        if (len_once == 0 && vaddr_once % MMU_SUPERSECTION_SIZE == 0 && vaddr_once + MMU_SUPERSECTION_SIZE <= vaddr_end)
        {
            pPage = os_phymem_alloc(MMU_SUPERSECTION_SIZE);
            if ( pPage != NULL )
            {
                len_once = MMU_SUPERSECTION_SIZE;
            }
        }

        /* try section */
        if (len_once == 0 && vaddr_once % MMU_SECTION_SIZE == 0 && vaddr_once + MMU_SECTION_SIZE <= vaddr_end)
        {
            pPage = os_phymem_alloc(MMU_SECTION_SIZE);
            if ( pPage != NULL )
            {
                len_once = MMU_SECTION_SIZE;
            }
        }

        /* try large page */
        if (len_once == 0 && vaddr_once % MMU_LARGEPAGE_SIZE == 0 && vaddr_once + MMU_LARGEPAGE_SIZE <= vaddr_end)
        {
            pPage = os_phymem_alloc(MMU_LARGEPAGE_SIZE);
            if ( pPage != NULL )
            {
                len_once = MMU_LARGEPAGE_SIZE;
            }
        }

        /* try small page */
        if (len_once == 0 && vaddr_once % MMU_SMALLPAGE_SIZE == 0 && vaddr_once + MMU_SMALLPAGE_SIZE <= vaddr_end)
        {
            pPage = os_phymem_alloc(MMU_SMALLPAGE_SIZE);
            if ( pPage != NULL )
            {
                len_once = MMU_SMALLPAGE_SIZE;
            }
        }

        if ( len_once == 0 )
        {
            return -1;
        }

        paddr_once = os_phymem_vir2phy(pPage);
        ret = os_mmu_mmap((uint32_t *)pgtable, vaddr_once, paddr_once, len_once, mem_attr);
        if ( ret != 0 )
        {
            return ret;
        }

        memset(pPage, 0, len_once);

        if ( copy_from == NULL )
        {
            continue;
        }

        /* copy "vaddr_start ~ vaddr_start + copy_len"
           to "vaddr_once ~ vaddr_once + len_once" */
        vaddr_copy_start = vaddr_start > vaddr_once ? vaddr_start : vaddr_once;
        vaddr_copy_end   = vaddr_start + copy_len < vaddr_once + len_once ? 
                           vaddr_start + copy_len : vaddr_once + len_once;
        if ( vaddr_copy_start < vaddr_copy_end )
        {
            /* copy text and data */
            memcpy((char *)pPage + (vaddr_copy_start - vaddr_once),
                   copy_from + (vaddr_copy_start - vaddr_start),
                   vaddr_copy_end - vaddr_copy_start);
        }
    }
    return 0;
}

void *os_load_file2ram(const char *path)
{
#if 0
    int fd, filelen = 0;
    void *pbuff;

    fd = open(path, O_RDONLY, 0);
    if (fd < 0)
    {
        return NULL;
    }

    filelen = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    if (filelen == 0)
    {
        close(fd);
        return NULL;
    }

    pbuff = krhino_mm_alloc(filelen);
    if (pbuff == NULL)
    {
        close(fd);
        return NULL;
    }

    if (read(fd, pbuff, filelen) != filelen)
    {
        close(fd);
        return NULL;
    }

    close(fd);
    fd = -1;

    return pbuff;
#else
    return (void *)path;
#endif
}

int32_t os_load_check(Elf32_Ehdr *ehdr)
{
    if ( (ehdr->e_ident[EI_MAG0] != ELFMAG0)
      || (ehdr->e_ident[EI_MAG1] != ELFMAG1)
      || (ehdr->e_ident[EI_MAG2] != ELFMAG2)
      || (ehdr->e_ident[EI_MAG3] != ELFMAG3))
    {
        return -1;
    }

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS32)
    {
        return -1;
    }

    if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        return -1;
    }

    if (ehdr->e_machine != EM_ARM)
    {
        return -1;
    }

    return  0;
}

int32_t os_load_exec(kload_app_t *app, Elf32_Ehdr *ehdr)
{
    int32_t i, ret;
    Elf32_Phdr *phdr;
    preamble_t *preamble = NULL;
    uint32_t   *pgtable;
    uintptr_t   copy_from;

    if (ehdr->e_machine != EM_ARM)
    {
        return -1;
    }

    phdr = (Elf32_Phdr *)((char *)ehdr + ehdr->e_phoff);

    /* find preamble_t */
    for (i = 0; i < ehdr->e_phnum; i++, phdr++) 
    {
        if (phdr->p_type == PT_LOAD && phdr->p_filesz != 0)
        {
            if ( preamble == NULL )
            {
                preamble = (preamble_t *)((char *)ehdr + phdr->p_offset);
                break;
            }
            else
            {
                /* only support 1 load segment for execution object file */
                return -2;
            }
        }
    }
    if ( preamble == NULL || preamble->magicword != 0x878654321 )
    {
        /* can not find preamble_t */
        return -3;
    }

    /* create lvl1 page table */
    pgtable = os_phymem_alloc(MMU_TALBE_SIZE_LVL1);
    if ( pgtable == NULL )
    {
        return -4;
    }
    memcpy(&app->preamble, preamble, sizeof(app->preamble));
    app->vaddr_pgtable = (uintptr_t)pgtable;
    app->paddr_pgtable = os_phymem_vir2phy(app->vaddr_pgtable);

    if (!(preamble->text_start <= preamble->text_end 
       && preamble->text_end <= preamble->data_start 
       && preamble->data_start <= preamble->data_end 
       && preamble->data_end <= preamble->zero_start 
       && preamble->zero_start <= preamble->zero_end) )
    {
        /* preamble_t error */
        return -5;
    }

    if ( preamble->text_start < phdr->p_vaddr 
      || preamble->data_end   > phdr->p_vaddr + phdr->p_filesz)
    {
        /* loadable segment not match the phdr */
        return -6;
    }

    copy_from = (uintptr_t)ehdr + phdr->p_offset;

    /* are text and data stick togather? */
    if ( preamble->text_end / SECTION_ALIGN_SIZE == preamble->data_start / SECTION_ALIGN_SIZE )
    {
        /* text and data not separated, load togather */
        ret = os_load_mmap_copy(pgtable, 
                                preamble->text_start,
                                preamble->zero_end, 
                                (void *)(copy_from + preamble->text_start - phdr->p_vaddr),
                                preamble->data_end - preamble->text_start,
                                SEG_EXE_DATA);
        if ( ret != 0 )
        {
            return ret;
        }
    }
    else
    {
        /* load text */
        ret = os_load_mmap_copy(pgtable, 
                                preamble->text_start,
                                preamble->text_end, 
                                (void *)(copy_from + preamble->text_start - phdr->p_vaddr),
                                preamble->text_end - preamble->text_start,
                                SEG_EXE);

        if ( ret != 0 )
        {
            return ret;
        }

        /* load data & bss */
        ret = os_load_mmap_copy(pgtable, 
                                preamble->data_start,
                                preamble->zero_end, 
                                (void *)(copy_from + preamble->data_start - phdr->p_vaddr),
                                preamble->data_end - preamble->data_start,
                                SEG_DATA);
        if ( ret != 0 )
        {
            return ret;
        }
    }

    return 0;
}

void os_unload_page_free(uintptr_t vaddr, uintptr_t paddr, size_t len)
{
    void *vaddr_kernel;

    if ( os_phymem_check_phy(paddr) )
    {
        vaddr_kernel = (void *)os_phymem_phy2vir(paddr);
        os_phymem_free(vaddr_kernel, len);
    }
}

int32_t os_unload_exec(kload_app_t *app)
{
    if ( app == NULL )
    {
        return -1;
    }
    if ( (void *)app->vaddr_pgtable != NULL )
    {
        /* munmap and recycle physical pages */
        os_mmu_munmap_all((uint32_t *)(app->vaddr_pgtable), os_unload_page_free);
        
        os_phymem_free((void *)app->vaddr_pgtable, MMU_TALBE_SIZE_LVL1);

        app->vaddr_pgtable = 0;
        app->paddr_pgtable = 0;
    }

    app->name = NULL;

    return 0;
}

int32_t os_load_init()
{
    int32_t i;

    /* 1. init apps info */
    memset(&g_kload_info, 0, sizeof(g_kload_info));
    for ( i = 0 ; i < K_CONFIG_APP_MAX ; i++ )
    {
        g_kload_info.apps[i].appid = i;
    }

    /* app[0] as kernel */
    g_kload_info.apps[0].name   = "kernel";

    /* 2. init physical mem pool */
    /* constraint: viraddr = phyaddr
       TODO: break this constraint */
    if ( g_kload_info.phymem.paddr_start % SECTION_ALIGN_SIZE != 0
      || (g_kload_info.phymem.paddr_end - g_kload_info.phymem.paddr_start) % SECTION_ALIGN_SIZE != 0)
    {
        return -1;
    }
    os_mmpage_init(&g_kload_info.phymem.info, 
                   g_kload_info.phymem.paddr_start,
                   g_kload_info.phymem.paddr_start,
                   g_kload_info.phymem.paddr_end - g_kload_info.phymem.paddr_start);

    return 0;
}

int32_t k_load_exec(const char *path, const char *name)
{
    int32_t idx, ret;
    Elf32_Ehdr  *ehdr = NULL;
    kload_app_t *app  = NULL;

    /* 1. load image to ram */
    ehdr = (Elf32_Ehdr *)os_load_file2ram(path);
    if ( ehdr == NULL )
    {
        goto _exit;
    }

    /* 2. alloc a app info */
    for ( idx = 1 ; idx < K_CONFIG_APP_MAX ; idx++ )
    {
        if ( g_kload_info.apps[idx].name == NULL )
        {
            break;
        }
    }
    if ( idx == K_CONFIG_APP_MAX )
    {
        goto _exit;
    }
    app = &g_kload_info.apps[idx];
    app->entry = (void *)ehdr->e_entry;
    app->name  = name == NULL ? "unnamed" : name;

    /* 3. iamge load */
    if ( os_load_check(ehdr) != 0 )
    {
        goto _exit;
    }

    switch ( ehdr->e_type )
    {
        case ET_EXEC :
            ret = os_load_exec(app, ehdr);
            if ( ret != 0 )
            {
                goto _exit;
            }
            break;
        case ET_REL :
        case ET_DYN :
        default:
            /* error type */
            goto _exit;
    }

    /* 4. user tasks create */

    return 0;

_exit:
    if ( ehdr != NULL )
    {
        krhino_mm_free(ehdr);
    }
    if ( app != NULL )
    {
        os_unload_exec(app);
    }
    return -1;
}


/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>

#include "k_api.h"
#include "k_arch.h"
#include "k_elf.h"

#include <u_task.h>
#include "mmu_adapt.h"
#include "aos/log.h"

/* alloc form physical mem pool, block size is size, and align to size
   return virtual address in kernel space*/
#define os_phymem_alloc(size)       \
        os_phymm_malloc(&g_proc_info.phymem.info, size, size)

#define os_phymem_free(ptr, size)   \
        os_phymm_free(&g_proc_info.phymem.info, ptr, size)

/* check if the physical memory belongs to apps phymm pool */
#define os_phymem_check_phy(paddr)  \
        ((paddr) - g_proc_info.phymem.info.phy_addr < g_proc_info.phymem.info.total_size)


kproc_info_t g_proc_info;
kproc_app_t *g_proc_now = NULL;

int32_t os_proc_mmap_free(uintptr_t vaddr_start,
                          uintptr_t vaddr_end,
                          uint32_t  pid)
{
    uintptr_t paddr_once;
    uintptr_t vaddr_once;
    uintptr_t vaddr_map_start;
    uintptr_t vaddr_map_end;
    uintptr_t paddr_start, paddr_end;
    size_t    len_once;

    vaddr_map_start = ALIGN_DOWN(vaddr_start, K_MMU_PAGE_ALIGN);
    vaddr_map_end   = ALIGN_UP(vaddr_end, K_MMU_PAGE_ALIGN);

    for ( vaddr_once = vaddr_map_start; vaddr_once < vaddr_map_end; vaddr_once += len_once )
    {
        len_once   = 0;

        paddr_start = os_lookup_physaddr(vaddr_once);
        /* try supersection */
        if ( len_once == 0
          && vaddr_once % MMU_SUPERSECTION_SIZE == 0
          && vaddr_once + MMU_SUPERSECTION_SIZE <= vaddr_map_end)
        {
            paddr_end = os_lookup_physaddr(vaddr_once + MMU_SUPERSECTION_SIZE - MMU_SMALLPAGE_SIZE);
            if (paddr_end - paddr_start == (MMU_SUPERSECTION_SIZE - MMU_SMALLPAGE_SIZE))
            {
                len_once = MMU_SUPERSECTION_SIZE;
            }
        }

        /* try section */
        if ( len_once == 0
          && vaddr_once % MMU_SECTION_SIZE == 0
          && vaddr_once + MMU_SECTION_SIZE <= vaddr_map_end)
        {
            paddr_end = os_lookup_physaddr(vaddr_once + MMU_SECTION_SIZE - MMU_SMALLPAGE_SIZE);
            if (paddr_end - paddr_start == (MMU_SECTION_SIZE - MMU_SMALLPAGE_SIZE))
            {
                len_once = MMU_SECTION_SIZE;
            }
        }

        /* try large page */
        if ( len_once == 0
          && vaddr_once % MMU_LARGEPAGE_SIZE == 0
          && vaddr_once + MMU_LARGEPAGE_SIZE <= vaddr_map_end)
        {
            paddr_end = os_lookup_physaddr(vaddr_once + MMU_LARGEPAGE_SIZE - MMU_SMALLPAGE_SIZE);
            if (paddr_end - paddr_start == (MMU_LARGEPAGE_SIZE - MMU_SMALLPAGE_SIZE)) {
                len_once = MMU_LARGEPAGE_SIZE;
            }
        }

        /* try small page */
        if ( len_once == 0
          && vaddr_once % MMU_SMALLPAGE_SIZE == 0
          && vaddr_once + MMU_SMALLPAGE_SIZE <= vaddr_map_end)
        {
            len_once = MMU_SMALLPAGE_SIZE;
        }

        if (len_once)
        {
            paddr_start = os_lookup_physaddr(vaddr_once);
            os_phymem_free(paddr_start, len_once);
            LOGD("k_proc", "pid %d unmap v 0x%x-0x%x -> p 0x%x-0x%x, size 0x%x\r\n",
                   pid, vaddr_once, vaddr_once + len_once,
                   paddr_start, paddr_start + len_once, len_once);
            os_proc_remove_mapping(vaddr_once, paddr_start, len_once, pid);
        }
        else
        {
            break;
        }
    }

    return 0;
}

/*
attach a physical memory for proc:
1. alloc physical memory, match vaddr_start ~ vaddr_end
2. mmu map cover it
3. copy buff, from copy_from to proc's vaddr_start
*/
int32_t os_proc_mmap_copy(uintptr_t  vaddr_start,
                          uintptr_t  vaddr_end,
                          void      *copy_from,
                          size_t     copy_len,
                          seg_type_t seg_type)
{
    int32_t   ret;
    uintptr_t paddr_once;
    uintptr_t vaddr_once;
    size_t    len_once;
    uintptr_t vaddr_copy_start;
    uintptr_t vaddr_copy_end;
    uintptr_t vaddr_map_start;
    uintptr_t vaddr_map_end;

    /* mmap a page each time, len is 16M / 1M / 64K / 16K */

    /* mapped address must be aligned */
    vaddr_map_start = ALIGN_DOWN(vaddr_start, K_MMU_PAGE_ALIGN);
    vaddr_map_end   = ALIGN_UP(vaddr_end, K_MMU_PAGE_ALIGN);

    /* Loop once: complete the configuration of one page, and fill the page memory */
    for ( vaddr_once = vaddr_map_start; vaddr_once < vaddr_map_end; vaddr_once += len_once )
    {
        len_once   = 0;
        paddr_once = 0;

        /* try supersection */
        if ( len_once == 0
          && vaddr_once % MMU_SUPERSECTION_SIZE == 0
          && vaddr_once + MMU_SUPERSECTION_SIZE <= vaddr_map_end)
        {
            paddr_once = os_phymem_alloc(MMU_SUPERSECTION_SIZE);
            if ( paddr_once != 0 )
            {
                len_once = MMU_SUPERSECTION_SIZE;
            }
        }

        /* try section */
        if ( len_once == 0
          && vaddr_once % MMU_SECTION_SIZE == 0
          && vaddr_once + MMU_SECTION_SIZE <= vaddr_map_end)
        {
            paddr_once = os_phymem_alloc(MMU_SECTION_SIZE);
            if ( paddr_once != 0 )
            {
                len_once = MMU_SECTION_SIZE;
            }
        }

        /* try large page */
        if ( len_once == 0
          && vaddr_once % MMU_LARGEPAGE_SIZE == 0
          && vaddr_once + MMU_LARGEPAGE_SIZE <= vaddr_map_end)
        {
            paddr_once = os_phymem_alloc(MMU_LARGEPAGE_SIZE);
            if ( paddr_once != 0 )
            {
                len_once = MMU_LARGEPAGE_SIZE;
            }
        }

        /* try small page */
        if ( len_once == 0
          && vaddr_once % MMU_SMALLPAGE_SIZE == 0
          && vaddr_once + MMU_SMALLPAGE_SIZE <= vaddr_map_end)
        {
            paddr_once = os_phymem_alloc(MMU_SMALLPAGE_SIZE);
            if ( paddr_once != 0 )
            {
                len_once = MMU_SMALLPAGE_SIZE;
            }
        }

        if ( len_once == 0 )
        {
            vaddr_map_end = vaddr_once;
            goto free_phymem;
        }

        LOGD("k_proc", "pid %d map v 0x%x-0x%x -> p 0x%x-0x%x, size 0x%x\r\n",
             g_proc_now->pid, vaddr_once, vaddr_once + len_once,
             paddr_once, paddr_once + len_once, len_once);

        ret = os_proc_create_mapping(vaddr_once, paddr_once, len_once,
                                     seg_type, g_proc_now->pid);
        if ( ret != 0 )
        {
            return ret;
        }

        memset((void *)vaddr_once, 0, len_once);

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
            memcpy((char *)vaddr_copy_start,
                   (char*)copy_from + (vaddr_copy_start - vaddr_start),
                   vaddr_copy_end - vaddr_copy_start);
        }
    }

    return 0;

free_phymem:
    os_proc_mmap_free(vaddr_map_start, vaddr_map_end, g_proc_now->pid);
    return -__LINE__;
}

void *os_proc_file2ram(const char *path)
{
#if (K_CONFIG_RAM_LOAD == 1)
    return (void *)path;
#else
    uint32_t read_len = 0,read_count = 0;
    int fd, file_len = 0;
    void *pbuff;

    fd = fs_open(path, 0, 0);
    if (fd < 0)
    {
        return NULL;
    }

    file_len = fs_lseek(fd, 0, SEEK_END);
    fs_lseek(fd, 0, SEEK_SET);

    if (file_len == 0)
    {
        fs_close(fd);
        return NULL;
    }

    pbuff = krhino_mm_alloc(file_len);
    if (pbuff == NULL)
    {
        fs_close(fd);
        return NULL;
    }

    do{
        read_len = fs_read(fd, (void *)((uint32_t)pbuff + (uint32_t)read_len), file_len);
        read_count += read_len;
    } while (read_len > 0);
    fs_close(fd);
    if (read_count != file_len)
    {
        return NULL;
    }

    fs_close(fd);
    fd = -1;

    return pbuff;
#endif
}

int32_t os_proc_check(Elf32_Ehdr *ehdr)
{
    if ( (ehdr->e_ident[EI_MAG0] != ELFMAG0)
      || (ehdr->e_ident[EI_MAG1] != ELFMAG1)
      || (ehdr->e_ident[EI_MAG2] != ELFMAG2)
      || (ehdr->e_ident[EI_MAG3] != ELFMAG3))
    {
        return -__LINE__;
    }

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS32)
    {
        return -__LINE__;
    }

    if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        return -__LINE__;
    }

    if (ehdr->e_machine != EM_ARM)
    {
        return -__LINE__;
    }

    return  0;
}

int32_t os_proc_check_overlap(Elf32_Ehdr *ehdr)
{
    int32_t i, ret;
    Elf32_Phdr *phdr;
    kproc_app_t *proc;
    preamble_t *preamble = NULL;
    uintptr_t text_start;
    uintptr_t text_end;
    uintptr_t data_start;
    uintptr_t data_end;
    uintptr_t zero_start;
    uintptr_t zero_end;
    uintptr_t temp_min, temp_max;
    uintptr_t cur_min, cur_max;

    if (ehdr->e_machine != EM_ARM)
    {
        return -__LINE__;
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
                return -__LINE__;
            }
        }
    }

    if ( preamble == NULL || preamble->magicword != PREAMBLE_MAGIC )
    {
        /* can not find preamble_t */
        return -__LINE__;
    }

    if (!((uintptr_t)preamble->text_start <= (uintptr_t)preamble->text_end
       && (uintptr_t)preamble->text_end <=(uintptr_t) preamble->data_start
       && (uintptr_t)preamble->data_start <= (uintptr_t)preamble->data_end
       && (uintptr_t)preamble->data_end <= (uintptr_t)preamble->zero_start
       && (uintptr_t)preamble->zero_start <= (uintptr_t)preamble->zero_end) )
    {
        /* preamble_t error */
        return -__LINE__;
    }

    cur_min = (preamble->text_start < preamble->data_start)? (uintptr_t)preamble->text_start: (uintptr_t)preamble->data_start;
    cur_min = (cur_min < (uintptr_t)preamble->zero_start)? cur_min: (uintptr_t)preamble->zero_start;
    cur_max = (preamble->text_end > preamble->data_end)? (uintptr_t)preamble->text_end: (uintptr_t)preamble->data_end;
    cur_max = (cur_max > (uintptr_t)preamble->zero_end)? cur_max: (uintptr_t)preamble->zero_end;

    for (i = 1; i < ARM_MAX_ASIDS; i++)
    {
        proc = &g_proc_info.procs[i];
    if (proc->name == NULL)
            continue;

    text_start = (uintptr_t)proc->preamble.text_start;
    text_end   = (uintptr_t)proc->preamble.text_end;
    data_start = (uintptr_t)proc->preamble.data_start;
    data_end   = (uintptr_t)proc->preamble.data_end;
    zero_start = (uintptr_t)proc->preamble.zero_start;
    zero_end   = (uintptr_t)proc->preamble.zero_end;

    temp_min = (text_start < data_start)? text_start: data_start;
    temp_min = (temp_min < zero_start)? temp_min: zero_start;
    temp_max = (text_end > data_end)? text_end: data_end;
    temp_max = (temp_max > zero_end)? temp_max: zero_end;

    if ((cur_min >= temp_min && cur_min <= temp_max)
            || (cur_max >= temp_min && cur_max <= temp_max))
    {
            return -__LINE__;
        }
    }

    return 0;
}


int32_t os_proc_load(kproc_app_t *proc, Elf32_Ehdr *ehdr)
{
    int32_t i, ret;
    Elf32_Phdr *phdr;
    preamble_t *preamble = NULL;
    uintptr_t   copy_from;

    if (ehdr->e_machine != EM_ARM)
    {
        return -__LINE__;
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
                return -__LINE__;
            }
        }
    }
    if ( preamble == NULL || preamble->magicword != PREAMBLE_MAGIC )
    {
        /* can not find preamble_t */
        return -__LINE__;
    }

    memcpy(&proc->preamble, preamble, sizeof(proc->preamble));

    if (!((uintptr_t)preamble->text_start <= (uintptr_t)preamble->text_end
       && (uintptr_t)preamble->text_end <=(uintptr_t) preamble->data_start
       && (uintptr_t)preamble->data_start <= (uintptr_t)preamble->data_end
       && (uintptr_t)preamble->data_end <= (uintptr_t)preamble->zero_start
       && (uintptr_t)preamble->zero_start <= (uintptr_t)preamble->zero_end) )
    {
        /* preamble_t error */
        return -__LINE__;
    }

    // TODO: There is a bug if the image is compiled by armcc
#if 0
    if ( (uintptr_t)preamble->text_start < phdr->p_vaddr
      || (uintptr_t)preamble->data_end   > phdr->p_vaddr + phdr->p_filesz)
    {
        /* loadable segment not match the phdr */
        return -__LINE__;
    }
#endif

    copy_from = (uintptr_t)ehdr + phdr->p_offset;

    /* are text and data stick togather? */
    if ( ((uintptr_t)preamble->text_end) / K_MMU_PAGE_ALIGN ==
         ((uintptr_t)preamble->data_start) / K_MMU_PAGE_ALIGN )
    {
        /* text and data not separated, load togather */
        ret = os_proc_mmap_copy((uintptr_t)preamble->text_start,
                                (uintptr_t)preamble->zero_end,
                                (void *)(copy_from + (uintptr_t)preamble->text_start - phdr->p_vaddr),
                                (uintptr_t)preamble->data_end - (uintptr_t)preamble->text_start,
                                SEG_EXE_DATA);
        if ( ret != 0 )
        {
            return ret;
        }
    }
    else
    {
        /* load text */
        ret = os_proc_mmap_copy((uintptr_t)preamble->text_start,
                                (uintptr_t)preamble->text_end,
                                (void *)(copy_from + (uintptr_t)preamble->text_start - phdr->p_vaddr),
                                (uintptr_t)preamble->text_end - (uintptr_t)preamble->text_start,
                                SEG_EXE);

        if ( ret != 0 )
        {
            return ret;
        }

        /* load data & bss */
        ret = os_proc_mmap_copy((uintptr_t)preamble->data_start,
                                (uintptr_t)preamble->zero_end,
                                (void *)(copy_from + (uintptr_t)preamble->data_start - phdr->p_vaddr),
                                (uintptr_t)preamble->data_end - (uintptr_t)preamble->data_start,
                                SEG_DATA);
        if ( ret != 0 )
        {
            return ret;
        }
    }

    return 0;
}

void os_proc_page_free(uintptr_t vaddr, uintptr_t paddr, size_t len, int32_t isKernel)
{
    if ( isKernel == 0 && os_phymem_check_phy(paddr) )
    {
        os_phymem_free(paddr, len);
    }
}

int32_t os_proc_unload(kproc_app_t *proc)
{
    preamble_t *preamble;
    uintptr_t vaddr_start, vaddr_end;
    size_t   size;

    if ( proc == NULL )
    {
        return -__LINE__;
    }

    if ( proc->name != NULL )
    {
        /* munmap and recycle physical pages */
        //k_mmu_munmap_all(proc->pid, os_proc_page_free);

        preamble = &proc->preamble;

        if ( ((uintptr_t)preamble->text_end) / K_MMU_PAGE_ALIGN ==
             ((uintptr_t)preamble->data_start) / K_MMU_PAGE_ALIGN ) {
            vaddr_start = (uintptr_t)preamble->text_start;
            vaddr_end   = (uintptr_t)preamble->zero_end;
            os_proc_mmap_free(vaddr_start, vaddr_end, proc->pid);
        } else {
            // 1. text segment
            vaddr_start = (uintptr_t)preamble->text_start;
            vaddr_end   = (uintptr_t)preamble->text_end;
            os_proc_mmap_free(vaddr_start, vaddr_end, proc->pid);
            // 2. data segment
            vaddr_start = (uintptr_t)preamble->data_start;
            vaddr_end   = (uintptr_t)preamble->zero_end;
            os_proc_mmap_free(vaddr_start, vaddr_end, proc->pid);
        }

        LOGI("k_proc", "free pid %d\r\n", proc->pid);
        os_dealloc_asid(proc->pid);
        proc->name = NULL;
    }

    return 0;
}

/* Initialize multi-process,
   The physical memory pool is shared between all process */
int32_t os_proc_init(uintptr_t paddr_pool, size_t len)
{
    int32_t i;

    /* 1. init apps info */
    memset(&g_proc_info, 0, sizeof(g_proc_info));
    for ( i = 0 ; i < ARM_MAX_ASIDS ; i++ )
    {
        g_proc_info.procs[i].pid  = i;
        g_proc_info.procs[i].name = NULL;
    }

    /* proc[0] as kernel */
    g_proc_info.procs[0].name = "kernel";
    g_proc_now = &g_proc_info.procs[0];

    /* 2. init physical mem pool */
    if ( paddr_pool % K_MMU_PAGE_ALIGN != 0
      || len % K_MMU_PAGE_ALIGN != 0)
    {
        return -__LINE__;
    }

    g_proc_info.phymem.paddr_start = paddr_pool;
    g_proc_info.phymem.len         = len;
    os_phymm_init(&g_proc_info.phymem.info, paddr_pool, len);

    return 0;
}

int32_t k_proc_init()
{
    uint32_t phyaddr, size;

    phyaddr = (uint32_t)os_get_phymm_start();
    size    = (uint32_t)os_get_phymm_size();
    return os_proc_init(phyaddr, size);
}

/* Load a proc image,
   The format must be excutable elf object
   K_CONFIG_RAM_LOAD == 1: "file" is pointing to the ram file
   K_CONFIG_RAM_LOAD == 0: "file" is the path to the file system */
int32_t k_proc_load(const char *file, const char *name)
{
    int32_t pid, cur_pid;
    int32_t ret;
    kstat_t state;
    Elf32_Ehdr  *ehdr = NULL;
    kproc_app_t *proc = NULL;

    CPSR_ALLOC();

    /* 1. load image to ram */
    ehdr = (Elf32_Ehdr *)os_proc_file2ram(file);
    if ( ehdr == NULL )
    {
        ret = -__LINE__;
        goto _exit1;
    }

    /* 2. iamge check */
    if ( os_proc_check(ehdr) != 0 )
    {
        LOGD("k_proc", "ehdr check faile\r\n");
        ret = -__LINE__;
        goto _exit1;
    }

    if ( os_proc_check_overlap(ehdr) != 0 )
    {
        LOGD("k_proc", "proc overlap\r\n");
        ret = -__LINE__;
        goto _exit1;
    }

    /* 3. alloc a proc info */

    pid = os_alloc_asid();
    if (pid >= ARM_MAX_ASIDS) {
        os_dealloc_asid(pid);
        ret = -__LINE__;
        goto _exit1;
    }
    proc = &g_proc_info.procs[pid];
    proc->entry = (void *)ehdr->e_entry;
    proc->name  = name == NULL ? "unnamed" : name;

    /* 4. create page table and switch*/
    // int os_copy_kernel_pgtble(int pid)
    // success: return 0;
    // fail none zero
    ret = os_copy_kernel_pgtble(pid);
    if  ( ret != 0 )
    {
        ret = -__LINE__;
        goto _exit1;
    }

    RHINO_CRITICAL_ENTER();
    g_sched_lock[cpu_cur_get()]++;
    RHINO_CRITICAL_EXIT();

    cur_pid = krhino_cur_task_get()->pid;
    k_proc_switch(pid);

    /* 5. iamge load */
    switch ( ehdr->e_type )
    {
        case ET_EXEC :
            ret = os_proc_load(proc, ehdr);
            if ( ret != 0 )
            {
                LOGD("k_proc", "os_proc_load failed\r\n");
                proc->name = NULL;
                os_dealloc_asid(proc->pid);
                goto _exit2;
            }
            break;
        case ET_REL :
        case ET_DYN :
        default:
            /* error type */
            ret = -__LINE__;
            goto _exit2;
    }

    /* 6. user tasks create */
    if ( proc->entry != NULL )
    {
        if (proc->preamble.debug_mode == 1) {
            krhino_breakpoint_set(proc->pid, (uint32_t)proc->preamble.app_entry);
        }

        state = krhino_uprocess_create((const name_t*)proc->preamble.app_name,
                                       proc->preamble.task_struct, "utask_entry", 0,
                                       proc->preamble.priority, (tick_t)0,
                                       proc->preamble.ustack,
                                       proc->preamble.ustack_size, // ustasck size
                                       proc->preamble.kstack_size, // kstack size
                                       (task_entry_t)proc->preamble.app_entry,
                                       proc->pid, 1);

        if (state != RHINO_SUCCESS) {
            ret = __LINE__;
            goto _exit2;
        }
    } else {
        LOGD("k_proc", "Invalid ELF format\r\n");
    }

    k_proc_switch(cur_pid);

#if (K_CONFIG_RAM_LOAD != 1)
    krhino_mm_free(ehdr);
#endif

    RHINO_CRITICAL_ENTER();
    g_sched_lock[cpu_cur_get()]--;
    RHINO_CRITICAL_EXIT();

    return pid;

_exit2:
    k_proc_switch(cur_pid);

    RHINO_CRITICAL_ENTER();
    g_sched_lock[cpu_cur_get()]--;
    RHINO_CRITICAL_EXIT();

_exit1:
    if ( ehdr != NULL )
    {
#if (K_CONFIG_RAM_LOAD != 1)
        krhino_mm_free(ehdr);
#endif
    }
    if ( proc != NULL )
    {
        os_proc_unload(proc);
    }
    return ret;
}

int32_t k_proc_unload(int32_t pid)
{
    if ( pid <= 0 || pid >= ARM_MAX_ASIDS )
    {
        return -__LINE__;
    }

    if ( g_proc_info.procs[pid].pid != pid )
    {
        return -__LINE__;
    }

    return os_proc_unload(&g_proc_info.procs[pid]);
}

int32_t k_proc_switch(int32_t pid)
{
    /*
    Each process maintains a separate first-level translation table. On a context switch:
    • TTBR0 is updated to point to the first-level translation table for the new context
    • TTBCR is updated if this change changes the size of the translation table
    • the CONTEXTIDR is updated.
    */

    /* pid as asid */
    if ( pid >= ARM_MAX_ASIDS )
    {
        return -__LINE__;
    }

    os_set_CONTEXTIDR(pid);
    /* the viraddr of pgtable is equal with phyaddr */
    os_set_TTBR0((uintptr_t)os_get_mmu_pgtable(pid));

    OS_DSB();
    OS_ISB();

    g_proc_now = &g_proc_info.procs[pid];

    return 0;
}

void k_proc_show(void)
{
    kphymm_info_t *pmm;
    kproc_app_t   *papp;
    int32_t pid;

    pmm = &g_proc_info.phymem.info;
    LOGI("k_proc", "[k_proc_show] Physical Memory Pool:\n");
    LOGI("k_proc", "    phy_addr      = 0x%x\n", pmm->phy_addr);
    LOGI("k_proc", "    total_size    = 0x%x\n", pmm->total_size);
    LOGI("k_proc", "    free_size     = 0x%x\n", pmm->free_size);
    LOGI("k_proc", "    free_size_min = 0x%x\n", pmm->free_size_min);

    LOGI("k_proc", "[k_proc_show] Process Info:\n");
    for ( pid = 0 ; pid < ARM_MAX_ASIDS ; pid++ )
    {
        papp = &g_proc_info.procs[pid];
        if ( papp->name != NULL )
        {
            LOGI("k_proc", "  PID (%d %s):\n", papp->pid, papp->name);
            LOGI("k_proc", "    Entry = 0x%x\n", papp->entry);
        }
    }
}

kproc_app_t *kproc_app_get(int pid)
{
    return &g_proc_info.procs[pid];
}

int proc_address_access(int pid, unsigned long addr, unsigned long size)
{
    kproc_app_t *app;
    preamble_t *preamble;

    if (addr == 0 || size == 0) {
        return 0;
    }

    app = &g_proc_info.procs[pid];

    if (app->pid != (int32_t)pid) {
        return 0;
    }

    preamble = &app->preamble;

    if (addr > (unsigned long)preamble->data_start
    && (addr + size) < (unsigned long)preamble->zero_end) {
        return 1;
    }

    return 0;
}


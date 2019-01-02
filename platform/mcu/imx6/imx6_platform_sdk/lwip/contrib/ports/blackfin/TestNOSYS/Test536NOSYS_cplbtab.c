/* MANAGED-BY-SYSTEM-BUILDER                                    */
/* VisualDSP++ 5.0 Update 10                                    */
/* LDF Printer version: 5.9.0.1                                 */
/* ldfgen.exe version: 5.9.0.1                                  */
/* VDSG version: 5.9.0.1                                        */

/*
** CPLB table definitions generated on Dec 21, 2012 at 17:25:13.
**
** Copyright (C) 2000-2010 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected
** in the LDF Wizard. Changes to the LDF configuration should be made by
** changing the appropriate options rather than editing this file.
**
** Configuration:-
**     crt_doj:                                Test536NOSYS_basiccrt.doj
**     processor:                              ADSP-BF536
**     product_name:                           VisualDSP++ 5.0 Update 10
**     si_revision:                            0.3
**     default_silicon_revision_from_archdef:  0.3
**     cplb_init_cplb_ctrl:                    123
**     cplb_init_dcache_ctrl:                  dcache_a_wb
**     cplb_init_cplb_src_file:                C:\Dev\LwIP\contrib\ports\blackfin\TestNOSYS\Test536NOSYS_cplbtab.c
**     cplb_init_cplb_obj_file:                Test536NOSYS_cplbtab.doj
**     using_cplusplus:                        true
**     mem_init:                               false
**     use_vdk:                                false
**     use_mt:                                 false
**     use_eh:                                 true
**     use_argv:                               false
**     running_from_internal_memory:           true
**     user_heap_src_file:                     C:\Dev\LwIP\contrib\ports\blackfin\TestNOSYS\Test536NOSYS_heaptab.c
**     libraries_use_stdlib:                   true
**     libraries_use_fileio_libs:              false
**     libraries_use_ieeefp_emulation_libs:    false
**     libraries_use_eh_enabled_libs:          false
**     libraries_use_fixed_point_io_libs:      false
**     libraries_use_utility_rom:              true
**     detect_stackoverflow:                   false
**     system_heap:                            L3
**     system_heap_size:                       4095k
**     system_stack:                           L1
**     system_stack_min_size:                  2k
**     use_sdram:                              true
**     use_sdram_size:                         16MB
**     use_sdram_partitioned:                  default
**
*/


#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rule_2_2)
#pragma diag(suppress:misra_rule_8_10)
#pragma diag(suppress:misra_rule_10_1_a)
#endif /* _MISRA_RULES */



#define CACHE_MEM_MODE (CPLB_DDOCACHE_WB)



#include <sys/platform.h>
#include <cplbtab.h>

#pragma section("cplb_data")

cplb_entry dcplbs_table[] = {



/*$VDSG<customizable-data-cplb-table>                           */
/* This code is preserved if the CPLB tables are re-generated.  */


   // L1 Data A & B, (set write-through bit to avoid 1st write exceptions)
   {0xFF800000, (PAGE_SIZE_4MB | CPLB_DNOCACHE | CPLB_LOCK | CPLB_WT)}, 

   // Async Memory Bank 2 (Second)
   // Async Memory Bank 1 (Prim B)
   // Async Memory Bank 0 (Prim A)
   {0x20200000, (PAGE_SIZE_1MB | CPLB_DNOCACHE)}, 
   {0x20100000, (PAGE_SIZE_1MB | CACHE_MEM_MODE)}, 
   {0x20000000, (PAGE_SIZE_1MB | CACHE_MEM_MODE)}, 

      // 128 MB (Maximum) SDRAM memory space 
   {0x00000000, (PAGE_SIZE_4MB | CACHE_MEM_MODE | CPLB_DIRTY | CPLB_LOCK)}, 
   {0x00400000, (PAGE_SIZE_4MB | CACHE_MEM_MODE | CPLB_DIRTY | CPLB_LOCK)}, 
   {0x00800000, (PAGE_SIZE_4MB | CACHE_MEM_MODE | CPLB_DIRTY | CPLB_LOCK)}, 
   {0x00C00000, (PAGE_SIZE_4MB | CACHE_MEM_MODE | CPLB_DIRTY | CPLB_LOCK)}, 
   
   // CPLBs covering 1MB
   {0x01000000, (PAGE_SIZE_1MB | CACHE_MEM_MODE)},

   // Async Memory Bank 3
   {0x20300000, (PAGE_SIZE_1MB | CPLB_DNOCACHE)}, 

   // end of section - termination
   {0xffffffff, 0}, 
/*$VDSG<customizable-data-cplb-table>                           */


}; /* dcplbs_table */

#pragma section("cplb_data")

cplb_entry icplbs_table[] = {



/*$VDSG<customizable-instr-cplb-table>                          */
/* This code is preserved if the CPLB tables are re-generated.  */


   // L1 Code
   {0xFFA00000, (PAGE_SIZE_1MB | CPLB_I_PAGE_MGMT)}, 

   // Async Memory Bank 2 (Secnd)
   // Async Memory Bank 1 (Prim B)
   // Async Memory Bank 0 (Prim A)
   {0x20200000, (PAGE_SIZE_1MB | CPLB_INOCACHE)}, 
   {0x20100000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)}, 
   {0x20000000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)}, 

      // 512 MB (Maximum) SDRAM memory space 
   
   // CPLBs covering 16MB
   {0x00000000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00100000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00200000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00300000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00400000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00500000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00600000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00700000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00800000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00900000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00a00000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00b00000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00c00000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00d00000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00e00000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},
   {0x00f00000, (PAGE_SIZE_1MB | CPLB_IDOCACHE)},

   // Async Memory Bank 3 
   {0x20300000, (PAGE_SIZE_1MB | CPLB_INOCACHE)}, 

   // end of section - termination
   {0xffffffff, 0}, 
/*$VDSG<customizable-instr-cplb-table>                          */


}; /* icplbs_table */



#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */



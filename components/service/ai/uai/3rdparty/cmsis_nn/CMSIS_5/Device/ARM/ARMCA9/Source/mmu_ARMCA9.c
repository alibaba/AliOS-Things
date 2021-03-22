/**************************************************************************//**
 * @file     mmu_ARMCA9.c
 * @brief    MMU Configuration for Arm Cortex-A9 Device Series
 * @version  V1.2.0
 * @date     15. May 2019
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Memory map description from: DUI0447G_v2m_p1_trm.pdf 4.2.2 Arm Cortex-A Series memory map

                                                     Memory Type
0xffffffff |--------------------------|             ------------
           |       FLAG SYNC          |             Device Memory
0xfffff000 |--------------------------|             ------------
           |         Fault            |                Fault
0xfff00000 |--------------------------|             ------------
           |                          |                Normal
           |                          |
           |      Daughterboard       |
           |         memory           |
           |                          |
0x80505000 |--------------------------|             ------------
           |TTB (L2 Sync Flags   ) 4k |                Normal
0x80504C00 |--------------------------|             ------------
           |TTB (L2 Peripherals-B) 16k|                Normal
0x80504800 |--------------------------|             ------------
           |TTB (L2 Peripherals-A) 16k|                Normal
0x80504400 |--------------------------|             ------------
           |TTB (L2 Priv Periphs)  4k |                Normal
0x80504000 |--------------------------|             ------------
           |    TTB (L1 Descriptors)  |                Normal
0x80500000 |--------------------------|             ------------
           |          Stack           |                Normal
           |--------------------------|             ------------
           |          Heap            |                Normal
0x80400000 |--------------------------|             ------------
           |         ZI Data          |                Normal
0x80300000 |--------------------------|             ------------
           |         RW Data          |                Normal
0x80200000 |--------------------------|             ------------
           |         RO Data          |                Normal
           |--------------------------|             ------------
           |         RO Code          |              USH Normal
0x80000000 |--------------------------|             ------------
           |      Daughterboard       |                Fault
           |      HSB AXI buses       |
0x40000000 |--------------------------|             ------------
           |      Daughterboard       |                Fault
           |  test chips peripherals  |
0x2c002000 |--------------------------|             ------------
           |     Private Address      |            Device Memory
0x2c000000 |--------------------------|             ------------
           |      Daughterboard       |                Fault
           |  test chips peripherals  |
0x20000000 |--------------------------|             ------------
           |       Peripherals        |           Device Memory RW/RO
           |                          |              & Fault
0x00000000 |--------------------------|
*/

// L1 Cache info and restrictions about architecture of the caches (CCSIR register):
// Write-Through support *not* available
// Write-Back support available.
// Read allocation support available.
// Write allocation support available.

//Note: You should use the Shareable attribute carefully.
//For cores without coherency logic (such as SCU) marking a region as shareable forces the processor to not cache that region regardless of the inner cache settings.
//Cortex-A versions of RTX use LDREX/STREX instructions relying on Local monitors. Local monitors will be used only when the region gets cached, regions that are not cached will use the Global Monitor.
//Some Cortex-A implementations do not include Global Monitors, so wrongly setting the attribute Shareable may cause STREX to fail.

//Recall: When the Shareable attribute is applied to a memory region that is not Write-Back, Normal memory, data held in this region is treated as Non-cacheable.
//When SMP bit = 0, Inner WB/WA Cacheable Shareable attributes are treated as Non-cacheable.
//When SMP bit = 1, Inner WB/WA Cacheable Shareable attributes are treated as Cacheable.


//Following MMU configuration is expected
//SCTLR.AFE == 1 (Simplified access permissions model - AP[2:1] define access permissions, AP[0] is an access flag)
//SCTLR.TRE == 0 (TEX remap disabled, so memory type and attributes are described directly by bits in the descriptor)
//Domain 0 is always the Client domain
//Descriptors should place all memory in domain 0

#include "ARMCA9.h"
#include "mem_ARMCA9.h"

// TTB base address
#define TTB_BASE ((uint32_t*)__TTB_BASE)

// L2 table pointers
//----------------------------------------
#define TTB_L1_SIZE                    (0x00004000)                        // The L1 translation table divides the full 4GB address space of a 32-bit core
                                                                           // into 4096 equally sized sections, each of which describes 1MB of virtual memory space.
                                                                           // The L1 translation table therefore contains 4096 32-bit (word-sized) entries.

#define PRIVATE_TABLE_L2_BASE_4k       (__TTB_BASE + TTB_L1_SIZE)          // Map 4k Private Address space
#define PERIPHERAL_A_TABLE_L2_BASE_64k (__TTB_BASE + TTB_L1_SIZE + 0x400)  // Map 64k Peripheral #1 0x1C000000 - 0x1C00FFFFF
#define PERIPHERAL_B_TABLE_L2_BASE_64k (__TTB_BASE + TTB_L1_SIZE + 0x800)  // Map 64k Peripheral #2 0x1C100000 - 0x1C1FFFFFF
#define SYNC_FLAGS_TABLE_L2_BASE_4k    (__TTB_BASE + TTB_L1_SIZE + 0xC00)  // Map 4k Flag synchronization

//--------------------- PERIPHERALS -------------------
#define PERIPHERAL_A_FAULT (0x00000000 + 0x1c000000) //0x1C000000-0x1C00FFFF (1M)
#define PERIPHERAL_B_FAULT (0x00100000 + 0x1c000000) //0x1C100000-0x1C10FFFF (1M)

//--------------------- SYNC FLAGS --------------------
#define FLAG_SYNC     0xFFFFF000
#define F_SYNC_BASE   0xFFF00000  //1M aligned

static uint32_t Sect_Normal;     //outer & inner wb/wa, non-shareable, executable, rw, domain 0, base addr 0
static uint32_t Sect_Normal_Cod; //outer & inner wb/wa, non-shareable, executable, ro, domain 0, base addr 0
static uint32_t Sect_Normal_RO;  //as Sect_Normal_Cod, but not executable
static uint32_t Sect_Normal_RW;  //as Sect_Normal_Cod, but writeable and not executable
static uint32_t Sect_Device_RO;  //device, non-shareable, non-executable, ro, domain 0, base addr 0
static uint32_t Sect_Device_RW;  //as Sect_Device_RO, but writeable

/* Define global descriptors */
static uint32_t Page_L1_4k  = 0x0;  //generic
static uint32_t Page_L1_64k = 0x0;  //generic
static uint32_t Page_4k_Device_RW;  //Shared device, not executable, rw, domain 0
static uint32_t Page_64k_Device_RW; //Shared device, not executable, rw, domain 0

void MMU_CreateTranslationTable(void)
{
    mmu_region_attributes_Type region;

    //Create 4GB of faulting entries
    MMU_TTSection (TTB_BASE, 0, 4096, DESCRIPTOR_FAULT);

    /*
     * Generate descriptors. Refer to core_ca.h to get information about attributes
     *
     */
    //Create descriptors for Vectors, RO, RW, ZI sections
    section_normal(Sect_Normal, region);
    section_normal_cod(Sect_Normal_Cod, region);
    section_normal_ro(Sect_Normal_RO, region);
    section_normal_rw(Sect_Normal_RW, region);
    //Create descriptors for peripherals
    section_device_ro(Sect_Device_RO, region);
    section_device_rw(Sect_Device_RW, region);
    //Create descriptors for 64k pages
    page64k_device_rw(Page_L1_64k, Page_64k_Device_RW, region);
    //Create descriptors for 4k pages
    page4k_device_rw(Page_L1_4k, Page_4k_Device_RW, region);


    /*
     *  Define MMU flat-map regions and attributes
     *
     */

    //Define Image
    MMU_TTSection (TTB_BASE, __ROM_BASE, __ROM_SIZE/0x100000, Sect_Normal_Cod); // multiple of 1MB sections
    MMU_TTSection (TTB_BASE, __RAM_BASE, __RAM_SIZE/0x100000, Sect_Normal_RW);  // multiple of 1MB sections

    //--------------------- PERIPHERALS -------------------
    MMU_TTSection (TTB_BASE, VE_A9_MP_FLASH_BASE0   , 64, Sect_Device_RO); // 64MB NOR
    MMU_TTSection (TTB_BASE, VE_A9_MP_FLASH_BASE1   , 64, Sect_Device_RO); // 64MB NOR
    MMU_TTSection (TTB_BASE, VE_A9_MP_SRAM_BASE     , 32, Sect_Device_RW); // 32MB RAM
    MMU_TTSection (TTB_BASE, VE_A9_MP_VRAM_BASE     , 32, Sect_Device_RW); // 32MB RAM
    MMU_TTSection (TTB_BASE, VE_A9_MP_ETHERNET_BASE , 16, Sect_Device_RW);
    MMU_TTSection (TTB_BASE, VE_A9_MP_USB_BASE      , 16, Sect_Device_RW);

    // Create (16 * 64k)=1MB faulting entries to cover peripheral range 0x1C000000-0x1C00FFFF
    MMU_TTPage64k(TTB_BASE, PERIPHERAL_A_FAULT      , 16, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, DESCRIPTOR_FAULT);
    // Define peripheral range 0x1C000000-0x1C00FFFF
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_DAP_BASE       ,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_SYSTEM_REG_BASE,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_SERIAL_BASE    ,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_AACI_BASE      ,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_MMCI_BASE      ,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_KMI0_BASE      ,  2, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_UART_BASE      ,  4, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_WDT_BASE       ,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);

    // Create (16 * 64k)=1MB faulting entries to cover peripheral range 0x1C100000-0x1C10FFFF
    MMU_TTPage64k(TTB_BASE, PERIPHERAL_B_FAULT      , 16, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, DESCRIPTOR_FAULT);
    // Define peripheral range 0x1C100000-0x1C10FFFF
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_TIMER_BASE     ,  2, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_DVI_BASE       ,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_RTC_BASE       ,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_UART4_BASE     ,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);
    MMU_TTPage64k(TTB_BASE, VE_A9_MP_CLCD_BASE      ,  1, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);

    // Create (256 * 4k)=1MB faulting entries to cover private address space. Needs to be marked as Device memory
    MMU_TTPage4k (TTB_BASE, __get_CBAR()            ,256,  Page_L1_4k, (uint32_t *)PRIVATE_TABLE_L2_BASE_4k, DESCRIPTOR_FAULT);
    // Define private address space entry.
    MMU_TTPage4k (TTB_BASE, __get_CBAR()            ,  2,  Page_L1_4k, (uint32_t *)PRIVATE_TABLE_L2_BASE_4k, Page_4k_Device_RW);
    // Define L2CC entry.  Uncomment if PL310 is present
    //    MMU_TTPage4k (TTB_BASE, VE_A5_MP_PL310_BASE     ,  1,  Page_L1_4k, (uint32_t *)PRIVATE_TABLE_L2_BASE_4k, Page_4k_Device_RW);

    // Create (256 * 4k)=1MB faulting entries to synchronization space (Useful if some non-cacheable DMA agent is present in the SoC)
    MMU_TTPage4k (TTB_BASE, F_SYNC_BASE             , 256, Page_L1_4k, (uint32_t *)SYNC_FLAGS_TABLE_L2_BASE_4k, DESCRIPTOR_FAULT);
    // Define synchronization space entry.
    MMU_TTPage4k (TTB_BASE, FLAG_SYNC               ,   1, Page_L1_4k, (uint32_t *)SYNC_FLAGS_TABLE_L2_BASE_4k, Page_4k_Device_RW);

    /* Set location of level 1 page table
    ; 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
    ; 13:7  - 0x0
    ; 6     - IRGN[0] 0x1  (Inner WB WA)
    ; 5     - NOS     0x0  (Non-shared)
    ; 4:3   - RGN     0x01 (Outer WB WA)
    ; 2     - IMP     0x0  (Implementation Defined)
    ; 1     - S       0x0  (Non-shared)
    ; 0     - IRGN[1] 0x0  (Inner WB WA) */
    __set_TTBR0(__TTB_BASE | 0x48);
    __ISB();

    /* Set up domain access control register
    ; We set domain 0 to Client and all other domains to No Access.
    ; All translation table entries specify domain 0 */
    __set_DACR(1);
    __ISB();
}

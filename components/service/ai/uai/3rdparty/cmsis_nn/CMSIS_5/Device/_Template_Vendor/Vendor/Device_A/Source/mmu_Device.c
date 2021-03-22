/**************************************************************************//**
 * @file     system_Device.c
 * @brief    MMU Configuration
 *           Device <DeviceAbbreviation>
 * @version  V1.1.0
 * @date     23. November 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
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

/* Memory map description

   ToDo: add in this file your device memory map description
         following is an example of a Cortex-A9 Arm FVP device

                                                     Memory Type
0xFFFFFFFF |--------------------------|             ------------
           |       FLAG SYNC          |             Device Memory
0xFFFFF000 |--------------------------|             ------------
           |         Fault            |                Fault
0xFFF00000 |--------------------------|             ------------
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
           |           Heap           |                Normal
           |--------------------------|             ------------
           |          Stack           |                Normal
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
0x2C002000 |--------------------------|             ------------
           |     Private Address      |            Device Memory
0x2C000000 |--------------------------|             ------------
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

// Note: You should use the Shareable attribute carefully.
// For cores without coherency logic (such as SCU) marking a region as shareable forces the processor to not cache that region regardless of the inner cache settings.
// Cortex-A versions of RTX use LDREX/STREX instructions relying on Local monitors. Local monitors will be used only when the region gets cached, regions that are not cached will use the Global Monitor.
// Some Cortex-A implementations do not include Global Monitors, so wrongly setting the attribute Shareable may cause STREX to fail.
   
// Recall: When the Shareable attribute is applied to a memory region that is not Write-Back, Normal memory, data held in this region is treated as Non-cacheable.
// When SMP bit = 0, Inner WB/WA Cacheable Shareable attributes are treated as Non-cacheable.
// When SMP bit = 1, Inner WB/WA Cacheable Shareable attributes are treated as Cacheable.
   
// Following MMU configuration is expected
// SCTLR.AFE == 1 (Simplified access permissions model - AP[2:1] define access permissions, AP[0] is an access flag)
// SCTLR.TRE == 0 (TEX remap disabled, so memory type and attributes are described directly by bits in the descriptor)
// Domain 0 is always the Client domain
// Descriptors should place all memory in domain 0

#include "<Device>.h" /* ToDo: replace '<Device>' with your device name */

// L2 table pointers
//-----------------------------------------------------
#define PRIVATE_TABLE_L2_BASE_4k       (0x80504000) //Map 4k Private Address space
#define SYNC_FLAGS_TABLE_L2_BASE_4k    (0x80504C00) //Map 4k Flag synchronization
#define PERIPHERAL_A_TABLE_L2_BASE_64k (0x80504400) //Map 64k Peripheral #1 
#define PERIPHERAL_B_TABLE_L2_BASE_64k (0x80504800) //Map 64k Peripheral #2 

//--------------------- PERIPHERALS -------------------
#define PERIPHERAL_A_FAULT             (0x00000000 + 0x1C000000) 
#define PERIPHERAL_B_FAULT             (0x00100000 + 0x1C000000) 

//--------------------- SYNC FLAGS --------------------
#define FLAG_SYNC                       0xFFFFF000
#define F_SYNC_BASE                     0xFFF00000  //1M aligned

//Import symbols from linker
extern uint32_t Image$$VECTORS$$Base;
extern uint32_t Image$$RW_DATA$$Base;
extern uint32_t Image$$ZI_DATA$$Base;
extern uint32_t Image$$TTB$$ZI$$Base;

static uint32_t Sect_Normal;        // outer & inner wb/wa, non-shareable, executable, rw, domain 0, base addr 0
static uint32_t Sect_Normal_Cod;    // outer & inner wb/wa, non-shareable, executable, ro, domain 0, base addr 0
static uint32_t Sect_Normal_RO;     // as Sect_Normal_Cod, but not executable
static uint32_t Sect_Normal_RW;     // as Sect_Normal_Cod, but writeable and not executable
static uint32_t Sect_Device_RO;     // device, non-shareable, non-executable, ro, domain 0, base addr 0
static uint32_t Sect_Device_RW;     // as Sect_Device_RO, but writeable
                                       
/* Define global descriptors */        
static uint32_t Page_L1_4k  = 0x0;  // generic
static uint32_t Page_L1_64k = 0x0;  // generic
static uint32_t Page_4k_Device_RW;  // shared device, not executable, rw, domain 0
static uint32_t Page_64k_Device_RW; // shared device, not executable, rw, domain 0

void MMU_CreateTranslationTable(void)
{
  mmu_region_attributes_Type region;

  // Create 4GB of faulting entries
  MMU_TTSection (&Image$$TTB$$ZI$$Base, 0, 4096, DESCRIPTOR_FAULT);

  /*
   * Generate descriptors. Refer to core_ca.h to get information about attributes
   *
   */
  // Create descriptors for Vectors, RO, RW, ZI sections
  section_normal(Sect_Normal, region);
  section_normal_cod(Sect_Normal_Cod, region);
  section_normal_ro(Sect_Normal_RO, region);
  section_normal_rw(Sect_Normal_RW, region);
  // Create descriptors for peripherals
  section_Device_ro(Sect_Device_RO, region);
  section_Device_rw(Sect_Device_RW, region);
  // Create descriptors for 64k pages
  page64k_Device_rw(Page_L1_64k, Page_64k_Device_RW, region);
  // Create descriptors for 4k pages
  page4k_Device_rw(Page_L1_4k, Page_4k_Device_RW, region);

  /*
   *  Define MMU flat-map regions and attributes
   *
   */
  // Define Image
  MMU_TTSection (&Image$$TTB$$ZI$$Base, (uint32_t)&Image$$VECTORS$$Base     ,    1U, Sect_Normal_Cod);
  MMU_TTSection (&Image$$TTB$$ZI$$Base, (uint32_t)&Image$$RW_DATA$$Base     ,    1U, Sect_Normal_RW);
  MMU_TTSection (&Image$$TTB$$ZI$$Base, (uint32_t)&Image$$ZI_DATA$$Base     ,    1U, Sect_Normal_RW);

  // All DRAM executable, RW, cacheable - applications may choose to divide memory into RO executable
  MMU_TTSection (&Image$$TTB$$ZI$$Base, (uint32_t)&Image$$TTB$$ZI$$Base     , 2043U, Sect_Normal);

  //--------------------- PERIPHERALS -------------------
  MMU_TTSection (&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_FLASH_BASE0    ,   64U, Sect_Device_RO);
  MMU_TTSection (&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_FLASH_BASE1    ,   64U, Sect_Device_RO);
  MMU_TTSection (&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_SRAM_BASE      ,   64U, Sect_Device_RW);
  MMU_TTSection (&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_VRAM_BASE      ,   32U, Sect_Device_RW);
  MMU_TTSection (&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_ETHERNET_BASE  ,   16U, Sect_Device_RW);
  MMU_TTSection (&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_USB_BASE       ,   16U, Sect_Device_RW);
                                                                                
  // Create (16 * 64k)=1MB faulting entries to cover peripheral range           
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, PERIPHERAL_A_FAULT                   ,   16U, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, DESCRIPTOR_FAULT);
  // Define peripheral range                                                    
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_DAP_BASE        ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_SYSTEM_REG_BASE ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_SERIAL_BASE     ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_AACI_BASE       ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_MMCI_BASE       ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_KMI0_BASE       ,    2U, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_UART_BASE       ,    4U, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_WDT_BASE        ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_A_TABLE_L2_BASE_64k, Page_64k_Device_RW);
                                                                                
  // Create (16 * 64k)=1MB faulting entries to cover peripheral range           
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, PERIPHERAL_B_FAULT                   ,   16U, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, DESCRIPTOR_FAULT);
  // Define peripheral range                                                    
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_TIMER_BASE      ,    2U, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_DVI_BASE        ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_RTC_BASE        ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_UART4_BASE      ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);
  MMU_TTPage64k(&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_CLCD_BASE       ,    1U, Page_L1_64k, (uint32_t *)PERIPHERAL_B_TABLE_L2_BASE_64k, Page_64k_Device_RW);

  // Create (256 * 4k)=1MB faulting entries to cover private address space. Needs to be marked as Device memory
  MMU_TTPage4k (&Image$$TTB$$ZI$$Base, __get_CBAR()                         ,  256U,  Page_L1_4k, (uint32_t *)PRIVATE_TABLE_L2_BASE_4k, DESCRIPTOR_FAULT);
  // Define private address space entry
  MMU_TTPage4k (&Image$$TTB$$ZI$$Base, __get_CBAR()                         ,    2U,  Page_L1_4k, (uint32_t *)PRIVATE_TABLE_L2_BASE_4k, Page_4k_Device_RW);
  // Define L2CC entry
  MMU_TTPage4k (&Image$$TTB$$ZI$$Base, <DeviceAbbreviation>_L2C_BASE        ,    1U,  Page_L1_4k, (uint32_t *)PRIVATE_TABLE_L2_BASE_4k, Page_4k_Device_RW);

  // Create (256 * 4k)=1MB faulting entries to synchronization space (Useful if some non-cacheable DMA agent is present in the SoC)
  MMU_TTPage4k (&Image$$TTB$$ZI$$Base, F_SYNC_BASE                          ,  256U, Page_L1_4k, (uint32_t *)SYNC_FLAGS_TABLE_L2_BASE_4k, DESCRIPTOR_FAULT);
  // Define synchronization space entry.                       
  MMU_TTPage4k (&Image$$TTB$$ZI$$Base, FLAG_SYNC                            ,    1U, Page_L1_4k, (uint32_t *)SYNC_FLAGS_TABLE_L2_BASE_4k, Page_4k_Device_RW);

  /* Set location of level 1 page table
  ; 31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
  ; 13:7  - 0x0
  ; 6     - IRGN[0] 0x1  (Inner WB WA)
  ; 5     - NOS     0x0  (Non-shared)
  ; 4:3   - RGN     0x01 (Outer WB WA)
  ; 2     - IMP     0x0  (Implementation Defined)
  ; 1     - S       0x0  (Non-shared)
  ; 0     - IRGN[1] 0x0  (Inner WB WA) */
  __set_TTBR0(((uint32_t)&Image$$TTB$$ZI$$Base) | 0x48);
  __ISB();

  /* Set up domain access control register
  ; We set domain 0 to Client and all other domains to No Access.
  ; All translation table entries specify domain 0 */
  __set_DACR(1);
  __ISB();
}

/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _CP0_H_
#define _CP0_H_



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================




/* Cause register exception codes */

#define ExcCode(x)      ((x)<<2)

/* Hardware exception codes */
#define ExcCode_INT     ExcCode(0)      /* interrupt */
#define ExcCode_MOD     ExcCode(1)      /* TLB modification */
#define ExcCode_TLBL    ExcCode(2)      /* TLB miss, load or fetch */
#define ExcCode_TLBS    ExcCode(3)      /* TLB miss, store */
#define ExcCode_AdEL    ExcCode(4)      /* Read Address Error */
#define ExcCode_AdES    ExcCode(5)      /* Write Address Error */
#define ExcCode_IBE     ExcCode(6)      /* Instruction Bus Error */
#define ExcCode_DBE     ExcCode(7)      /* Data Bus Error */
#define ExcCode_SYSCALL ExcCode(8)      /* SYSCALL */
#define ExcCode_BREAK   ExcCode(9)      /* BREAKpoint */
#define ExcCode_RI      ExcCode(10)     /* Illegal Instruction */
#define ExcCode_CpU     ExcCode(11)     /* CoProcessor Unusable */
#define ExcCode_Ovf     ExcCode(12)     /* OVerflow */

/*
 * cp0 registers
 */
#define Index           $0
#define Random          $1
#define EntryLo         $2
#define Context         $4
#define BadVAddr        $8
#define EntryHi         $10
#define Status          $12
#define Cause           $13
#define EPC             $14
#define PRId            $15


/*
 * Mask for Index Register
 */
#define Index_P         0x80000000      /* Probe Failure bit */
#define Index_Index     0x00003f00      /* Index field */

/*
 * Random register 
 */
#define Random_Index    0x00003f00      /* Random field */

/*
 * Entry Low
 */
#define EntryLo_PFN     0xfffff000      /* page frame number field */
#define EntryLo_N       0x00000800      /* non cachable */
#define EntryLo_D       0x00000400      /* dirty */
#define EntryLo_V       0x00000200      /* valid bit */
#define EntryLo_G       0x00000100      /* global bit */

/*
 * Context register 
 */
#define Context_PTEBase 0xffe00000      /* base for page table entry */
#define Context_BadVPN  0x001ffffc      /* failing virtual page number */

/*
 * Entry High Register
 */
#define EntryHi_VPN     0xfffff000      /* virtual page number field */
#define EntryHi_PID     0x00000fc0      /* process ID field */

/*
 * PRId Register
 */
#define PRID_Imp        0x0000ff00      /* implementation ID */
#define PRID_Rev        0x000000ff      /* revision ID */

/*
 * Status register
 */
#define Status_CUMASK   0xf0000000      /* coproc usable bits */
#define Status_RE       0x02000000      /* Bootstrap exception vector */
#define Status_BEV      0x00400000      /* Bootstrap exception vector */
#define Status_TS       0x200000        /* TLB Shutdown bit */
#define Status_IntMASK  0xff00          /* Interrupt mask */
#define Status_KUo      0x0020          /* old kernel/user, 0 => k, 1 => u */
#define Status_IEo      0x0010          /* old interrupt enable, 1 => ena */
#define Status_KUp      0x0008          /* prev kernel/user, 0 => k, 1 => u */
#define Status_IEp      0x0004          /* prev interrupt enable, 1 => ena */
#define Status_KUc      0x0002          /* cur kernel/user, 0 => k, 1 => u */
#define Status_IEc      0x0001          /* cur interrupt enable, 1 => enable*/

/*
 * Cause Register
 */
#define Cause_BD        0x80000000      /* Branch delay slot */
#define Cause_CE        0x30000000      /* coprocessor error */
#define Cause_IP        0xff00          /* Pending interrupt mask */
#define Cause_SIP       0x0300          /* Pending software interrupt mask */
#define Cause_HIP       0xfc00          /* Pending hardware interrupt mask */
#define Cause_ExcCode   0x003C          /* Exception code bits */

#define DRAM_HANDLER    0x80c00200      /* address of exception handler */
#define EPROM_HANDLER   0xa0e00180      /* address of exception handler BEV */



#endif


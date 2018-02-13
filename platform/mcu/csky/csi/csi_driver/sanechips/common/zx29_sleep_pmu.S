/* ****************************************************************************
 *                                                                          * 
 * C-Sky Microsystems Confidential                                          *
 * -------------------------------                                          *
 * This file and all its contents are properties of C-Sky Microsystems. The * 
 * information contained herein is confidential and proprietary and is not  *
 * to be disclosed outside of C-Sky Microsystems except under a             *
 * Non-Disclosured Agreement (NDA).                                         *
 *                                                                          *
 ****************************************************************************
 FILE_NAME           : sleep.S
 FUNCTION            : save the cpu context:
 ******************************************************************************/
#include "zx29_pmu.h"

.global ck802_ns_cpu_save
.global ck802_ns_cpu_resume
.global arch_tw_do_cpu_resume
.global arch_tw_do_cpu_save
#define PSM_SCPU_CONTENT_ADDR       (PMU_IRAM_AON_BASE + 0x1fc)

ck802_ns_cpu_save:
	subi	r14, 4
	stw r0, (r14, 0x0)
	lrw r0, PSM_NSCPU_CONTENT_ADDR
	stw r1, (r0, 0x4)
	stw r2, (r0, 0x8)
	stw r3, (r0, 0xc)
	stw r4, (r0, 0x10)
	stw r5, (r0, 0x14)
	stw r6, (r0, 0x18)
	stw r7, (r0, 0x1c)
	stw r8, (r0, 0x20)
	stw r9, (r0, 0x24)
	stw r10, (r0, 0x28)
	stw r11, (r0, 0x2c)
	stw r12, (r0, 0x30)
	stw r13, (r0, 0x34)
	stw r15, (r0, 0x3c)
	mfcr    r2, psr
	stw r2, (r0, 0x40)
	mfcr    r2, vbr
	stw r2, (r0, 0x44)
	mfcr    r2, epsr
	stw r2, (r0, 0x48)
	mfcr    r2, epc
	stw r2, (r0, 0x4c)
	mfcr    r2, cr<11, 0>
	stw r2, (r0, 0x50)
	mfcr    r2, cr<12, 0>
	stw r2, (r0, 0x54)
	mfcr    r2, cr<13, 0>
	stw r2, (r0, 0x58)
	mfcr    r2, cr<18,0>
	stw r2, (r0, 0x5c)
	mfcr    r2, cr<19,0>
	stw r2, (r0, 0x60)
	mfcr    r2, cr<20,0>
	stw r2, (r0, 0x64)
	mfcr    r2, cr<21,0>
	stw r2, (r0, 0x68)
    /*add by anainv 20170207*/
    mfcr    r2, cr<1, 1>
	stw r2, (r0, 0xc8)
    /*add by anainv to save cru and vic reg*/
    lrw r1, CPU_CK_CRU_BASE
    ldw r2, (r1, 0x8)
    ldw r3, (r1, 0xc)
    ldw r4, (r1, 0x0)
    stw r2, (r0, 0x70)
    stw r3, (r0, 0x74)
    stw r4, (r0, 0x78)

    lrw r1, CPU_CK_INTC_BASE
    addi r1,0x100
    ldw r2, (r1, 0x80)
    ldw r3, (r1, 0xc0)
    ldw r4, (r1, 0x100)
    ldw r5, (r1, 0x140)
    ldw r6, (r1, 0x180)
    ldw r7, (r1, 0x1c0)
    ldw r8, (r1, 0x200)
    stw r2, (r0, 0x7c)
    stw r3, (r0, 0x80)
    stw r4, (r0, 0x84)
    stw r5, (r0, 0x88)
    stw r6, (r0, 0x8c)
    stw r7, (r0, 0x90)
    stw r8, (r0, 0x94)
    ldw r2, (r1, 0x300)
    ldw r3, (r1, 0x304)
    ldw r4, (r1, 0x308)
    ldw r5, (r1, 0x30c)
    ldw r6, (r1, 0x310)
    ldw r7, (r1, 0x314)
    ldw r8, (r1, 0x318)
    ldw r9, (r1, 0x31c)
    ldw r10,(r1, 0xb00)
    ldw r11,(r1, 0xb04)
    stw r2, (r0, 0x98)
    stw r3, (r0, 0x9c)
    stw r4, (r0, 0xa0)
    stw r5, (r0, 0xa4)
    stw r6, (r0, 0xa8)
    stw r7, (r0, 0xac)
    stw r8, (r0, 0xb0)
    stw r9, (r0, 0xb4)
    stw r10,(r0, 0xb8)
    stw r11,(r0, 0xbc)
    ldw r2, (r1, 0x40)
    ldw r3, (r1, 0x0)
    stw r2, (r0, 0xc0)
    stw r3, (r0, 0xc4)
    /*add end-------------------------------*/
	ldw r2, (r14, 0x0)
	addi	r14, 4
	stw r14, (r0, 0x38)
	stw r2, (r0, 0x0)
	movi    r0, 0
	jmp r15

ck802_ns_cpu_resume:
    //br .
	lrw   r0, 0x80000100  
	mtcr  r0, psr
	lrw	r0,	PSM_NSCPU_CONTENT_ADDR
	ldw r1, (r0, 0x4)
	ldw r3, (r0, 0xc)
	ldw r4, (r0, 0x10)
	ldw r5, (r0, 0x14)
	ldw r6, (r0, 0x18)
	ldw r7, (r0, 0x1c)
	ldw r8, (r0, 0x20)
	ldw r9, (r0, 0x24)
	ldw r10, (r0, 0x28)
	ldw r11, (r0, 0x2c)
	ldw r12, (r0, 0x30)
	ldw r13, (r0, 0x34)
	ldw r14, (r0, 0x38)
	ldw r15, (r0, 0x3c)
	ldw r2, (r0, 0x40)
	mtcr    r2, psr
	ldw r2, (r0, 0x44)
	mtcr    r2, vbr
	ldw r2, (r0, 0x48)
	mtcr    r2, epsr
	ldw r2, (r0, 0x4c)
	mtcr    r2, epc
	ldw r2, (r0, 0x50)
	mtcr    r2, cr<11, 0>
	ldw r2, (r0, 0x54)
	mtcr    r2, cr<12, 0>
	ldw r2, (r0, 0x58)
	mtcr    r2, cr<13, 0>
	ldw r2, (r0, 0x5c)
	mtcr    r2, cr<18, 0>
	ldw r2, (r0, 0x60)
	mtcr    r2, cr<19, 0>
	ldw r2, (r0, 0x64)
	mtcr    r2, cr<20, 0>
	ldw r2, (r0, 0x68)
	mtcr    r2, cr<21, 0>
    /*add by anainv 20170207*/
	ldw r2, (r0, 0xc8)
    mtcr    r2, cr<1, 1>
   // /*add by anainv to resume cru and vic reg*/
    lrw r1, CPU_CK_CRU_BASE
   ldw r2, (r0, 0x70)
   //stw r2, (r1, 0x8)
   ldw r2, (r0, 0x74)
   //stw r2, (r1, 0xc)
  // lrw r2,  0x1       /*add inv all cache  by zhangpei*/
   //stw r2, (r1, 0x4) 
   ldw r2, (r0, 0x78)
   //stw r2, (r1, 0x0)

   lrw r1, CPU_CK_INTC_BASE
   addi r1,0x100
   ldw r2, (r0, 0x7c)
   //stw r2, (r1, 0x80) //del
   ldw r2, (r0, 0x80)
   // stw r2, (r1, 0xc0)//del
   ldw r2, (r0, 0x84)
   //stw r2, (r1, 0x100)
    ldw r2, (r0, 0x88)
   //stw r2, (r1, 0x140) //ISSR/
    ldw r2, (r0, 0x8c)
   //stw r2, (r1, 0x180)//del
    ldw r2, (r0, 0x90)
   // stw r2, (r1, 0x1c0)//ICSR
    ldw r2, (r0, 0x94)
    //stw r2, (r1, 0x200)

	ldw r2, (r0, 0x98)
	stw r2, (r1, 0x300)
	ldw r2, (r0, 0x9c)
	stw r2, (r1, 0x304)
	ldw r2, (r0, 0xa0)
	stw r2, (r1, 0x308)
	ldw r2, (r0, 0xa4)
	stw r2, (r1, 0x30c)
	ldw r2, (r0, 0xa8)
	stw r2, (r1, 0x310)
	ldw r2, (r0, 0xac)
	stw r2, (r1, 0x314)
	ldw r2, (r0, 0xb0)
	stw r2, (r1, 0x318)
	ldw r2, (r0, 0xb4)
	stw r2, (r1, 0x31c)
	ldw r2, (r0, 0xb8)
	//stw r2, (r1, 0xb00)
	ldw r2, (r0, 0xbc)
	//stw r2, (r1, 0xb04)
    
    ldw r2, (r0, 0xc0)
    stw r2, (r1, 0x40)
    ldw r2, (r0, 0xc4)
    stw r2, (r1, 0x0)
   // /*add end-------------------------------*/
	ldw r2, (r0, 0x8)
	ldw r1, (r0, 0x4)
	ldw r2, (r0, 0x8)
	ldw r0, (r0, 0x0)
	movi	r0,	1
	jmp r15

arch_tw_do_cpu_resume:
    //movi r1, 0x3
    //mtcr r1, cr<8, 3>
    lrw r0, PSM_SCPU_CONTENT_ADDR
    ldw  r2, (r0, 0x6c)
    mtcr r2, cr<8, 3>
    ldw r1, (r0, 0x4)
    ldw r3, (r0, 0xc)
    ldw r4, (r0, 0x10) 
    ldw r5, (r0, 0x14)
    ldw r6, (r0, 0x18)
    ldw r7, (r0, 0x1c)
    ldw r8, (r0, 0x20) 
    ldw r9, (r0, 0x24)
    ldw r10, (r0, 0x28)
    ldw r11, (r0, 0x2c)
    ldw r12, (r0, 0x30)
    ldw r13, (r0, 0x34)
    ldw r14, (r0, 0x38)
    ldw r15, (r0, 0x3c)
    ldw r2, (r0, 0x40)
    mtcr    r2, psr
    ldw r2, (r0, 0x44)
    mtcr    r2, vbr
    ldw r2, (r0, 0x48)
    mtcr    r2, epsr
    ldw r2, (r0, 0x4c)
    mtcr    r2, epc
    ldw r2, (r0, 0x50)
    mtcr    r2, cr<6, 3>
    ldw r2, (r0, 0x54)
    mtcr    r2, cr<0, 3>
    ldw r2, (r0, 0x58)
    mtcr    r2, cr<1, 3>
    ldw r2, (r0, 0x5c)
    mtcr    r2, cr<2, 3>
    ldw r2, (r0, 0x60)
    mtcr    r2, cr<4, 3>
    ldw r2, (r0, 0x64)
    mtcr    r2, cr<10, 3>
    ldw r2, (r0, 0x68)
    mtcr    r2, cr<1, 1>
    ldw r2, (r0, 0x8)
    ldw r0, (r0, 0x0)
    movi    r0, 1
    jmp r15

arch_tw_do_cpu_save:
    subi    r14, 4
    stw r0, (r14, 0x0)
    stw r1, (r0, 0x4)
    stw r2, (r0, 0x8)
    stw r3, (r0, 0xc)
    stw r4, (r0, 0x10)
    stw r5, (r0, 0x14)
    stw r6, (r0, 0x18)
    stw r7, (r0, 0x1c)
    stw r8, (r0, 0x20)
    stw r9, (r0, 0x24)
    stw r10, (r0, 0x28)
    stw r11, (r0, 0x2c)
    stw r12, (r0, 0x30)
    stw r13, (r0, 0x34)
    stw r15, (r0, 0x3c)
    mfcr    r2, psr
    stw r2, (r0, 0x40)
    mfcr    r2, vbr
    stw r2, (r0, 0x44)
    mfcr    r2, epsr
    stw r2, (r0, 0x48)
    mfcr    r2, epc
    stw r2, (r0, 0x4c)
    // save NT_SSP
    mfcr    r2, cr<6, 3>
    stw r2, (r0, 0x50)
    // save NT_PSR
    mfcr    r2, cr<0, 3>
    stw r2, (r0, 0x54)
    // save NT_VBR
    mfcr    r2, cr<1, 3>
    stw r2, (r0, 0x58)
    // save NT_EPSR
    mfcr    r2, cr<2, 3>
    stw r2, (r0, 0x5c)
    // save NT_EPC
    mfcr    r2, cr<4, 3>
    stw r2, (r0, 0x60)
    // save NT_EBR
    mfcr    r2, cr<10,3>
    stw r2, (r0, 0x64)
    // save T_EBR
    mfcr    r2, cr<1,1>
    stw r2, (r0, 0x68)
    //save jtag
    mfcr    r2, cr<8, 3>
    stw r2, (r0, 0x6c)

    ldw r2, (r14, 0x0)
    addi    r14, 4
    stw r14, (r0, 0x38)
    stw r2, (r0, 0x0)
    movi    r0, 0
    jmp r15

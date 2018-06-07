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


#ifndef _REGDEF_H_
#define _REGDEF_H_



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================



/* Set or not mips16 */
#ifdef CT_ASM_MIPS16
#define SET_P_MIPS16 .set mips16
#else
#define SET_P_MIPS16 .set nomips16
#endif 

#define zero    $0
#define AT      $at
#define v0      $2
#define v1      $3
#define a0      $4
#define a1      $5
#define a2      $6
#define a3      $7
#define t0      $8
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
#define s0      $16
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define t8      $24
#define t9      $25
#define k0      $26
#define k1      $27
#define gp      $28
#define sp      $29
#define fp      $30
#define ra      $31

#define r0      $0
#define r1      $1
#define r2      $2
#define r3      $3
#define r4      $4
#define r5      $5
#define r6      $6
#define r7      $7
#define r8      $8
#define r9      $9
#define r10     $10
#define r11     $11
#define r12     $12
#define r13     $13
#define r14     $14
#define r15     $15
#define r16     $16
#define r17     $17
#define r18     $18
#define r19     $19
#define r20     $20
#define r21     $21
#define r22     $22
#define r23     $23
#define r24     $24
#define r25     $25
#define r26     $26
#define r27     $27
#define r28     $28
#define r29     $29
#define r30     $30
#define r31     $31

#define fp0     $f0
#define fp1     $f1
#define fp2     $f2
#define fp3     $f3
#define fp4     $f4
#define fp5     $f5
#define fp6     $f6
#define fp7     $f7
#define fp8     $f8
#define fp9     $f9
#define fp10    $f10
#define fp11    $f11
#define fp12    $f12
#define fp13    $f13
#define fp14    $f14
#define fp15    $f15
#define fp16    $f16
#define fp17    $f17
#define fp18    $f18
#define fp19    $f19
#define fp20    $f20
#define fp21    $f21
#define fp22    $f22
#define fp23    $f23
#define fp24    $f24
#define fp25    $f25
#define fp26    $f26
#define fp27    $f27
#define fp28    $f28
#define fp29    $f29
#define fp30    $f30
#define fp31    $f31

#define fpucntl $31



#endif


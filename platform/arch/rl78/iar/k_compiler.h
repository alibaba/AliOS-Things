/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_COMPILER_H
#define K_COMPILER_H

#define RHINO_INLINE                static inline
/* get the return address of the current function
   unsigned int __get_LR(void) */
#define RHINO_GET_RA()              (void *)__get_LR()
/* get the  the value of the stack pointer
   unsigned int __get_SP(void) */
#define RHINO_GET_SP()              (void *)__get_SP()
/* get the value of the stack pointer register
   unsigned int __CLZ(unsigned int) */
//#define RHINO_BIT_CLZ(x)            __CLZ(x)

#endif /* K_COMPILER_H */


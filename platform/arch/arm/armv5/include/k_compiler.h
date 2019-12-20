/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_COMPILER_H
#define K_COMPILER_H

#if defined(__CC_ARM)

#define RHINO_INLINE                static __inline

/* get the return address of the current function
   unsigned int __return_address(void) */
#define RHINO_GET_RA()              (void *)__return_address()

/* get the  the value of the stack pointer
   unsigned int __current_sp(void) */
#define RHINO_GET_SP()              (void *)__current_sp()

/* Returns the number of leading 0-bits in x,
   starting at the most signifi cant bit position. */
#define RHINO_BIT_CLZ(x)            __builtin_clz(x)

/* Returns the number of trailing 0-bits in x,
   starting at the least signifi cant bit position. */
#define RHINO_BIT_CTZ(x)            __builtin_ctz(x)

#ifndef RHINO_WEAK
#define RHINO_WEAK                  __weak
#endif

#ifndef RHINO_ASM
#define RHINO_ASM                   __asm
#endif

#elif defined(__ICCARM__)

#define RHINO_INLINE                static inline

/* get the return address of the current function
   unsigned int __get_LR(void) */
#define RHINO_GET_RA()              (void *)__get_LR()

/* get the  the value of the stack pointer
   unsigned int __get_SP(void) */
#define RHINO_GET_SP()              (void *)__get_SP()

/* Returns the number of leading 0-bits in x,
   starting at the most signifi cant bit position. */
#define RHINO_BIT_CLZ(x)            __CLZ(x)

//#define RHINO_BIT_CTZ(x)

#ifndef RHINO_WEAK
#define RHINO_WEAK                  __weak
#endif

#ifndef RHINO_ASM
#define RHINO_ASM                   asm
#endif


#elif defined(__GNUC__)

#define RHINO_INLINE                static inline

/* get the return address of the current function
   void * __builtin_return_address (unsigned int level) */
#define RHINO_GET_RA()              __builtin_return_address(0)

/* get the return address of the current function */
__attribute__((always_inline)) RHINO_INLINE void *RHINO_GET_SP(void)
{
    void *sp;
    asm volatile("mov %0, SP\n":"=r"(sp));
    return sp;
}

/* Returns the number of leading 0-bits in x,
   starting at the most signifi cant bit position. */
#define RHINO_BIT_CLZ(x)            __builtin_clz(x)

/* Returns the number of trailing 0-bits in x,
   starting at the least signifi cant bit position. */
#define RHINO_BIT_CTZ(x)            __builtin_ctz(x)

#ifndef RHINO_WEAK
#define RHINO_WEAK                  __attribute__((weak))
#endif

#ifndef RHINO_ASM
#define RHINO_ASM                   __asm__
#endif

#else
#error "Unsupported compiler"
#endif

#endif /* K_COMPILER_H */


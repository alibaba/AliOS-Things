#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#define RISCV_MSTATUS_MIE               (1<<3)        /*machine-level interrupt bit*/
#define RISCV_MSTATUS_MPIE              (1<<7)        /*machine-level pre interrupt bit*/
#define RISCV_MSTATUS_MPP               (0x3<<10)     /*machine-level MPP bit*/

#define RISCV_MSTATUS_MPP_MPIE          (RISCV_MSTATUS_MPIE | RISCV_MSTATUS_MPP)

#if __riscv_xlen == 64
# define SLL32    sllw
# define STORE    sd
# define LOAD     ld
# define LWU      lwu
# define LOG_REGBYTES 3
#else
# define SLL32    sll
# define STORE    sw
# define LOAD     lw
# define LWU      lw
# define LOG_REGBYTES 2
#endif
#define REGBYTES (1 << LOG_REGBYTES)

#endif // _TYPEDEF_H_
// eof


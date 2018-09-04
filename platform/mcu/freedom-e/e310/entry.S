// See LICENSE for license details

#ifndef ENTRY_S
#define ENTRY_S

#include "encoding.h"
#include "sifive/bits.h"
#include "typedef.h"

.extern restore_sp

.global except_before_task_running
.global except_from_intrpt

  .section      .text.entry	
  .align 2
  .weak trap_entry
  .global trap_entry
trap_entry:
  addi sp, sp, -32*REGBYTES

  STORE x1, 2*REGBYTES(sp)
  //STORE x2, 2*REGBYTES(sp)
  STORE x3, 3*REGBYTES(sp)
  STORE x4, 4*REGBYTES(sp)
  STORE x5, 5*REGBYTES(sp)
  STORE x6, 6*REGBYTES(sp)
  STORE x7, 7*REGBYTES(sp)
  STORE x8, 8*REGBYTES(sp)
  STORE x9, 9*REGBYTES(sp)
  STORE x10, 10*REGBYTES(sp)
  STORE x11, 11*REGBYTES(sp)
  STORE x12, 12*REGBYTES(sp)
  STORE x13, 13*REGBYTES(sp)
  STORE x14, 14*REGBYTES(sp)
  STORE x15, 15*REGBYTES(sp)
  STORE x16, 16*REGBYTES(sp)
  STORE x17, 17*REGBYTES(sp)
  STORE x18, 18*REGBYTES(sp)
  STORE x19, 19*REGBYTES(sp)
  STORE x20, 20*REGBYTES(sp)
  STORE x21, 21*REGBYTES(sp)
  STORE x22, 22*REGBYTES(sp)
  STORE x23, 23*REGBYTES(sp)
  STORE x24, 24*REGBYTES(sp)
  STORE x25, 25*REGBYTES(sp)
  STORE x26, 26*REGBYTES(sp)
  STORE x27, 27*REGBYTES(sp)
  STORE x28, 28*REGBYTES(sp)
  STORE x29, 29*REGBYTES(sp)
  STORE x30, 30*REGBYTES(sp)
  STORE x31, 31*REGBYTES(sp)

  li    t1, (RISCV_MSTATUS_MPIE | RISCV_MSTATUS_MPP)
  STORE t1, 0*REGBYTES(sp)
  csrr a1, mepc
  STORE a1, 1*REGBYTES(sp)
 

  la      t1, g_sys_stat
  LOAD    t2, (t1)
  li      t3, 3
  bne     t2, t3, except_before_task_running                 // RHINO_RUNNING = 3


  call    krhino_intrpt_enter                               // g_intrpt_nested_level++;

  la      t1, g_intrpt_nested_level
  lb      t2, (t1)
  li      t3, 1
  bne     t2, t3, except_from_intrpt                         // if (g_intrpt_nested_level == 1)

except_from_task:
  // g_active_task->task_stack = context region
  la      t1, g_active_task                                 // g_active_task->task_stack = SP;
  LOAD    t2, (t1)
  STORE   sp, (t2)

  la   sp, _sp                                              // Switch to except stack.

  csrr a0, mcause
  csrr a1, mepc
  mv a2, sp
  call handle_trap
  csrw mepc, a0

  call krhino_intrpt_exit

  la    t1, g_active_task
  LOAD  t2, (t1)
  LOAD  sp, (t2)

  j   restore_sp


except_from_intrpt:
  csrr a0, mcause
  csrr a1, mepc
  mv a2, sp
  call handle_trap
  csrw mepc, a0

  la     t1,g_intrpt_nested_level                           // g_intrpt_nested_level--;
  lb     t2, (t1)
  add      t2, t2, -1
  STORE    t2, (t1)

  j   restore_sp


except_before_task_running:
  csrr a0, mcause
  csrr a1, mepc
  mv a2, sp
  call handle_trap
  csrw mepc, a0

  j   restore_sp


.weak handle_trap
handle_trap:
1:
  j 1b
	
#endif

// See LICENSE for license details

#ifndef VENTRY_S
#define VENTRY_S

#include "encoding.h"
#include "sifive/bits.h"

#only save caller registers
.macro TRAP_ENTRY
  addi sp, sp, -16*REGBYTES

  STORE x1,  0*REGBYTES(sp)
  STORE x5,  1*REGBYTES(sp)
  STORE x6,  2*REGBYTES(sp)
  STORE x7,  3*REGBYTES(sp)
  STORE x10, 4*REGBYTES(sp)
  STORE x11, 5*REGBYTES(sp)
  STORE x12, 6*REGBYTES(sp)
  STORE x13, 7*REGBYTES(sp)
  STORE x14, 8*REGBYTES(sp)
  STORE x15, 9*REGBYTES(sp)
  STORE x16, 10*REGBYTES(sp)
  STORE x17, 11*REGBYTES(sp)
  STORE x28, 12*REGBYTES(sp)
  STORE x29, 13*REGBYTES(sp)
  STORE x30, 14*REGBYTES(sp)
  STORE x31, 15*REGBYTES(sp)
.endm

#restore caller registers
.macro TRAP_EXIT
# Remain in M-mode after mret
  li t0, MSTATUS_MPP
  csrs mstatus, t0

  LOAD x1,  0*REGBYTES(sp)
  LOAD x5,  1*REGBYTES(sp)
  LOAD x6,  2*REGBYTES(sp)
  LOAD x7,  3*REGBYTES(sp)
  LOAD x10, 4*REGBYTES(sp)
  LOAD x11, 5*REGBYTES(sp)
  LOAD x12, 6*REGBYTES(sp)
  LOAD x13, 7*REGBYTES(sp)
  LOAD x14, 8*REGBYTES(sp)
  LOAD x15, 9*REGBYTES(sp)
  LOAD x16, 10*REGBYTES(sp)
  LOAD x17, 11*REGBYTES(sp)
  LOAD x28, 12*REGBYTES(sp)
  LOAD x29, 13*REGBYTES(sp)
  LOAD x30, 14*REGBYTES(sp)
  LOAD x31, 15*REGBYTES(sp)

  addi sp, sp, 16*REGBYTES
  mret
.endm



#Vector table for E31/E51

  .section      .text.entry	
  .align 8
  .global vtrap_entry
vtrap_entry:
  j sync_trap
  .align 2
  j reserved
  .align 2
  j reserved
  .align 2
  j vmsi_Handler
  .align 2
  j reserved
  .align 2
  j reserved
  .align 2
  j reserved
  .align 2
  j vmti_Handler
  .align 2
  j reserved
  .align 2
  j reserved
  .align 2
  j reserved
  .align 2
  j vmei_Handler
  .align 2
  j reserved
  .align 2
  j reserved
  .align 2
  j reserved
   .align 2
  j reserved
  .align 2
  j vlip_Handler0
  .align 2
  j vlip_Handler1
  .align 2
  j vlip_Handler2
  .align 2
  j vlip_Handler3
  .align 2
  j vlip_Handler4
  .align 2
  j vlip_Handler5
  .align 2
  j vlip_Handler6
  .align 2
  j vlip_Handler7
  .align 2
  j vlip_Handler8
  .align 2
  j vlip_Handler9
  .align 2
  j vlip_Handler10
  .align 2
  j vlip_Handler11
  .align 2
  j vlip_Handler12
  .align 2
  j vlip_Handler13
  .align 2
  j vlip_Handler14
  .align 2
  j vlip_Handler15
  
#synchronous trap
sync_trap:
  TRAP_ENTRY
  jal handle_sync_trap
  TRAP_EXIT

#Machine Software Interrupt
vmsi_Handler:
  TRAP_ENTRY
  jal reserved
  TRAP_EXIT

#Machine Timer Interrupt
vmti_Handler:
  TRAP_ENTRY
  jal handle_m_time_interrupt
  TRAP_EXIT

#Machine External Interrupt
vmei_Handler:
  TRAP_ENTRY
  jal handle_m_external_interrupt
  TRAP_EXIT

#LIP0
vlip_Handler0:
  TRAP_ENTRY
  jal handle_local_interrupt0
  TRAP_EXIT

#LIP1
vlip_Handler1:
  TRAP_ENTRY
  jal handle_local_interrupt1
  TRAP_EXIT

#LIP2
vlip_Handler2:
  TRAP_ENTRY
  jal handle_local_interrupt2
  TRAP_EXIT

#LIP3
vlip_Handler3:
  TRAP_ENTRY
  jal handle_local_interrupt3
  TRAP_EXIT

#LIP4
vlip_Handler4:
  TRAP_ENTRY
  jal handle_local_interrupt4
  TRAP_EXIT

#LIP5
vlip_Handler5:
  TRAP_ENTRY
  jal handle_local_interrupt5
  TRAP_EXIT

#LIP6
vlip_Handler6:
  TRAP_ENTRY
  jal handle_local_interrupt6
  TRAP_EXIT

#LIP7
vlip_Handler7:
  TRAP_ENTRY
  jal handle_local_interrupt7
  TRAP_EXIT

#LIP8
vlip_Handler8:
  TRAP_ENTRY
  jal handle_local_interrupt8
  TRAP_EXIT

#LIP9
vlip_Handler9:
  TRAP_ENTRY
  jal handle_local_interrupt9
  TRAP_EXIT

#LIP10
vlip_Handler10:
  TRAP_ENTRY
  jal handle_local_interrupt10
  TRAP_EXIT

#LIP11
vlip_Handler11:
  TRAP_ENTRY
  jal handle_local_interrupt11
  TRAP_EXIT

#LIP12
vlip_Handler12:
  TRAP_ENTRY
  jal handle_local_interrupt12
  TRAP_EXIT

#LIP13
vlip_Handler13:
  TRAP_ENTRY
  jal handle_local_interrupt13
  TRAP_EXIT

#LIP14
vlip_Handler14:
  TRAP_ENTRY
  jal handle_local_interrupt14
  TRAP_EXIT

#LIP15
vlip_Handler15:
  TRAP_ENTRY
  jal handle_local_interrupt15
  TRAP_EXIT

#unimplemented ISRs trap here
.weak reserved
reserved:
.weak handle_local_interrupt0
handle_local_interrupt0:
.weak handle_local_interrupt1
handle_local_interrupt1:
.weak handle_local_interrupt2
handle_local_interrupt2:
.weak handle_local_interrupt3
handle_local_interrupt3:
.weak handle_local_interrupt4
handle_local_interrupt4:
.weak handle_local_interrupt5
handle_local_interrupt5:
.weak handle_local_interrupt6
handle_local_interrupt6:
.weak handle_local_interrupt7
handle_local_interrupt7:
.weak handle_local_interrupt8
handle_local_interrupt8:
.weak handle_local_interrupt9
handle_local_interrupt9:
.weak handle_local_interrupt10
handle_local_interrupt10:
.weak handle_local_interrupt11
handle_local_interrupt11:
.weak handle_local_interrupt12
handle_local_interrupt12:
.weak handle_local_interrupt13
handle_local_interrupt13:
.weak handle_local_interrupt14
handle_local_interrupt14:
.weak handle_local_interrupt15
handle_local_interrupt15:
1:
  j 1b
	
#endif

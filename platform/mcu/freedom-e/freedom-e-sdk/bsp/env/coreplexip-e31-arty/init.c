//See LICENSE for license details.
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "platform.h"
#include "encoding.h"

#define CPU_FREQ 65000000
#define XSTR(x) #x
#define STR(x) XSTR(x)

#ifndef VECT_IRQ
  #define TRAP_ENTRY trap_entry
#else
  #define TRAP_ENTRY vtrap_entry
#endif

extern int main(int argc, char** argv);
extern void TRAP_ENTRY();

unsigned long get_cpu_freq()
{
  return CPU_FREQ;
}

unsigned long get_timer_freq()
{
  return get_cpu_freq();
}

uint64_t get_timer_value()
{
#if __riscv_xlen == 32
  while (1) {
    uint32_t hi = read_csr(mcycleh);
    uint32_t lo = read_csr(mcycle);
    if (hi == read_csr(mcycleh))
      return ((uint64_t)hi << 32) | lo;
  }
#else
  return read_csr(mcycle);
#endif
}

static void uart_init(size_t baud_rate)
{
  UART0_REG(UART_REG_DIV) = (get_cpu_freq() / 2) / baud_rate - 1;
  UART0_REG(UART_REG_TXCTRL) |= UART_TXEN;
}


#ifdef USE_PLIC
extern void handle_m_ext_interrupt();
#endif

#ifdef USE_M_TIME
extern void handle_m_time_interrupt();
#endif

#ifdef USE_LOCAL_ISR
typedef void (*my_interrupt_function_ptr_t) (void);
extern my_interrupt_function_ptr_t localISR[];
#endif

#ifndef VECT_IRQ
uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc) __attribute__((noinline));
uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc)
{
  if (0){
#ifdef USE_PLIC
    // External Machine-Level interrupt from PLIC
  } else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_EXT)) {
    handle_m_ext_interrupt();
#endif
#ifdef USE_M_TIME
    // External Machine-Level interrupt from PLIC
  } else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)){
    handle_m_time_interrupt();
#endif
#ifdef USE_LOCAL_ISR
  } else if (mcause & MCAUSE_INT) {
    localISR[mcause & MCAUSE_CAUSE] ();
#endif
  }
  else {
    write(1, "Unhandled Trap:\n", 16);
    _exit(1 + mcause);
  }
  return epc;
}
#endif 

#ifdef USE_CLIC
void trap_entry(void) __attribute__((interrupt("SiFive-CLIC-preemptible"), aligned(64)));
void trap_entry(void)
{
  unsigned long mcause = read_csr(mcause);
  unsigned long mepc = read_csr(mepc);
  handle_trap(mcause, mepc);
}
#endif

void _init()
{
  #ifndef NO_INIT
  uart_init(115200);

  puts("core freq at " STR(CPU_FREQ) " Hz\n");

#ifdef USE_CLIC
  write_csr(mtvec, ((unsigned long)&trap_entry | MTVEC_CLIC));
#else
  write_csr(mtvec, ((unsigned long)&TRAP_ENTRY | MTVEC_VECTORED));
#endif

  #endif
}

void _fini()
{
}

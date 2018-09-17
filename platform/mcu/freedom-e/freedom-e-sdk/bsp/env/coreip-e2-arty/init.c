//See LICENSE for license details.
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "platform.h"
#include "encoding.h"

#define CPU_FREQ 32000000
#define XSTR(x) #x
#define STR(x) XSTR(x)

extern int main(int argc, char** argv);

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
  UART0_REG(UART_REG_DIV) = (get_cpu_freq() ) / baud_rate - 1;
  UART0_REG(UART_REG_TXCTRL) |= UART_TXEN;
}


typedef void (*interrupt_function_ptr_t) (void);
interrupt_function_ptr_t localISR[CLIC_NUM_INTERRUPTS] __attribute__((aligned(64)));


void trap_entry(void) __attribute__((interrupt, aligned(64)));
void trap_entry(void)
{
  unsigned long mcause = read_csr(mcause);
  unsigned long mepc = read_csr(mepc);
  if (mcause & MCAUSE_INT)  {
    localISR[mcause & MCAUSE_CAUSE] ();
  } else {
    while(1); 
  }
}

#ifdef CLIC_DIRECT
#else
void default_handler(void)__attribute__((interrupt));;
#endif
void default_handler(void)
{
  puts("default handler\n");
  while(1);
}

void _init()
{
#ifndef NO_INIT
  uart_init(115200);

  puts("core freq at " STR(CPU_FREQ) " Hz\n");

//initialize vector table
  int i=0;
  while(i<CLIC_NUM_INTERRUPTS)	{
    localISR[i++] = default_handler;
  }
  
  write_csr(mtvt, localISR);

#ifdef CLIC_DIRECT
  write_csr(mtvec, ((unsigned long)&trap_entry | MTVEC_CLIC));
#else
  write_csr(mtvec, ((unsigned long)&trap_entry | MTVEC_CLIC_VECT));
#endif

#endif
}

void _fini()
{
}

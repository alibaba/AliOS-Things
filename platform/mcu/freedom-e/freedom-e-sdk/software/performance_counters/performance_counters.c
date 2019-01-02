// See LICENSE for license details.

// This demo shows how to use basic
// RISC-V profiling counters, mcycle
// (counts the number of processor cycles)
// and minstret (counts the number of retired instructions). 
// Note that both are writable as well.

#include <stdio.h>

// The CSR encodings are in this header.
#include "encoding.h"

// The mcycle counter is 64-bit counter, but since
// Freedom E platforms use RV32, we must access it as
// 2 32-bit registers. At 256MHz, the lower bits will
// roll over approx. every 5 seconds, so we check for
// rollover with this routine as suggested by the
// RISC-V Priviledged Architecture Specification.

#if __riscv_xlen == 64
#define rdmcycle(x)  {					\
    uint64_t hi;					\
    __asm__ __volatile__ ("1:\n\t"			\
			  "csrr %0, mcycle\n\t"		\
			  : "=r" (hi)) ;		\
    *(x) = hi;		 				\
  }
#else
#define rdmcycle(x)  {				       \
    uint32_t lo, hi, hi2;			       \
    __asm__ __volatile__ ("1:\n\t"		       \
			  "csrr %0, mcycleh\n\t"       \
			  "csrr %1, mcycle\n\t"	       \
			  "csrr %2, mcycleh\n\t"       \
			  "bne  %0, %2, 1b\n\t"			\
			  : "=r" (hi), "=r" (lo), "=r" (hi2)) ;	\
    *(x) = lo | ((uint64_t) hi << 32); 				\
  }
#endif


// The minstret counter is 64-bit counter, but
// Freedom E platforms use RV32, we must access it as
// 2 32-bit registers, same as for mcycle.

#if __riscv_xlen == 64
#define rdminstret(x)  {				\
    uint64_t hi;					\
    __asm__ __volatile__ ("1:\n\t"			\
			  "csrr %0, minstret\n\t"	\
			  : "=r" (hi)) ;		\
    *(x) = hi;						\
  }
#else
#define rdminstret(x)  {			       \
    uint32_t lo, hi, hi2;			       \
    __asm__ __volatile__ ("1:\n\t"		       \
			  "csrr %0, minstreth\n\t"       \
			  "csrr %1, minstret\n\t"	       \
			  "csrr %2, minstreth\n\t"       \
			  "bne  %0, %2, 1b\n\t"			\
			  : "=r" (hi), "=r" (lo), "=r" (hi2)) ;	\
    *(x) = lo | ((uint64_t) hi << 32); 				\
  }
#endif

// Simple program to measure the performance of.

int factorial(int i){

  int result = 1;
  for (int ii = 1; ii <= i; ii++) {
    result = result * i;
  }

  return result;
  
}


int main()
{

  uint64_t before_cycle;
  uint64_t before_instret;
  
  uint64_t after_cycle;
  uint64_t after_instret;
    
  printf("\n\nDemo 1: Using Counter Differences.\n");
  
  for (int ii = 0; ii < 3; ii++){
    rdmcycle(&before_cycle);
    rdminstret(&before_instret);

    volatile int result = factorial (100);
    
    rdmcycle(&after_cycle);
    rdminstret(&after_instret);
    
    printf("Loop %d: Retired %d instructions in %d cycles\n",
	   ii,
	   (uint32_t)(after_instret - before_instret), 
	   (uint32_t)(after_cycle - before_cycle));
  }

  printf("\n\nDemo 2: Clearing Counters, Using Values Directly.\n");

  for (int ii = 0; ii < 3; ii++){

    write_csr(mcycle,  0);
    write_csr(minstret, 0);
#if __riscv_xlen == 32
    write_csr(mcycleh, 0);
    write_csr(minstreth, 0);
#endif
    
    volatile int result = factorial (100);
    
    rdmcycle(&after_cycle);
    rdminstret(&after_instret);
    
    printf("Loop %d: Retired %d instructions in %d cycles\n",
	   ii,
	   (uint32_t)(after_instret),
	   (uint32_t)(after_cycle));

  }

  return 0;

}

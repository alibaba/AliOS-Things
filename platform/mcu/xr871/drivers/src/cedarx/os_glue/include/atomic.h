#ifndef _OS_GLUE_ATOMIC
#define _OS_GLUE_ATOMIC
#include <stddef.h>
#include "sys/interrupt.h"

#define ENTER_CRITICAL() arch_irq_disable()

#define EXIT_CRITICAL() arch_irq_enable()

#define __sync_fetch_and_add				sync_fetch_and_add
#define __sync_fetch_and_sub				sync_fetch_and_sub
#define __sync_fetch_and_or					sync_fetch_and_or
#define __sync_fetch_and_and				sync_fetch_and_and
#define __sync_fetch_and_xor				sync_fetch_and_xor
#define __sync_fetch_and_nand				sync_fetch_and_nand
#define __sync_add_and_fetch				sync_add_and_fetch
#define __sync_sub_and_fetch				sync_sub_and_fetch
#define __sync_or_and_fetch					sync_or_and_fetch
#define __sync_and_and_fetch				sync_and_and_fetch
#define __sync_xor_and_fetch				sync_xor_and_fetch
#define __sync_nand_and_fetch				sync_nand_and_fetch
#define __sync_lock_test_and_set			sync_lock_test_and_set
#define __sync_bool_compare_and_swap		sync_bool_compare_and_swap
#define __sync_bool_compare_and_swap		sync_bool_compare_and_swap

signed long sync_fetch_and_add(volatile signed long *ptr, signed long value);
signed long sync_fetch_and_sub(volatile signed long *ptr, signed long value);
signed long sync_fetch_and_or(volatile signed long *ptr,  signed long value);
signed long sync_fetch_and_and(volatile signed long *ptr, signed long value);
signed long sync_fetch_and_xor(volatile signed long *ptr, signed long value);
signed long sync_fetch_and_nand(volatile signed long *ptr,signed long value);
signed long sync_add_and_fetch(volatile signed long *ptr, signed long value);
signed long sync_sub_and_fetch(volatile signed long *ptr, signed long value);
signed long sync_or_and_fetch(volatile signed long *ptr,  signed long value);
signed long sync_and_and_fetch(volatile signed long *ptr, signed long value);
signed long sync_xor_and_fetch(volatile signed long *ptr, signed long value);
signed long sync_nand_and_fetch(volatile signed long *ptr,signed long value);
signed long sync_lock_test_and_set(volatile signed long *ptr,signed long value);
int sync_bool_compare_and_swap(volatile signed long *ptr,signed long oldvalue, signed long newvalue);

#endif

/*
 * Copyright (C) 2017 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
2017_12_27,WangMin(Rocky) created.
*/

/*
 * DESCRIPTION
 * This library is used to provide the atomic operators for CPU
 * which do not support native atomic operations.
 *
 * The design principle is disable the interrupt when execute the
 * atomic operations and enable the interrupt after finish the
 * operation.
 *
 * This library can be added into system by defining
 * RHINO_CONFIG_ATOMIC_GENERIC.
 */

#include "k_api.h"
#include "k_atomic.h"

/**
 * This routine atomically adds <*target> and <value>, placing the result in
 * <*target>.  The operation is done using unsigned integer arithmetic.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @param target memory location to add to
 * @param value the value to add
 *
 * @return The previous value from <target>
 */

atomic_val_t rhino_atomic_add(atomic_t *target, atomic_val_t value)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	*target += value;

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine atomically subtracts <value> from <*target>, result is placed
 * in <*target>.  The operation is done using unsigned integer arithmetic.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @param target the memory location to subtract from
 * @param value the value to subtract
 *
 * @return The previous value from <target>
 */

atomic_val_t rhino_atomic_sub(atomic_t *target, atomic_val_t value)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	*target -= value;

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine atomically increments the value in <*target>.  The operation is
 * done using unsigned integer arithmetic.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @return The value from <target> before the increment
 */

atomic_val_t rhino_atomic_inc(atomic_t *target)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	(*target)++;

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine atomically decrement the value in <*target>.  The operation is
 * done using unsigned integer arithmetic.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @return The value from <target> before the increment
 */

atomic_val_t rhino_atomic_dec(atomic_t *target)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	(*target)--;

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine atomically sets <*target> to <value> and returns the old value
 * that was in <*target>.  Normally all CPU architectures can atomically write
 * to a variable of size atomic_t without the help of this routine.
 * This routine is intended for software that needs to atomically fetch and
 * replace the value of a memory location.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @param target the memory location to write to
 * @param value the value to write
 *
 * @return The previous value from <target>
 */

atomic_val_t rhino_atomic_set(atomic_t *target, atomic_val_t value)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	*target = value;

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine atomically read a value from <target>.
 * This routine can be used from both task and interrupt context. 
 *
 * @return The value read from <target>
 */

atomic_val_t rhino_atomic_get(const atomic_t *target)
{
	return *target;
}

/**
 * This routine atomically performs a bitwise OR operation of <*target>
 * and <value>, placing the result in <*target>.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @param target the memory location to be modified
 * @param value the value to OR
 *
 * @return The previous value from <target>
 */

atomic_val_t rhino_atomic_or(atomic_t *target, atomic_val_t value)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	*target |= value;

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine atomically performs a bitwise XOR operation of <*target> and
 * <value>, placing the result in <*target>.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @param target the memory location to be modified
 * @param value the value to XOR
 *
 * @return The previous value from <target>
 */

atomic_val_t rhino_atomic_xor(atomic_t *target, atomic_val_t value)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	*target ^= value;

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine atomically performs a bitwise AND operation of <*target> and
 * <value>, placing the result in <*target>.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @param target the memory location to be modified
 * @param value the value to AND
 *
 * @return The previous value from <target>
 */

atomic_val_t rhino_atomic_and(atomic_t *target, atomic_val_t value)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	*target &= value;

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine atomically performs a bitwise NAND operation of <*target> and
 * <value>, placing the result in <*target>.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @param target the memory location to be modified
 * @param value the value to NAND
 *
 * @return The previous value from <target>
 */

atomic_val_t rhino_atomic_nand(atomic_t *target, atomic_val_t value)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	*target = ~(*target & value);

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine provides the atomic clear operator. The value of 0 is atomically
 * written at <target> and the previous value at <target> is returned.
 *
 * This routine can be used from both task and interrupt context.
 *
 * @param target the memory location to write
 *
 * @return The previous value from <target>
 */

atomic_val_t rhino_atomic_clear(atomic_t *target)
{
	CPSR_ALLOC();
	atomic_val_t old_value;

	RHINO_CPU_INTRPT_DISABLE();

	old_value = *target;
	*target = 0;

	RHINO_CPU_INTRPT_ENABLE();

	return old_value;
}

/**
 * This routine performs an atomic compare-and-swap, it test that whether
 * <*target> equal to <oldValue>, and if TRUE, setting the value of <*target>
 * to <newValue> and return 1.
 *
 * If the original value at <target> does not equal <oldValue>, then the target
 * will not be updated and return 0.
 *
 * @param target address to be tested
 * @param old_value value to compare against
 * @param new_value value to compare against
 * @return Returns 1 if <new_value> is written, 0 otherwise.
 */

int rhino_atomic_cas(atomic_t *target, atomic_val_t old_value,
			  atomic_val_t new_value)
{
    CPSR_ALLOC();
	int ret = 0;

	RHINO_CPU_INTRPT_DISABLE();

	if (*target == old_value) 
        {
		*target = new_value;
		ret = 1;
	    }

	RHINO_CPU_INTRPT_ENABLE();

	return ret;
}


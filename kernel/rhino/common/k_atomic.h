/*
 * Copyright (C) 2017 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
2017_12_27,WangMin(Rocky) created.
*/

#ifndef K_ATOMIC_H
#define K_ATOMIC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int    atomic_t;
typedef atomic_t        atomic_val_t;

extern atomic_val_t rhino_atomic_add(atomic_t *target, atomic_val_t value);
extern atomic_val_t rhino_atomic_sub(atomic_t *target, atomic_val_t value);
extern atomic_val_t rhino_atomic_inc(atomic_t *target);
extern atomic_val_t rhino_atomic_dec(atomic_t *target);
extern atomic_val_t rhino_atomic_set(atomic_t *target, atomic_val_t value);
extern atomic_val_t rhino_atomic_get(const atomic_t *target);
extern atomic_val_t rhino_atomic_or(atomic_t *target, atomic_val_t value);
extern atomic_val_t rhino_atomic_xor(atomic_t *target, atomic_val_t value);
extern atomic_val_t rhino_atomic_and(atomic_t *target, atomic_val_t value);
extern atomic_val_t rhino_atomic_nand(atomic_t *target, atomic_val_t value);
extern atomic_val_t rhino_atomic_clear(atomic_t *target);
extern int rhino_atomic_cas(atomic_t *target, atomic_val_t old_value,
			                atomic_val_t new_value);

#ifdef __cplusplus
}
#endif

#endif /* K_ATOMIC_H */


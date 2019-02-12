/* cpu.h - automatically selects the correct arch.h file to include */

/*
 * Copyright (c) 1997-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ARCHCPU_H__
#define __ARCHCPU_H__

unsigned int irq_lock(void);
void irq_unlock(unsigned int key);

#endif /* __ARCHCPU_H__ */

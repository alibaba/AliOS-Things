/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Trace buffer.
 */
#ifndef __TRACE_H__
#define __TRACE_H__
#include <stdint.h>
void trace_init();
void trace_insert(uint32_t x);
#endif /* __TRACE_H__ */

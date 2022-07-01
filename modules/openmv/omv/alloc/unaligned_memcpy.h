/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Fast unaligned memcpy functions.
 */
#ifndef __UNALIGNED_MEMCPY_H__
#define __UNALIGNED_MEMCPY_H__
void *unaligned_memcpy(void *dest, void *src, size_t n);
void *unaligned_memcpy_rev16(void *dest, void *src, size_t n);
void *unaligned_2_to_1_memcpy(void *dest, void *src, size_t n);
#endif //__UNALIGNED_MEMCPY_H__

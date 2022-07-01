/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Interface for using extra frame buffer RAM as a stack.
 *
 * Theory of operation:
 *
 * The frame buffer stack may be used to allocate large areas of RAM very quickly. You can allocate
 * memory using fb_alloc() which returns a poiner to an allocated region of memory equal in size to
 * the amount requested. If the memory is not avaiable fb_alloc() will generate an exception.
 *
 * After RAM is allocated with fb_alloc() you can free it with fb_free() in the order of allocs.
 *
 * Now, to prevent leaking allocated regions on the frame buffer stack all fb_alloc()s should be
 * preceded by fb_alloc_mark() which starts an fb_alloc() region (which may have many fb_alloc()s
 * in it). This ensures that if an exception occurs all fb_alloc()s are freed in the region.
 *
 * This is because all exceptions call fb_alloc_free_till_mark() to free the previously allocated
 * region. Your code should call fb_alloc_free_till_mark() to free previously allocated memory also
 * once you are done with it. This will cleanup all allocs along with the alloced mark.
 *
 * You may conveniently use fb_alloc_free_till_mark() to avoid having to manually free all
 * previous allocs in one go very easily.
 *
 * Now, it can be tricky to allocate a region permanently that you do not want freed because
 * exceptions pop the frame buffer stack using fb_alloc_free_till_mark(). Additionally, you may
 * actually want exceptions to do this until you know an allocation operation that has multiple
 * steps has succeeded. To handle these situations call fb_alloc_mark_permanent() after a complex
 * operation to prevent fb_alloc_free_till_mark() from freeing past the last marked alloc.
 *
 * When you want deallocate this permanent region just call fb_alloc_free_till_mark_permanent()
 * which will ignore the permanent mark and free backwards until it hits the previously allocated
 * mark.
 *
 * Note that fb_free() and fb_free_all() do not respect any marks and permanent regions.
 *
 * Regardings the flags below:
 * - FB_ALLOC_NO_HINT - fb_alloc doesn't do anything special.
 * - FB_ALLOC_PREFER_SPEED - fb_alloc will make sure the allocated region is in the fatest possible
 *                           memory. E.g. allocs will be in SRAM versus SDRAM if SDRAM is available.
 *                           Setting this flag affects where fb_alloc_all() gets RAM from. If this
 *                           flag is set then fb_alloc_all() will not use the SDRAM.
 * - FB_ALLOC_PREFER_SIZE - fb_alloc will make sure the allocated region is the largest possible
 *                          memory. E.g. allocs will be in SDRAM versus SRAM if SDRAM is available.
 *                          Setting this flag affects where fb_alloc_all() gets RAM from. If this
 *                          flag is set then fb_alloc_all() will use the SDRAM (default).
 * - FB_ALLOC_CACHE_ALIGN - Aligns the starting address returned to a cache line and makes sure
 *                          the amount of memory allocated is padded to the end of a cache line.
 */
#ifndef __FB_ALLOC_H__
#define __FB_ALLOC_H__
#include <stdint.h>
#define FB_ALLOC_NO_HINT 0
#define FB_ALLOC_PREFER_SPEED 1
#define FB_ALLOC_PREFER_SIZE 2
#define FB_ALLOC_CACHE_ALIGN 4
char *fb_alloc_stack_pointer();
void fb_alloc_fail();
void fb_alloc_init0();
uint32_t fb_avail();
void fb_alloc_mark();
void fb_alloc_free_till_mark();
void fb_alloc_mark_permanent(); // tag memory that should not be popped on exception
void fb_alloc_free_till_mark_past_mark_permanent(); // frees past marked permanent allocations
void *fb_alloc(uint32_t size, int hints);
void *fb_alloc0(uint32_t size, int hints);
void *fb_alloc_all(uint32_t *size, int hints); // returns pointer and sets size
void *fb_alloc0_all(uint32_t *size, int hints); // returns pointer and sets size
void fb_free();
void fb_free_all();
#endif /* __FF_ALLOC_H__ */

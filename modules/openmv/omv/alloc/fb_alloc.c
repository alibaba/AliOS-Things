/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Interface for using extra frame buffer RAM as a stack.
 *
 */
#include "py/obj.h"
#include "py/runtime.h"
#include "fb_alloc.h"
#include "framebuffer.h"
#include "omv_boardconfig.h"

#ifndef __DCACHE_PRESENT
#define FB_ALLOC_ALIGNMENT 32 // Use 32-byte alignment on MCUs with no cache for DMA buffer alignment.
#else
#define FB_ALLOC_ALIGNMENT __SCB_DCACHE_LINE_SIZE
#endif

extern char _fballoc;
static char *pointer = &_fballoc;

#if defined(FB_ALLOC_STATS)
static uint32_t alloc_bytes;
static uint32_t alloc_bytes_peak;
#endif

#if defined(OMV_FB_OVERLAY_MEMORY)
#define FB_OVERLAY_MEMORY_FLAG 0x1
extern char _fballoc_overlay_end, _fballoc_overlay_start;
static char *pointer_overlay = &_fballoc_overlay_end;
#endif

// fb_alloc_free_till_mark() will not free past this.
// Use fb_alloc_free_till_mark_permanent() instead.
#define FB_PERMANENT_FLAG 0x2

char *fb_alloc_stack_pointer()
{
    return pointer;
}

MP_WEAK NORETURN void fb_alloc_fail()
{
    mp_raise_msg(&mp_type_MemoryError,
        MP_ERROR_TEXT("Out of fast Frame Buffer Stack Memory!"
        " Please reduce the resolution of the image you are running this algorithm on to bypass this issue!"));
}

void fb_alloc_init0()
{
    pointer = &_fballoc;
    #if defined(OMV_FB_OVERLAY_MEMORY)
    pointer_overlay = &_fballoc_overlay_end;
    #endif
}

uint32_t fb_avail()
{
    uint32_t temp = pointer - framebuffer_get_buffers_end() - sizeof(uint32_t);
    return (temp < sizeof(uint32_t)) ? 0 : temp;
}

void fb_alloc_mark()
{
    char *new_pointer = pointer - sizeof(uint32_t);

    // Check if allocation overwrites the framebuffer pixels
    if (new_pointer < framebuffer_get_buffers_end()) {
        nlr_raise_for_fb_alloc_mark(mp_obj_new_exception_msg(&mp_type_MemoryError,
            MP_ERROR_TEXT("Out of fast Frame Buffer Stack Memory!"
            " Please reduce the resolution of the image you are running this algorithm on to bypass this issue!")));
    }

    // fb_alloc does not allow regions which are a size of 0 to be alloced,
    // meaning that the value below is always 8 or more but never 4. So,
    // we will use a size value of 4 as a marker in the alloc stack.
    *((uint32_t *) new_pointer) = sizeof(uint32_t); // Save size.
    pointer = new_pointer;
    #if defined(FB_ALLOC_STATS)
    alloc_bytes = 0;
    alloc_bytes_peak = 0;
    #endif
}

static void int_fb_alloc_free_till_mark(bool free_permanent)
{
    // Previously there was a marks counting method used to provide a semaphore lock for this code:
    //
    // https://github.com/openmv/openmv/commit/c982617523766018fda70c15818f643ee8b1fd33
    //
    // This does not really help you in complex memory allocation operations where you want to be
    // able to unwind things until after a certain point. It also did not handle preventing
    // fb_alloc_free_till_mark() from running in recursive call situations (see find_blobs()).
    while (pointer < &_fballoc) {
        uint32_t size = *((uint32_t *) pointer);
        if ((!free_permanent) && (size & FB_PERMANENT_FLAG)) return;
        size &= ~FB_PERMANENT_FLAG;
        #if defined(OMV_FB_OVERLAY_MEMORY)
        if (size & FB_OVERLAY_MEMORY_FLAG) { // Check for fast flag.
            size &= ~FB_OVERLAY_MEMORY_FLAG; // Remove it.
            pointer_overlay += size - sizeof(uint32_t);
        }
        #endif
        pointer += size; // Get size and pop.
        if (size == sizeof(uint32_t)) break; // Break on first marker.
    }
    #if defined(FB_ALLOC_STATS)
    printf("fb_alloc peak memory: %lu\n", alloc_bytes_peak);
    #endif
}

void fb_alloc_free_till_mark()
{
    int_fb_alloc_free_till_mark(false);
}

void fb_alloc_mark_permanent()
{
    if (pointer < &_fballoc) *((uint32_t *) pointer) |= FB_PERMANENT_FLAG;
}

void fb_alloc_free_till_mark_past_mark_permanent()
{
    int_fb_alloc_free_till_mark(true);
}

// returns null pointer without error if size==0
void *fb_alloc(uint32_t size, int hints)
{
    if (!size) {
        return NULL;
    }

    size = ((size + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t); // Round Up

    if (hints & FB_ALLOC_CACHE_ALIGN) {
        size = ((size + FB_ALLOC_ALIGNMENT - 1) / FB_ALLOC_ALIGNMENT) * FB_ALLOC_ALIGNMENT;
        size += FB_ALLOC_ALIGNMENT - sizeof(uint32_t);
    }

    char *result = pointer - size;
    char *new_pointer = result - sizeof(uint32_t);

    // Check if allocation overwrites the framebuffer pixels
    if (new_pointer < framebuffer_get_buffers_end()) {
        fb_alloc_fail();
    }

    // size is always 4/8/12/etc. so the value below must be 8 or more.
    *((uint32_t *) new_pointer) = size + sizeof(uint32_t); // Save size.
    pointer = new_pointer;

    #if defined(FB_ALLOC_STATS)
    alloc_bytes += size;
    if (alloc_bytes > alloc_bytes_peak) {
        alloc_bytes_peak = alloc_bytes;
    }
    printf("fb_alloc %lu bytes\n", size);
    #endif

    #if defined(OMV_FB_OVERLAY_MEMORY)
    if ((!(hints & FB_ALLOC_PREFER_SIZE))
    && (((uint32_t) (pointer_overlay - &_fballoc_overlay_start)) >= size)) {
        // Return overlay memory instead.
        pointer_overlay -= size;
        result = pointer_overlay;
        *new_pointer |= FB_OVERLAY_MEMORY_FLAG; // Add flag.
    }
    #endif

    if (hints & FB_ALLOC_CACHE_ALIGN) {
        int offset = ((uint32_t) result) % FB_ALLOC_ALIGNMENT;
        if (offset) {
            result += FB_ALLOC_ALIGNMENT - offset;
        }
    }

    return result;
}

// returns null pointer without error if passed size==0
void *fb_alloc0(uint32_t size, int hints)
{
    void *mem = fb_alloc(size, hints);
    memset(mem, 0, size); // does nothing if size is zero.
    return mem;
}

void *fb_alloc_all(uint32_t *size, int hints)
{
    uint32_t temp = pointer - framebuffer_get_buffers_end() - sizeof(uint32_t);

    if (temp < sizeof(uint32_t)) {
        *size = 0;
        return NULL;
    }

    #if defined(OMV_FB_OVERLAY_MEMORY)
    if (!(hints & FB_ALLOC_PREFER_SIZE)) {
        *size = (uint32_t) (pointer_overlay - &_fballoc_overlay_start);
        temp = IM_MIN(temp, *size);
    }
    #endif

    *size = (temp / sizeof(uint32_t)) * sizeof(uint32_t); // Round Down

    char *result = pointer - *size;
    char *new_pointer = result - sizeof(uint32_t);

    // size is always 4/8/12/etc. so the value below must be 8 or more.
    *((uint32_t *) new_pointer) = *size + sizeof(uint32_t); // Save size.
    pointer = new_pointer;

    #if defined(FB_ALLOC_STATS)
    alloc_bytes += *size;
    if (alloc_bytes > alloc_bytes_peak) {
        alloc_bytes_peak = alloc_bytes;
    }
    printf("fb_alloc_all %lu bytes\n", *size);
    #endif

    #if defined(OMV_FB_OVERLAY_MEMORY)
    if (!(hints & FB_ALLOC_PREFER_SIZE)) {
        // Return overlay memory instead.
        pointer_overlay -= *size;
        result = pointer_overlay;
        *new_pointer |= FB_OVERLAY_MEMORY_FLAG; // Add flag.
    }
    #endif

    if (hints & FB_ALLOC_CACHE_ALIGN) {
        int offset = ((uint32_t) result) % FB_ALLOC_ALIGNMENT;
        if (offset) {
            int inc = FB_ALLOC_ALIGNMENT - offset;
            result += inc;
            *size -= inc;
        }
        *size = (*size / FB_ALLOC_ALIGNMENT) * FB_ALLOC_ALIGNMENT;
    }

    return result;
}

// returns null pointer without error if returned size==0
void *fb_alloc0_all(uint32_t *size, int hints)
{
    void *mem = fb_alloc_all(size, hints);
    memset(mem, 0, *size); // does nothing if size is zero.
    return mem;
}

void fb_free()
{
    if (pointer < &_fballoc) {
        uint32_t size = *((uint32_t *) pointer);
        size &= ~FB_PERMANENT_FLAG;
        #if defined(OMV_FB_OVERLAY_MEMORY)
        if (size & FB_OVERLAY_MEMORY_FLAG) { // Check for fast flag.
            size &= ~FB_OVERLAY_MEMORY_FLAG; // Remove it.
            pointer_overlay += size - sizeof(uint32_t);
        }
        #endif
        #if defined(FB_ALLOC_STATS)
        alloc_bytes -= size;
        #endif
        pointer += size; // Get size and pop.
    }
}

void fb_free_all()
{
    while (pointer < &_fballoc) {
        uint32_t size = *((uint32_t *) pointer);
        size &= ~FB_PERMANENT_FLAG;
        #if defined(OMV_FB_OVERLAY_MEMORY)
        if (size & FB_OVERLAY_MEMORY_FLAG) { // Check for fast flag.
            size &= ~FB_OVERLAY_MEMORY_FLAG; // Remove it.
            pointer_overlay += size - sizeof(uint32_t);
        }
        #endif
        #if defined(FB_ALLOC_STATS)
        alloc_bytes -= size;
        #endif
        pointer += size; // Get size and pop.
    }
}

/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * AprilTags library.
 */
#include <float.h>
#include <stdarg.h>
#include <stdio.h>
#include "imlib.h"

// Enable new code optimizations
#define OPTIMIZED

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

/* Copyright (C) 2013-2016, The Regents of The University of Michigan.
All rights reserved.

This software was developed in the APRIL Robotics Lab under the
direction of Edwin Olson, ebolson@umich.edu. This software may be
available under alternative licensing terms; contact the address above.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the Regents of The University of Michigan.
*/
#define fprintf(format, ...)
#define free(ptr) ({ umm_free(ptr); })
#define malloc(size) ({ void *_r = umm_malloc(size); if(!_r) umm_alloc_fail(); _r; })
#define realloc(ptr, size) ({ void *_r = umm_realloc((ptr), (size)); if(!_r) umm_alloc_fail(); _r; })
#define calloc(num, item_size) ({ void *_r = umm_calloc((num), (item_size)); if(!_r) umm_alloc_fail(); _r; })
#define assert(expression)
#define sqrt(x) fast_sqrtf(x)
#define sqrtf(x) fast_sqrtf(x)
#define floor(x) fast_floorf(x)
#define floorf(x) fast_floorf(x)
#define ceil(x) fast_ceilf(x)
#define ceilf(x) fast_ceilf(x)
#define round(x) fast_roundf(x)
#define roundf(x) fast_roundf(x)
#define atan(x) fast_atanf(x)
#define atanf(x) fast_atanf(x)
#define atan2(y, x) fast_atan2f((y), (x))
#define atan2f(y, x) fast_atan2f((y), (x))
#define exp(x) fast_expf(x)
#define expf(x) fast_expf(x)
#define cbrt(x) fast_cbrtf(x)
#define cbrtf(x) fast_cbrtf(x)
#define fabs(x) fast_fabsf(x)
#define fabsf(x) fast_fabsf(x)
#define log(x) fast_log(x)
#define logf(x) fast_log(x)
#undef log2
#define log2(x) fast_log2(x)
#undef log2f
#define log2f(x) fast_log2(x)
#define sin(x) arm_sin_f32(x)
#define cos(x) arm_cos_f32(x)
#define fmin(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define fminf(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define fmax(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define fmaxf(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "zarray.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Defines a structure which acts as a resize-able array ala Java's ArrayList.
 */
typedef struct zarray zarray_t;
struct zarray
{
    size_t el_sz; // size of each element

    int size; // how many elements?
    int alloc; // we've allocated storage for how many elements?
    char *data;
};

/**
 * Creates and returns a variable array structure capable of holding elements of
 * the specified size. It is the caller's responsibility to call zarray_destroy()
 * on the returned array when it is no longer needed.
 */
static inline zarray_t *zarray_create(size_t el_sz)
{
    assert(el_sz > 0);

    zarray_t *za = (zarray_t*) calloc(1, sizeof(zarray_t));
    za->el_sz = el_sz;
    return za;
}

/**
 * Creates and returns a variable array structure capable of holding elements of
 * the specified size. It is the caller's responsibility to call zarray_destroy()
 * on the returned array when it is no longer needed.
 */
static inline zarray_t *zarray_create_fail_ok(size_t el_sz)
{
    assert(el_sz > 0);

    zarray_t *za = (zarray_t*) umm_calloc(1, sizeof(zarray_t));
    if (za) za->el_sz = el_sz;
    return za;
}

/**
 * Frees all resources associated with the variable array structure which was
 * created by zarray_create(). After calling, 'za' will no longer be valid for storage.
 */
static inline void zarray_destroy(zarray_t *za)
{
    if (za == NULL)
        return;

    if (za->data != NULL)
        free(za->data);
    memset(za, 0, sizeof(zarray_t));
    free(za);
}

/** Allocate a new zarray that contains a copy of the data in the argument. **/
static inline zarray_t *zarray_copy(const zarray_t *za)
{
    assert(za != NULL);

    zarray_t *zb = (zarray_t*) calloc(1, sizeof(zarray_t));
    zb->el_sz = za->el_sz;
    zb->size = za->size;
    zb->alloc = za->alloc;
    zb->data = (char*) malloc(zb->alloc * zb->el_sz);
    memcpy(zb->data, za->data, za->size * za->el_sz);
    return zb;
}

static int iceillog2(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

/**
 * Allocate a new zarray that contains a subset of the original
 * elements. NOTE: end index is EXCLUSIVE, that is one past the last
 * element you want.
 */
static inline zarray_t *zarray_copy_subset(const zarray_t *za,
                             int start_idx,
                             int end_idx_exclusive)
{
    zarray_t *out = (zarray_t*) calloc(1, sizeof(zarray_t));
    out->el_sz = za->el_sz;
    out->size = end_idx_exclusive - start_idx;
    out->alloc = iceillog2(out->size); // round up pow 2
    out->data = (char*) malloc(out->alloc * out->el_sz);
    memcpy(out->data,  za->data +(start_idx*out->el_sz), out->size*out->el_sz);
    return out;
}

/**
 * Retrieves the number of elements currently being contained by the passed
 * array, which may be different from its capacity. The index of the last element
 * in the array will be one less than the returned value.
 */
static inline int zarray_size(const zarray_t *za)
{
    assert(za != NULL);

    return za->size;
}

/**
 * Returns 1 if zarray_size(za) == 0,
 * returns 0 otherwise.
 */
/*
JUST CALL zarray_size
int zarray_isempty(const zarray_t *za)
{
    assert(za != NULL);
    if (za->size <= 0)
        return 1;
    else
        return 0;
}
*/


/**
 * Allocates enough internal storage in the supplied variable array structure to
 * guarantee that the supplied number of elements (capacity) can be safely stored.
 */
static inline void zarray_ensure_capacity(zarray_t *za, int capacity)
{
    assert(za != NULL);

    if (capacity <= za->alloc)
        return;

    while (za->alloc < capacity) {
        za->alloc += 8; // use less memory // *= 2;
        if (za->alloc < 8)
            za->alloc = 8;
    }

    za->data = (char*) realloc(za->data, za->alloc * za->el_sz);
}

/**
 * Adds a new element to the end of the supplied array, and sets its value
 * (by copying) from the data pointed to by the supplied pointer 'p'.
 * Automatically ensures that enough storage space is available for the new element.
 */
static inline void zarray_add(zarray_t *za, const void *p)
{
    assert(za != NULL);
    assert(p != NULL);

    zarray_ensure_capacity(za, za->size + 1);

    memcpy(&za->data[za->size*za->el_sz], p, za->el_sz);
    za->size++;
}

/**
 * Adds a new element to the end of the supplied array, and sets its value
 * (by copying) from the data pointed to by the supplied pointer 'p'.
 * Automatically ensures that enough storage space is available for the new element.
 */
static inline void zarray_add_fail_ok(zarray_t *za, const void *p)
{
    assert(za != NULL);
    assert(p != NULL);

    if ((za->size + 1) > za->alloc)
    {
        char *old_data = za->data;
        int old_alloc = za->alloc;

        while (za->alloc < (za->size + 1)) {
            za->alloc += 8; // use less memory // *= 2;
            if (za->alloc < 8)
                za->alloc = 8;
        }

        za->data = (char*) umm_realloc(za->data, za->alloc * za->el_sz);

        if (!za->data) {
            za->data = old_data;
            za->alloc = old_alloc;
            return;
        }
    }

    memcpy(&za->data[za->size*za->el_sz], p, za->el_sz);
    za->size++;
}

/**
 * Retrieves the element from the supplied array located at the zero-based
 * index of 'idx' and copies its value into the variable pointed to by the pointer
 * 'p'.
 */
static inline void zarray_get(const zarray_t *za, int idx, void *p)
{
    assert(za != NULL);
    assert(p != NULL);
    assert(idx >= 0);
    assert(idx < za->size);

    memcpy(p, &za->data[idx*za->el_sz], za->el_sz);
}

/**
 * Similar to zarray_get(), but returns a "live" pointer to the internal
 * storage, avoiding a memcpy. This pointer is not valid across
 * operations which might move memory around (i.e. zarray_remove_value(),
 * zarray_remove_index(), zarray_insert(), zarray_sort(), zarray_clear()).
 * 'p' should be a pointer to the pointer which will be set to the internal address.
 */
inline static void zarray_get_volatile(const zarray_t *za, int idx, void *p)
{
    assert(za != NULL);
    assert(p != NULL);
    assert(idx >= 0);
    assert(idx < za->size);

    *((void**) p) = &za->data[idx*za->el_sz];
}

inline static void zarray_truncate(zarray_t *za, int sz)
{
   assert(za != NULL);
   assert(sz <= za->size);
   za->size = sz;
}

/**
 * Copies the memory array used internally by zarray to store its owned
 * elements to the address pointed by 'buffer'. It is the caller's responsibility
 * to allocate zarray_size()*el_sz bytes for the copy to be stored and
 * to free the memory when no longer needed. The memory allocated at 'buffer'
 * and the internal zarray storage must not overlap. 'buffer_bytes' should be
 * the size of the 'buffer' memory space, in bytes, and must be at least
 * zarray_size()*el_sz.
 *
 * Returns the number of bytes copied into 'buffer'.
 */
static inline size_t zarray_copy_data(const zarray_t *za, void *buffer, size_t buffer_bytes)
{
    assert(za != NULL);
    assert(buffer != NULL);
    assert(buffer_bytes >= za->el_sz * za->size);
    memcpy(buffer, za->data, za->el_sz * za->size);
    return za->el_sz * za->size;
}

/**
 * Removes the entry at index 'idx'.
 * If shuffle is true, the last element in the array will be placed in
 * the newly-open space; if false, the zarray is compacted.
 */
static inline void zarray_remove_index(zarray_t *za, int idx, int shuffle)
{
    assert(za != NULL);
    assert(idx >= 0);
    assert(idx < za->size);

    if (shuffle) {
        if (idx < za->size-1)
            memcpy(&za->data[idx*za->el_sz], &za->data[(za->size-1)*za->el_sz], za->el_sz);
        za->size--;
        return;
    } else {
        // size = 10, idx = 7. Should copy 2 entries (at idx=8 and idx=9).
        // size = 10, idx = 9. Should copy 0 entries.
        int ncopy = za->size - idx - 1;
        if (ncopy > 0)
            memmove(&za->data[idx*za->el_sz], &za->data[(idx+1)*za->el_sz], ncopy*za->el_sz);
        za->size--;
        return;
    }
}

/**
 * Remove the entry whose value is equal to the value pointed to by 'p'.
 * If shuffle is true, the last element in the array will be placed in
 * the newly-open space; if false, the zarray is compacted. At most
 * one element will be removed.
 *
 * Note that objects will be compared using memcmp over the full size
 * of the value. If the value is a struct that contains padding,
 * differences in the padding bytes can cause comparisons to
 * fail. Thus, it remains best practice to bzero all structs so that
 * the padding is set to zero.
 *
 * Returns the number of elements removed (0 or 1).
 */
// remove the entry whose value is equal to the value pointed to by p.
// if shuffle is true, the last element in the array will be placed in
// the newly-open space; if false, the zarray is compacted.
static inline int zarray_remove_value(zarray_t *za, const void *p, int shuffle)
{
    assert(za != NULL);
    assert(p != NULL);

    for (int idx = 0; idx < za->size; idx++) {
        if (!memcmp(p, &za->data[idx*za->el_sz], za->el_sz)) {
            zarray_remove_index(za, idx, shuffle);
            return 1;
        }
    }

    return 0;
}


/**
 * Creates a new entry and inserts it into the array so that it will have the
 * index 'idx' (i.e. before the item which currently has that index). The value
 * of the new entry is set to (copied from) the data pointed to by 'p'. 'idx'
 * can be one larger than the current max index to place the new item at the end
 * of the array, or zero to add it to an empty array.
 */
static inline void zarray_insert(zarray_t *za, int idx, const void *p)
{
    assert(za != NULL);
    assert(p != NULL);
    assert(idx >= 0);
    assert(idx <= za->size);

    zarray_ensure_capacity(za, za->size + 1);
    // size = 10, idx = 7. Should copy three entries (idx=7, idx=8, idx=9)
    int ncopy = za->size - idx;

    memmove(&za->data[(idx+1)*za->el_sz], &za->data[idx*za->el_sz], ncopy*za->el_sz);
    memcpy(&za->data[idx*za->el_sz], p, za->el_sz);

    za->size++;
}


/**
 * Sets the value of the current element at index 'idx' by copying its value from
 * the data pointed to by 'p'. The previous value of the changed element will be
 * copied into the data pointed to by 'outp' if it is not null.
 */
static inline void zarray_set(zarray_t *za, int idx, const void *p, void *outp)
{
    assert(za != NULL);
    assert(p != NULL);
    assert(idx >= 0);
    assert(idx < za->size);

    if (outp != NULL)
        memcpy(outp, &za->data[idx*za->el_sz], za->el_sz);

    memcpy(&za->data[idx*za->el_sz], p, za->el_sz);
}

/**
 * Calls the supplied function for every element in the array in index order.
 * The map function will be passed a pointer to each element in turn and must
 * have the following format:
 *
 * void map_function(element_type *element)
 */
static inline void zarray_map(zarray_t *za, void (*f)(void*))
{
    assert(za != NULL);
    assert(f != NULL);

    for (int idx = 0; idx < za->size; idx++)
        f(&za->data[idx*za->el_sz]);
}

/**
 * Calls the supplied function for every element in the array in index order.
 * HOWEVER values are passed to the function, not pointers to values. In the
 * case where the zarray stores object pointers, zarray_vmap allows you to
 * pass in the object's destroy function (or free) directly. Can only be used
 * with zarray's which contain pointer data. The map function should have the
 * following format:
 *
 * void map_function(element_type *element)
 */
    void zarray_vmap(zarray_t *za, void (*f)());

/**
 * Removes all elements from the array and sets its size to zero. Pointers to
 * any data elements obtained i.e. by zarray_get_volatile() will no longer be
 * valid.
 */
static inline void zarray_clear(zarray_t *za)
{
    assert(za != NULL);
    za->size = 0;
}

/**
 * Determines whether any element in the array has a value which matches the
 * data pointed to by 'p'.
 *
 * Returns 1 if a match was found anywhere in the array, else 0.
 */
static inline int zarray_contains(const zarray_t *za, const void *p)
{
    assert(za != NULL);
    assert(p != NULL);

    for (int idx = 0; idx < za->size; idx++) {
        if (!memcmp(p, &za->data[idx*za->el_sz], za->el_sz)) {
            return 1;
        }
    }

    return 0;
}

/**
 * Uses qsort() to sort the elements contained by the array in ascending order.
 * Uses the supplied comparison function to determine the appropriate order.
 *
 * The comparison function will be passed a pointer to two elements to be compared
 * and should return a measure of the difference between them (see strcmp()).
 * I.e. it should return a negative number if the first element is 'less than'
 * the second, zero if they are equivalent, and a positive number if the first
 * element is 'greater than' the second. The function should have the following format:
 *
 * int comparison_function(const element_type *first, const element_type *second)
 *
 * zstrcmp() can be used as the comparison function for string elements, which
 * will call strcmp() internally.
 */
static inline void zarray_sort(zarray_t *za, int (*compar)(const void*, const void*))
{
    assert(za != NULL);
    assert(compar != NULL);
    if (za->size == 0)
        return;

    qsort(za->data, za->size, za->el_sz, compar);
}

/**
 * A comparison function for comparing strings which can be used by zarray_sort()
 * to sort arrays with char* elements.
 */
    int zstrcmp(const void * a_pp, const void * b_pp);

/**
  * Find the index of an element, or return -1 if not found. Remember that p is
  * a pointer to the element.
 **/
// returns -1 if not in array. Remember p is a pointer to the item.
static inline int zarray_index_of(const zarray_t *za, const void *p)
{
    assert(za != NULL);
    assert(p != NULL);

    for (int i = 0; i < za->size; i++) {
        if (!memcmp(p, &za->data[i*za->el_sz], za->el_sz))
            return i;
    }

    return -1;
}



/**
 * Add all elements from 'source' into 'dest'. el_size must be the same
 * for both lists
 **/
static inline void zarray_add_all(zarray_t * dest, const zarray_t * source)
{
    assert(dest->el_sz == source->el_sz);

    // Don't allocate on stack because el_sz could be larger than ~8 MB
    // stack size
    char *tmp = (char*)calloc(1, dest->el_sz);

    for (int i = 0; i < zarray_size(source); i++) {
        zarray_get(source, i, tmp);
        zarray_add(dest, tmp);
   }

    free(tmp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "zarray.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

int zstrcmp(const void * a_pp, const void * b_pp)
{
    assert(a_pp != NULL);
    assert(b_pp != NULL);

    char * a = *(void**)a_pp;
    char * b = *(void**)b_pp;

    return strcmp(a,b);
}

void zarray_vmap(zarray_t *za, void (*f)())
{
    assert(za != NULL);
    assert(f != NULL);
    assert(za->el_sz == sizeof(void*));

    for (int idx = 0; idx < za->size; idx++) {
        void *pp = &za->data[idx*za->el_sz];
        void *p = *(void**) pp;
        f(p);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "math_util.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef M_TWOPI
# define M_TWOPI       6.2831853071795862319959  /* 2*pi */
#endif

#ifndef M_PI
# define M_PI 3.141592653589793238462643383279502884196
#endif

#define to_radians(x) ( (x) * (M_PI / 180.0 ))
#define to_degrees(x) ( (x) * (180.0 / M_PI ))

#define max(A, B) (A < B ? B : A)
#define min(A, B) (A < B ? A : B)

  /* DEPRECATE, threshold meaningless without context.
static inline int dequals(float a, float b)
{
    float thresh = 1e-9;
    return (fabs(a-b) < thresh);
}
  */

static inline int dequals_mag(float a, float b, float thresh)
{
    return (fabs(a-b) < thresh);
}

static inline int isq(int v)
{
    return v*v;
}

static inline float fsq(float v)
{
    return v*v;
}

static inline float sq(float v)
{
    return v*v;
}

static inline float sgn(float v)
{
    return (v>=0) ? 1 : -1;
}

// random number between [0, 1)
static inline float randf()
{
    return ((float) rand()) / (RAND_MAX + 1.0);
}


static inline float signed_randf()
{
    return randf()*2 - 1;
}

// return a random integer between [0, bound)
static inline int irand(int bound)
{
    int v = (int) (randf()*bound);
    if (v == bound)
        return (bound-1);
    //assert(v >= 0);
    //assert(v < bound);
    return v;
}

/** Map vin to [0, 2*PI) **/
static inline float mod2pi_positive(float vin)
{
    return vin - M_TWOPI * floor(vin / M_TWOPI);
}

/** Map vin to [-PI, PI) **/
static inline float mod2pi(float vin)
{
    return mod2pi_positive(vin + M_PI) - M_PI;
}

/** Return vin such that it is within PI degrees of ref **/
static inline float mod2pi_ref(float ref, float vin)
{
    return ref + mod2pi(vin - ref);
}

/** Map vin to [0, 360) **/
static inline float mod360_positive(float vin)
{
    return vin - 360 * floor(vin / 360);
}

/** Map vin to [-180, 180) **/
static inline float mod360(float vin)
{
    return mod360_positive(vin + 180) - 180;
}

static inline int theta_to_int(float theta, int max)
{
    theta = mod2pi_ref(M_PI, theta);
    int v = (int) (theta / M_TWOPI * max);

    if (v == max)
        v = 0;

    assert (v >= 0 && v < max);

    return v;
}

static inline int imin(int a, int b)
{
    return (a < b) ? a : b;
}

static inline int imax(int a, int b)
{
    return (a > b) ? a : b;
}

static inline int64_t imin64(int64_t a, int64_t b)
{
    return (a < b) ? a : b;
}

static inline int64_t imax64(int64_t a, int64_t b)
{
    return (a > b) ? a : b;
}

static inline int iclamp(int v, int minv, int maxv)
{
    return imax(minv, imin(v, maxv));
}

static inline float dclamp(float a, float min, float max)
{
    if (a < min)
        return min;
    if (a > max)
        return max;
    return a;
}

static inline int fltcmp (float f1, float f2)
{
    float epsilon = f1-f2;
    if (epsilon < 0.0)
        return -1;
    else if (epsilon > 0.0)
        return  1;
    else
        return  0;
}

static inline int dblcmp (float d1, float d2)
{
    float epsilon = d1-d2;
    if (epsilon < 0.0)
        return -1;
    else if (epsilon > 0.0)
        return  1;
    else
        return  0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "svd22.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

void svd22(const float A[4], float U[4], float S[2], float V[4]);

// for the matrix [a b; b d]
void svd_sym_singular_values(float A00, float A01, float A11,
                             float *Lmin, float *Lmax);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "svd22.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

/** SVD 2x2.

    Computes singular values and vectors without squaring the input
    matrix. With double precision math, results are accurate to about
    1E-16.

    U = [ cos(theta) -sin(theta) ]
        [ sin(theta)  cos(theta) ]

    S = [ e  0 ]
        [ 0  f ]

    V = [ cos(phi)   -sin(phi) ]
        [ sin(phi)   cos(phi)  ]


    Our strategy is basically to analytically multiply everything out
    and then rearrange so that we can solve for theta, phi, e, and
    f. (Derivation by ebolson@umich.edu 5/2016)

   V' = [ CP  SP ]
        [ -SP CP ]

USV' = [ CT -ST ][  e*CP  e*SP ]
       [ ST  CT ][ -f*SP  f*CP ]

     = [e*CT*CP + f*ST*SP     e*CT*SP - f*ST*CP ]
       [e*ST*CP - f*SP*CT     e*SP*ST + f*CP*CT ]

A00+A11 = e*CT*CP + f*ST*SP + e*SP*ST + f*CP*CT
        = e*(CP*CT + SP*ST) + f*(SP*ST + CP*CT)
        = (e+f)(CP*CT + SP*ST)
B0	    = (e+f)*cos(P-T)

A00-A11 = e*CT*CP + f*ST*SP - e*SP*ST - f*CP*CT
        = e*(CP*CT - SP*ST) - f*(-ST*SP + CP*CT)
        = (e-f)(CP*CT - SP*ST)
B1	    = (e-f)*cos(P+T)

A01+A10 = e*CT*SP - f*ST*CP + e*ST*CP - f*SP*CT
        = e(CT*SP + ST*CP) - f*(ST*CP + SP*CT)
        = (e-f)*(CT*SP + ST*CP)
B2	    = (e-f)*sin(P+T)

A01-A10 = e*CT*SP - f*ST*CP - e*ST*CP + f*SP*CT
    = e*(CT*SP - ST*CP) + f(SP*CT - ST*CP)
    = (e+f)*(CT*SP - ST*CP)
B3	= (e+f)*sin(P-T)

B0 = (e+f)*cos(P-T)
B1 = (e-f)*cos(P+T)
B2 = (e-f)*sin(P+T)
B3 = (e+f)*sin(P-T)

B3/B0 = tan(P-T)

B2/B1 = tan(P+T)
 **/
void svd22(const float A[4], float U[4], float S[2], float V[4])
{
    float A00 = A[0];
    float A01 = A[1];
    float A10 = A[2];
    float A11 = A[3];

    float B0 = A00 + A11;
    float B1 = A00 - A11;
    float B2 = A01 + A10;
    float B3 = A01 - A10;

    float PminusT = atan2(B3, B0);
    float PplusT = atan2(B2, B1);

    float P = (PminusT + PplusT) / 2;
    float T = (-PminusT + PplusT) / 2;

    float CP = cos(P), SP = sin(P);
    float CT = cos(T), ST = sin(T);

    U[0] = CT;
    U[1] = -ST;
    U[2] = ST;
    U[3] = CT;

    V[0] = CP;
    V[1] = -SP;
    V[2] = SP;
    V[3] = CP;

    // C0 = e+f. There are two ways to compute C0; we pick the one
    // that is better conditioned.
    float CPmT = cos(P-T), SPmT = sin(P-T);
    float C0 = 0;
    if (fabs(CPmT) > fabs(SPmT))
        C0 = B0 / CPmT;
    else
        C0 = B3 / SPmT;

    // C1 = e-f. There are two ways to compute C1; we pick the one
    // that is better conditioned.
    float CPpT = cos(P+T), SPpT = sin(P+T);
    float C1 = 0;
    if (fabs(CPpT) > fabs(SPpT))
        C1 = B1 / CPpT;
    else
        C1 = B2 / SPpT;

    // e and f are the singular values
    float e = (C0 + C1) / 2;
    float f = (C0 - C1) / 2;

    if (e < 0) {
        e = -e;
        U[0] = -U[0];
        U[2] = -U[2];
    }

    if (f < 0) {
        f = -f;
        U[1] = -U[1];
        U[3] = -U[3];
    }

    // sort singular values.
    if (e > f) {
        // already in big-to-small order.
        S[0] = e;
        S[1] = f;
    } else {
        // Curiously, this code never seems to get invoked.  Why is it
        // that S[0] always ends up the dominant vector?  However,
        // this code has been tested (flipping the logic forces us to
        // sort the singular values in ascending order).
        //
        // P = [ 0 1 ; 1 0 ]
        // USV' = (UP)(PSP)(PV')
        //      = (UP)(PSP)(VP)'
        //      = (UP)(PSP)(P'V')'
        S[0] = f;
        S[1] = e;

        // exchange columns of U and V
        float tmp[2];
        tmp[0] = U[0];
        tmp[1] = U[2];
        U[0] = U[1];
        U[2] = U[3];
        U[1] = tmp[0];
        U[3] = tmp[1];

        tmp[0] = V[0];
        tmp[1] = V[2];
        V[0] = V[1];
        V[2] = V[3];
        V[1] = tmp[0];
        V[3] = tmp[1];
    }

    /*
    float SM[4] = { S[0], 0, 0, S[1] };

    doubles_print_mat(U, 2, 2, "%20.10g");
    doubles_print_mat(SM, 2, 2, "%20.10g");
    doubles_print_mat(V, 2, 2, "%20.10g");
    printf("A:\n");
    doubles_print_mat(A, 2, 2, "%20.10g");

    float SVt[4];
    doubles_mat_ABt(SM, 2, 2, V, 2, 2, SVt, 2, 2);
    float USVt[4];
    doubles_mat_AB(U, 2, 2, SVt, 2, 2, USVt, 2, 2);

    printf("USVt\n");
    doubles_print_mat(USVt, 2, 2, "%20.10g");

    float diff[4];
    for (int i = 0; i < 4; i++)
        diff[i] = A[i] - USVt[i];

    printf("diff\n");
    doubles_print_mat(diff, 2, 2, "%20.10g");

    */

}


// for the matrix [a b; b d]
void svd_sym_singular_values(float A00, float A01, float A11,
                             float *Lmin, float *Lmax)
{
    float A10 = A01;

    float B0 = A00 + A11;
    float B1 = A00 - A11;
    float B2 = A01 + A10;
    float B3 = A01 - A10;

    float PminusT = atan2(B3, B0);
    float PplusT = atan2(B2, B1);

    float P = (PminusT + PplusT) / 2;
    float T = (-PminusT + PplusT) / 2;

    // C0 = e+f. There are two ways to compute C0; we pick the one
    // that is better conditioned.
    float CPmT = cos(P-T), SPmT = sin(P-T);
    float C0 = 0;
    if (fabs(CPmT) > fabs(SPmT))
        C0 = B0 / CPmT;
    else
        C0 = B3 / SPmT;

    // C1 = e-f. There are two ways to compute C1; we pick the one
    // that is better conditioned.
    float CPpT = cos(P+T), SPpT = sin(P+T);
    float C1 = 0;
    if (fabs(CPpT) > fabs(SPpT))
        C1 = B1 / CPpT;
    else
        C1 = B2 / SPpT;

    // e and f are the singular values
    float e = (C0 + C1) / 2;
    float f = (C0 - C1) / 2;

    *Lmin = fmin(e, f);
    *Lmax = fmax(e, f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "matd.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Defines a matrix structure for holding float-precision values with
 * data in row-major order (i.e. index = row*ncols + col).
 *
 * nrows and ncols are 1-based counts with the exception that a scalar (non-matrix)
 *   is represented with nrows=0 and/or ncols=0.
 */
typedef struct
{
    unsigned int nrows, ncols;
    float data[];
//    float *data;
} matd_t;

#define MATD_ALLOC(name, nrows, ncols) float name ## _storage [nrows*ncols]; matd_t name = { .nrows = nrows, .ncols = ncols, .data = &name ## _storage };

/**
 * Defines a small value which can be used in place of zero for approximating
 * calculations which are singular at zero values (i.e. inverting a matrix with
 * a zero or near-zero determinant).
 */
#define MATD_EPS 1e-8

/**
 * A macro to reference a specific matd_t data element given it's zero-based
 * row and column indexes. Suitable for both retrieval and assignment.
 */
#define MATD_EL(m, row, col) (m)->data[((row)*(m)->ncols + (col))]

/**
 * Creates a float matrix with the given number of rows and columns (or a scalar
 * in the case where rows=0 and/or cols=0). All data elements will be initialized
 * to zero. It is the caller's responsibility to call matd_destroy() on the
 * returned matrix.
 */
matd_t *matd_create(int rows, int cols);

/**
 * Creates a float matrix with the given number of rows and columns (or a scalar
 * in the case where rows=0 and/or cols=0). All data elements will be initialized
 * using the supplied array of data, which must contain at least rows*cols elements,
 * arranged in row-major order (i.e. index = row*ncols + col). It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
matd_t *matd_create_data(int rows, int cols, const float *data);

/**
 * Creates a float matrix with the given number of rows and columns (or a scalar
 * in the case where rows=0 and/or cols=0). All data elements will be initialized
 * using the supplied array of float data, which must contain at least rows*cols elements,
 * arranged in row-major order (i.e. index = row*ncols + col). It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
matd_t *matd_create_dataf(int rows, int cols, const float *data);

/**
 * Creates a square identity matrix with the given number of rows (and
 * therefore columns), or a scalar with value 1 in the case where dim=0.
 * It is the caller's responsibility to call matd_destroy() on the
 * returned matrix.
 */
matd_t *matd_identity(int dim);

/**
 * Creates a scalar with the supplied value 'v'. It is the caller's responsibility
 * to call matd_destroy() on the returned matrix.
 *
 * NOTE: Scalars are different than 1x1 matrices (implementation note:
 * they are encoded as 0x0 matrices). For example: for matrices A*B, A
 * and B must both have specific dimensions. However, if A is a
 * scalar, there are no restrictions on the size of B.
 */
matd_t *matd_create_scalar(float v);

/**
 * Retrieves the cell value for matrix 'm' at the given zero-based row and column index.
 * Performs more thorough validation checking than MATD_EL().
 */
float matd_get(const matd_t *m, int row, int col);

/**
 * Assigns the given value to the matrix cell at the given zero-based row and
 * column index. Performs more thorough validation checking than MATD_EL().
 */
void matd_put(matd_t *m, int row, int col, float value);

/**
 * Retrieves the scalar value of the given element ('m' must be a scalar).
 * Performs more thorough validation checking than MATD_EL().
 */
float matd_get_scalar(const matd_t *m);

/**
 * Assigns the given value to the supplied scalar element ('m' must be a scalar).
 * Performs more thorough validation checking than MATD_EL().
 */
void matd_put_scalar(matd_t *m, float value);

/**
 * Creates an exact copy of the supplied matrix 'm'. It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
matd_t *matd_copy(const matd_t *m);

/**
 * Creates a copy of a subset of the supplied matrix 'a'. The subset will include
 * rows 'r0' through 'r1', inclusive ('r1' >= 'r0'), and columns 'c0' through 'c1',
 * inclusive ('c1' >= 'c0'). All parameters are zero-based (i.e. matd_select(a, 0, 0, 0, 0)
 * will return only the first cell). Cannot be used on scalars or to extend
 * beyond the number of rows/columns of 'a'. It is the caller's  responsibility to
 * call matd_destroy() on the returned matrix.
 */
matd_t *matd_select(const matd_t *a, int r0, int r1, int c0, int c1);

/**
 * Prints the supplied matrix 'm' to standard output by applying the supplied
 * printf format specifier 'fmt' for each individual element. Each row will
 * be printed on a separate newline.
 */
void matd_print(const matd_t *m, const char *fmt);

/**
 * Prints the transpose of the supplied matrix 'm' to standard output by applying
 * the supplied printf format specifier 'fmt' for each individual element. Each
 * row will be printed on a separate newline.
 */
void matd_print_transpose(const matd_t *m, const char *fmt);

/**
 * Adds the two supplied matrices together, cell-by-cell, and returns the results
 * as a new matrix of the same dimensions. The supplied matrices must have
 * identical dimensions.  It is the caller's responsibility to call matd_destroy()
 * on the returned matrix.
 */
matd_t *matd_add(const matd_t *a, const matd_t *b);

/**
 * Adds the values of 'b' to matrix 'a', cell-by-cell, and overwrites the
 * contents of 'a' with the results. The supplied matrices must have
 * identical dimensions.
 */
void matd_add_inplace(matd_t *a, const matd_t *b);

/**
 * Subtracts matrix 'b' from matrix 'a', cell-by-cell, and returns the results
 * as a new matrix of the same dimensions. The supplied matrices must have
 * identical dimensions.  It is the caller's responsibility to call matd_destroy()
 * on the returned matrix.
 */
matd_t *matd_subtract(const matd_t *a, const matd_t *b);

/**
 * Subtracts the values of 'b' from matrix 'a', cell-by-cell, and overwrites the
 * contents of 'a' with the results. The supplied matrices must have
 * identical dimensions.
 */
void matd_subtract_inplace(matd_t *a, const matd_t *b);

/**
 * Scales all cell values of matrix 'a' by the given scale factor 's' and
 * returns the result as a new matrix of the same dimensions. It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
matd_t *matd_scale(const matd_t *a, float s);

/**
 * Scales all cell values of matrix 'a' by the given scale factor 's' and
 * overwrites the contents of 'a' with the results.
 */
void matd_scale_inplace(matd_t *a, float s);

/**
 * Multiplies the two supplied matrices together (matrix product), and returns the
 * results as a new matrix. The supplied matrices must have dimensions such that
 * columns(a) = rows(b). The returned matrix will have a row count of rows(a)
 * and a column count of columns(b). It is the caller's responsibility to call
 * matd_destroy() on the returned matrix.
 */
matd_t *matd_multiply(const matd_t *a, const matd_t *b);

/**
 * Creates a matrix which is the transpose of the supplied matrix 'a'. It is the
 * caller's responsibility to call matd_destroy() on the returned matrix.
 */
matd_t *matd_transpose(const matd_t *a);

/**
 * Calculates the determinant of the supplied matrix 'a'.
 */
float matd_det(const matd_t *a);

/**
 * Attempts to compute an inverse of the supplied matrix 'a' and return it as
 * a new matrix. This is strictly only possible if the determinant of 'a' is
 * non-zero (matd_det(a) != 0).
 *
 * If the determinant is zero, NULL is returned. It is otherwise the
 * caller's responsibility to cope with the results caused by poorly
 * conditioned matrices. (E.g.., if such a situation is likely to arise, compute
 * the pseudo-inverse from the SVD.)
 **/
matd_t *matd_inverse(const matd_t *a);

static inline void matd_set_data(matd_t *m, const float *data)
{
    memcpy(m->data, data, m->nrows * m->ncols * sizeof(float));
}

/**
 * Determines whether the supplied matrix 'a' is a scalar (positive return) or
 * not (zero return, indicating a matrix of dimensions at least 1x1).
 */
static inline int matd_is_scalar(const matd_t *a)
{
    assert(a != NULL);
    return a->ncols == 0 || a->nrows == 0;
}

/**
 * Determines whether the supplied matrix 'a' is a row or column vector
 * (positive return) or not (zero return, indicating either 'a' is a scalar or a
 * matrix with at least one dimension > 1).
 */
static inline int matd_is_vector(const matd_t *a)
{
    assert(a != NULL);
    return a->ncols == 1 || a->nrows == 1;
}

/**
 * Determines whether the supplied matrix 'a' is a row or column vector
 * with a dimension of 'len' (positive return) or not (zero return).
 */
static inline int matd_is_vector_len(const matd_t *a, int len)
{
    assert(a != NULL);
    return (a->ncols == 1 && a->nrows == len) || (a->ncols == len && a->nrows == 1);
}

/**
 * Calculates the magnitude of the supplied matrix 'a'.
 */
float matd_vec_mag(const matd_t *a);

/**
 * Calculates the magnitude of the distance between the points represented by
 * matrices 'a' and 'b'. Both 'a' and 'b' must be vectors and have the same
 * dimension (although one may be a row vector and one may be a column vector).
 */
float matd_vec_dist(const matd_t *a, const matd_t *b);


/**
 * Same as matd_vec_dist, but only uses the first 'n' terms to compute distance
 */
float matd_vec_dist_n(const matd_t *a, const matd_t *b, int n);

/**
 * Calculates the dot product of two vectors. Both 'a' and 'b' must be vectors
 * and have the same dimension (although one may be a row vector and one may be
 * a column vector).
 */
float matd_vec_dot_product(const matd_t *a, const matd_t *b);

/**
 * Calculates the normalization of the supplied vector 'a' (i.e. a unit vector
 * of the same dimension and orientation as 'a' with a magnitude of 1) and returns
 * it as a new vector. 'a' must be a vector of any dimension and must have a
 * non-zero magnitude. It is the caller's responsibility to call matd_destroy()
 * on the returned matrix.
 */
matd_t *matd_vec_normalize(const matd_t *a);

/**
 * Calculates the cross product of supplied matrices 'a' and 'b' (i.e. a x b)
 * and returns it as a new matrix. Both 'a' and 'b' must be vectors of dimension
 * 3, but can be either row or column vectors. It is the caller's responsibility
 * to call matd_destroy() on the returned matrix.
 */
matd_t *matd_crossproduct(const matd_t *a, const matd_t *b);

float matd_err_inf(const matd_t *a, const matd_t *b);

/**
 * Creates a new matrix by applying a series of matrix operations, as expressed
 * in 'expr', to the supplied list of matrices. Each matrix to be operated upon
 * must be represented in the expression by a separate matrix placeholder, 'M',
 * and there must be one matrix supplied as an argument for each matrix
 * placeholder in the expression. All rules and caveats of the corresponding
 * matrix operations apply to the operated-on matrices. It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 *
 * Available operators (in order of increasing precedence):
 *   M+M   add two matrices together
 *   M-M   subtract one matrix from another
 *   M*M   multiply to matrices together (matrix product)
 *   MM    multiply to matrices together (matrix product)
 *   -M    negate a matrix
 *   M^-1  take the inverse of a matrix
 *   M'    take the transpose of a matrix
 *
 * Expressions can be combined together and grouped by enclosing them in
 * parenthesis, i.e.:
 *   -M(M+M+M)-(M*M)^-1
 *
 * Scalar values can be generated on-the-fly, i.e.:
 *   M*2.2  scales M by 2.2
 *   -2+M   adds -2 to all elements of M
 *
 * All whitespace in the expression is ignored.
 */
matd_t *matd_op(const char *expr, ...);

/**
 * Frees the memory associated with matrix 'm', being the result of an earlier
 * call to a matd_*() function, after which 'm' will no longer be usable.
 */
void matd_destroy(matd_t *m);

typedef struct
{
    matd_t *U;
    matd_t *S;
    matd_t *V;
} matd_svd_t;

/** Compute a complete SVD of a matrix. The SVD exists for all
 * matrices. For a matrix MxN, we will have:
 *
 * A = U*S*V'
 *
 * where A is MxN, U is MxM (and is an orthonormal basis), S is MxN
 * (and is diagonal up to machine precision), and V is NxN (and is an
 * orthonormal basis).
 *
 * The caller is responsible for destroying U, S, and V.
 **/
matd_svd_t matd_svd(matd_t *A);

#define MATD_SVD_NO_WARNINGS 1
    matd_svd_t matd_svd_flags(matd_t *A, int flags);

////////////////////////////////
// PLU Decomposition

// All square matrices (even singular ones) have a partially-pivoted
// LU decomposition such that A = PLU, where P is a permutation
// matrix, L is a lower triangular matrix, and U is an upper
// triangular matrix.
//
typedef struct
{
    // was the input matrix singular? When a zero pivot is found, this
    // flag is set to indicate that this has happened.
    int singular;

    unsigned int *piv; // permutation indices
    int pivsign; // either +1 or -1

    // The matd_plu_t object returned "owns" the enclosed LU matrix. It
    // is not expected that the returned object is itself useful to
    // users: it contains the L and U information all smushed
    // together.
    matd_t *lu; // combined L and U matrices, permuted so they can be triangular.
} matd_plu_t;

matd_plu_t *matd_plu(const matd_t *a);
void matd_plu_destroy(matd_plu_t *mlu);
float matd_plu_det(const matd_plu_t *lu);
matd_t *matd_plu_p(const matd_plu_t *lu);
matd_t *matd_plu_l(const matd_plu_t *lu);
matd_t *matd_plu_u(const matd_plu_t *lu);
matd_t *matd_plu_solve(const matd_plu_t *mlu, const matd_t *b);

// uses LU decomposition internally.
matd_t *matd_solve(matd_t *A, matd_t *b);

////////////////////////////////
// Cholesky Factorization

/**
 * Creates a float matrix with the Cholesky lower triangular matrix
 * of A. A must be symmetric, positive definite. It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
//matd_t *matd_cholesky(const matd_t *A);

typedef struct
{
    int is_spd;
    matd_t *u;
} matd_chol_t;

matd_chol_t *matd_chol(matd_t *A);
matd_t *matd_chol_solve(const matd_chol_t *chol, const matd_t *b);
void matd_chol_destroy(matd_chol_t *chol);
// only sensible on PSD matrices
matd_t *matd_chol_inverse(matd_t *a);

void matd_ltransposetriangle_solve(matd_t *u, const float *b, float *x);
void matd_ltriangle_solve(matd_t *u, const float *b, float *x);
void matd_utriangle_solve(matd_t *u, const float *b, float *x);


float matd_max(matd_t *m);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "matd.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

// a matd_t with rows=0 cols=0 is a SCALAR.

// to ease creating mati, matf, etc. in the future.
#define TYPE float

matd_t *matd_create(int rows, int cols)
{
    assert(rows >= 0);
    assert(cols >= 0);

    if (rows == 0 || cols == 0)
        return matd_create_scalar(0);

    matd_t *m = calloc(1, sizeof(matd_t) + (rows*cols*sizeof(float)));
    m->nrows = rows;
    m->ncols = cols;

    return m;
}

matd_t *matd_create_scalar(TYPE v)
{
    matd_t *m = calloc(1, sizeof(matd_t) + sizeof(float));
    m->nrows = 0;
    m->ncols = 0;
    m->data[0] = v;

    return m;
}

matd_t *matd_create_data(int rows, int cols, const TYPE *data)
{
    if (rows == 0 || cols == 0)
        return matd_create_scalar(data[0]);

    matd_t *m = matd_create(rows, cols);
    for (int i = 0; i < rows * cols; i++)
        m->data[i] = data[i];

    return m;
}

matd_t *matd_create_dataf(int rows, int cols, const float *data)
{
    if (rows == 0 || cols == 0)
        return matd_create_scalar(data[0]);

    matd_t *m = matd_create(rows, cols);
    for (int i = 0; i < rows * cols; i++)
        m->data[i] = (float)data[i];

    return m;
}

matd_t *matd_identity(int dim)
{
    if (dim == 0)
        return matd_create_scalar(1);

    matd_t *m = matd_create(dim, dim);
    for (int i = 0; i < dim; i++)
        MATD_EL(m, i, i) = 1;

    return m;
}

// row and col are zero-based
TYPE matd_get(const matd_t *m, int row, int col)
{
    assert(m != NULL);
    assert(!matd_is_scalar(m));
    assert(row >= 0);
    assert(row < m->nrows);
    assert(col >= 0);
    assert(col < m->ncols);

    return MATD_EL(m, row, col);
}

// row and col are zero-based
void matd_put(matd_t *m, int row, int col, TYPE value)
{
    assert(m != NULL);

    if (matd_is_scalar(m)) {
        matd_put_scalar(m, value);
        return;
    }

    assert(row >= 0);
    assert(row < m->nrows);
    assert(col >= 0);
    assert(col < m->ncols);

    MATD_EL(m, row, col) = value;
}

TYPE matd_get_scalar(const matd_t *m)
{
    assert(m != NULL);
    assert(matd_is_scalar(m));

    return (m->data[0]);
}

void matd_put_scalar(matd_t *m, TYPE value)
{
    assert(m != NULL);
    assert(matd_is_scalar(m));

    m->data[0] = value;
}

matd_t *matd_copy(const matd_t *m)
{
    assert(m != NULL);

    matd_t *x = matd_create(m->nrows, m->ncols);
    if (matd_is_scalar(m))
        x->data[0] = m->data[0];
    else
        memcpy(x->data, m->data, sizeof(TYPE)*m->ncols*m->nrows);

    return x;
}

matd_t *matd_select(const matd_t * a, int r0, int r1, int c0, int c1)
{
    assert(a != NULL);

    assert(r0 >= 0 && r0 < a->nrows);
    assert(c0 >= 0 && c0 < a->ncols);

    int nrows = r1 - r0 + 1;
    int ncols = c1 - c0 + 1;

    matd_t * r = matd_create(nrows, ncols);

    for (int row = r0; row <= r1; row++)
        for (int col = c0; col <= c1; col++)
            MATD_EL(r,row-r0,col-c0) = MATD_EL(a,row,col);

    return r;
}

void matd_print(const matd_t *m, const char *fmt)
{
    assert(m != NULL);
    assert(fmt != NULL);

    if (matd_is_scalar(m)) {
        printf(fmt, (double) MATD_EL(m, 0, 0));
        printf("\n");
    } else {
        for (int i = 0; i < m->nrows; i++) {
            for (int j = 0; j < m->ncols; j++) {
                printf(fmt, (double) MATD_EL(m, i, j));
            }
            printf("\n");
        }
    }
}

void matd_print_transpose(const matd_t *m, const char *fmt)
{
    assert(m != NULL);
    assert(fmt != NULL);

    if (matd_is_scalar(m)) {
        printf(fmt, (double) MATD_EL(m, 0, 0));
        printf("\n");
    } else {
        for (int j = 0; j < m->ncols; j++) {
            for (int i = 0; i < m->nrows; i++) {
                printf(fmt, (double) MATD_EL(m, i, j));
            }
            printf("\n");
        }
    }
}

void matd_destroy(matd_t *m)
{
    if (!m)
        return;

    assert(m != NULL);
    free(m);
}

matd_t *matd_multiply(const matd_t *a, const matd_t *b)
{
    assert(a != NULL);
    assert(b != NULL);

    if (matd_is_scalar(a))
        return matd_scale(b, a->data[0]);
    if (matd_is_scalar(b))
        return matd_scale(a, b->data[0]);

    assert(a->ncols == b->nrows);
    matd_t *m = matd_create(a->nrows, b->ncols);

    for (int i = 0; i < m->nrows; i++) {
        for (int j = 0; j < m->ncols; j++) {
            TYPE acc = 0;
            for (int k = 0; k < a->ncols; k++) {
                acc += MATD_EL(a, i, k) * MATD_EL(b, k, j);
            }
            MATD_EL(m, i, j) = acc;
        }
    }

    return m;
}

matd_t *matd_scale(const matd_t *a, float s)
{
    assert(a != NULL);

    if (matd_is_scalar(a))
        return matd_create_scalar(a->data[0] * s);

    matd_t *m = matd_create(a->nrows, a->ncols);

    for (int i = 0; i < m->nrows; i++) {
        for (int j = 0; j < m->ncols; j++) {
            MATD_EL(m, i, j) = s * MATD_EL(a, i, j);
        }
    }

    return m;
}

void matd_scale_inplace(matd_t *a, float s)
{
    assert(a != NULL);

    if (matd_is_scalar(a)) {
        a->data[0] *= s;
        return;
    }

    for (int i = 0; i < a->nrows; i++) {
        for (int j = 0; j < a->ncols; j++) {
            MATD_EL(a, i, j) *= s;
        }
    }
}

matd_t *matd_add(const matd_t *a, const matd_t *b)
{
    assert(a != NULL);
    assert(b != NULL);
    assert(a->nrows == b->nrows);
    assert(a->ncols == b->ncols);

    if (matd_is_scalar(a))
        return matd_create_scalar(a->data[0] + b->data[0]);

    matd_t *m = matd_create(a->nrows, a->ncols);

    for (int i = 0; i < m->nrows; i++) {
        for (int j = 0; j < m->ncols; j++) {
            MATD_EL(m, i, j) = MATD_EL(a, i, j) + MATD_EL(b, i, j);
        }
    }

    return m;
}

void matd_add_inplace(matd_t *a, const matd_t *b)
{
    assert(a != NULL);
    assert(b != NULL);
    assert(a->nrows == b->nrows);
    assert(a->ncols == b->ncols);

    if (matd_is_scalar(a)) {
        a->data[0] += b->data[0];
        return;
    }

    for (int i = 0; i < a->nrows; i++) {
        for (int j = 0; j < a->ncols; j++) {
            MATD_EL(a, i, j) += MATD_EL(b, i, j);
        }
    }
}


matd_t *matd_subtract(const matd_t *a, const matd_t *b)
{
    assert(a != NULL);
    assert(b != NULL);
    assert(a->nrows == b->nrows);
    assert(a->ncols == b->ncols);

    if (matd_is_scalar(a))
        return matd_create_scalar(a->data[0] - b->data[0]);

    matd_t *m = matd_create(a->nrows, a->ncols);

    for (int i = 0; i < m->nrows; i++) {
        for (int j = 0; j < m->ncols; j++) {
            MATD_EL(m, i, j) = MATD_EL(a, i, j) - MATD_EL(b, i, j);
        }
    }

    return m;
}

void matd_subtract_inplace(matd_t *a, const matd_t *b)
{
    assert(a != NULL);
    assert(b != NULL);
    assert(a->nrows == b->nrows);
    assert(a->ncols == b->ncols);

    if (matd_is_scalar(a)) {
        a->data[0] -= b->data[0];
        return;
    }

    for (int i = 0; i < a->nrows; i++) {
        for (int j = 0; j < a->ncols; j++) {
            MATD_EL(a, i, j) -= MATD_EL(b, i, j);
        }
    }
}


matd_t *matd_transpose(const matd_t *a)
{
    assert(a != NULL);

    if (matd_is_scalar(a))
        return matd_create_scalar(a->data[0]);

    matd_t *m = matd_create(a->ncols, a->nrows);

    for (int i = 0; i < a->nrows; i++) {
        for (int j = 0; j < a->ncols; j++) {
            MATD_EL(m, j, i) = MATD_EL(a, i, j);
        }
    }
    return m;
}

static
float matd_det_general(const matd_t *a)
{
    // Use LU decompositon to calculate the determinant
    matd_plu_t *mlu = matd_plu(a);
    matd_t *L = matd_plu_l(mlu);
    matd_t *U = matd_plu_u(mlu);

    // The determinants of the L and U matrices are the products of
    // their respective diagonal elements
    float detL = 1; float detU = 1;
    for (int i = 0; i < a->nrows; i++) {
        detL *= matd_get(L, i, i);
        detU *= matd_get(U, i, i);
    }

    // The determinant of a can be calculated as
    //     epsilon*det(L)*det(U),
    // where epsilon is just the sign of the corresponding permutation
    // (which is +1 for an even number of permutations and is −1
    // for an uneven number of permutations).
    float det = mlu->pivsign * detL * detU;

    // Cleanup
    matd_plu_destroy(mlu);
    matd_destroy(L);
    matd_destroy(U);

    return det;
}

float matd_det(const matd_t *a)
{
    assert(a != NULL);
    assert(a->nrows == a->ncols);

    switch(a->nrows) {
        case 0:
            // scalar: invalid
            assert(a->nrows > 0);
            break;

        case 1:
            // 1x1 matrix
            return a->data[0];

        case 2:
            // 2x2 matrix
            return a->data[0] * a->data[3] - a->data[1] * a->data[2];

        case 3:
            // 3x3 matrix
            return  a->data[0]*a->data[4]*a->data[8]
                - a->data[0]*a->data[5]*a->data[7]
                + a->data[1]*a->data[5]*a->data[6]
                - a->data[1]*a->data[3]*a->data[8]
                + a->data[2]*a->data[3]*a->data[7]
                - a->data[2]*a->data[4]*a->data[6];

        case 4: {
            // 4x4 matrix
            float m00 = MATD_EL(a,0,0), m01 = MATD_EL(a,0,1), m02 = MATD_EL(a,0,2), m03 = MATD_EL(a,0,3);
            float m10 = MATD_EL(a,1,0), m11 = MATD_EL(a,1,1), m12 = MATD_EL(a,1,2), m13 = MATD_EL(a,1,3);
            float m20 = MATD_EL(a,2,0), m21 = MATD_EL(a,2,1), m22 = MATD_EL(a,2,2), m23 = MATD_EL(a,2,3);
            float m30 = MATD_EL(a,3,0), m31 = MATD_EL(a,3,1), m32 = MATD_EL(a,3,2), m33 = MATD_EL(a,3,3);

            return m00 * m11 * m22 * m33 - m00 * m11 * m23 * m32 -
                m00 * m21 * m12 * m33 + m00 * m21 * m13 * m32 + m00 * m31 * m12 * m23 -
                m00 * m31 * m13 * m22 - m10 * m01 * m22 * m33 +
                m10 * m01 * m23 * m32 + m10 * m21 * m02 * m33 -
                m10 * m21 * m03 * m32 - m10 * m31 * m02 * m23 +
                m10 * m31 * m03 * m22 + m20 * m01 * m12 * m33 -
                m20 * m01 * m13 * m32 - m20 * m11 * m02 * m33 +
                m20 * m11 * m03 * m32 + m20 * m31 * m02 * m13 -
                m20 * m31 * m03 * m12 - m30 * m01 * m12 * m23 +
                m30 * m01 * m13 * m22 + m30 * m11 * m02 * m23 -
                m30 * m11 * m03 * m22 - m30 * m21 * m02 * m13 +
                m30 * m21 * m03 * m12;
        }

        default:
            return matd_det_general(a);
    }

    assert(0);
    return 0;
}

// returns NULL if the matrix is (exactly) singular. Caller is
// otherwise responsible for knowing how to cope with badly
// conditioned matrices.
matd_t *matd_inverse(const matd_t *x)
{
    matd_t *m = NULL;

    assert(x != NULL);
    assert(x->nrows == x->ncols);

    if (matd_is_scalar(x)) {
        if (x->data[0] == 0)
            return NULL;

        return matd_create_scalar(1.0 / x->data[0]);
    }

    switch(x->nrows) {
        case 1: {
            float det = x->data[0];
            if (det == 0)
                return NULL;

            float invdet = 1.0 / det;

            m = matd_create(x->nrows, x->nrows);
            MATD_EL(m, 0, 0) = 1.0 * invdet;
            return m;
        }

        case 2: {
            float det = x->data[0] * x->data[3] - x->data[1] * x->data[2];
            if (det == 0)
                return NULL;

            float invdet = 1.0 / det;

            m = matd_create(x->nrows, x->nrows);
            MATD_EL(m, 0, 0) = MATD_EL(x, 1, 1) * invdet;
            MATD_EL(m, 0, 1) = - MATD_EL(x, 0, 1) * invdet;
            MATD_EL(m, 1, 0) = - MATD_EL(x, 1, 0) * invdet;
            MATD_EL(m, 1, 1) = MATD_EL(x, 0, 0) * invdet;
            return m;
        }

        default: {
            matd_plu_t *plu = matd_plu(x);

            matd_t *inv = NULL;
            if (!plu->singular) {
                matd_t *ident = matd_identity(x->nrows);
                inv = matd_plu_solve(plu, ident);
                matd_destroy(ident);
            }

            matd_plu_destroy(plu);

            return inv;
        }
    }

    return NULL; // unreachable
}



// TODO Optimization: Some operations we could perform in-place,
// saving some memory allocation work. E.g., ADD, SUBTRACT. Just need
// to make sure that we don't do an in-place modification on a matrix
// that was an input argument!

// handle right-associative operators, greedily consuming them. These
// include transpose and inverse. This is called by the main recursion
// method.
static inline matd_t *matd_op_gobble_right(const char *expr, int *pos, matd_t *acc, matd_t **garb, int *garbpos)
{
    while (expr[*pos] != 0) {

        switch (expr[*pos]) {

            case '\'': {
                assert(acc != NULL); // either a syntax error or a math op failed, producing null
                matd_t *res = matd_transpose(acc);
                garb[*garbpos] = res;
                (*garbpos)++;
                acc = res;

                (*pos)++;
                break;
            }

                // handle inverse ^-1. No other exponents are allowed.
            case '^': {
                assert(acc != NULL);
                assert(expr[*pos+1] == '-');
                assert(expr[*pos+2] == '1');

                matd_t *res = matd_inverse(acc);
                garb[*garbpos] = res;
                (*garbpos)++;
                acc = res;

                (*pos)+=3;
                break;
            }

            default:
                return acc;
        }
    }

    return acc;
}

// @garb, garbpos  A list of every matrix allocated during evaluation... used to assist cleanup.
// @oneterm: we should return at the end of this term (i.e., stop at a PLUS, MINUS, LPAREN).
static matd_t *matd_op_recurse(const char *expr, int *pos, matd_t *acc, matd_t **args, int *argpos,
                               matd_t **garb, int *garbpos, int oneterm)
{
    while (expr[*pos] != 0) {

        switch (expr[*pos]) {

            case '(': {
                if (oneterm && acc != NULL)
                    return acc;
                (*pos)++;
                matd_t *rhs = matd_op_recurse(expr, pos, NULL, args, argpos, garb, garbpos, 0);
                rhs = matd_op_gobble_right(expr, pos, rhs, garb, garbpos);

                if (acc == NULL) {
                    acc = rhs;
                } else {
                    matd_t *res = matd_multiply(acc, rhs);
                    garb[*garbpos] = res;
                    (*garbpos)++;
                    acc = res;
                }

                break;
            }

            case ')': {
                if (oneterm)
                    return acc;

                (*pos)++;
                return acc;
            }

            case '*': {
                (*pos)++;

                matd_t *rhs = matd_op_recurse(expr, pos, NULL, args, argpos, garb, garbpos, 1);
                rhs = matd_op_gobble_right(expr, pos, rhs, garb, garbpos);

                if (acc == NULL) {
                    acc = rhs;
                } else {
                    matd_t *res = matd_multiply(acc, rhs);
                    garb[*garbpos] = res;
                    (*garbpos)++;
                    acc = res;
                }

                break;
            }

            case 'F': {
                matd_t *rhs = args[*argpos];
                garb[*garbpos] = rhs;
                (*garbpos)++;

                (*pos)++;
                (*argpos)++;

                rhs = matd_op_gobble_right(expr, pos, rhs, garb, garbpos);

                if (acc == NULL) {
                    acc = rhs;
                } else {
                    matd_t *res = matd_multiply(acc, rhs);
                    garb[*garbpos] = res;
                    (*garbpos)++;
                    acc = res;
                }

                break;
            }

            case 'M': {
                matd_t *rhs = args[*argpos];

                (*pos)++;
                (*argpos)++;

                rhs = matd_op_gobble_right(expr, pos, rhs, garb, garbpos);

                if (acc == NULL) {
                    acc = rhs;
                } else {
                    matd_t *res = matd_multiply(acc, rhs);
                    garb[*garbpos] = res;
                    (*garbpos)++;
                    acc = res;
                }

                break;
            }

/*
  case 'D': {
  int rows = expr[*pos+1]-'0';
  int cols = expr[*pos+2]-'0';

  matd_t *rhs = matd_create(rows, cols);

  break;
  }
*/
                // a constant (SCALAR) defined inline. Treat just like M, creating a matd_t on the fly.
//            case '0':
//            case '1':
//            case '2':
//            case '3':
//            case '4':
//            case '5':
//            case '6':
//            case '7':
//            case '8':
//            case '9':
//            case '.': {
//                const char *start = &expr[*pos];
//                char *end;
//                float s = strtod(start, &end);
//                (*pos) += (end - start);
//                matd_t *rhs = matd_create_scalar(s);
//                garb[*garbpos] = rhs;
//                (*garbpos)++;

//                rhs = matd_op_gobble_right(expr, pos, rhs, garb, garbpos);

//                if (acc == NULL) {
//                    acc = rhs;
//                } else {
//                    matd_t *res = matd_multiply(acc, rhs);
//                    garb[*garbpos] = res;
//                    (*garbpos)++;
//                    acc = res;
//                }

//                break;
//            }

            case '+': {
                if (oneterm && acc != NULL)
                    return acc;

                // don't support unary plus
                assert(acc != NULL);
                (*pos)++;
                matd_t *rhs = matd_op_recurse(expr, pos, NULL, args, argpos, garb, garbpos, 1);
                rhs = matd_op_gobble_right(expr, pos, rhs, garb, garbpos);

                matd_t *res = matd_add(acc, rhs);

                garb[*garbpos] = res;
                (*garbpos)++;
                acc = res;
                break;
            }

            case '-': {
                if (oneterm && acc != NULL)
                    return acc;

                if (acc == NULL) {
                    // unary minus
                    (*pos)++;
                    matd_t *rhs = matd_op_recurse(expr, pos, NULL, args, argpos, garb, garbpos, 1);
                    rhs = matd_op_gobble_right(expr, pos, rhs, garb, garbpos);

                    matd_t *res = matd_scale(rhs, -1);
                    garb[*garbpos] = res;
                    (*garbpos)++;
                    acc = res;
                } else {
                    // subtract
                    (*pos)++;
                    matd_t *rhs = matd_op_recurse(expr, pos, NULL, args, argpos, garb, garbpos, 1);
                    rhs = matd_op_gobble_right(expr, pos, rhs, garb, garbpos);

                    matd_t *res = matd_subtract(acc, rhs);
                    garb[*garbpos] = res;
                    (*garbpos)++;
                    acc = res;
                }
                break;
            }

            case ' ': {
                // nothing to do. spaces are meaningless.
                (*pos)++;
                break;
            }

            default: {
                fprintf(stderr, "matd_op(): Unknown character: '%c'\n", expr[*pos]);
                assert(expr[*pos] != expr[*pos]);
            }
        }
    }
    return acc;
}

// always returns a new matrix.
matd_t *matd_op(const char *expr, ...)
{
    int nargs = 0;
    int exprlen = 0;

    assert(expr != NULL);

    for (const char *p = expr; *p != 0; p++) {
        if (*p == 'M' || *p == 'F')
            nargs++;
        exprlen++;
    }

    assert(nargs > 0);

    if (!exprlen) // expr = ""
        return NULL;

    va_list ap;
    va_start(ap, expr);

    matd_t *args[nargs];
    for (int i = 0; i < nargs; i++) {
        args[i] = va_arg(ap, matd_t*);
        // XXX: sanity check argument; emit warning/error if args[i]
        // doesn't look like a matd_t*.
    }

    va_end(ap);

    int pos = 0;
    int argpos = 0;
    int garbpos = 0;

    matd_t *garb[2*exprlen]; // can't create more than 2 new result per character
                             // one result, and possibly one argument to free

    matd_t *res = matd_op_recurse(expr, &pos, NULL, args, &argpos, garb, &garbpos, 0);

    // 'res' may need to be freed as part of garbage collection (i.e. expr = "F")
    matd_t *res_copy = (res ? matd_copy(res) : NULL);

    for (int i = 0; i < garbpos; i++) {
        matd_destroy(garb[i]);
    }

    return res_copy;
}

float matd_vec_mag(const matd_t *a)
{
    assert(a != NULL);
    assert(matd_is_vector(a));

    float mag = 0.0;
    int len = a->nrows*a->ncols;
    for (int i = 0; i < len; i++)
        mag += sq(a->data[i]);
    return sqrt(mag);
}

float matd_vec_dist(const matd_t *a, const matd_t *b)
{
    assert(a != NULL);
    assert(b != NULL);
    assert(matd_is_vector(a) && matd_is_vector(b));
    assert(a->nrows*a->ncols == b->nrows*b->ncols);

    int lena = a->nrows*a->ncols;
    return matd_vec_dist_n(a, b, lena);
}

float matd_vec_dist_n(const matd_t *a, const matd_t *b, int n)
{
    assert(a != NULL);
    assert(b != NULL);
    assert(matd_is_vector(a) && matd_is_vector(b));

    int lena = a->nrows*a->ncols;
    int lenb = b->nrows*b->ncols;

    assert(n <= lena && n <= lenb);

    float mag = 0.0;
    for (int i = 0; i < n; i++)
        mag += sq(a->data[i] - b->data[i]);
    return sqrt(mag);
}

// find the index of the off-diagonal element with the largest mag
static inline int max_idx(const matd_t *A, int row, int maxcol)
{
    int maxi = 0;
    float maxv = -1;

    for (int i = 0; i < maxcol; i++) {
        if (i == row)
            continue;
        float v = fabs(MATD_EL(A, row, i));
        if (v > maxv) {
            maxi = i;
            maxv = v;
        }
    }

    return maxi;
}

float matd_vec_dot_product(const matd_t *a, const matd_t *b)
{
    assert(a != NULL);
    assert(b != NULL);
    assert(matd_is_vector(a) && matd_is_vector(b));
    int adim = a->ncols*a->nrows;
    int bdim = b->ncols*b->nrows;
    assert(adim == bdim);

    float acc = 0;
    for (int i = 0; i < adim; i++) {
        acc += a->data[i] * b->data[i];
    }
    return acc;
}


matd_t *matd_vec_normalize(const matd_t *a)
{
    assert(a != NULL);
    assert(matd_is_vector(a));

    float mag = matd_vec_mag(a);
    assert(mag > 0);

    matd_t *b = matd_create(a->nrows, a->ncols);

    int len = a->nrows*a->ncols;
    for(int i = 0; i < len; i++)
        b->data[i] = a->data[i] / mag;

    return b;
}

matd_t *matd_crossproduct(const matd_t *a, const matd_t *b)
{ // only defined for vecs (col or row) of length 3
    assert(a != NULL);
    assert(b != NULL);
    assert(matd_is_vector_len(a, 3) && matd_is_vector_len(b, 3));

    matd_t * r = matd_create(a->nrows, a->ncols);

    r->data[0] = a->data[1] * b->data[2] - a->data[2] * b->data[1];
    r->data[1] = a->data[2] * b->data[0] - a->data[0] * b->data[2];
    r->data[2] = a->data[0] * b->data[1] - a->data[1] * b->data[0];

    return r;
}

TYPE matd_err_inf(const matd_t *a, const matd_t *b)
{
    assert(a->nrows == b->nrows);
    assert(a->ncols == b->ncols);

    TYPE maxf = 0;

    for (int i = 0; i < a->nrows; i++) {
        for (int j = 0; j < a->ncols; j++) {
            TYPE av = MATD_EL(a, i, j);
            TYPE bv = MATD_EL(b, i, j);

            TYPE err = fabs(av - bv);
            maxf = fmax(maxf, err);
        }
    }

    return maxf;
}

// Computes an SVD for square or tall matrices. This code doesn't work
// for wide matrices, because the bidiagonalization results in one
// non-zero element too far to the right for us to rotate away.
//
// Caller is responsible for destroying U, S, and V.
static matd_svd_t matd_svd_tall(matd_t *A, int flags)
{
    matd_t *B = matd_copy(A);

    // Apply householder reflections on each side to reduce A to
    // bidiagonal form. Specifically:
    //
    // A = LS*B*RS'
    //
    // Where B is bidiagonal, and LS/RS are unitary.
    //
    // Why are we doing this? Some sort of transformation is necessary
    // to reduce the matrix's nz elements to a square region. QR could
    // work too. We need nzs confined to a square region so that the
    // subsequent iterative process, which is based on rotations, can
    // work. (To zero out a term at (i,j), our rotations will also
    // affect (j,i).
    //
    // We prefer bidiagonalization over QR because it gets us "closer"
    // to the SVD, which should mean fewer iterations.

    // LS: cumulative left-handed transformations
    matd_t *LS = matd_identity(A->nrows);

    // RS: cumulative right-handed transformations.
    matd_t *RS = matd_identity(A->ncols);

    for (int hhidx = 0; hhidx < A->nrows; hhidx++)  {

        if (hhidx < A->ncols) {
            // We construct the normal of the reflection plane: let u
            // be the vector to reflect, x =[ M 0 0 0 ] the target
            // location for u (u') after reflection (with M = ||u||).
            //
            // The normal vector is then n = (u - x), but since we
            // could equally have the target location be x = [-M 0 0 0
            // ], we could use n = (u + x).
            //
            // We then normalize n. To ensure a reasonable magnitude,
            // we select the sign of M so as to maximize the magnitude
            // of the first element of (x +/- M). (Otherwise, we could
            // end up with a divide-by-zero if u[0] and M cancel.)
            //
            // The householder reflection matrix is then H=(I - nn'), and
            // u' = Hu.
            //
            //
            int vlen = A->nrows - hhidx;

            float v[vlen];

            float mag2 = 0;
            for (int i = 0; i < vlen; i++) {
                v[i] = MATD_EL(B, hhidx+i, hhidx);
                mag2 += v[i]*v[i];
            }

            float oldv0 = v[0];
            if (oldv0 < 0)
                v[0] -= sqrt(mag2);
            else
                v[0] += sqrt(mag2);

            mag2 += -oldv0*oldv0 + v[0]*v[0];

            // normalize v
            float mag = sqrt(mag2);

            // this case arises with matrices of all zeros, for example.
            if (mag == 0)
                continue;

            for (int i = 0; i < vlen; i++)
                v[i] /= mag;

            // Q = I - 2vv'
            //matd_t *Q = matd_identity(A->nrows);
            //for (int i = 0; i < vlen; i++)
            //  for (int j = 0; j < vlen; j++)
            //    MATD_EL(Q, i+hhidx, j+hhidx) -= 2*v[i]*v[j];


            // LS = matd_op("F*M", LS, Q);
            // Implementation: take each row of LS, compute dot product with n,
            // subtract n (scaled by dot product) from it.
            for (int i = 0; i < LS->nrows; i++) {
                float dot = 0;
                for (int j = 0; j < vlen; j++)
                    dot += MATD_EL(LS, i, hhidx+j) * v[j];
                for (int j = 0; j < vlen; j++)
                    MATD_EL(LS, i, hhidx+j) -= 2*dot*v[j];
            }

            //  B = matd_op("M*F", Q, B); // should be Q', but Q is symmetric.
            for (int i = 0; i < B->ncols; i++) {
                float dot = 0;
                for (int j = 0; j < vlen; j++)
                    dot += MATD_EL(B, hhidx+j, i) * v[j];
                for (int j = 0; j < vlen; j++)
                    MATD_EL(B, hhidx+j, i) -= 2*dot*v[j];
            }
        }

        if (hhidx+2 < A->ncols) {
            int vlen = A->ncols - hhidx - 1;

            float v[vlen];

            float mag2 = 0;
            for (int i = 0; i < vlen; i++) {
                v[i] = MATD_EL(B, hhidx, hhidx+i+1);
                mag2 += v[i]*v[i];
            }

            float oldv0 = v[0];
            if (oldv0 < 0)
                v[0] -= sqrt(mag2);
            else
                v[0] += sqrt(mag2);

            mag2 += -oldv0*oldv0 + v[0]*v[0];

            // compute magnitude of ([1 0 0..]+v)
            float mag = sqrt(mag2);

            // this case can occur when the vectors are already perpendicular
            if (mag == 0)
                continue;

            for (int i = 0; i < vlen; i++)
                v[i] /= mag;

            // TODO: optimize these multiplications
            // matd_t *Q = matd_identity(A->ncols);
            //  for (int i = 0; i < vlen; i++)
            //    for (int j = 0; j < vlen; j++)
            //       MATD_EL(Q, i+1+hhidx, j+1+hhidx) -= 2*v[i]*v[j];

            //  RS = matd_op("F*M", RS, Q);
            for (int i = 0; i < RS->nrows; i++) {
                float dot = 0;
                for (int j = 0; j < vlen; j++)
                    dot += MATD_EL(RS, i, hhidx+1+j) * v[j];
                for (int j = 0; j < vlen; j++)
                    MATD_EL(RS, i, hhidx+1+j) -= 2*dot*v[j];
            }

            //   B = matd_op("F*M", B, Q); // should be Q', but Q is symmetric.
            for (int i = 0; i < B->nrows; i++) {
                float dot = 0;
                for (int j = 0; j < vlen; j++)
                    dot += MATD_EL(B, i, hhidx+1+j) * v[j];
                for (int j = 0; j < vlen; j++)
                    MATD_EL(B, i, hhidx+1+j) -= 2*dot*v[j];
            }
        }
    }

    // maxiters used to be smaller to prevent us from looping forever,
    // but this doesn't seem to happen any more with our more stable
    // svd22 implementation.
    int maxiters = 1UL << 5; // 1UL << 30;
    assert(maxiters > 0); // reassure clang
    int iter;

    float maxv; // maximum non-zero value being reduced this iteration

    float tol = 1E-5; // 1E-10;

    // which method will we use to find the largest off-diagonal
    // element of B?
    const int find_max_method = 1; //(B->ncols < 6) ? 2 : 1;

    // for each of the first B->ncols rows, which index has the
    // maximum absolute value? (used by method 1)
    int maxrowidx[B->ncols];
    int lastmaxi, lastmaxj;

    if (find_max_method == 1) {
        for (int i = 2; i < B->ncols; i++)
            maxrowidx[i] = max_idx(B, i, B->ncols);

        // note that we started the array at 2. That's because by setting
        // these values below, we'll recompute first two entries on the
        // first iteration!
        lastmaxi = 0, lastmaxj = 1;
    }

    for (iter = 0; iter < maxiters; iter++) {

        // No diagonalization required for 0x0 and 1x1 matrices.
        if (B->ncols < 2)
            break;

        // find the largest off-diagonal element of B, and put its
        // coordinates in maxi, maxj.
        int maxi, maxj;

        if (find_max_method == 1) {
            // method 1 is the "smarter" method which does at least
            // 4*ncols work. More work might be needed (up to
            // ncols*ncols), depending on data. Thus, this might be a
            // bit slower than the default method for very small
            // matrices.
            maxi = -1;
            maxv = -1;

            // every iteration, we must deal with the fact that rows
            // and columns lastmaxi and lastmaxj have been
            // modified. Update maxrowidx accordingly.

            // now, EVERY row also had columns lastmaxi and lastmaxj modified.
            for (int rowi = 0; rowi < B->ncols; rowi++) {

                // the magnitude of the largest off-diagonal element
                // in this row.
                float thismaxv;

                // row 'lastmaxi' and 'lastmaxj' have been completely
                // changed. compute from scratch.
                if (rowi == lastmaxi || rowi == lastmaxj) {
                    maxrowidx[rowi] = max_idx(B, rowi, B->ncols);
                    thismaxv = fabs(MATD_EL(B, rowi, maxrowidx[rowi]));
                    goto endrowi;
                }

                // our maximum entry was just modified. We don't know
                // if it went up or down, and so we don't know if it
                // is still the maximum. We have to update from
                // scratch.
                if (maxrowidx[rowi] == lastmaxi || maxrowidx[rowi] == lastmaxj) {
                    maxrowidx[rowi] = max_idx(B, rowi, B->ncols);
                    thismaxv = fabs(MATD_EL(B, rowi, maxrowidx[rowi]));
                    goto endrowi;
                }

                // This row is unchanged, except for columns
                // 'lastmaxi' and 'lastmaxj', and those columns were
                // not previously the largest entry...  just check to
                // see if they are now the maximum entry in their
                // row. (Remembering to consider off-diagonal entries
                // only!)
                thismaxv = fabs(MATD_EL(B, rowi, maxrowidx[rowi]));

                // check column lastmaxi. Is it now the maximum?
                if (lastmaxi != rowi) {
                    float v = fabs(MATD_EL(B, rowi, lastmaxi));
                    if (v > thismaxv) {
                        thismaxv = v;
                        maxrowidx[rowi] = lastmaxi;
                    }
                }

                // check column lastmaxj
                if (lastmaxj != rowi) {
                    float v = fabs(MATD_EL(B, rowi, lastmaxj));
                    if (v > thismaxv) {
                        thismaxv = v;
                        maxrowidx[rowi] = lastmaxj;
                    }
                }

                // does this row have the largest value we've seen so far?
              endrowi:
                if (thismaxv > maxv) {
                    maxv = thismaxv;
                    maxi = rowi;
                }
            }

            assert(maxi >= 0);
            maxj = maxrowidx[maxi];

            // save these for the next iteration.
            lastmaxi = maxi;
            lastmaxj = maxj;

            if (maxv < tol)
                break;

        } else if (find_max_method == 2) {
            // brute-force (reference) version.
            maxv = -1;

            // only search top "square" portion
            for (int i = 0; i < B->ncols; i++) {
                for (int j = 0; j < B->ncols; j++) {
                    if (i == j)
                        continue;

                    float v = fabs(MATD_EL(B, i, j));

                    if (v > maxv) {
                        maxi = i;
                        maxj = j;
                        maxv = v;
                    }
                }
            }

            // termination condition.
            if (maxv < tol)
                break;
        } else {
            assert(0);
        }

//        printf(">>> %5d %3d, %3d %15g\n", maxi, maxj, iter, maxv);

        // Now, solve the 2x2 SVD problem for the matrix
        // [ A0 A1 ]
        // [ A2 A3 ]
        float A0 = MATD_EL(B, maxi, maxi);
        float A1 = MATD_EL(B, maxi, maxj);
        float A2 = MATD_EL(B, maxj, maxi);
        float A3 = MATD_EL(B, maxj, maxj);

        if (1) {
            float AQ[4];
            AQ[0] = A0;
            AQ[1] = A1;
            AQ[2] = A2;
            AQ[3] = A3;

            float U[4], S[2], V[4];
            svd22(AQ, U, S, V);

/*  Reference (slow) implementation...

            // LS = LS * ROT(theta) = LS * QL
            matd_t *QL = matd_identity(A->nrows);
            MATD_EL(QL, maxi, maxi) = U[0];
            MATD_EL(QL, maxi, maxj) = U[1];
            MATD_EL(QL, maxj, maxi) = U[2];
            MATD_EL(QL, maxj, maxj) = U[3];

            matd_t *QR = matd_identity(A->ncols);
            MATD_EL(QR, maxi, maxi) = V[0];
            MATD_EL(QR, maxi, maxj) = V[1];
            MATD_EL(QR, maxj, maxi) = V[2];
            MATD_EL(QR, maxj, maxj) = V[3];

            LS = matd_op("F*M", LS, QL);
            RS = matd_op("F*M", RS, QR); // remember we'll transpose RS.
            B = matd_op("M'*F*M", QL, B, QR);

            matd_destroy(QL);
            matd_destroy(QR);
*/

            //  LS = matd_op("F*M", LS, QL);
            for (int i = 0; i < LS->nrows; i++) {
                float vi = MATD_EL(LS, i, maxi);
                float vj = MATD_EL(LS, i, maxj);

                MATD_EL(LS, i, maxi) = U[0]*vi + U[2]*vj;
                MATD_EL(LS, i, maxj) = U[1]*vi + U[3]*vj;
            }

            //  RS = matd_op("F*M", RS, QR); // remember we'll transpose RS.
            for (int i = 0; i < RS->nrows; i++) {
                float vi = MATD_EL(RS, i, maxi);
                float vj = MATD_EL(RS, i, maxj);

                MATD_EL(RS, i, maxi) = V[0]*vi + V[2]*vj;
                MATD_EL(RS, i, maxj) = V[1]*vi + V[3]*vj;
            }

            // B = matd_op("M'*F*M", QL, B, QR);
            // The QL matrix mixes rows of B.
            for (int i = 0; i < B->ncols; i++) {
                float vi = MATD_EL(B, maxi, i);
                float vj = MATD_EL(B, maxj, i);

                MATD_EL(B, maxi, i) = U[0]*vi + U[2]*vj;
                MATD_EL(B, maxj, i) = U[1]*vi + U[3]*vj;
            }

            // The QR matrix mixes columns of B.
            for (int i = 0; i < B->nrows; i++) {
                float vi = MATD_EL(B, i, maxi);
                float vj = MATD_EL(B, i, maxj);

                MATD_EL(B, i, maxi) = V[0]*vi + V[2]*vj;
                MATD_EL(B, i, maxj) = V[1]*vi + V[3]*vj;
            }
        }
    }

    if (!(flags & MATD_SVD_NO_WARNINGS) && iter == maxiters) {
        printf("WARNING: maximum iters (maximum = %d, matrix %d x %d, max=%.15f)\n",
               iter, A->nrows, A->ncols, (double) maxv);

//        matd_print(A, "%15f");
    }

    // them all positive by flipping the corresponding columns of
    // U/LS.
    int idxs[A->ncols];
    float vals[A->ncols];
    for (int i = 0; i < A->ncols; i++) {
        idxs[i] = i;
        vals[i] = MATD_EL(B, i, i);
    }

    // A bubble sort. Seriously.
    int changed;
    do {
        changed = 0;

        for (int i = 0; i + 1 < A->ncols; i++) {
            if (fabs(vals[i+1]) > fabs(vals[i])) {
                int tmpi = idxs[i];
                idxs[i] = idxs[i+1];
                idxs[i+1] = tmpi;

                float tmpv = vals[i];
                vals[i] = vals[i+1];
                vals[i+1] = tmpv;

                changed = 1;
            }
        }
    } while (changed);

    matd_t *LP = matd_identity(A->nrows);
    matd_t *RP = matd_identity(A->ncols);

    for (int i = 0; i < A->ncols; i++) {
        MATD_EL(LP, idxs[i], idxs[i]) = 0; // undo the identity above
        MATD_EL(RP, idxs[i], idxs[i]) = 0;

        MATD_EL(LP, idxs[i], i) = vals[i] < 0 ? -1 : 1;
        MATD_EL(RP, idxs[i], i) = 1; //vals[i] < 0 ? -1 : 1;
    }

    // we've factored:
    // LP*(something)*RP'

    // solve for (something)
    B = matd_op("M'*F*M", LP, B, RP);

    // update LS and RS, remembering that RS will be transposed.
    LS = matd_op("F*M", LS, LP);
    RS = matd_op("F*M", RS, RP);

    matd_destroy(LP);
    matd_destroy(RP);

    matd_svd_t res;
    memset(&res, 0, sizeof(res));

    // make B exactly diagonal

    for (int i = 0; i < B->nrows; i++) {
        for (int j = 0; j < B->ncols; j++) {
            if (i != j)
                MATD_EL(B, i, j) = 0;
        }
    }

    res.U = LS;
    res.S = B;
    res.V = RS;

    return res;
}

matd_svd_t matd_svd(matd_t *A)
{
    return matd_svd_flags(A, 0);
}

matd_svd_t matd_svd_flags(matd_t *A, int flags)
{
    matd_svd_t res;

    if (A->ncols <= A->nrows) {
        res = matd_svd_tall(A, flags);
    } else {
        matd_t *At = matd_transpose(A);

        // A =U  S  V'
        // A'=V  S' U'

        matd_svd_t tmp = matd_svd_tall(At, flags);

        memset(&res, 0, sizeof(res));
        res.U = tmp.V; //matd_transpose(tmp.V);
        res.S = matd_transpose(tmp.S);
        res.V = tmp.U; //matd_transpose(tmp.U);

        matd_destroy(tmp.S);
        matd_destroy(At);
    }

/*
  matd_t *check = matd_op("M*M*M'-M", res.U, res.S, res.V, A);
  float maxerr = 0;

  for (int i = 0; i < check->nrows; i++)
  for (int j = 0; j < check->ncols; j++)
  maxerr = fmax(maxerr, fabs(MATD_EL(check, i, j)));

  matd_destroy(check);

  if (maxerr > 1e-7) {
  printf("bad maxerr: %15f\n", maxerr);
  }

  if (maxerr > 1e-5) {
  printf("bad maxerr: %15f\n", maxerr);
  matd_print(A, "%15f");
  assert(0);
  }

*/
    return res;
}


matd_plu_t *matd_plu(const matd_t *a)
{
    unsigned int *piv = calloc(a->nrows, sizeof(unsigned int));
    int pivsign = 1;
    matd_t *lu = matd_copy(a);

    // only for square matrices.
    assert(a->nrows == a->ncols);

    matd_plu_t *mlu = calloc(1, sizeof(matd_plu_t));

    for (int i = 0; i < a->nrows; i++)
        piv[i] = i;

    for (int j = 0; j < a->ncols; j++) {
        for (int i = 0; i < a->nrows; i++) {
            int kmax = i < j ? i : j; // min(i,j)

            // compute dot product of row i with column j (up through element kmax)
            float acc = 0;
            for (int k = 0; k < kmax; k++)
                acc += MATD_EL(lu, i, k) * MATD_EL(lu, k, j);

            MATD_EL(lu, i, j) -= acc;
        }

        // find pivot and exchange if necessary.
        int p = j;
        if (1) {
            for (int i = j+1; i < lu->nrows; i++) {
                if (fabs(MATD_EL(lu,i,j)) > fabs(MATD_EL(lu, p, j))) {
                    p = i;
                }
            }
        }

        // swap rows p and j?
        if (p != j) {
            TYPE tmp[lu->ncols];
            memcpy(tmp, &MATD_EL(lu, p, 0), sizeof(TYPE) * lu->ncols);
            memcpy(&MATD_EL(lu, p, 0), &MATD_EL(lu, j, 0), sizeof(TYPE) * lu->ncols);
            memcpy(&MATD_EL(lu, j, 0), tmp, sizeof(TYPE) * lu->ncols);
            int k = piv[p];
            piv[p] = piv[j];
            piv[j] = k;
            pivsign = -pivsign;
        }

        float LUjj = MATD_EL(lu, j, j);

        // If our pivot is very small (which means the matrix is
        // singular or nearly singular), replace with a new pivot of the
        // right sign.
        if (fabs(LUjj) < MATD_EPS) {
/*
            if (LUjj < 0)
                LUjj = -MATD_EPS;
            else
                LUjj = MATD_EPS;

            MATD_EL(lu, j, j) = LUjj;
*/
            mlu->singular = 1;
        }

        if (j < lu->ncols && j < lu->nrows && LUjj != 0) {
            LUjj = 1.0 / LUjj;
            for (int i = j+1; i < lu->nrows; i++)
                MATD_EL(lu, i, j) *= LUjj;
        }
    }

    mlu->lu = lu;
    mlu->piv = piv;
    mlu->pivsign = pivsign;

    return mlu;
}

void matd_plu_destroy(matd_plu_t *mlu)
{
    matd_destroy(mlu->lu);
    free(mlu->piv);
    memset(mlu, 0, sizeof(matd_plu_t));
    free(mlu);
}

float matd_plu_det(const matd_plu_t *mlu)
{
    matd_t *lu = mlu->lu;
    float det = mlu->pivsign;

    if (lu->nrows == lu->ncols) {
        for (int i = 0; i < lu->ncols; i++)
            det *= MATD_EL(lu, i, i);
    }

    return det;
}

matd_t *matd_plu_p(const matd_plu_t *mlu)
{
    matd_t *lu = mlu->lu;
    matd_t *P = matd_create(lu->nrows, lu->nrows);

    for (int i = 0; i < lu->nrows; i++) {
        MATD_EL(P, mlu->piv[i], i) = 1;
    }

    return P;
}

matd_t *matd_plu_l(const matd_plu_t *mlu)
{
    matd_t *lu = mlu->lu;

    matd_t *L = matd_create(lu->nrows, lu->ncols);
    for (int i = 0; i < lu->nrows; i++) {
        MATD_EL(L, i, i) = 1;

        for (int j = 0; j < i; j++) {
            MATD_EL(L, i, j) = MATD_EL(lu, i, j);
        }
    }

    return L;
}

matd_t *matd_plu_u(const matd_plu_t *mlu)
{
    matd_t *lu = mlu->lu;

    matd_t *U = matd_create(lu->ncols, lu->ncols);
    for (int i = 0; i < lu->ncols; i++) {
        for (int j = 0; j < lu->ncols; j++) {
            if (i <= j)
                MATD_EL(U, i, j) = MATD_EL(lu, i, j);
        }
    }

    return U;
}

// PLU = A
// Ax = B
// PLUx = B
// LUx = P'B
matd_t *matd_plu_solve(const matd_plu_t *mlu, const matd_t *b)
{
    matd_t *x = matd_copy(b);

    // permute right hand side
    for (int i = 0; i < mlu->lu->nrows; i++)
        memcpy(&MATD_EL(x, i, 0), &MATD_EL(b, mlu->piv[i], 0), sizeof(TYPE) * b->ncols);

    // solve Ly = b
    for (int k = 0; k < mlu->lu->nrows; k++) {
        for (int i = k+1; i < mlu->lu->nrows; i++) {
            float LUik = -MATD_EL(mlu->lu, i, k);
            for (int t = 0; t < b->ncols; t++)
                MATD_EL(x, i, t) += MATD_EL(x, k, t) * LUik;
        }
    }

    // solve Ux = y
    for (int k = mlu->lu->ncols-1; k >= 0; k--) {
        float LUkk = 1.0 / MATD_EL(mlu->lu, k, k);
        for (int t = 0; t < b->ncols; t++)
            MATD_EL(x, k, t) *= LUkk;

        for (int i = 0; i < k; i++) {
            float LUik = -MATD_EL(mlu->lu, i, k);
            for (int t = 0; t < b->ncols; t++)
                MATD_EL(x, i, t) += MATD_EL(x, k, t) *LUik;
        }
    }

    return x;
}

matd_t *matd_solve(matd_t *A, matd_t *b)
{
    matd_plu_t *mlu = matd_plu(A);
    matd_t *x = matd_plu_solve(mlu, b);

    matd_plu_destroy(mlu);
    return x;
}

#if 0

static int randi()
{
    int v = random()&31;
    v -= 15;
    return v;
}

static float randf()
{
    float v = 1.0 *random() / RAND_MAX;
    return 2*v - 1;
}

int main(int argc, char *argv[])
{
    if (1) {
        int maxdim = 16;
        matd_t *A = matd_create(maxdim, maxdim);

        for (int iter = 0; 1; iter++) {
            srand(iter);

            if (iter % 1000 == 0)
                printf("%d\n", iter);

            int m = 1 + (random()%(maxdim-1));
            int n = 1 + (random()%(maxdim-1));

            for (int i = 0; i < m*n; i++)
                A->data[i] = randi();

            A->nrows = m;
            A->ncols = n;

//            printf("%d %d ", m, n);
            matd_svd_t svd = matd_svd(A);
            matd_destroy(svd.U);
            matd_destroy(svd.S);
            matd_destroy(svd.V);

        }

/*        matd_t *A = matd_create_data(2, 5, (float[]) { 1, 5, 2, 6,
          3, 3, 0, 7,
          1, 1, 0, -2,
          4, 0, 9, 9, 2, 6, 1, 3, 2, 5, 5, 4, -1, 2, 5, 9, 8, 2 });

          matd_svd(A);
*/
        return 0;
    }


    struct svd22 s;

    srand(0);

    matd_t *A = matd_create(2, 2);
    MATD_EL(A,0,0) = 4;
    MATD_EL(A,0,1) = 7;
    MATD_EL(A,1,0) = 2;
    MATD_EL(A,1,1) = 6;

    matd_t *U = matd_create(2, 2);
    matd_t *V = matd_create(2, 2);
    matd_t *S = matd_create(2, 2);

    for (int iter = 0; 1; iter++) {
        if (iter % 100000 == 0)
            printf("%d\n", iter);

        MATD_EL(A,0,0) = randf();
        MATD_EL(A,0,1) = randf();
        MATD_EL(A,1,0) = randf();
        MATD_EL(A,1,1) = randf();

        matd_svd22_impl(A->data, &s);

        memcpy(U->data, s.U, 4*sizeof(float));
        memcpy(V->data, s.V, 4*sizeof(float));
        MATD_EL(S,0,0) = s.S[0];
        MATD_EL(S,1,1) = s.S[1];

        assert(s.S[0] >= s.S[1]);
        assert(s.S[0] >= 0);
        assert(s.S[1] >= 0);
        if (s.S[0] == 0) {
//            printf("*"); fflush(NULL);
//            printf("%15f %15f %15f %15f\n", MATD_EL(A,0,0), MATD_EL(A,0,1), MATD_EL(A,1,0), MATD_EL(A,1,1));
        }
        if (s.S[1] == 0) {
//            printf("#"); fflush(NULL);
        }

        matd_t *USV = matd_op("M*M*M'", U, S, V);

        float maxerr = 0;
        for (int i = 0; i < 4; i++)
            maxerr = fmax(maxerr, fabs(USV->data[i] - A->data[i]));

        if (0) {
            printf("------------------------------------\n");
            printf("A:\n");
            matd_print(A, "%15f");
            printf("\nUSV':\n");
            matd_print(USV, "%15f");
            printf("maxerr: %.15f\n", maxerr);
            printf("\n\n");
        }

        matd_destroy(USV);

        assert(maxerr < 0.00001);
    }
}

#endif

// XXX NGV Cholesky
/*static float *matd_cholesky_raw(float *A, int n)
  {
  float *L = (float*)calloc(n * n, sizeof(float));

  for (int i = 0; i < n; i++) {
  for (int j = 0; j < (i+1); j++) {
  float s = 0;
  for (int k = 0; k < j; k++)
  s += L[i * n + k] * L[j * n + k];
  L[i * n + j] = (i == j) ?
  sqrt(A[i * n + i] - s) :
  (1.0 / L[j * n + j] * (A[i * n + j] - s));
  }
  }

  return L;
  }

  matd_t *matd_cholesky(const matd_t *A)
  {
  assert(A->nrows == A->ncols);
  float *L_data = matd_cholesky_raw(A->data, A->nrows);
  matd_t *L = matd_create_data(A->nrows, A->ncols, L_data);
  free(L_data);
  return L;
  }*/

// NOTE: The below implementation of Cholesky is different from the one
// used in NGV.
matd_chol_t *matd_chol(matd_t *A)
{
    assert(A->nrows == A->ncols);
    int N = A->nrows;

    // make upper right
    matd_t *U = matd_copy(A);

    // don't actually need to clear lower-left... we won't touch it.
/*    for (int i = 0; i < U->nrows; i++) {
      for (int j = 0; j < i; j++) {
//            assert(MATD_EL(U, i, j) == MATD_EL(U, j, i));
MATD_EL(U, i, j) = 0;
}
}
*/
    int is_spd = 1; // (A->nrows == A->ncols);

    for (int i = 0; i < N; i++) {
        float d = MATD_EL(U, i, i);
        is_spd &= (d > 0);

        if (d < MATD_EPS)
            d = MATD_EPS;
        d = 1.0 / sqrt(d);

        for (int j = i; j < N; j++)
            MATD_EL(U, i, j) *= d;

        for (int j = i+1; j < N; j++) {
            float s = MATD_EL(U, i, j);

            if (s == 0)
                continue;

            for (int k = j; k < N; k++) {
                MATD_EL(U, j, k) -= MATD_EL(U, i, k)*s;
            }
        }
    }

    matd_chol_t *chol = calloc(1, sizeof(matd_chol_t));
    chol->is_spd = is_spd;
    chol->u = U;
    return chol;
}

void matd_chol_destroy(matd_chol_t *chol)
{
    matd_destroy(chol->u);
    free(chol);
}

// Solve: (U')x = b, U is upper triangular
void matd_ltransposetriangle_solve(matd_t *u, const TYPE *b, TYPE *x)
{
    int n = u->ncols;
    memcpy(x, b, n*sizeof(TYPE));
    for (int i = 0; i < n; i++) {
        x[i] /= MATD_EL(u, i, i);

        for (int j = i+1; j < u->ncols; j++) {
            x[j] -= x[i] * MATD_EL(u, i, j);
        }
    }
}

// Solve: Lx = b, L is lower triangular
void matd_ltriangle_solve(matd_t *L, const TYPE *b, TYPE *x)
{
    int n = L->ncols;

    for (int i = 0; i < n; i++) {
        float acc = b[i];

        for (int j = 0; j < i; j++) {
            acc -= MATD_EL(L, i, j)*x[j];
        }

        x[i] = acc / MATD_EL(L, i, i);
    }
}

// solve Ux = b, U is upper triangular
void matd_utriangle_solve(matd_t *u, const TYPE *b, TYPE *x)
{
    for (int i = u->ncols-1; i >= 0; i--) {
        float bi = b[i];

        float diag = MATD_EL(u, i, i);

        for (int j = i+1; j < u->ncols; j++)
            bi -= MATD_EL(u, i, j)*x[j];

        x[i] = bi / diag;
    }
}

matd_t *matd_chol_solve(const matd_chol_t *chol, const matd_t *b)
{
    matd_t *u = chol->u;

    matd_t *x = matd_copy(b);

    // LUx = b

    // solve Ly = b ==> (U')y = b

    for (int i = 0; i < u->nrows; i++) {
        for (int j = 0; j < i; j++) {
            // b[i] -= L[i,j]*x[j]... replicated across columns of b
            //   ==> i.e., ==>
            // b[i,k] -= L[i,j]*x[j,k]
            for (int k = 0; k < b->ncols; k++) {
                MATD_EL(x, i, k) -= MATD_EL(u, j, i)*MATD_EL(x, j, k);
            }
        }
        // x[i] = b[i] / L[i,i]
        for (int k = 0; k < b->ncols; k++) {
            MATD_EL(x, i, k) /= MATD_EL(u, i, i);
        }
    }

    // solve Ux = y
    for (int k = u->ncols-1; k >= 0; k--) {
        float LUkk = 1.0 / MATD_EL(u, k, k);
        for (int t = 0; t < b->ncols; t++)
            MATD_EL(x, k, t) *= LUkk;

        for (int i = 0; i < k; i++) {
            float LUik = -MATD_EL(u, i, k);
            for (int t = 0; t < b->ncols; t++)
                MATD_EL(x, i, t) += MATD_EL(x, k, t) *LUik;
        }
    }

    return x;
}

/*void matd_chol_solve(matd_chol_t *chol, const TYPE *b, TYPE *x)
  {
  matd_t *u = chol->u;

  TYPE y[u->ncols];
  matd_ltransposetriangle_solve(u, b, y);
  matd_utriangle_solve(u, y, x);
  }
*/
// only sensible on PSD matrices. had expected it to be faster than
// inverse via LU... for now, doesn't seem to be.
matd_t *matd_chol_inverse(matd_t *a)
{
    assert(a->nrows == a->ncols);

    matd_chol_t *chol = matd_chol(a);

    matd_t *eye = matd_identity(a->nrows);
    matd_t *inv = matd_chol_solve(chol, eye);
    matd_destroy(eye);
    matd_chol_destroy(chol);

    return inv;
}

float matd_max(matd_t *m)
{
    float d = -FLT_MAX;
    for(int x=0; x<m->nrows; x++) {
        for(int y=0; y<m->ncols; y++) {
            if(MATD_EL(m, x, y) > d)
                d = MATD_EL(m, x, y);
        }
    }

    return d;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "homography.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Given a 3x3 homography matrix and the focal lengths of the
     * camera, compute the pose of the tag. The focal lengths should
     * be given in pixels. For example, if the camera's focal length
     * is twice the width of the sensor, and the sensor is 600 pixels
     * across, the focal length in pixels is 2*600. Note that the
     * focal lengths in the fx and fy direction will be approximately
     * equal for most lenses, and is not a function of aspect ratio.
     *
     * Theory: The homography matrix is the product of the camera
     * projection matrix and the tag's pose matrix (the matrix that
     * projects points from the tag's local coordinate system to the
     * camera's coordinate frame).
     *
    * [ h00 h01 h02 h03] = [ fx   0     cx 0 ] [ R00 R01 R02 TX ]
     * [ h10 h11 h12 h13] = [ 0    fy    cy 0 ] [ R10 R11 R12 TY ]
     * [ h20 h21 h22 h23] = [ 0    0      s 0 ] [ R20 R21 R22 TZ ]
     *                                          [ 0   0   0   1  ]
     *
     * fx is the focal length in the x direction of the camera
     * (typically measured in pixels), fy is the focal length. cx and
     * cy give the focal center (usually the middle of the image), and
     * s is either +1 or -1, depending on the conventions you use. (We
     * use 1.)

     * When observing a tag, the points we project in world space all
     * have z=0, so we can form a 3x3 matrix by eliminating the 3rd
     * column of the pose matrix.
     *
     * [ h00 h01 h02 ] = [ fx   0    cx 0 ] [ R00 R01 TX ]
     * [ h10 h11 h12 ] = [ 0    fy   cy 0 ] [ R10 R11 TY ]
     * [ h20 h21 h22 ] = [ 0    0     s 0 ] [ R20 R21 TZ ]
     *                                      [ 0   0   1  ]
     *
     * (note that these h's are different from the ones above.)
     *
     * We can multiply the right-hand side to yield a set of equations
     * relating the values of h to the values of the pose matrix.
     *
     * There are two wrinkles. The first is that the homography matrix
     * is known only up to scale. We recover the unknown scale by
     * constraining the magnitude of the first two columns of the pose
     * matrix to be 1. We use the geometric average scale. The sign of
     * the scale factor is recovered by constraining the observed tag
     * to be in front of the camera. Once scaled, we recover the first
     * two colmuns of the rotation matrix. The third column is the
     * cross product of these.
     *
     * The second wrinkle is that the computed rotation matrix might
     * not be exactly orthogonal, so we perform a polar decomposition
     * to find a good pure rotation approximation.
     *
     * Tagsize is the size of the tag in your desired units. I.e., if
     * your tag measures 0.25m along the side, your tag size is
     * 0.25. (The homography is computed in terms of *half* the tag
     * size, i.e., that a tag is 2 units wide as it spans from -1 to
     * +1, but this code makes the appropriate adjustment.)
     *
     * A note on signs:
     *
     * The code below incorporates no additional negative signs, but
     * respects the sign of any parameters that you pass in. Flipping
     * the signs allows you to modify the projection to suit a wide
     * variety of conditions.
     *
     * In the "pure geometry" projection matrix, the image appears
     * upside down; i.e., the x and y coordinates on the left hand
     * side are the opposite of those on the right of the camera
     * projection matrix. This would happen for all parameters
     * positive: recall that points in front of the camera have
     * negative Z values, which will cause the sign of all points to
     * flip.
     *
     * However, most cameras flip things so that the image appears
     * "right side up" as though you were looking through the lens
     * directly. This means that the projected points should have the
     * same sign as the points on the right of the camera projection
     * matrix. To achieve this, flip fx and fy.
     *
     * One further complication: cameras typically put y=0 at the top
     * of the image, instead of the bottom. Thus you generally want to
     * flip y yet again (so it's now positive again).
     *
     * General advice: you probably want fx negative, fy positive, cx
     * and cy positive, and s=1.
     **/

// correspondences is a list of float[4]s, consisting of the points x
// and y concatenated. We will compute a homography such that y = Hx
// Specifically, float [] { a, b, c, d } where x = [a b], y = [c d].


#define HOMOGRAPHY_COMPUTE_FLAG_INVERSE 1
#define HOMOGRAPHY_COMPUTE_FLAG_SVD 0

matd_t *homography_compute(zarray_t *correspondences, int flags);

//void homography_project(const matd_t *H, float x, float y, float *ox, float *oy);
static inline void homography_project(const matd_t *H, float x, float y, float *ox, float *oy)
{
    float xx = MATD_EL(H, 0, 0)*x + MATD_EL(H, 0, 1)*y + MATD_EL(H, 0, 2);
    float yy = MATD_EL(H, 1, 0)*x + MATD_EL(H, 1, 1)*y + MATD_EL(H, 1, 2);
    float zz = MATD_EL(H, 2, 0)*x + MATD_EL(H, 2, 1)*y + MATD_EL(H, 2, 2);

    *ox = xx / zz;
    *oy = yy / zz;
}

// assuming that the projection matrix is:
// [ fx 0  cx 0 ]
// [  0 fy cy 0 ]
// [  0  0  1 0 ]
//
// And that the homography is equal to the projection matrix times the model matrix,
// recover the model matrix (which is returned). Note that the third column of the model
// matrix is missing in the expresison below, reflecting the fact that the homography assumes
// all points are at z=0 (i.e., planar) and that the element of z is thus omitted.
// (3x1 instead of 4x1).
//
// [ fx 0  cx 0 ] [ R00  R01  TX ]    [ H00 H01 H02 ]
// [  0 fy cy 0 ] [ R10  R11  TY ] =  [ H10 H11 H12 ]
// [  0  0  1 0 ] [ R20  R21  TZ ] =  [ H20 H21 H22 ]
//                [  0    0    1 ]
//
// fx*R00 + cx*R20 = H00   (note, H only known up to scale; some additional adjustments required; see code.)
// fx*R01 + cx*R21 = H01
// fx*TX  + cx*TZ  = H02
// fy*R10 + cy*R20 = H10
// fy*R11 + cy*R21 = H11
// fy*TY  + cy*TZ  = H12
// R20 = H20
// R21 = H21
// TZ  = H22
matd_t *homography_to_pose(const matd_t *H, float fx, float fy, float cx, float cy);

// Similar to above
// Recover the model view matrix assuming that the projection matrix is:
//
// [ F  0  A  0 ]     (see glFrustrum)
// [ 0  G  B  0 ]
// [ 0  0  C  D ]
// [ 0  0 -1  0 ]

matd_t *homography_to_model_view(const matd_t *H, float F, float G, float A, float B, float C, float D);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "homography.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

// correspondences is a list of float[4]s, consisting of the points x
// and y concatenated. We will compute a homography such that y = Hx
matd_t *homography_compute(zarray_t *correspondences, int flags)
{
    // compute centroids of both sets of points (yields a better
    // conditioned information matrix)
    float x_cx = 0, x_cy = 0;
    float y_cx = 0, y_cy = 0;

    for (int i = 0; i < zarray_size(correspondences); i++) {
        float *c;
        zarray_get_volatile(correspondences, i, &c);

        x_cx += c[0];
        x_cy += c[1];
        y_cx += c[2];
        y_cy += c[3];
    }

    int sz = zarray_size(correspondences);
    x_cx /= sz;
    x_cy /= sz;
    y_cx /= sz;
    y_cy /= sz;

    // NB We don't normalize scale; it seems implausible that it could
    // possibly make any difference given the dynamic range of IEEE
    // doubles.

    matd_t *A = matd_create(9,9);
    for (int i = 0; i < zarray_size(correspondences); i++) {
        float *c;
        zarray_get_volatile(correspondences, i, &c);

        // (below world is "x", and image is "y")
        float worldx = c[0] - x_cx;
        float worldy = c[1] - x_cy;
        float imagex = c[2] - y_cx;
        float imagey = c[3] - y_cy;

        float a03 = -worldx;
        float a04 = -worldy;
        float a05 = -1;
        float a06 = worldx*imagey;
        float a07 = worldy*imagey;
        float a08 = imagey;

        MATD_EL(A, 3, 3) += a03*a03;
        MATD_EL(A, 3, 4) += a03*a04;
        MATD_EL(A, 3, 5) += a03*a05;
        MATD_EL(A, 3, 6) += a03*a06;
        MATD_EL(A, 3, 7) += a03*a07;
        MATD_EL(A, 3, 8) += a03*a08;
        MATD_EL(A, 4, 4) += a04*a04;
        MATD_EL(A, 4, 5) += a04*a05;
        MATD_EL(A, 4, 6) += a04*a06;
        MATD_EL(A, 4, 7) += a04*a07;
        MATD_EL(A, 4, 8) += a04*a08;
        MATD_EL(A, 5, 5) += a05*a05;
        MATD_EL(A, 5, 6) += a05*a06;
        MATD_EL(A, 5, 7) += a05*a07;
        MATD_EL(A, 5, 8) += a05*a08;
        MATD_EL(A, 6, 6) += a06*a06;
        MATD_EL(A, 6, 7) += a06*a07;
        MATD_EL(A, 6, 8) += a06*a08;
        MATD_EL(A, 7, 7) += a07*a07;
        MATD_EL(A, 7, 8) += a07*a08;
        MATD_EL(A, 8, 8) += a08*a08;

        float a10 = worldx;
        float a11 = worldy;
        float a12 = 1;
        float a16 = -worldx*imagex;
        float a17 = -worldy*imagex;
        float a18 = -imagex;

        MATD_EL(A, 0, 0) += a10*a10;
        MATD_EL(A, 0, 1) += a10*a11;
        MATD_EL(A, 0, 2) += a10*a12;
        MATD_EL(A, 0, 6) += a10*a16;
        MATD_EL(A, 0, 7) += a10*a17;
        MATD_EL(A, 0, 8) += a10*a18;
        MATD_EL(A, 1, 1) += a11*a11;
        MATD_EL(A, 1, 2) += a11*a12;
        MATD_EL(A, 1, 6) += a11*a16;
        MATD_EL(A, 1, 7) += a11*a17;
        MATD_EL(A, 1, 8) += a11*a18;
        MATD_EL(A, 2, 2) += a12*a12;
        MATD_EL(A, 2, 6) += a12*a16;
        MATD_EL(A, 2, 7) += a12*a17;
        MATD_EL(A, 2, 8) += a12*a18;
        MATD_EL(A, 6, 6) += a16*a16;
        MATD_EL(A, 6, 7) += a16*a17;
        MATD_EL(A, 6, 8) += a16*a18;
        MATD_EL(A, 7, 7) += a17*a17;
        MATD_EL(A, 7, 8) += a17*a18;
        MATD_EL(A, 8, 8) += a18*a18;

        float a20 = -worldx*imagey;
        float a21 = -worldy*imagey;
        float a22 = -imagey;
        float a23 = worldx*imagex;
        float a24 = worldy*imagex;
        float a25 = imagex;

        MATD_EL(A, 0, 0) += a20*a20;
        MATD_EL(A, 0, 1) += a20*a21;
        MATD_EL(A, 0, 2) += a20*a22;
        MATD_EL(A, 0, 3) += a20*a23;
        MATD_EL(A, 0, 4) += a20*a24;
        MATD_EL(A, 0, 5) += a20*a25;
        MATD_EL(A, 1, 1) += a21*a21;
        MATD_EL(A, 1, 2) += a21*a22;
        MATD_EL(A, 1, 3) += a21*a23;
        MATD_EL(A, 1, 4) += a21*a24;
        MATD_EL(A, 1, 5) += a21*a25;
        MATD_EL(A, 2, 2) += a22*a22;
        MATD_EL(A, 2, 3) += a22*a23;
        MATD_EL(A, 2, 4) += a22*a24;
        MATD_EL(A, 2, 5) += a22*a25;
        MATD_EL(A, 3, 3) += a23*a23;
        MATD_EL(A, 3, 4) += a23*a24;
        MATD_EL(A, 3, 5) += a23*a25;
        MATD_EL(A, 4, 4) += a24*a24;
        MATD_EL(A, 4, 5) += a24*a25;
        MATD_EL(A, 5, 5) += a25*a25;
    }

    // make symmetric
    for (int i = 0; i < 9; i++)
        for (int j = i+1; j < 9; j++)
            MATD_EL(A, j, i) = MATD_EL(A, i, j);

    matd_t *H = matd_create(3,3);

    if (flags & HOMOGRAPHY_COMPUTE_FLAG_INVERSE) {
        // compute singular vector by (carefully) inverting the rank-deficient matrix.

        if (1) {
            matd_t *Ainv = matd_inverse(A);
            float scale = 0;

            for (int i = 0; i < 9; i++)
                scale += sq(MATD_EL(Ainv, i, 0));
            scale = sqrt(scale);

            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    MATD_EL(H, i, j) = MATD_EL(Ainv, 3*i+j, 0) / scale;

            matd_destroy(Ainv);
        } else {

            matd_t *b = matd_create_data(9, 1, (float[]) { 1, 0, 0, 0, 0, 0, 0, 0, 0 });
            matd_t *Ainv = NULL;

            if (0) {
                matd_plu_t *lu = matd_plu(A);
                Ainv = matd_plu_solve(lu, b);
                matd_plu_destroy(lu);
            } else {
                matd_chol_t *chol = matd_chol(A);
                Ainv = matd_chol_solve(chol, b);
                matd_chol_destroy(chol);
            }

            float scale = 0;

            for (int i = 0; i < 9; i++)
                scale += sq(MATD_EL(Ainv, i, 0));
            scale = sqrt(scale);

            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    MATD_EL(H, i, j) = MATD_EL(Ainv, 3*i+j, 0) / scale;

            matd_destroy(b);
            matd_destroy(Ainv);
        }

    } else {
        // compute singular vector using SVD. A bit slower, but more accurate.
        matd_svd_t svd = matd_svd_flags(A, MATD_SVD_NO_WARNINGS);

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                MATD_EL(H, i, j) = MATD_EL(svd.U, 3*i+j, 8);

        matd_destroy(svd.U);
        matd_destroy(svd.S);
        matd_destroy(svd.V);

    }

    matd_t *Tx = matd_identity(3);
    MATD_EL(Tx,0,2) = -x_cx;
    MATD_EL(Tx,1,2) = -x_cy;

    matd_t *Ty = matd_identity(3);
    MATD_EL(Ty,0,2) = y_cx;
    MATD_EL(Ty,1,2) = y_cy;

    matd_t *H2 = matd_op("M*M*M", Ty, H, Tx);

    matd_destroy(A);
    matd_destroy(Tx);
    matd_destroy(Ty);
    matd_destroy(H);

    return H2;
}


// assuming that the projection matrix is:
// [ fx 0  cx 0 ]
// [  0 fy cy 0 ]
// [  0  0  1 0 ]
//
// And that the homography is equal to the projection matrix times the
// model matrix, recover the model matrix (which is returned). Note
// that the third column of the model matrix is missing in the
// expresison below, reflecting the fact that the homography assumes
// all points are at z=0 (i.e., planar) and that the element of z is
// thus omitted.  (3x1 instead of 4x1).
//
// [ fx 0  cx 0 ] [ R00  R01  TX ]    [ H00 H01 H02 ]
// [  0 fy cy 0 ] [ R10  R11  TY ] =  [ H10 H11 H12 ]
// [  0  0  1 0 ] [ R20  R21  TZ ] =  [ H20 H21 H22 ]
//                [  0    0    1 ]
//
// fx*R00 + cx*R20 = H00   (note, H only known up to scale; some additional adjustments required; see code.)
// fx*R01 + cx*R21 = H01
// fx*TX  + cx*TZ  = H02
// fy*R10 + cy*R20 = H10
// fy*R11 + cy*R21 = H11
// fy*TY  + cy*TZ  = H12
// R20 = H20
// R21 = H21
// TZ  = H22

matd_t *homography_to_pose(const matd_t *H, float fx, float fy, float cx, float cy)
{
    // Note that every variable that we compute is proportional to the scale factor of H.
    float R20 = MATD_EL(H, 2, 0);
    float R21 = MATD_EL(H, 2, 1);
    float TZ  = MATD_EL(H, 2, 2);
    float R00 = (MATD_EL(H, 0, 0) - cx*R20) / fx;
    float R01 = (MATD_EL(H, 0, 1) - cx*R21) / fx;
    float TX  = (MATD_EL(H, 0, 2) - cx*TZ)  / fx;
    float R10 = (MATD_EL(H, 1, 0) - cy*R20) / fy;
    float R11 = (MATD_EL(H, 1, 1) - cy*R21) / fy;
    float TY  = (MATD_EL(H, 1, 2) - cy*TZ)  / fy;

    // compute the scale by requiring that the rotation columns are unit length
    // (Use geometric average of the two length vectors we have)
    float length1 = sqrtf(R00*R00 + R10*R10 + R20*R20);
    float length2 = sqrtf(R01*R01 + R11*R11 + R21*R21);
    float s = 1.0 / sqrtf(length1 * length2);

    // get sign of S by requiring the tag to be in front the camera;
    // we assume camera looks in the -Z direction.
    if (TZ > 0)
        s *= -1;

    R20 *= s;
    R21 *= s;
    TZ  *= s;
    R00 *= s;
    R01 *= s;
    TX  *= s;
    R10 *= s;
    R11 *= s;
    TY  *= s;

    // now recover [R02 R12 R22] by noting that it is the cross product of the other two columns.
    float R02 = R10*R21 - R20*R11;
    float R12 = R20*R01 - R00*R21;
    float R22 = R00*R11 - R10*R01;

    // Improve rotation matrix by applying polar decomposition.
    if (1) {
        // do polar decomposition. This makes the rotation matrix
        // "proper", but probably increases the reprojection error. An
        // iterative alignment step would be superior.

        matd_t *R = matd_create_data(3, 3, (float[]) { R00, R01, R02,
                                                       R10, R11, R12,
                                                       R20, R21, R22 });

        matd_svd_t svd = matd_svd(R);
        matd_destroy(R);

        R = matd_op("M*M'", svd.U, svd.V);

        matd_destroy(svd.U);
        matd_destroy(svd.S);
        matd_destroy(svd.V);

        R00 = MATD_EL(R, 0, 0);
        R01 = MATD_EL(R, 0, 1);
        R02 = MATD_EL(R, 0, 2);
        R10 = MATD_EL(R, 1, 0);
        R11 = MATD_EL(R, 1, 1);
        R12 = MATD_EL(R, 1, 2);
        R20 = MATD_EL(R, 2, 0);
        R21 = MATD_EL(R, 2, 1);
        R22 = MATD_EL(R, 2, 2);

        matd_destroy(R);
    }

    return matd_create_data(4, 4, (float[]) { R00, R01, R02, TX,
                                               R10, R11, R12, TY,
                                               R20, R21, R22, TZ,
                                                0, 0, 0, 1 });
}

// Similar to above
// Recover the model view matrix assuming that the projection matrix is:
//
// [ F  0  A  0 ]     (see glFrustrum)
// [ 0  G  B  0 ]
// [ 0  0  C  D ]
// [ 0  0 -1  0 ]

matd_t *homography_to_model_view(const matd_t *H, float F, float G, float A, float B, float C, float D)
{
    // Note that every variable that we compute is proportional to the scale factor of H.
    float R20 = -MATD_EL(H, 2, 0);
    float R21 = -MATD_EL(H, 2, 1);
    float TZ  = -MATD_EL(H, 2, 2);
    float R00 = (MATD_EL(H, 0, 0) - A*R20) / F;
    float R01 = (MATD_EL(H, 0, 1) - A*R21) / F;
    float TX  = (MATD_EL(H, 0, 2) - A*TZ)  / F;
    float R10 = (MATD_EL(H, 1, 0) - B*R20) / G;
    float R11 = (MATD_EL(H, 1, 1) - B*R21) / G;
    float TY  = (MATD_EL(H, 1, 2) - B*TZ)  / G;

    // compute the scale by requiring that the rotation columns are unit length
    // (Use geometric average of the two length vectors we have)
    float length1 = sqrtf(R00*R00 + R10*R10 + R20*R20);
    float length2 = sqrtf(R01*R01 + R11*R11 + R21*R21);
    float s = 1.0 / sqrtf(length1 * length2);

    // get sign of S by requiring the tag to be in front of the camera
    // (which is Z < 0) for our conventions.
    if (TZ > 0)
        s *= -1;

    R20 *= s;
    R21 *= s;
    TZ  *= s;
    R00 *= s;
    R01 *= s;
    TX  *= s;
    R10 *= s;
    R11 *= s;
    TY  *= s;

    // now recover [R02 R12 R22] by noting that it is the cross product of the other two columns.
    float R02 = R10*R21 - R20*R11;
    float R12 = R20*R01 - R00*R21;
    float R22 = R00*R11 - R10*R01;

    // TODO XXX: Improve rotation matrix by applying polar decomposition.

    return matd_create_data(4, 4, (float[]) { R00, R01, R02, TX,
        R10, R11, R12, TY,
        R20, R21, R22, TZ,
        0, 0, 0, 1 });
}

// Only uses the upper 3x3 matrix.
/*
static void matrix_to_quat(const matd_t *R, float q[4])
{
    // see: "from quaternion to matrix and back"

    // trace: get the same result if R is 4x4 or 3x3:
    float T = MATD_EL(R, 0, 0) + MATD_EL(R, 1, 1) + MATD_EL(R, 2, 2) + 1;
    float S = 0;

    float m0  = MATD_EL(R, 0, 0);
    float m1  = MATD_EL(R, 1, 0);
    float m2  = MATD_EL(R, 2, 0);
    float m4  = MATD_EL(R, 0, 1);
    float m5  = MATD_EL(R, 1, 1);
    float m6  = MATD_EL(R, 2, 1);
    float m8  = MATD_EL(R, 0, 2);
    float m9  = MATD_EL(R, 1, 2);
    float m10 = MATD_EL(R, 2, 2);

    if (T > 0.0000001) {
        S = sqrtf(T) * 2;
        q[1] = -( m9 - m6 ) / S;
        q[2] = -( m2 - m8 ) / S;
        q[3] = -( m4 - m1 ) / S;
        q[0] = 0.25 * S;
    } else if ( m0 > m5 && m0 > m10 )  {	// Column 0:
        S  = sqrtf( 1.0 + m0 - m5 - m10 ) * 2;
        q[1] = -0.25 * S;
        q[2] = -(m4 + m1 ) / S;
        q[3] = -(m2 + m8 ) / S;
        q[0] = (m9 - m6 ) / S;
    } else if ( m5 > m10 ) {			// Column 1:
        S  = sqrtf( 1.0 + m5 - m0 - m10 ) * 2;
        q[1] = -(m4 + m1 ) / S;
        q[2] = -0.25 * S;
        q[3] = -(m9 + m6 ) / S;
        q[0] = (m2 - m8 ) / S;
    } else {
        // Column 2:
        S  = sqrtf( 1.0 + m10 - m0 - m5 ) * 2;
        q[1] = -(m2 + m8 ) / S;
        q[2] = -(m9 + m6 ) / S;
        q[3] = -0.25 * S;
        q[0] = (m4 - m1 ) / S;
    }

    float mag2 = 0;
    for (int i = 0; i < 4; i++)
        mag2 += q[i]*q[i];
    float norm = 1.0 / sqrtf(mag2);
    for (int i = 0; i < 4; i++)
        q[i] *= norm;
}
*/

// overwrites upper 3x3 area of matrix M. Doesn't touch any other elements of M.
void quat_to_matrix(const float q[4], matd_t *M)
{
    float w = q[0], x = q[1], y = q[2], z = q[3];

    MATD_EL(M, 0, 0) = w*w + x*x - y*y - z*z;
    MATD_EL(M, 0, 1) = 2*x*y - 2*w*z;
    MATD_EL(M, 0, 2) = 2*x*z + 2*w*y;

    MATD_EL(M, 1, 0) = 2*x*y + 2*w*z;
    MATD_EL(M, 1, 1) = w*w - x*x + y*y - z*z;
    MATD_EL(M, 1, 2) = 2*y*z - 2*w*x;

    MATD_EL(M, 2, 0) = 2*x*z - 2*w*y;
    MATD_EL(M, 2, 1) = 2*y*z + 2*w*x;
    MATD_EL(M, 2, 2) = w*w - x*x - y*y + z*z;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "g2d.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

// This library tries to avoid needless proliferation of types.
//
// A point is a float[2]. (Note that when passing a float[2] as an
// argument, it is passed by pointer, not by value.)
//
// A polygon is a zarray_t of float[2]. (Note that in this case, the
// zarray contains the actual vertex data, and not merely a pointer to
// some other data. IMPORTANT: A polygon must be specified in CCW
// order.  It is implicitly closed (do not list the same point at the
// beginning at the end.
//
// Where sensible, it is assumed that objects should be allocated
// sparingly; consequently "init" style methods, rather than "create"
// methods are used.

////////////////////////////////////////////////////////////////////
// Lines

typedef struct
{
    // Internal representation: a point that the line goes through (p) and
    // the direction of the line (u).
    float p[2];
    float u[2]; // always a unit vector
} g2d_line_t;

// initialize a line object.
void g2d_line_init_from_points(g2d_line_t *line, const float p0[2], const float p1[2]);

// The line defines a one-dimensional coordinate system whose origin
// is p. Where is q? (If q is not on the line, the point nearest q is
// returned.
float g2d_line_get_coordinate(const g2d_line_t *line, const float q[2]);

// Intersect two lines. The intersection, if it exists, is written to
// p (if not NULL), and 1 is returned. Else, zero is returned.
int g2d_line_intersect_line(const g2d_line_t *linea, const g2d_line_t *lineb, float *p);

////////////////////////////////////////////////////////////////////
// Line Segments. line.p is always one endpoint; p1 is the other
// endpoint.
typedef struct
{
    g2d_line_t line;
    float p1[2];
} g2d_line_segment_t;

void g2d_line_segment_init_from_points(g2d_line_segment_t *seg, const float p0[2], const float p1[2]);

// Intersect two segments. The intersection, if it exists, is written
// to p (if not NULL), and 1 is returned. Else, zero is returned.
int g2d_line_segment_intersect_segment(const g2d_line_segment_t *sega, const g2d_line_segment_t *segb, float *p);

void g2d_line_segment_closest_point(const g2d_line_segment_t *seg, const float *q, float *p);
float g2d_line_segment_closest_point_distance(const g2d_line_segment_t *seg, const float *q);

////////////////////////////////////////////////////////////////////
// Polygons

zarray_t *g2d_polygon_create_data(float v[][2], int sz);

zarray_t *g2d_polygon_create_zeros(int sz);

zarray_t *g2d_polygon_create_empty();

void g2d_polygon_add(zarray_t *poly, float v[2]);

// Takes a polygon in either CW or CCW and modifies it (if necessary)
// to be CCW.
void g2d_polygon_make_ccw(zarray_t *poly);

// Return 1 if point q lies within poly.
int g2d_polygon_contains_point(const zarray_t *poly, float q[2]);

// Do the edges of the polygons cross? (Does not test for containment).
int g2d_polygon_intersects_polygon(const zarray_t *polya, const zarray_t *polyb);

// Does polya completely contain polyb?
int g2d_polygon_contains_polygon(const zarray_t *polya, const zarray_t *polyb);

// Is there some point which is in both polya and polyb?
int g2d_polygon_overlaps_polygon(const zarray_t *polya, const zarray_t *polyb);

// returns the number of points written to x. see comments.
int g2d_polygon_rasterize(const zarray_t *poly, float y, float *x);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "g2d.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

float g2d_distance(const float a[2], const float b[2])
{
    return sqrtf(sq(a[0]-b[0]) + sq(a[1]-b[1]));
}

zarray_t *g2d_polygon_create_empty()
{
    return zarray_create(sizeof(float[2]));
}

void g2d_polygon_add(zarray_t *poly, float v[2])
{
    zarray_add(poly, v);
}

zarray_t *g2d_polygon_create_data(float v[][2], int sz)
{
    zarray_t *points = g2d_polygon_create_empty();

    for (int i = 0; i < sz; i++)
        g2d_polygon_add(points, v[i]);

    return points;
}

zarray_t *g2d_polygon_create_zeros(int sz)
{
    zarray_t *points = zarray_create(sizeof(float[2]));

    float z[2] = { 0, 0 };

    for (int i = 0; i < sz; i++)
        zarray_add(points, z);

    return points;
}

void g2d_polygon_make_ccw(zarray_t *poly)
{
    // Step one: we want the points in counter-clockwise order.
    // If the points are in clockwise order, we'll reverse them.
    float total_theta = 0;
    float last_theta = 0;

    // Count the angle accumulated going around the polygon. If
    // the sum is +2pi, it's CCW. Otherwise, we'll get -2pi.
    int sz = zarray_size(poly);

    for (int i = 0; i <= sz; i++) {
        float p0[2], p1[2];
        zarray_get(poly, i % sz, &p0);
        zarray_get(poly, (i+1) % sz, &p1);

        float this_theta = atan2(p1[1]-p0[1], p1[0]-p0[0]);

        if (i > 0) {
            float dtheta = mod2pi(this_theta-last_theta);
            total_theta += dtheta;
        }

        last_theta = this_theta;
    }

    int ccw = (total_theta > 0);

    // reverse order if necessary.
    if (!ccw) {
        for (int i = 0; i < sz / 2; i++) {
            float a[2], b[2];

            zarray_get(poly, i, a);
            zarray_get(poly, sz-1-i, b);
            zarray_set(poly, i, b, NULL);
            zarray_set(poly, sz-1-i, a, NULL);
        }
    }
}

int g2d_polygon_contains_point_ref(const zarray_t *poly, float q[2])
{
    // use winding. If the point is inside the polygon, we'll wrap
    // around it (accumulating 6.28 radians). If we're outside the
    // polygon, we'll accumulate zero.
    int psz = zarray_size(poly);

    float acc_theta = 0;

    float last_theta;

    for (int i = 0; i <= psz; i++) {
        float p[2];

        zarray_get(poly, i % psz, &p);

        float this_theta = atan2(q[1]-p[1], q[0]-p[0]);

        if (i != 0)
            acc_theta += mod2pi(this_theta - last_theta);

        last_theta = this_theta;
    }

    return acc_theta > M_PI;
}

/*
// sort by x coordinate, ascending
static int g2d_convex_hull_sort(const void *_a, const void *_b)
{
    float *a = (float*) _a;
    float *b = (float*) _b;

    if (a[0] < b[0])
        return -1;
    if (a[0] == b[0])
        return 0;
    return 1;
}
*/

/*
zarray_t *g2d_convex_hull2(const zarray_t *points)
{
    zarray_t *hull = zarray_copy(points);

    zarray_sort(hull, g2d_convex_hull_sort);

    int hsz = zarray_size(hull);
    int hout = 0;

    for (int hin = 1; hin < hsz; hin++) {
        float *p;
        zarray_get_volatile(hull, i, &p);

        // Everything to the right of hin is already convex. We now
        // add one point, p, which begins "connected" by two
        // (coincident) edges from the last right-most point to p.
        float *last;
        zarray_get_volatile(hull, hout, &last);

        // We now remove points from the convex hull by moving
    }

    return hull;
}
*/

// creates and returns a zarray(float[2]). The resulting polygon is
// CCW and implicitly closed. Unnecessary colinear points are omitted.
zarray_t *g2d_convex_hull(const zarray_t *points)
{
    zarray_t *hull = zarray_create(sizeof(float[2]));

    // gift-wrap algorithm.

    // step 1: find left most point.
    int insz = zarray_size(points);

    // must have at least 2 points. (XXX need 3?)
    assert(insz >= 2);

    float *pleft = NULL;
    for (int i = 0; i < insz; i++) {
        float *p;
        zarray_get_volatile(points, i, &p);

        if (pleft == NULL || p[0] < pleft[0])
            pleft = p;
    }

    // cannot be NULL since there must be at least one point.
    assert(pleft != NULL);

    zarray_add(hull, pleft);

    // step 2. gift wrap. Keep searching for points that make the
    // smallest-angle left-hand turn. This implementation is carefully
    // written to use only addition/subtraction/multiply. No division
    // or sqrts. This guarantees exact results for integer-coordinate
    // polygons (no rounding/precision problems).
    float *p = pleft;

    while (1) {
        assert(p != NULL);

        float *q = NULL;
        float n0 = 0, n1 = 0; // the normal to the line (p, q) (not
                       // necessarily unit length).

        // Search for the point q for which the line (p,q) is most "to
        // the right of" the other points. (i.e., every time we find a
        // point that is to the right of our current line, we change
        // lines.)
        for (int i = 0; i < insz; i++) {
            float *thisq;
            zarray_get_volatile(points, i, &thisq);

            if (thisq == p)
                continue;

            // the first time we find another point, we initialize our
            // value of q, forming the line (p,q)
            if (q == NULL) {
                q = thisq;
                n0 = q[1] - p[1];
                n1 = -q[0] + p[0];
            } else {
                // we already have a line (p,q). is point thisq RIGHT OF line (p, q)?
                float e0 = thisq[0] - p[0], e1 = thisq[1] - p[1];
                float dot = e0*n0 + e1*n1;

                if (dot > 0) {
                    // it is. change our line.
                    q = thisq;
                    n0 = q[1] - p[1];
                    n1 = -q[0] + p[0];
                }
            }
        }

        // we must have elected *some* line, so long as there are at
        // least 2 points in the polygon.
        assert(q != NULL);

        // loop completed?
        if (q == pleft)
            break;

        int colinear = 0;

        // is this new point colinear with the last two?
        if (zarray_size(hull) > 1) {
            float *o;
            zarray_get_volatile(hull, zarray_size(hull) - 2, &o);

            float e0 = o[0] - p[0];
            float e1 = o[1] - p[1];

            if (n0*e0 + n1*e1 == 0)
                colinear = 1;
        }

        // if it is colinear, overwrite the last one.
        if (colinear)
            zarray_set(hull, zarray_size(hull)-1, q, NULL);
        else
            zarray_add(hull, q);

        p = q;
    }

    return hull;
}

// Find point p on the boundary of poly that is closest to q.
void g2d_polygon_closest_boundary_point(const zarray_t *poly, const float q[2], float *p)
{
    int psz = zarray_size(poly);
    float min_dist = HUGE_VALF;

    for (int i = 0; i < psz; i++) {
        float *p0, *p1;

        zarray_get_volatile(poly, i, &p0);
        zarray_get_volatile(poly, (i+1) % psz, &p1);

        g2d_line_segment_t seg;
        g2d_line_segment_init_from_points(&seg, p0, p1);

        float thisp[2];
        g2d_line_segment_closest_point(&seg, q, thisp);

        float dist = g2d_distance(q, thisp);
        if (dist < min_dist) {
            memcpy(p, thisp, sizeof(float[2]));
            min_dist = dist;
        }
    }
}

int g2d_polygon_contains_point(const zarray_t *poly, float q[2])
{
    // use winding. If the point is inside the polygon, we'll wrap
    // around it (accumulating 6.28 radians). If we're outside the
    // polygon, we'll accumulate zero.
    int psz = zarray_size(poly);
    assert(psz > 0);

    int last_quadrant;
    int quad_acc = 0;

    for (int i = 0; i <= psz; i++) {
        float *p;

        zarray_get_volatile(poly, i % psz, &p);

        // p[0] < q[0]       p[1] < q[1]    quadrant
        //     0                 0              0
        //     0                 1              3
        //     1                 0              1
        //     1                 1              2

        // p[1] < q[1]       p[0] < q[0]    quadrant
        //     0                 0              0
        //     0                 1              1
        //     1                 0              3
        //     1                 1              2

        int quadrant;
        if (p[0] < q[0])
            quadrant = (p[1] < q[1]) ? 2 : 1;
        else
            quadrant = (p[1] < q[1]) ? 3 : 0;

        if (i > 0) {
            int dquadrant = quadrant - last_quadrant;

            // encourage a jump table by mapping to small positive integers.
            switch (dquadrant) {
                case -3:
                case 1:
                    quad_acc ++;
                    break;
                case -1:
                case 3:
                    quad_acc --;
                    break;
                case 0:
                    break;
                case -2:
                case 2:
                {
                    // get the previous point.
                    float *p0;
                    zarray_get_volatile(poly, i-1, &p0);

                    // Consider the points p0 and p (the points around the
                    //polygon that we are tracing) and the query point q.
                    //
                    // If we've moved diagonally across quadrants, we want
                    // to measure whether we have rotated +PI radians or
                    // -PI radians. We can test this by computing the dot
                    // product of vector (p0-q) with the vector
                    // perpendicular to vector (p-q)
                    float nx = p[1] - q[1];
                    float ny = -p[0] + q[0];

                    float dot = nx*(p0[0]-q[0]) + ny*(p0[1]-q[1]);
                    if (dot < 0)
                        quad_acc -= 2;
                    else
                        quad_acc += 2;

                    break;
                }
            }
        }

        last_quadrant = quadrant;
    }

    int v = (quad_acc >= 2) || (quad_acc <= -2);

    if (0 && v != g2d_polygon_contains_point_ref(poly, q)) {
        printf("FAILURE %d %d\n", v, quad_acc);
        exit(-1);
    }

    return v;
}

void g2d_line_init_from_points(g2d_line_t *line, const float p0[2], const float p1[2])
{
    line->p[0] = p0[0];
    line->p[1] = p0[1];
    line->u[0] = p1[0]-p0[0];
    line->u[1] = p1[1]-p0[1];
    float mag = sqrtf(sq(line->u[0]) + sq(line->u[1]));

    line->u[0] /= mag;
    line->u[1] /= mag;
}

float g2d_line_get_coordinate(const g2d_line_t *line, const float q[2])
{
    return (q[0]-line->p[0])*line->u[0] + (q[1]-line->p[1])*line->u[1];
}

// Compute intersection of two line segments. If they intersect,
// result is stored in p and 1 is returned. Otherwise, zero is
// returned. p may be NULL.
int g2d_line_intersect_line(const g2d_line_t *linea, const g2d_line_t *lineb, float *p)
{
    // this implementation is many times faster than the original,
    // mostly due to avoiding a general-purpose LU decomposition in
    // Matrix.inverse().
    float m00, m01, m10, m11;
    float i00, i01;
    float b00, b10;

    m00 = linea->u[0];
    m01= -lineb->u[0];
    m10 = linea->u[1];
    m11= -lineb->u[1];

    // determinant of m
    float det = m00*m11-m01*m10;

    // parallel lines?
    if (fabs(det) < 0.00000001)
        return 0;

    // inverse of m
    i00 = m11/det;
    i01 = -m01/det;

    b00 = lineb->p[0] - linea->p[0];
    b10 = lineb->p[1] - linea->p[1];

    float x00; //, x10;
    x00 = i00*b00+i01*b10;

    if (p != NULL) {
        p[0] = linea->u[0]*x00 + linea->p[0];
        p[1] = linea->u[1]*x00 + linea->p[1];
    }

    return 1;
}


void g2d_line_segment_init_from_points(g2d_line_segment_t *seg, const float p0[2], const float p1[2])
{
    g2d_line_init_from_points(&seg->line, p0, p1);
    seg->p1[0] = p1[0];
    seg->p1[1] = p1[1];
}

// Find the point p on segment seg that is closest to point q.
void g2d_line_segment_closest_point(const g2d_line_segment_t *seg, const float *q, float *p)
{
    float a = g2d_line_get_coordinate(&seg->line, seg->line.p);
    float b = g2d_line_get_coordinate(&seg->line, seg->p1);
    float c = g2d_line_get_coordinate(&seg->line, q);

    if (a < b)
        c = dclamp(c, a, b);
    else
        c = dclamp(c, b, a);

    p[0] = seg->line.p[0] + c * seg->line.u[0];
    p[1] = seg->line.p[1] + c * seg->line.u[1];
}

// Compute intersection of two line segments. If they intersect,
// result is stored in p and 1 is returned. Otherwise, zero is
// returned. p may be NULL.
int g2d_line_segment_intersect_segment(const g2d_line_segment_t *sega, const g2d_line_segment_t *segb, float *p)
{
    float tmp[2];

    if (!g2d_line_intersect_line(&sega->line, &segb->line, tmp))
        return 0;

    float a = g2d_line_get_coordinate(&sega->line, sega->line.p);
    float b = g2d_line_get_coordinate(&sega->line, sega->p1);
    float c = g2d_line_get_coordinate(&sega->line, tmp);

    // does intersection lie on the first line?
    if ((c<a && c<b) || (c>a && c>b))
        return 0;

    a = g2d_line_get_coordinate(&segb->line, segb->line.p);
    b = g2d_line_get_coordinate(&segb->line, segb->p1);
    c = g2d_line_get_coordinate(&segb->line, tmp);

    // does intersection lie on second line?
    if ((c<a && c<b) || (c>a && c>b))
        return 0;

    if (p != NULL) {
        p[0] = tmp[0];
        p[1] = tmp[1];
    }

    return 1;
}

// Compute intersection of a line segment and a line. If they
// intersect, result is stored in p and 1 is returned. Otherwise, zero
// is returned. p may be NULL.
int g2d_line_segment_intersect_line(const g2d_line_segment_t *seg, const g2d_line_t *line, float *p)
{
    float tmp[2];

    if (!g2d_line_intersect_line(&seg->line, line, tmp))
        return 0;

    float a = g2d_line_get_coordinate(&seg->line, seg->line.p);
    float b = g2d_line_get_coordinate(&seg->line, seg->p1);
    float c = g2d_line_get_coordinate(&seg->line, tmp);

    // does intersection lie on the first line?
    if ((c<a && c<b) || (c>a && c>b))
        return 0;

    if (p != NULL) {
        p[0] = tmp[0];
        p[1] = tmp[1];
    }

    return 1;
}

// do the edges of polya and polyb collide? (Does NOT test for containment).
int g2d_polygon_intersects_polygon(const zarray_t *polya, const zarray_t *polyb)
{
    // do any of the line segments collide? If so, the answer is no.

    // dumb N^2 method.
    for (int ia = 0; ia < zarray_size(polya); ia++) {
        float pa0[2], pa1[2];
        zarray_get(polya, ia, pa0);
        zarray_get(polya, (ia+1)%zarray_size(polya), pa1);

        g2d_line_segment_t sega;
        g2d_line_segment_init_from_points(&sega, pa0, pa1);

        for (int ib = 0; ib < zarray_size(polyb); ib++) {
            float pb0[2], pb1[2];
            zarray_get(polyb, ib, pb0);
            zarray_get(polyb, (ib+1)%zarray_size(polyb), pb1);

            g2d_line_segment_t segb;
            g2d_line_segment_init_from_points(&segb, pb0, pb1);

            if (g2d_line_segment_intersect_segment(&sega, &segb, NULL))
                return 1;
        }
    }

    return 0;
}

// does polya completely contain polyb?
int g2d_polygon_contains_polygon(const zarray_t *polya, const zarray_t *polyb)
{
    // do any of the line segments collide? If so, the answer is no.
    if (g2d_polygon_intersects_polygon(polya, polyb))
        return 0;

    // if none of the edges cross, then the polygon is either fully
    // contained or fully outside.
    float p[2];
    zarray_get(polyb, 0, p);

    return g2d_polygon_contains_point(polya, p);
}

// compute a point that is inside the polygon. (It may not be *far* inside though)
void g2d_polygon_get_interior_point(const zarray_t *poly, float *p)
{
    // take the first three points, which form a triangle. Find the middle point
    float a[2], b[2], c[2];

    zarray_get(poly, 0, a);
    zarray_get(poly, 1, b);
    zarray_get(poly, 2, c);

    p[0] = (a[0]+b[0]+c[0])/3;
    p[1] = (a[1]+b[1]+c[1])/3;
}

int g2d_polygon_overlaps_polygon(const zarray_t *polya, const zarray_t *polyb)
{
    // do any of the line segments collide? If so, the answer is yes.
    if (g2d_polygon_intersects_polygon(polya, polyb))
        return 1;

    // if none of the edges cross, then the polygon is either fully
    // contained or fully outside.
    float p[2];
    g2d_polygon_get_interior_point(polyb, p);

    if (g2d_polygon_contains_point(polya, p))
        return 1;

    g2d_polygon_get_interior_point(polya, p);

    if (g2d_polygon_contains_point(polyb, p))
        return 1;

    return 0;
}

static int double_sort_up(const void *_a, const void *_b)
{
    float a = *((float*) _a);
    float b = *((float*) _b);

    if (a < b)
        return -1;

    if (a == b)
        return 0;

    return 1;
}

// Compute the crossings of the polygon along line y, storing them in
// the array x. X must be allocated to be at least as long as
// zarray_size(poly). X will be sorted, ready for
// rasterization. Returns the number of intersections (and elements
// written to x).
/*
  To rasterize, do something like this:

  float res = 0.099;
  for (float y = y0; y < y1; y += res) {
  float xs[zarray_size(poly)];

  int xsz = g2d_polygon_rasterize(poly, y, xs);
  int xpos = 0;
  int inout = 0; // start off "out"

  for (float x = x0; x < x1; x += res) {
      while (x > xs[xpos] && xpos < xsz) {
        xpos++;
        inout ^= 1;
      }

    if (inout)
       printf("y");
    else
       printf(" ");
  }
  printf("\n");
*/

// returns the number of x intercepts
int g2d_polygon_rasterize(const zarray_t *poly, float y, float *x)
{
    int sz = zarray_size(poly);

    g2d_line_t line;
    if (1) {
        float p0[2] = { 0, y };
        float p1[2] = { 1, y };

        g2d_line_init_from_points(&line, p0, p1);
    }

    int xpos = 0;

    for (int i = 0; i < sz; i++) {
        g2d_line_segment_t seg;
        float *p0, *p1;
        zarray_get_volatile(poly, i, &p0);
        zarray_get_volatile(poly, (i+1)%sz, &p1);

        g2d_line_segment_init_from_points(&seg, p0, p1);

        float q[2];
        if (g2d_line_segment_intersect_line(&seg, &line, q))
            x[xpos++] = q[0];
    }

    qsort(x, xpos, sizeof(float), double_sort_up);

    return xpos;
}

/*
  /---(1,5)
  (-2,4)-/        |
  \          |
  \        (1,2)--(2,2)\
  \                     \
  \                      \
  (0,0)------------------(4,0)
*/
#if 0

#include "timeprofile.h"

int main(int argc, char *argv[])
{
    timeprofile_t *tp = timeprofile_create();

    zarray_t *polya = g2d_polygon_create_data((float[][2]) {
            { 0, 0},
            { 4, 0},
            { 2, 2},
            { 1, 2},
            { 1, 5},
            { -2,4} }, 6);

    zarray_t *polyb = g2d_polygon_create_data((float[][2]) {
            { .1, .1},
            { .5, .1},
            { .1, .5 } }, 3);

    zarray_t *polyc = g2d_polygon_create_data((float[][2]) {
            { 3, 0},
            { 5, 0},
            { 5, 1} }, 3);

    zarray_t *polyd = g2d_polygon_create_data((float[][2]) {
            { 5, 5},
            { 6, 6},
            { 5, 6} }, 3);

/*
  5      L---K
  4      |I--J
  3      |H-G
  2      |E-F
  1      |D--C
  0      A---B
  01234
*/
    zarray_t *polyE = g2d_polygon_create_data((float[][2]) {
            {0,0}, {4,0}, {4, 1}, {1,1},
                                  {1,2}, {3,2}, {3,3}, {1,3},
                                                       {1,4}, {4,4}, {4,5}, {0,5}}, 12);

    srand(0);

    timeprofile_stamp(tp, "begin");

    if (1) {
        int niters = 100000;

        for (int i = 0; i < niters; i++) {
            float q[2];
            q[0] = 10.0f * random() / RAND_MAX - 2;
            q[1] = 10.0f * random() / RAND_MAX - 2;

            g2d_polygon_contains_point(polyE, q);
        }

        timeprofile_stamp(tp, "fast");

        for (int i = 0; i < niters; i++) {
            float q[2];
            q[0] = 10.0f * random() / RAND_MAX - 2;
            q[1] = 10.0f * random() / RAND_MAX - 2;

            g2d_polygon_contains_point_ref(polyE, q);
        }

        timeprofile_stamp(tp, "slow");

        for (int i = 0; i < niters; i++) {
            float q[2];
            q[0] = 10.0f * random() / RAND_MAX - 2;
            q[1] = 10.0f * random() / RAND_MAX - 2;

            int v0 = g2d_polygon_contains_point(polyE, q);
            int v1 = g2d_polygon_contains_point_ref(polyE, q);
            assert(v0 == v1);
        }

        timeprofile_stamp(tp, "both");
        timeprofile_display(tp);
    }

    if (1) {
        zarray_t *poly = polyE;

        float res = 0.399;
        for (float y = 5.2; y >= -.5; y -= res) {
            float xs[zarray_size(poly)];

            int xsz = g2d_polygon_rasterize(poly, y, xs);
            int xpos = 0;
            int inout = 0; // start off "out"
            for (float x = -3; x < 6; x += res) {
                while (x > xs[xpos] && xpos < xsz) {
                    xpos++;
                    inout ^= 1;
                }

                if (inout)
                    printf("y");
                else
                    printf(" ");
            }
            printf("\n");

            for (float x = -3; x < 6; x += res) {
                float q[2] = {x, y};
                if (g2d_polygon_contains_point(poly, q))
                    printf("X");
                else
                    printf(" ");
            }
            printf("\n");
        }
    }



/*
// CW order
float p[][2] =  { { 0, 0},
{ -2, 4},
{1, 5},
{1, 2},
{2, 2},
{4, 0} };
*/

     float q[2] = { 10, 10 };
     printf("0==%d\n", g2d_polygon_contains_point(polya, q));

     q[0] = 1; q[1] = 1;
     printf("1==%d\n", g2d_polygon_contains_point(polya, q));

     q[0] = 3; q[1] = .5;
     printf("1==%d\n", g2d_polygon_contains_point(polya, q));

     q[0] = 1.2; q[1] = 2.1;
     printf("0==%d\n", g2d_polygon_contains_point(polya, q));

     printf("0==%d\n", g2d_polygon_contains_polygon(polya, polyb));

     printf("0==%d\n", g2d_polygon_contains_polygon(polya, polyc));

     printf("0==%d\n", g2d_polygon_contains_polygon(polya, polyd));

     ////////////////////////////////////////////////////////
     // Test convex hull
     if (1) {
         zarray_t *hull = g2d_convex_hull(polyE);

         for (int k = 0; k < zarray_size(hull); k++) {
             float *h;
             zarray_get_volatile(hull, k, &h);

             printf("%15f, %15f\n", h[0], h[1]);
         }
     }

     for (int i = 0; i < 100000; i++) {
         zarray_t *points = zarray_create(sizeof(float[2]));

         for (int j = 0; j < 100; j++) {
             float q[2];
             q[0] = 10.0f * random() / RAND_MAX - 2;
             q[1] = 10.0f * random() / RAND_MAX - 2;

             zarray_add(points, q);
         }

         zarray_t *hull = g2d_convex_hull(points);
         for (int j = 0; j < zarray_size(points); j++) {
             float *q;
             zarray_get_volatile(points, j, &q);

             int on_edge;

             float p[2];
             g2d_polygon_closest_boundary_point(hull, q, p);
             if (g2d_distance(q, p) < .00001)
                 on_edge = 1;

             assert(on_edge || g2d_polygon_contains_point(hull, q));
         }

         zarray_destroy(hull);
         zarray_destroy(points);
     }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "image_types.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

// to support conversions between different types, we define all image
// types at once. Type-specific implementations can then #include this
// file, assured that the basic types of each image are known.

typedef struct image_u8 image_u8_t;
struct image_u8
{
    int32_t width;
    int32_t height;
    int32_t stride;

    uint8_t *buf;
};

typedef struct image_u8x3 image_u8x3_t;
struct image_u8x3
{
    const int32_t width;
    const int32_t height;
    const int32_t stride; // bytes per line

    uint8_t *buf;
};

typedef struct image_u8x4 image_u8x4_t;
struct image_u8x4
{
    const int32_t width;
    const int32_t height;
    const int32_t stride; // bytes per line

    uint8_t *buf;
};

typedef struct image_f32 image_f32_t;
struct image_f32
{
    const int32_t width;
    const int32_t height;
    const int32_t stride; // floats per line

    float *buf; // indexed as buf[y*stride + x]
};

typedef struct image_u32 image_u32_t;
struct image_u32
{
    const int32_t width;
    const int32_t height;
    const int32_t stride; // int32_ts per line

    uint32_t *buf; // indexed as buf[y*stride + x]
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "apriltag_math.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

// Computes the cholesky factorization of A, putting the lower
// triangular matrix into R.
static inline void mat33_chol(const float *A,
                              float *R)
{
    // A[0] = R[0]*R[0]
    R[0] = sqrt(A[0]);

    // A[1] = R[0]*R[3];
    R[3] = A[1] / R[0];

    // A[2] = R[0]*R[6];
    R[6] = A[2] / R[0];

    // A[4] = R[3]*R[3] + R[4]*R[4]
    R[4] = sqrt(A[4] - R[3]*R[3]);

    // A[5] = R[3]*R[6] + R[4]*R[7]
    R[7] = (A[5] - R[3]*R[6]) / R[4];

    // A[8] = R[6]*R[6] + R[7]*R[7] + R[8]*R[8]
    R[8] = sqrt(A[8] - R[6]*R[6] - R[7]*R[7]);

    R[1] = 0;
    R[2] = 0;
    R[5] = 0;
}

static inline void mat33_lower_tri_inv(const float *A,
                                       float *R)
{
    // A[0]*R[0] = 1
    R[0] = 1 / A[0];

    // A[3]*R[0] + A[4]*R[3] = 0
    R[3] = -A[3]*R[0] / A[4];

    // A[4]*R[4] = 1
    R[4] = 1 / A[4];

    // A[6]*R[0] + A[7]*R[3] + A[8]*R[6] = 0
    R[6] = (-A[6]*R[0] - A[7]*R[3]) / A[8];

    // A[7]*R[4] + A[8]*R[7] = 0
    R[7] = -A[7]*R[4] / A[8];

    // A[8]*R[8] = 1
    R[8] = 1 / A[8];
}


static inline void mat33_sym_solve(const float *A,
                                   const float *B,
                                   float *R)
{
    float L[9];
    mat33_chol(A, L);

    float M[9];
    mat33_lower_tri_inv(L, M);

    float tmp[3];
    tmp[0] = M[0]*B[0];
    tmp[1] = M[3]*B[0] + M[4]*B[1];
    tmp[2] = M[6]*B[0] + M[7]*B[1] + M[8]*B[2];

    R[0] = M[0]*tmp[0] + M[3]*tmp[1] + M[6]*tmp[2];
    R[1] = M[4]*tmp[1] + M[7]*tmp[2];
    R[2] = M[8]*tmp[2];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "apriltag.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

struct quad
{
    float p[4][2]; // corners

    // H: tag coordinates ([-1,1] at the black corners) to pixels
    // Hinv: pixels to tag
    matd_t *H, *Hinv;
};

// Represents a tag family. Every tag belongs to a tag family. Tag
// families are generated by the Java tool
// april.tag.TagFamilyGenerator and can be converted to C using
// april.tag.TagToC.
typedef struct apriltag_family apriltag_family_t;
struct apriltag_family
{
    // How many codes are there in this tag family?
    uint32_t ncodes;

    // how wide (in bit-sizes) is the black border? (usually 1)
    uint32_t black_border;

    // how many bits tall and wide is it? (e.g. 36bit tag ==> 6)
    uint32_t d;

    // minimum hamming distance between any two codes. (e.g. 36h11 => 11)
    uint32_t h;

    // The codes in the family.
    uint64_t codes[];
};

struct apriltag_quad_thresh_params
{
    // reject quads containing too few pixels
    int min_cluster_pixels;

    // how many corner candidates to consider when segmenting a group
    // of pixels into a quad.
    int max_nmaxima;

    // Reject quads where pairs of edges have angles that are close to
    // straight or close to 180 degrees. Zero means that no quads are
    // rejected. (In radians).
    float critical_rad;

    // When fitting lines to the contours, what is the maximum mean
    // squared error allowed?  This is useful in rejecting contours
    // that are far from being quad shaped; rejecting these quads "early"
    // saves expensive decoding processing.
    float max_line_fit_mse;

    // When we build our model of black & white pixels, we add an
    // extra check that the white model must be (overall) brighter
    // than the black model.  How much brighter? (in pixel values,
    // [0,255]). .
    int min_white_black_diff;

    // should the thresholded image be deglitched? Only useful for
    // very noisy images
    int deglitch;
};

// Represents a detector object. Upon creating a detector, all fields
// are set to reasonable values, but can be overridden by accessing
// these fields.
typedef struct apriltag_detector apriltag_detector_t;
struct apriltag_detector
{
    ///////////////////////////////////////////////////////////////
    // User-configurable parameters.

    // When non-zero, the edges of the each quad are adjusted to "snap
    // to" strong gradients nearby. This is useful when decimation is
    // employed, as it can increase the quality of the initial quad
    // estimate substantially. Generally recommended to be on (1).
    //
    // Very computationally inexpensive. Option is ignored if
    // quad_decimate = 1.
    int refine_edges;

    // when non-zero, detections are refined in a way intended to
    // increase the number of detected tags. Especially effective for
    // very small tags near the resolution threshold (e.g. 10px on a
    // side).
    int refine_decode;

    // when non-zero, detections are refined in a way intended to
    // increase the accuracy of the extracted pose. This is done by
    // maximizing the contrast around the black and white border of
    // the tag. This generally increases the number of successfully
    // detected tags, though not as effectively (or quickly) as
    // refine_decode.
    //
    // This option must be enabled in order for "goodness" to be
    // computed.
    int refine_pose;

    struct apriltag_quad_thresh_params qtp;

    ///////////////////////////////////////////////////////////////
    // Statistics relating to last processed frame

    uint32_t nedges;
    uint32_t nsegments;
    uint32_t nquads;

    ///////////////////////////////////////////////////////////////
    // Internal variables below

    // Not freed on apriltag_destroy; a tag family can be shared
    // between multiple users. The user should ultimately destroy the
    // tag family passed into the constructor.
    zarray_t *tag_families;
};

// Represents the detection of a tag. These are returned to the user
// and must be individually destroyed by the user.
typedef struct apriltag_detection apriltag_detection_t;
struct apriltag_detection
{
    // a pointer for convenience. not freed by apriltag_detection_destroy.
    apriltag_family_t *family;

    // The decoded ID of the tag
    int id;

    // How many error bits were corrected? Note: accepting large numbers of
    // corrected errors leads to greatly increased false positive rates.
    // NOTE: As of this implementation, the detector cannot detect tags with
    // a hamming distance greater than 2.
    int hamming;

    // A measure of the quality of tag localization: measures the
    // average contrast of the pixels around the border of the
    // tag. refine_pose must be enabled, or this field will be zero.
    float goodness;

    // A measure of the quality of the binary decoding process: the
    // average difference between the intensity of a data bit versus
    // the decision threshold. Higher numbers roughly indicate better
    // decodes. This is a reasonable measure of detection accuracy
    // only for very small tags-- not effective for larger tags (where
    // we could have sampled anywhere within a bit cell and still
    // gotten a good detection.)
    float decision_margin;

    // The 3x3 homography matrix describing the projection from an
    // "ideal" tag (with corners at (-1,-1), (1,-1), (1,1), and (-1,
    // 1)) to pixels in the image. This matrix will be freed by
    // apriltag_detection_destroy.
    matd_t *H;

    // The center of the detection in image pixel coordinates.
    float c[2];

    // The corners of the tag in image pixel coordinates. These always
    // wrap counter-clock wise around the tag.
    float p[4][2];
};

// don't forget to add a family!
apriltag_detector_t *apriltag_detector_create();

// add a family to the apriltag detector. caller still "owns" the family.
// a single instance should only be provided to one apriltag detector instance.
void apriltag_detector_add_family_bits(apriltag_detector_t *td, apriltag_family_t *fam, int bits_corrected);

// Tunable, but really, 2 is a good choice. Values of >=3
// consume prohibitively large amounts of memory, and otherwise
// you want the largest value possible.
static inline void apriltag_detector_add_family(apriltag_detector_t *td, apriltag_family_t *fam)
{
    apriltag_detector_add_family_bits(td, fam, 2);
}

// does not deallocate the family.
void apriltag_detector_remove_family(apriltag_detector_t *td, apriltag_family_t *fam);

// unregister all families, but does not deallocate the underlying tag family objects.
void apriltag_detector_clear_families(apriltag_detector_t *td);

// Destroy the april tag detector (but not the underlying
// apriltag_family_t used to initialize it.)
void apriltag_detector_destroy(apriltag_detector_t *td);

// Detect tags from an image and return an array of
// apriltag_detection_t*. You can use apriltag_detections_destroy to
// free the array and the detections it contains, or call
// _detection_destroy and zarray_destroy yourself.
zarray_t *apriltag_detector_detect(apriltag_detector_t *td, image_u8_t *im_orig);

// Call this method on each of the tags returned by apriltag_detector_detect
void apriltag_detection_destroy(apriltag_detection_t *det);

// destroys the array AND the detections within it.
void apriltag_detections_destroy(zarray_t *detections);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "tag16h5"
////////////////////////////////////////////////////////////////////////////////////////////////////

const apriltag_family_t tag16h5 = {
    .ncodes = 30,
    .black_border = 1,
    .d = 4,
    .h = 5,
    .codes = {
        0x000000000000231bUL,
        0x0000000000002ea5UL,
        0x000000000000346aUL,
        0x00000000000045b9UL,
        0x00000000000079a6UL,
        0x0000000000007f6bUL,
        0x000000000000b358UL,
        0x000000000000e745UL,
        0x000000000000fe59UL,
        0x000000000000156dUL,
        0x000000000000380bUL,
        0x000000000000f0abUL,
        0x0000000000000d84UL,
        0x0000000000004736UL,
        0x0000000000008c72UL,
        0x000000000000af10UL,
        0x000000000000093cUL,
        0x00000000000093b4UL,
        0x000000000000a503UL,
        0x000000000000468fUL,
        0x000000000000e137UL,
        0x0000000000005795UL,
        0x000000000000df42UL,
        0x0000000000001c1dUL,
        0x000000000000e9dcUL,
        0x00000000000073adUL,
        0x000000000000ad5fUL,
        0x000000000000d530UL,
        0x00000000000007caUL,
        0x000000000000af2eUL
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "tag25h7"
////////////////////////////////////////////////////////////////////////////////////////////////////

const apriltag_family_t tag25h7 = {
    .ncodes = 242,
    .black_border = 1,
    .d = 5,
    .h = 7,
    .codes = {
        0x00000000004b770dUL,
        0x00000000011693e6UL,
        0x0000000001a599abUL,
        0x0000000000c3a535UL,
        0x000000000152aafaUL,
        0x0000000000accd98UL,
        0x0000000001cad922UL,
        0x00000000002c2fadUL,
        0x0000000000bb3572UL,
        0x00000000014a3b37UL,
        0x000000000186524bUL,
        0x0000000000c99d4cUL,
        0x000000000023bfeaUL,
        0x000000000141cb74UL,
        0x0000000001d0d139UL,
        0x0000000001670aebUL,
        0x0000000000851675UL,
        0x000000000150334eUL,
        0x00000000006e3ed8UL,
        0x0000000000fd449dUL,
        0x0000000000aa55ecUL,
        0x0000000001c86176UL,
        0x00000000015e9b28UL,
        0x00000000007ca6b2UL,
        0x000000000147c38bUL,
        0x0000000001d6c950UL,
        0x00000000008b0e8cUL,
        0x00000000011a1451UL,
        0x0000000001562b65UL,
        0x00000000013f53c8UL,
        0x0000000000d58d7aUL,
        0x0000000000829ec9UL,
        0x0000000000faccf1UL,
        0x000000000136e405UL,
        0x00000000007a2f06UL,
        0x00000000010934cbUL,
        0x00000000016a8b56UL,
        0x0000000001a6a26aUL,
        0x0000000000f85545UL,
        0x000000000195c2e4UL,
        0x000000000024c8a9UL,
        0x00000000012bfc96UL,
        0x00000000016813aaUL,
        0x0000000001a42abeUL,
        0x0000000001573424UL,
        0x0000000001044573UL,
        0x0000000000b156c2UL,
        0x00000000005e6811UL,
        0x0000000001659bfeUL,
        0x0000000001d55a63UL,
        0x00000000005bf065UL,
        0x0000000000e28667UL,
        0x0000000001e9ba54UL,
        0x00000000017d7c5aUL,
        0x0000000001f5aa82UL,
        0x0000000001a2bbd1UL,
        0x00000000001ae9f9UL,
        0x0000000001259e51UL,
        0x000000000134062bUL,
        0x0000000000e1177aUL,
        0x0000000000ed07a8UL,
        0x000000000162be24UL,
        0x000000000059128bUL,
        0x0000000001663e8fUL,
        0x00000000001a83cbUL,
        0x000000000045bb59UL,
        0x000000000189065aUL,
        0x00000000004bb370UL,
        0x00000000016fb711UL,
        0x000000000122c077UL,
        0x0000000000eca17aUL,
        0x0000000000dbc1f4UL,
        0x000000000088d343UL,
        0x000000000058ac5dUL,
        0x0000000000ba02e8UL,
        0x00000000001a1d9dUL,
        0x0000000001c72eecUL,
        0x0000000000924bc5UL,
        0x0000000000dccab3UL,
        0x0000000000886d15UL,
        0x000000000178c965UL,
        0x00000000005bc69aUL,
        0x0000000001716261UL,
        0x000000000174e2ccUL,
        0x0000000001ed10f4UL,
        0x0000000000156aa8UL,
        0x00000000003e2a8aUL,
        0x00000000002752edUL,
        0x000000000153c651UL,
        0x0000000001741670UL,
        0x0000000000765b05UL,
        0x000000000119c0bbUL,
        0x000000000172a783UL,
        0x00000000004faca1UL,
        0x0000000000f31257UL,
        0x00000000012441fcUL,
        0x00000000000d3748UL,
        0x0000000000c21f15UL,
        0x0000000000ac5037UL,
        0x000000000180e592UL,
        0x00000000007d3210UL,
        0x0000000000a27187UL,
        0x00000000002beeafUL,
        0x000000000026ff57UL,
        0x0000000000690e82UL,
        0x000000000077765cUL,
        0x0000000001a9e1d7UL,
        0x000000000140be1aUL,
        0x0000000001aa1e3aUL,
        0x0000000001944f5cUL,
        0x00000000019b5032UL,
        0x0000000000169897UL,
        0x0000000001068eb9UL,
        0x0000000000f30dbcUL,
        0x000000000106a151UL,
        0x0000000001d53e95UL,
        0x0000000001348ceeUL,
        0x0000000000cf4fcaUL,
        0x0000000001728bb5UL,
        0x0000000000dc1eecUL,
        0x000000000069e8dbUL,
        0x00000000016e1523UL,
        0x000000000105fa25UL,
        0x00000000018abb0cUL,
        0x0000000000c4275dUL,
        0x00000000006d8e76UL,
        0x0000000000e8d6dbUL,
        0x0000000000e16fd7UL,
        0x0000000001ac2682UL,
        0x000000000077435bUL,
        0x0000000000a359ddUL,
        0x00000000003a9c4eUL,
        0x000000000123919aUL,
        0x0000000001e25817UL,
        0x000000000002a836UL,
        0x00000000001545a4UL,
        0x0000000001209c8dUL,
        0x0000000000bb5f69UL,
        0x0000000001dc1f02UL,
        0x00000000005d5f7eUL,
        0x00000000012d0581UL,
        0x00000000013786c2UL,
        0x0000000000e15409UL,
        0x0000000001aa3599UL,
        0x000000000139aad8UL,
        0x0000000000b09d2aUL,
        0x000000000054488fUL,
        0x00000000013c351cUL,
        0x0000000000976079UL,
        0x0000000000b25b12UL,
        0x0000000001addb34UL,
        0x0000000001cb23aeUL,
        0x0000000001175738UL,
        0x0000000001303bb8UL,
        0x0000000000d47716UL,
        0x000000000188ceeaUL,
        0x0000000000baf967UL,
        0x0000000001226d39UL,
        0x000000000135e99bUL,
        0x000000000034adc5UL,
        0x00000000002e384dUL,
        0x000000000090d3faUL,
        0x0000000000232713UL,
        0x00000000017d49b1UL,
        0x0000000000aa84d6UL,
        0x0000000000c2ddf8UL,
        0x0000000001665646UL,
        0x00000000004f345fUL,
        0x00000000002276b1UL,
        0x0000000001255dd7UL,
        0x00000000016f4cccUL,
        0x00000000004aaffcUL,
        0x0000000000c46da6UL,
        0x000000000085c7b3UL,
        0x0000000001311fcbUL,
        0x00000000009c6c4fUL,
        0x000000000187d947UL,
        0x00000000008578e4UL,
        0x0000000000e2bf0bUL,
        0x0000000000a01b4cUL,
        0x0000000000a1493bUL,
        0x00000000007ad766UL,
        0x0000000000ccfe82UL,
        0x0000000001981b5bUL,
        0x0000000001cacc85UL,
        0x0000000000562cdbUL,
        0x00000000015b0e78UL,
        0x00000000008f66c5UL,
        0x00000000003332bfUL,
        0x00000000012ce754UL,
        0x0000000000096a76UL,
        0x0000000001d5e3baUL,
        0x000000000027ea41UL,
        0x00000000014412dfUL,
        0x000000000067b9b4UL,
        0x0000000000daa51aUL,
        0x00000000001dcb17UL,
        0x00000000004d4afdUL,
        0x00000000006335d5UL,
        0x0000000000ee2334UL,
        0x00000000017d4e55UL,
        0x0000000001b8b0f0UL,
        0x00000000014999e3UL,
        0x0000000001513dfaUL,
        0x0000000000765cf2UL,
        0x000000000056af90UL,
        0x00000000012e16acUL,
        0x0000000001d3d86cUL,
        0x0000000000ff279bUL,
        0x00000000018822ddUL,
        0x000000000099d478UL,
        0x00000000008dc0d2UL,
        0x000000000034b666UL,
        0x0000000000cf9526UL,
        0x000000000186443dUL,
        0x00000000007a8e29UL,
        0x00000000019c6aa5UL,
        0x0000000001f2a27dUL,
        0x00000000012b2136UL,
        0x0000000000d0cd0dUL,
        0x00000000012cb320UL,
        0x00000000017ddb0bUL,
        0x000000000005353bUL,
        0x00000000015b2cafUL,
        0x0000000001e5a507UL,
        0x000000000120f1e5UL,
        0x000000000114605aUL,
        0x00000000014efe4cUL,
        0x0000000000568134UL,
        0x00000000011b9f92UL,
        0x000000000174d2a7UL,
        0x0000000000692b1dUL,
        0x000000000039e4feUL,
        0x0000000000aaff3dUL,
        0x000000000096224cUL,
        0x00000000013c9f77UL,
        0x000000000110ee8fUL,
        0x0000000000f17beaUL,
        0x000000000099fb5dUL,
        0x0000000000337141UL,
        0x000000000002b54dUL,
        0x0000000001233a70UL
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "tag25h9"
////////////////////////////////////////////////////////////////////////////////////////////////////

const apriltag_family_t tag25h9 = {
    .ncodes = 35,
    .black_border = 1,
    .d = 5,
    .h = 9,
    .codes = {
        0x000000000155cbf1UL,
        0x0000000001e4d1b6UL,
        0x00000000017b0b68UL,
        0x0000000001eac9cdUL,
        0x00000000012e14ceUL,
        0x00000000003548bbUL,
        0x00000000007757e6UL,
        0x0000000001065dabUL,
        0x0000000001baa2e7UL,
        0x0000000000dea688UL,
        0x000000000081d927UL,
        0x000000000051b241UL,
        0x0000000000dbc8aeUL,
        0x0000000001e50e19UL,
        0x00000000015819d2UL,
        0x00000000016d8282UL,
        0x000000000163e035UL,
        0x00000000009d9b81UL,
        0x000000000173eec4UL,
        0x0000000000ae3a09UL,
        0x00000000005f7c51UL,
        0x0000000001a137fcUL,
        0x0000000000dc9562UL,
        0x0000000001802e45UL,
        0x0000000001c3542cUL,
        0x0000000000870fa4UL,
        0x0000000000914709UL,
        0x00000000016684f0UL,
        0x0000000000c8f2a5UL,
        0x0000000000833ebbUL,
        0x000000000059717fUL,
        0x00000000013cd050UL,
        0x0000000000fa0ad1UL,
        0x0000000001b763b0UL,
        0x0000000000b991ceUL
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "tag36h10"
////////////////////////////////////////////////////////////////////////////////////////////////////

const apriltag_family_t tag36h10 = {
    .ncodes = 2320,
    .black_border = 1,
    .d = 6,
    .h = 10,
    .codes = {
        0x00000001ca92a687UL,
        0x000000020521ac4cUL,
        0x000000027a3fb7d6UL,
        0x00000002b4cebd9bUL,
        0x00000003647bceeaUL,
        0x000000039f0ad4afUL,
        0x00000003d999da74UL,
        0x000000044eb7e5feUL,
        0x0000000538f3fd12UL,
        0x00000005738302d7UL,
        0x000000065dbf19ebUL,
        0x000000070d6c2b3aUL,
        0x00000007f7a8424eUL,
        0x0000000832374813UL,
        0x000000086cc64dd8UL,
        0x00000008a755539dUL,
        0x00000009570264ecUL,
        0x0000000991916ab1UL,
        0x0000000a06af763bUL,
        0x0000000ab65c878aUL,
        0x0000000b2b7a9314UL,
        0x0000000b660998d9UL,
        0x0000000bdb27a463UL,
        0x0000000cc563bb77UL,
        0x0000000e24bdde15UL,
        0x0000000ed46aef64UL,
        0x0000000f4988faeeUL,
        0x000000006e5417c7UL,
        0x0000000158902edbUL,
        0x00000001cdae3a65UL,
        0x0000000242cc45efUL,
        0x000000027d5b4bb4UL,
        0x00000002b7ea5179UL,
        0x000000032d085d03UL,
        0x00000003679762c8UL,
        0x00000003a226688dUL,
        0x00000003dcb56e52UL,
        0x000000048c627fa1UL,
        0x00000005769e96b5UL,
        0x00000006264ba804UL,
        0x0000000660daadc9UL,
        0x00000006d5f8b953UL,
        0x000000074b16c4ddUL,
        0x00000007fac3d62cUL,
        0x000000091f8ef305UL,
        0x000000095a1df8caUL,
        0x0000000994acfe8fUL,
        0x0000000a09cb0a19UL,
        0x0000000a445a0fdeUL,
        0x0000000a7ee915a3UL,
        0x0000000ab9781b68UL,
        0x0000000af407212dUL,
        0x0000000b69252cb7UL,
        0x0000000c8df04990UL,
        0x0000000d3d9d5adfUL,
        0x0000000d782c60a4UL,
        0x0000000f12158907UL,
        0x00000001d0c9ce43UL,
        0x000000020b58d408UL,
        0x00000002f594eb1cUL,
        0x00000003a541fc6bUL,
        0x0000000454ef0dbaUL,
        0x000000053f2b24ceUL,
        0x0000000629673be2UL,
        0x000000074e3258bbUL,
        0x00000008ad8c7b59UL,
        0x00000009d2579832UL,
        0x0000000a8204a981UL,
        0x0000000af722b50bUL,
        0x0000000b6c40c095UL,
        0x0000000ba6cfc65aUL,
        0x0000000f15311ce5UL,
        0x00000000748b3f83UL,
        0x00000000af1a4548UL,
        0x00000000e9a94b0dUL,
        0x00000002be217935UL,
        0x00000003e2ec960eUL,
        0x00000004cd28ad22UL,
        0x0000000507b7b2e7UL,
        0x000000054246b8acUL,
        0x000000057cd5be71UL,
        0x00000006a1a0db4aUL,
        0x00000006dc2fe10fUL,
        0x0000000876190972UL,
        0x000000099ae4264bUL,
        0x0000000abfaf4324UL,
        0x0000000c9427714cUL,
        0x0000000d09457cd6UL,
        0x0000000d43d4829bUL,
        0x0000000ea32ea539UL,
        0x0000000f52dbb688UL,
        0x0000000161e2ea75UL,
        0x0000000286ae074eUL,
        0x000000066a2d6963UL,
        0x00000008b3c3a315UL,
        0x00000008ee52a8daUL,
        0x0000000a131dc5b3UL,
        0x0000000e6bbb3352UL,
        0x0000000f55f74a66UL,
        0x0000000005a45bb5UL,
        0x000000007ac2673fUL,
        0x00000001da1c89ddUL,
        0x0000000289c99b2cUL,
        0x00000003ae94b805UL,
        0x000000050deedaa3UL,
        0x00000005830ce62dUL,
        0x00000005bd9bebf2UL,
        0x0000000632b9f77cUL,
        0x00000006e26708cbUL,
        0x0000000841c12b69UL,
        0x000000092bfd427dUL,
        0x00000009668c4842UL,
        0x00000009dbaa53ccUL,
        0x0000000b007570a5UL,
        0x0000000b3b04766aUL,
        0x0000000c25408d7eUL,
        0x0000000ea965ccf5UL,
        0x0000000f93a1e409UL,
        0x00000000434ef558UL,
        0x00000001681a1231UL,
        0x00000001dd381dbbUL,
        0x0000000302033a94UL,
        0x000000075aa0a833UL,
        0x000000092f18d65bUL,
        0x00000009a436e1e5UL,
        0x0000000a1954ed6fUL,
        0x0000000b78af100dUL,
        0x0000000bb33e15d2UL,
        0x0000000c62eb2721UL,
        0x00000000466a8936UL,
        0x00000000f6179a85UL,
        0x000000016b35a60fUL,
        0x0000000589440de9UL,
        0x00000006738024fdUL,
        0x0000000847f85325UL,
        0x00000009e1e17b88UL,
        0x0000000acc1d929cUL,
        0x0000000b06ac9861UL,
        0x0000000d5042d213UL,
        0x0000000fd468118aUL,
        0x00000000f9332e63UL,
        0x0000000342c96815UL,
        0x000000037d586ddaUL,
        0x0000000551d09c02UL,
        0x00000005c6eea78cUL,
        0x00000006017dad51UL,
        0x00000009354ffe17UL,
        0x00000009aa6e09a1UL,
        0x0000000a94aa20b5UL,
        0x0000000acf39267aUL,
        0x0000000bb9753d8eUL,
        0x0000000bf4044353UL,
        0x000000008730b6b7UL,
        0x00000001716ccdcbUL,
        0x000000022119df1aUL,
        0x00000003f5920d42UL,
        0x000000058f7b35a5UL,
        0x00000006b446527eUL,
        0x0000000972fa97baUL,
        0x00000009e818a344UL,
        0x0000000a5d36aeceUL,
        0x0000000d1beaf40aUL,
        0x0000000dcb980559UL,
        0x0000000f65812dbcUL,
        0x0000000139f95be4UL,
        0x00000006b761e65cUL,
        0x00000006f1f0ec21UL,
        0x0000000a605242acUL,
        0x0000000e43d1a4c1UL,
        0x000000029c6f1260UL,
        0x0000000386ab2974UL,
        0x00000004e6054c12UL,
        0x00000008c984ae27UL,
        0x000000097931bf76UL,
        0x00000009ee4fcb00UL,
        0x0000000d22221bc6UL,
        0x0000000e46ed389fUL,
        0x0000000ebc0b4429UL,
        0x00000006f82813ddUL,
        0x0000000732b719a2UL,
        0x00000009072f47caUL,
        0x0000000941be4d8fUL,
        0x000000097c4d5354UL,
        0x00000009f16b5edeUL,
        0x0000000b8b548741UL,
        0x0000000d253dafa4UL,
        0x0000000d5fccb569UL,
        0x0000000d9a5bbb2eUL,
        0x0000000e4a08cc7dUL,
        0x00000003c77156f5UL,
        0x00000007aaf0b90aUL,
        0x0000000b53e1155aUL,
        0x0000000b8e701b1fUL,
        0x000000005c2b9448UL,
        0x000000014667ab5cUL,
        0x000000047a39fc22UL,
        0x00000004ef5807acUL,
        0x000000064eb22a4aUL,
        0x0000000982847b10UL,
        0x0000000aa74f97e9UL,
        0x0000000ae1de9daeUL,
        0x0000000b56fca938UL,
        0x0000000f750b1112UL,
        0x00000001bea14ac4UL,
        0x00000001f9305089UL,
        0x0000000233bf564eUL,
        0x000000031dfb6d62UL,
        0x00000003931978ecUL,
        0x000000052d02a14fUL,
        0x00000005a220acd9UL,
        0x0000000b1f893751UL,
        0x0000000fb2b5aab5UL,
        0x000000040b531854UL,
        0x00000005a53c40b7UL,
        0x00000008d90e917dUL,
        0x00000009139d9742UL,
        0x000000094e2c9d07UL,
        0x0000000d6c3b04e1UL,
        0x0000000e910621baUL,
        0x0000000f40b33309UL,
        0x00000001152b6131UL,
        0x000000032432951eUL,
        0x00000003d3dfa66dUL,
        0x000000065804e5e4UL,
        0x0000000ab0a25383UL,
        0x0000000b604f64d2UL,
        0x0000000de474a449UL,
        0x000000011846f50fUL,
        0x00000006d03e854cUL,
        0x00000007455c90d6UL,
        0x0000000ab3bde761UL,
        0x0000000dad013262UL,
        0x0000000e973d4976UL,
        0x00000002b54bb150UL,
        0x00000009577f58a1UL,
        0x00000009920e5e66UL,
        0x0000000b66868c8eUL,
        0x0000000f4a05eea3UL,
        0x00000003dd326207UL,
        0x00000005b1aa902fUL,
        0x000000099529f244UL,
        0x0000000b2f131aa7UL,
        0x0000000d038b48cfUL,
        0x0000000d3e1a4e94UL,
        0x000000024664cd82UL,
        0x000000036b2fea5bUL,
        0x000000095db6805dUL,
        0x0000000a0d6391acUL,
        0x0000000abd10a2fbUL,
        0x000000015f444a4cUL,
        0x00000002be9e6ceaUL,
        0x000000057d52b226UL,
        0x00000005f270bdb0UL,
        0x0000000b6fd94828UL,
        0x0000000879b19105UL,
        0x0000000d476d0a2eUL,
        0x0000000e6c382707UL,
        0x0000000dbfa6a996UL,
        0x00000001689705e6UL,
        0x00000003b22d3f98UL,
        0x0000000636527f0fUL,
        0x00000007d03ba772UL,
        0x0000000ee78d5a4dUL,
        0x0000000bf165a32aUL,
        0x0000000c2bf4a8efUL,
        0x0000000517be89f2UL,
        0x000000067718ac90UL,
        0x00000006b1a7b255UL,
        0x0000000726c5bddfUL,
        0x0000000bb9f23143UL,
        0x00000001375abbbbUL,
        0x0000000296b4de59UL,
        0x00000008893b745bUL,
        0x0000000a9842a848UL,
        0x0000000b827ebf5cUL,
        0x00000003840c894bUL,
        0x00000006b7deda11UL,
        0x0000000bc02958ffUL,
        0x000000055ba04b51UL,
        0x000000076aa77f3eUL,
        0x00000009b43db8f0UL,
        0x00000009eeccbeb5UL,
        0x0000000a295bc47aUL,
        0x0000000b4e26e153UL,
        0x0000000e476a2c54UL,
        0x00000006be1601cdUL,
        0x00000006f8a50792UL,
        0x000000097cca4709UL,
        0x0000000bc66080bbUL,
        0x00000001093a056eUL,
        0x00000006fbc09b70UL,
        0x0000000b8eed0ed4UL,
        0x0000000cee473172UL,
        0x000000023120b625UL,
        0x00000005da111275UL,
        0x0000000cf162c550UL,
        0x0000000ec8f68756UL,
        0x0000000b5db0c4a9UL,
        0x00000002b2ad1127UL,
        0x0000000536d2509eUL,
        0x00000009c9fec402UL,
        0x0000000c1394fdb4UL,
        0x000000006c326b53UL,
        0x00000005e99af5cbUL,
        0x0000000af1e574b9UL,
        0x0000000e6046cb44UL,
        0x0000000661d49533UL,
        0x00000008e5f9d4aaUL,
        0x0000000db3b54dd3UL,
        0x0000000e63625f22UL,
        0x0000000e9df164e7UL,
        0x0000000455e8f524UL,
        0x00000005b54317c2UL,
        0x0000000be258b389UL,
        0x000000054340a016UL,
        0x00000005b85eaba0UL,
        0x00000001284dcc1aUL,
        0x000000024d18e8f3UL,
        0x00000004d13e286aUL,
        0x00000008b4bd8a7fUL,
        0x0000000215a5770cUL,
        0x000000046572d87aUL,
        0x0000000c2c719ca4UL,
        0x00000004ddac77e2UL,
        0x0000000d19c94796UL,
        0x00000002d1c0d7d3UL,
        0x00000009ae8384e9UL,
        0x00000009e9128aaeUL,
        0x0000000ca7c6cfeaUL,
        0x0000000016282675UL,
        0x0000000ad985c97eUL,
        0x00000004af8bc195UL,
        0x00000009f580da26UL,
        0x0000000a6a9ee5b0UL,
        0x0000000bc9f9084eUL,
        0x0000000d63e230b1UL,
        0x0000000c4232a7b6UL,
        0x0000000d66fdc48fUL,
        0x0000000ec657e72dUL,
        0x0000000a364707a7UL,
        0x0000000f79208c5aUL,
        0x0000000de88a1f91UL,
        0x0000000574f9ddf6UL,
        0x000000065f35f50aUL,
        0x000000069ce08eadUL,
        0x0000000490f4ee9eUL,
        0x0000000c9282b88dUL,
        0x0000000752c4c7b8UL,
        0x0000000b364429cdUL,
        0x00000008b53a7e34UL,
        0x0000000be90ccefaUL,
        0x0000000b0507dfa2UL,
        0x000000000d525e90UL,
        0x00000005c549eecdUL,
        0x0000000e3bf5c446UL,
        0x0000000936c6d936UL,
        0x00000009747172d9UL,
        0x0000000ca843c39fUL,
        0x0000000d57f0d4eeUL,
        0x00000002d5595f66UL,
        0x0000000bfbb2462eUL,
        0x0000000266727b98UL,
        0x00000007ac679429UL,
        0x000000026fc53732UL,
        0x0000000656602d25UL,
        0x00000002eb1a6a78UL,
        0x00000004850392dbUL,
        0x0000000e5b098af2UL,
        0x0000000ab534c280UL,
        0x00000009ce143f4aUL,
        0x0000000f4b7cc9c2UL,
        0x0000000035b8e0d6UL,
        0x0000000871d5b08aUL,
        0x00000005b958930aUL,
        0x00000000b429a7faUL,
        0x000000054d8d431aUL,
        0x00000007d1b28291UL,
        0x0000000a1e645021UL,
        0x0000000b80da069dUL,
        0x0000000eef3b5d28UL,
        0x0000000263d3db6fUL,
        0x000000009592d503UL,
        0x00000004b9d86499UL,
        0x00000006c8df9886UL,
        0x0000000a3740ef11UL,
        0x0000000c4963b6dcUL,
        0x000000006da94672UL,
        0x000000053b64bf9bUL,
        0x0000000b2deb559dUL,
        0x0000000f116ab7b2UL,
        0x00000008ace1aa04UL,
        0x00000008ea8c43a7UL,
        0x00000006a4119dd3UL,
        0x000000099d54e8d4UL,
        0x0000000c969833d5UL,
        0x0000000f554c7911UL,
        0x00000003ade9e6b0UL,
        0x00000006e1bc3776UL,
        0x00000007916948c5UL,
        0x0000000dbe7ee48cUL,
        0x000000079484dca3UL,
        0x0000000f992e3a70UL,
        0x0000000884f81b73UL,
        0x0000000c68777d88UL,
        0x0000000603ee6fdaUL,
        0x0000000728b98cb3UL,
        0x0000000b12701684UL,
        0x0000000d5f21e414UL,
        0x0000000058652f15UL,
        0x00000002dc8a6e8cUL,
        0x00000004767396efUL,
        0x0000000b8dc549caUL,
        0x0000000f36b5a61aUL,
        0x00000000d09ece7dUL,
        0x0000000dda77175aUL,
        0x000000005e9c56d1UL,
        0x000000073e7a97c5UL,
        0x0000000b21f9f9daUL,
        0x0000000de3c9d2f4UL,
        0x000000069504ae32UL,
        0x000000077f40c546UL,
        0x0000000ed1217de6UL,
        0x00000003a1f88aedUL,
        0x0000000e623a9a18UL,
        0x00000000aeec67a8UL,
        0x0000000bea83aa19UL,
        0x000000092eeaf8bbUL,
        0x0000000a5d08d12eUL,
        0x0000000819a9bf38UL,
        0x0000000473d4f6c6UL,
        0x0000000b192431f5UL,
        0x0000000a6c92b484UL,
        0x00000007046885b5UL,
        0x0000000b9ab08cf7UL,
        0x0000000782d94cd9UL,
        0x0000000f158032faUL,
        0x0000000077f5e976UL,
        0x000000012dda2281UL,
        0x0000000e72417123UL,
        0x00000003056de487UL,
        0x0000000e3de9931aUL,
        0x0000000eb3079ea4UL,
        0x0000000e4420bad6UL,
        0x0000000439c2e4b6UL,
        0x000000047da4a615UL,
        0x00000000d7cfdda3UL,
        0x000000056afc5107UL,
        0x0000000e978c5f8bUL,
        0x00000005aede1266UL,
        0x0000000af1b79719UL,
        0x0000000f8b1b3239UL,
        0x000000075e8845dbUL,
        0x0000000bf1b4b93fUL,
        0x0000000fd5341b54UL,
        0x0000000a2373b2d3UL,
        0x00000005967e672bUL,
        0x0000000a2cc66e6dUL,
        0x0000000b17028581UL,
        0x0000000b54ad1f24UL,
        0x0000000e91d22b84UL,
        0x0000000de85c41f1UL,
        0x000000053d588e6fUL,
        0x0000000e9e407afcUL,
        0x0000000fc6272bb3UL,
        0x0000000a8ca0629aUL,
        0x0000000b86665d04UL,
        0x000000005a58fde9UL,
        0x00000001855b427eUL,
        0x0000000aabb42946UL,
        0x0000000e204ca78dUL,
        0x000000032897267bUL,
        0x00000000a78d7ae2UL,
        0x000000096536a598UL,
        0x0000000bf2aea0a9UL,
        0x00000000c9bcd56cUL,
        0x000000081eb921eaUL,
        0x00000002732fe125UL,
        0x00000002eb69808dUL,
        0x000000061f3bd153UL,
        0x00000008ddf0168fUL,
        0x0000000921d1d7eeUL,
        0x00000002bd48ca40UL,
        0x000000083ab154b8UL,
        0x00000005f436aee4UL,
        0x000000093dca0abcUL,
        0x000000026d75ad1eUL,
        0x0000000872a1ba54UL,
        0x0000000373a9f700UL,
        0x000000050d931f63UL,
        0x000000012d2f512cUL,
        0x0000000c6efdbb59UL,
        0x000000088e99ed22UL,
        0x0000000903b7f8acUL,
        0x0000000a9da1210fUL,
        0x0000000a2eba3d41UL,
        0x0000000fe9cd615cUL,
        0x0000000fb8911731UL,
        0x00000001cab3defcUL,
        0x0000000e6289b02dUL,
        0x000000066d6a35b6UL,
        0x0000000b0096a91aUL,
        0x0000000c9afcc532UL,
        0x000000080aebe5acUL,
        0x0000000d2f2e9768UL,
        0x0000000cc67edb56UL,
        0x0000000a51e37f35UL,
        0x00000006ac0eb6c3UL,
        0x00000006af2a4aa1UL,
        0x0000000e76290ecbUL,
        0x000000037e738db9UL,
        0x000000072d9b11c5UL,
        0x000000076e613f46UL,
        0x00000004f073278bUL,
        0x00000000e1eea307UL,
        0x0000000e9e8f9111UL,
        0x00000000793ee6f5UL,
        0x000000017304e15fUL,
        0x00000007a3361104UL,
        0x0000000731339958UL,
        0x00000008daa6a511UL,
        0x0000000a4037ef6bUL,
        0x0000000210896f2fUL,
        0x0000000afc535032UL,
        0x0000000e3025a0f8UL,
        0x000000063e21ba5fUL,
        0x00000003ebb5b8c8UL,
        0x0000000f9c6b06c3UL,
        0x0000000ca95ee37eUL,
        0x000000081f852bb4UL,
        0x0000000d6895d823UL,
        0x00000007040cca75UL,
        0x00000004d66ec391UL,
        0x00000004a216e588UL,
        0x000000051d6c18ceUL,
        0x000000047711c319UL,
        0x00000006ae7f794cUL,
        0x00000004abe694d7UL,
        0x0000000bc96f6f6eUL,
        0x000000057aa76cd2UL,
        0x0000000f948f2648UL,
        0x000000031bcd1bc3UL,
        0x000000094c7b3f1dUL,
        0x000000032eef86acUL,
        0x0000000668f8ff2eUL,
        0x00000006de170ab8UL,
        0x00000009341b93e2UL,
        0x0000000974e1c163UL,
        0x000000073182af6dUL,
        0x00000005d85fb48bUL,
        0x000000078b257bdeUL,
        0x0000000173d0eb29UL,
        0x00000005add785d1UL,
        0x0000000af6e83240UL,
        0x0000000dce79166cUL,
        0x000000022716840bUL,
        0x00000007b408f1d9UL,
        0x0000000de43a217eUL,
        0x000000036e10fb6eUL,
        0x0000000ea9a83ddfUL,
        0x00000004dcf50162UL,
        0x00000009aab07a8bUL,
        0x0000000281364431UL,
        0x00000008392dd46eUL,
        0x00000000945e6aceUL,
        0x00000008d07b3a82UL,
        0x0000000d012f1990UL,
        0x0000000b7098acc7UL,
        0x0000000c2fcfa16cUL,
        0x00000001e7c731a9UL,
        0x0000000f16ea68eeUL,
        0x00000002fa69cb03UL,
        0x00000004cee1f92bUL,
        0x00000001e20cfda2UL,
        0x00000009e9d1ef4dUL,
        0x0000000e83358a6dUL,
        0x000000059a873d48UL,
        0x0000000a6842b671UL,
        0x00000006885bdbefUL,
        0x000000073e4014faUL,
        0x00000007b9954840UL,
        0x0000000548157ffdUL,
        0x00000008853a8c5dUL,
        0x0000000eb8874fe0UL,
        0x0000000b25d4f257UL,
        0x000000036b447da5UL,
        0x000000071d87958fUL,
        0x0000000eedd91553UL,
        0x00000004af23612aUL,
        0x0000000278329eacUL,
        0x0000000191121b76UL,
        0x00000006e691175dUL,
        0x00000000bd140329UL,
        0x00000006ed4532ceUL,
        0x0000000d5e3c8ff4UL,
        0x0000000e26c64033UL,
        0x0000000494a2097bUL,
        0x0000000f5e36d440UL,
        0x0000000b7818d202UL,
        0x0000000a63548c34UL,
        0x0000000682f0bdfdUL,
        0x00000003d3c65c17UL,
        0x00000004c4399ae7UL,
        0x00000006b18e67ffUL,
        0x00000004778211a3UL,
        0x00000004089b2dd5UL,
        0x000000021edee850UL,
        0x0000000739cede72UL,
        0x0000000858dfc744UL,
        0x00000004bc5dba6cUL,
        0x000000021cbd3bdcUL,
        0x0000000ac83de313UL,
        0x00000006135f08daUL,
        0x0000000d3a3a9f0bUL,
        0x0000000eb2716099UL,
        0x000000040b88e413UL,
        0x0000000992442a25UL,
        0x0000000c639de695UL,
        0x0000000683bcc7c7UL,
        0x00000006245fc74fUL,
        0x0000000543766bd5UL,
        0x0000000375356569UL,
        0x0000000fa45b7a88UL,
        0x00000009f29b1207UL,
        0x0000000ba245457cUL,
        0x00000005b98e5ec9UL,
        0x0000000204aca6b6UL,
        0x0000000d52e9605bUL,
        0x0000000504a40d28UL,
        0x0000000ab6e1695eUL,
        0x0000000a26481ebbUL,
        0x00000009455ec341UL,
        0x00000002f05f98e9UL,
        0x0000000ead83365cUL,
        0x0000000b928d8486UL,
        0x00000007b860de0bUL,
        0x0000000964ef7da2UL,
        0x00000002422962b9UL,
        0x000000028f5ddfb2UL,
        0x00000008c5c63713UL,
        0x00000009068c6494UL,
        0x000000050aad5744UL,
        0x000000093e7cca30UL,
        0x00000009825e8b8fUL,
        0x00000003a8b4947dUL,
        0x0000000fa06737b5UL,
        0x0000000cb9c963e8UL,
        0x000000091a2bc332UL,
        0x0000000284666b59UL,
        0x0000000ceb82a1c8UL,
        0x0000000a2fe9f06aUL,
        0x000000006fa50365UL,
        0x000000019aa747faUL,
        0x0000000a6e117dc2UL,
        0x00000002b3810910UL,
        0x0000000ff7e857b2UL,
        0x0000000048b55c3eUL,
        0x0000000975456ac2UL,
        0x0000000ad200ed37UL,
        0x0000000e4d4d86efUL,
        0x0000000b3ec62491UL,
        0x00000008cd465c4eUL,
        0x00000008a2be2d94UL,
        0x00000005a9f7d648UL,
        0x000000059e067a85UL,
        0x0000000b5c35327eUL,
        0x0000000c4ca8714eUL,
        0x00000000e927a04cUL,
        0x0000000f71eac628UL,
        0x0000000109354e62UL,
        0x00000001037b1a5bUL,
        0x000000026c27f893UL,
        0x00000003597fa385UL,
        0x0000000ee24b62efUL,
        0x00000004b31f921cUL,
        0x0000000650244e5dUL,
        0x0000000cfec64526UL,
        0x0000000cd4bb0a21UL,
        0x0000000648c56197UL,
        0x0000000bd95056f8UL,
        0x0000000983c8c183UL,
        0x0000000ddc662f22UL,
        0x00000005631bb980UL,
        0x00000001203f56f3UL,
        0x00000006a0c37549UL,
        0x0000000a59baa8a4UL,
        0x00000006a23a5068UL,
        0x0000000ad609c354UL,
        0x0000000f6f6d5e74UL,
        0x000000036bc95f79UL,
        0x0000000424c92c62UL,
        0x00000003692abf50UL,
        0x00000000a4bc460dUL,
        0x00000001b4434b89UL,
        0x00000004eb31302dUL,
        0x00000009a3a891f9UL,
        0x000000093af8d5e7UL,
        0x0000000ef60bfa02UL,
        0x0000000607a378d6UL,
        0x00000005a5a7d835UL,
        0x0000000536c0f467UL,
        0x000000011f66a7feUL,
        0x000000074c7c43c5UL,
        0x000000066eae7c29UL,
        0x0000000f5a785d2cUL,
        0x0000000d3948a5c0UL,
        0x0000000ec1094aa4UL,
        0x0000000fcad61c19UL,
        0x000000049ca7108aUL,
        0x000000077437f4b6UL,
        0x0000000553083d4aUL,
        0x00000005c26c14cdUL,
        0x00000006eb4caceeUL,
        0x0000000e8aded63cUL,
        0x0000000132aa4b5cUL,
        0x000000057603a19eUL,
        0x0000000ee359dda3UL,
        0x0000000d0f5ea330UL,
        0x000000046b0f0b1fUL,
        0x0000000d47cbfc81UL,
        0x00000003ed1b37b0UL,
        0x00000004c8c752d8UL,
        0x0000000ac202044bUL,
        0x0000000207f16128UL,
        0x000000053f5c3981UL,
        0x000000070e1a33a2UL,
        0x0000000ed8348baaUL,
        0x0000000798f94a3eUL,
        0x00000008896c890eUL,
        0x0000000521425a3fUL,
        0x0000000c8329e9eaUL,
        0x000000041f238ba5UL,
        0x00000001093d3c81UL,
        0x0000000cab628e90UL,
        0x0000000a1b4bf356UL,
        0x000000092eee2fceUL,
        0x000000059d35b9afUL,
        0x00000002176e9f53UL,
        0x00000007c9abfb89UL,
        0x0000000b06d107e9UL,
        0x0000000e94c318d5UL,
        0x00000002f397ae30UL,
        0x0000000d7e5a1a48UL,
        0x000000098c4abc47UL,
        0x0000000574737c29UL,
        0x0000000d7f5401b2UL,
        0x0000000852b87bc6UL,
        0x00000001180fa957UL,
        0x0000000501c63328UL,
        0x0000000fd28c0d13UL,
        0x0000000f764aa079UL,
        0x0000000c8a6f8c5aUL,
        0x0000000975b10240UL,
        0x00000006bd33e4c0UL,
        0x0000000a1113e39cUL,
        0x0000000abcfa8fb8UL,
        0x0000000149ea1facUL,
        0x0000000f6443556fUL,
        0x0000000959da07e7UL,
        0x00000004721a2ac4UL,
        0x000000030bde0f15UL,
        0x0000000c7c4fdef8UL,
        0x0000000b7feb4465UL,
        0x000000056675073cUL,
        0x000000096f3f57b9UL,
        0x0000000876f0386eUL,
        0x0000000393f0a7e8UL,
        0x000000032b40ebd6UL,
        0x000000010a11346aUL,
        0x0000000fb81f48aeUL,
        0x00000000a892877eUL,
        0x000000086f636e08UL,
        0x00000004fbc4d72bUL,
        0x0000000561d5f314UL,
        0x0000000ca18e2835UL,
        0x00000007e6f519f5UL,
        0x00000008b94e7983UL,
        0x0000000619adfaf3UL,
        0x00000004c9ddbbabUL,
        0x0000000cb6a461f2UL,
        0x00000000f6e22456UL,
        0x0000000858c9b401UL,
        0x000000092dc1b3b8UL,
        0x00000003a783615bUL,
        0x0000000c74b66f67UL,
        0x0000000ea90891bcUL,
        0x000000031a829e4bUL,
        0x00000007bd38f505UL,
        0x0000000b4476ea80UL,
        0x00000001af371feaUL,
        0x000000084894ff56UL,
        0x00000000537584dfUL,
        0x0000000d4ebdd1d0UL,
        0x0000000b43cc192bUL,
        0x000000076700d287UL,
        0x0000000aaad9fa58UL,
        0x0000000fa511710fUL,
        0x00000006e54699e5UL,
        0x00000006d73391aeUL,
        0x00000003c2f1fb49UL,
        0x00000004fbd96a75UL,
        0x0000000252e6304bUL,
        0x0000000e72826214UL,
        0x00000008fe6c9336UL,
        0x0000000326397743UL,
        0x00000000e03bc524UL,
        0x0000000dedac9594UL,
        0x000000004ff19096UL,
        0x0000000409b4cdbbUL,
        0x0000000b15921888UL,
        0x0000000a259bcd6dUL,
        0x000000043c67f305UL,
        0x00000001dc65dcecUL,
        0x00000001730b4f85UL,
        0x0000000f8a48f16aUL,
        0x000000057a30e743UL,
        0x000000005afd9839UL,
        0x0000000682d5f3aeUL,
        0x00000000b694337eUL,
        0x0000000758c7dacfUL,
        0x000000018fa1ae7dUL,
        0x00000005ba9b5984UL,
        0x000000032e1d45ddUL,
        0x0000000672f05518UL,
        0x0000000382ffc5b1UL,
        0x00000008f0b08f33UL,
        0x0000000b4a4d9ff0UL,
        0x000000053ba0f980UL,
        0x00000002ac0751fbUL,
        0x0000000ab005de73UL,
        0x000000061ab7be9bUL,
        0x000000063078c9adUL,
        0x000000027659d148UL,
        0x0000000c653c684fUL,
        0x0000000ecee05017UL,
        0x000000024378ce5eUL,
        0x0000000d0f7e5bacUL,
        0x00000008b4bf4199UL,
        0x0000000b7aa495fbUL,
        0x0000000f3d6b78a5UL,
        0x000000098bab1024UL,
        0x00000006b4971fadUL,
        0x00000000723d6c89UL,
        0x0000000a17071a75UL,
        0x0000000d55a301f4UL,
        0x0000000988de925bUL,
        0x00000007ca276f45UL,
        0x0000000321b6e484UL,
        0x0000000316149ed6UL,
        0x0000000b8dba5bb9UL,
        0x0000000aad4df3f4UL,
        0x0000000178e204f5UL,
        0x000000095cd2e357UL,
        0x000000073fb8a733UL,
        0x000000084ca10c86UL,
        0x000000089498492dUL,
        0x00000005b9bdf383UL,
        0x000000066c58bb10UL,
        0x0000000f153ac21eUL,
        0x00000007ca5d3b04UL,
        0x0000000637a521c7UL,
        0x0000000b5ed589c1UL,
        0x0000000d0a3c644eUL,
        0x00000003d5d0754fUL,
        0x0000000c6b901174UL,
        0x00000003e96fd3edUL,
        0x000000079c2fdf8cUL,
        0x00000006594ae371UL,
        0x00000003504fd77aUL,
        0x000000032b81dcc7UL,
        0x000000057b4f3e35UL,
        0x00000004c9808072UL,
        0x0000000a06c10993UL,
        0x0000000b059666afUL,
        0x000000072b69c034UL,
        0x0000000e33ae836eUL,
        0x0000000ad6cadf0dUL,
        0x000000019573ace1UL,
        0x0000000d562fd1e7UL,
        0x0000000847804d9dUL,
        0x00000009e32f6734UL,
        0x00000002355c580fUL,
        0x00000002f177b8d6UL,
        0x0000000d689ac650UL,
        0x0000000071b70abcUL,
        0x0000000254915730UL,
        0x0000000c5934f949UL,
        0x0000000134d59d09UL,
        0x00000002c731fb06UL,
        0x0000000d90c6c5cbUL,
        0x0000000cc3f9bca4UL,
        0x0000000bf078980cUL,
        0x000000080838e95aUL,
        0x00000005ccd6f054UL,
        0x00000000378bae56UL,
        0x00000008d1ddb978UL,
        0x000000093b875cf4UL,
        0x0000000e2ce90bc6UL,
        0x0000000ec291b91bUL,
        0x0000000d0a11bdc1UL,
        0x0000000751be7244UL,
        0x0000000579fcd29eUL,
        0x0000000f76e5ccb1UL,
        0x0000000eb33da184UL,
        0x0000000c0ac75b0fUL,
        0x000000015454fb33UL,
        0x00000008b92a411cUL,
        0x00000007da34b476UL,
        0x00000004f413d45eUL,
        0x000000010ec1dbeaUL,
        0x0000000650739b93UL,
        0x0000000315e08a31UL,
        0x0000000d4fd5f1bdUL,
        0x0000000b5058a126UL,
        0x000000020d5cb63bUL,
        0x0000000c1598dfe7UL,
        0x0000000a0a2a338dUL,
        0x0000000e29af7686UL,
        0x000000083fd0cac9UL,
        0x000000014a8094d8UL,
        0x0000000816e0afa3UL,
        0x0000000499ef5d2cUL,
        0x0000000bddbd0d95UL,
        0x0000000a30839ca9UL,
        0x00000004fd33fb4cUL,
        0x0000000ef63557b7UL,
        0x0000000535f06ab2UL,
        0x00000000d47d352eUL,
        0x0000000a371e6dc4UL,
        0x00000008ac914b17UL,
        0x00000006ba9d69d7UL,
        0x0000000096da89aaUL,
        0x000000065bbd5d14UL,
        0x0000000d41d2c5c4UL,
        0x000000052a283583UL,
        0x00000004c1f56d26UL,
        0x000000086cd9984aUL,
        0x000000026cbcedc6UL,
        0x00000001aa0eaa03UL,
        0x0000000b95d5ad2cUL,
        0x0000000e2eb56b20UL,
        0x0000000ff49d9d5cUL,
        0x0000000cce338378UL,
        0x0000000330e9fac7UL,
        0x0000000e2f53974aUL,
        0x0000000668d1c6d5UL,
        0x0000000eca0ba751UL,
        0x00000008d48ab5e6UL,
        0x0000000d205e18cdUL,
        0x00000001c391633cUL,
        0x0000000ef5d02e5fUL,
        0x0000000d12bb5f20UL,
        0x0000000323215199UL,
        0x000000088f5b3ffcUL,
        0x0000000931445f29UL,
        0x0000000b893cb727UL,
        0x000000032851ecc0UL,
        0x000000080b44d81bUL,
        0x00000005aa48da98UL,
        0x000000046d1e1284UL,
        0x00000004c837ba14UL,
        0x0000000eb22c26deUL,
        0x0000000e51e9d246UL,
        0x00000008d03deee6UL,
        0x00000005af8e0909UL,
        0x0000000bde9773a4UL,
        0x0000000bf611cabfUL,
        0x0000000d24ac96e7UL,
        0x00000009fe919318UL,
        0x000000050d0206a6UL,
        0x0000000b43b9741cUL,
        0x0000000ba48d4fb3UL,
        0x00000006bccd7290UL,
        0x00000008bc6bfb9cUL,
        0x0000000e5a036c9fUL,
        0x0000000a80a2cfeeUL,
        0x0000000c193655a7UL,
        0x00000007c8e5170dUL,
        0x00000006141edbbbUL,
        0x00000004d6b990dcUL,
        0x0000000cc49b5702UL,
        0x00000002343fef58UL,
        0x0000000d50cb593cUL,
        0x00000004248a60cdUL,
        0x0000000901cfbd4cUL,
        0x000000064a4c8736UL,
        0x00000001b2dcbaeaUL,
        0x0000000d691e5f4cUL,
        0x0000000df352a493UL,
        0x00000001991ac7daUL,
        0x00000004c4879f45UL,
        0x00000009b34aadeeUL,
        0x000000052bb3db0dUL,
        0x00000007b9a8c9d3UL,
        0x0000000d7ce6e47eUL,
        0x0000000ec0b922d8UL,
        0x00000008079cab6bUL,
        0x0000000abadc8899UL,
        0x00000000f57b93b7UL,
        0x000000005c4ef219UL,
        0x0000000d7a438d49UL,
        0x0000000f55ecca97UL,
        0x0000000d07899f1dUL,
        0x0000000260947d6cUL,
        0x0000000ffbd21ab6UL,
        0x0000000d04ff923eUL,
        0x0000000964b72033UL,
        0x000000031ac3fd7eUL,
        0x0000000d2c52e2c4UL,
        0x0000000799a640efUL,
        0x000000098dd061edUL,
        0x00000005cb2ab7b8UL,
        0x000000072f3881c8UL,
        0x0000000e65ed1164UL,
        0x000000034fa0bd5bUL,
        0x000000064f9823cdUL,
        0x00000003797e1ac0UL,
        0x00000002fb8a4751UL,
        0x00000006f347342eUL,
        0x000000022dd7ea0aUL,
        0x0000000b19b65e57UL,
        0x000000044fe83e8aUL,
        0x000000007732732eUL,
        0x000000064de20ed7UL,
        0x000000006c9ea834UL,
        0x00000008ce066650UL,
        0x0000000c2a685ff0UL,
        0x000000064f19b01fUL,
        0x0000000491ab8a88UL,
        0x000000041212fe5aUL,
        0x00000006f9916f3bUL,
        0x0000000694f72e71UL,
        0x0000000ad7a5b35eUL,
        0x0000000f62795292UL,
        0x0000000c8cdc3d3aUL,
        0x0000000fbc6b3518UL,
        0x000000067b631901UL,
        0x00000005b5ba79d5UL,
        0x0000000f4fadebddUL,
        0x0000000ac7c802e7UL,
        0x0000000385712d9dUL,
        0x000000064bd375b4UL,
        0x0000000c9a11df70UL,
        0x000000088355bf31UL,
        0x0000000606ffbb0aUL,
        0x0000000bda93c2d5UL,
        0x00000007c5f94f0aUL,
        0x000000076fe26501UL,
        0x00000005d8b9153cUL,
        0x0000000886bbb218UL,
        0x0000000acee2fecaUL,
        0x00000002ad19a925UL,
        0x000000083b97855cUL,
        0x0000000d36608312UL,
        0x00000008ac60dbc7UL,
        0x00000000885c8f58UL,
        0x00000008abbdf891UL,
        0x0000000ea1602271UL,
        0x0000000ad654fee1UL,
        0x00000006c461195eUL,
        0x00000005eeb1a327UL,
        0x000000018d743962UL,
        0x00000001fc7c55a5UL,
        0x0000000aba749670UL,
        0x00000009c9a59c60UL,
        0x00000006e5bafc06UL,
        0x000000096977db12UL,
        0x0000000a97b6ebfaUL,
        0x000000063d2d9da6UL,
        0x0000000fab00cd60UL,
        0x0000000d7bdf4632UL,
        0x0000000f83878d59UL,
        0x0000000b1c2c462eUL,
        0x000000014e5144a7UL,
        0x0000000f4a909b28UL,
        0x0000000e979a185bUL,
        0x0000000908090a64UL,
        0x000000099eccd798UL,
        0x0000000348780a96UL,
        0x0000000fdc7ad169UL,
        0x0000000a600c2e5bUL,
        0x0000000b0968cd98UL,
        0x00000001a45ec098UL,
        0x000000099118c1b4UL,
        0x00000008afa5cd5aUL,
        0x00000001db7e655eUL,
        0x00000009f637e452UL,
        0x00000009568504e3UL,
        0x0000000045b2a662UL,
        0x0000000f2a1455a2UL,
        0x00000006c1ca9e75UL,
        0x000000030a4a4639UL,
        0x0000000c6c2c1a30UL,
        0x000000087500b452UL,
        0x00000005e338bb2eUL,
        0x0000000d9dd11dffUL,
        0x00000008c4b5d012UL,
        0x00000008191194e0UL,
        0x0000000dd11db867UL,
        0x0000000c67c151ceUL,
        0x00000005cb1a00e4UL,
        0x0000000098b7a1c6UL,
        0x0000000369f35cd4UL,
        0x0000000ca2190bdbUL,
        0x00000006e14bb3b9UL,
        0x00000008d5692f8cUL,
        0x0000000ca4b2f4f8UL,
        0x0000000787f06877UL,
        0x00000008acbb8550UL,
        0x0000000535f4b56aUL,
        0x0000000f4caf7ecbUL,
        0x0000000d4615b258UL,
        0x0000000347ca7070UL,
        0x00000003c798c85dUL,
        0x0000000460506465UL,
        0x0000000870d0a5dcUL,
        0x00000006510b2464UL,
        0x0000000d1dba5544UL,
        0x0000000d57789a33UL,
        0x0000000e2417c5baUL,
        0x0000000b5ff8628cUL,
        0x0000000a3bb22787UL,
        0x0000000a16b64f34UL,
        0x0000000421e81d3dUL,
        0x000000035b4596a7UL,
        0x00000008d7a2dd7eUL,
        0x000000050b2d83faUL,
        0x00000009ea87e7c2UL,
        0x0000000d5055e752UL,
        0x0000000f96aa9da5UL,
        0x0000000b096e2a07UL,
        0x000000049970b44bUL,
        0x0000000867fb1518UL,
        0x00000005d0f5dba2UL,
        0x00000001b191d11eUL,
        0x00000008e839bb8fUL,
        0x00000001cd4aca15UL,
        0x0000000971ec5615UL,
        0x00000007d72a7ebdUL,
        0x00000008b1253bfbUL,
        0x0000000e11de1d25UL,
        0x00000000a7566839UL,
        0x0000000f4f3542e0UL,
        0x00000001ea791e32UL,
        0x000000032a84f759UL,
        0x0000000646f1844eUL,
        0x000000042af26809UL,
        0x00000001f4b464ffUL,
        0x0000000da684d2d9UL,
        0x0000000d854f5fb9UL,
        0x00000004d4d3e91aUL,
        0x00000005af3ef4e2UL,
        0x00000008a1ef5ce7UL,
        0x00000002354febf3UL,
        0x0000000b3c5a8944UL,
        0x000000098b62a144UL,
        0x00000009bdba0b4eUL,
        0x000000004aa99b42UL,
        0x00000008099ea151UL,
        0x00000002185463a3UL,
        0x0000000b0a1ae997UL,
        0x0000000e628d5770UL,
        0x0000000b40b5ac89UL,
        0x000000027213b17dUL,
        0x00000004d21db5b5UL,
        0x000000010d0748f7UL,
        0x00000002276c7876UL,
        0x0000000b98bee56dUL,
        0x0000000bd1ca6ae8UL,
        0x0000000824ab48faUL,
        0x0000000c6f35ae62UL,
        0x00000003547a563cUL,
        0x0000000f1fc0d824UL,
        0x000000058f55ed75UL,
        0x0000000aa9d0de01UL,
        0x00000004719dde60UL,
        0x0000000d5386b3ddUL,
        0x00000004d8d9f666UL,
        0x0000000aee36013bUL,
        0x0000000ba4ee322fUL,
        0x0000000898d2db4eUL,
        0x00000009fe364808UL,
        0x0000000bb13e8045UL,
        0x0000000be346d43aUL,
        0x0000000b4c9f886fUL,
        0x0000000c9a6f53b8UL,
        0x00000000ed5a7b6fUL,
        0x00000002a1fac740UL,
        0x0000000b8c134a59UL,
        0x0000000b1f773993UL,
        0x0000000c4d9d0025UL,
        0x0000000ca905bdcaUL,
        0x00000003150a39a7UL,
        0x0000000e8329fad5UL,
        0x0000000bd4f98059UL,
        0x00000003bc5cf6cdUL,
        0x0000000c982fdd03UL,
        0x00000000a372de28UL,
        0x000000073fe2e35aUL,
        0x00000000b9f684ecUL,
        0x0000000c543ff680UL,
        0x00000001bcf5f09aUL,
        0x000000051b2a8099UL,
        0x0000000ee53277c2UL,
        0x00000000b3835a6cUL,
        0x0000000aed6765c1UL,
        0x000000092cfd64c8UL,
        0x0000000d20c60ed2UL,
        0x000000059dbd9f51UL,
        0x0000000b6acb694bUL,
        0x0000000427dcd5fdUL,
        0x0000000646336a75UL,
        0x00000008008dea4dUL,
        0x00000000af2bdc7cUL,
        0x0000000b8a46478aUL,
        0x0000000b02c535b6UL,
        0x0000000c645d8631UL,
        0x0000000044b4af3dUL,
        0x0000000c9edfe6cbUL,
        0x000000032ac8ea2aUL,
        0x000000079266a23fUL,
        0x0000000c2d902e93UL,
        0x00000006ae5cfbdbUL,
        0x00000002c66c633eUL,
        0x0000000eb7a8a4e3UL,
        0x0000000cb17281cfUL,
        0x00000007ca378680UL,
        0x00000007ac81509dUL,
        0x0000000a59a05073UL,
        0x0000000c9cb9f18dUL,
        0x0000000b78100d29UL,
        0x0000000fab49420aUL,
        0x0000000d0a4e69c4UL,
        0x0000000d6c33f722UL,
        0x000000068d21bff8UL,
        0x00000001fdad8ca3UL,
        0x00000002884d6968UL,
        0x0000000b091ff264UL,
        0x0000000eb5fb236fUL,
        0x0000000a3d2a1839UL,
        0x0000000527db0bc8UL,
        0x00000002dc68cd9fUL,
        0x0000000e3f4ea98aUL,
        0x0000000a629fe44fUL,
        0x0000000b73bd7d66UL,
        0x00000002abfd7b6bUL,
        0x00000001b4056054UL,
        0x0000000d6efaac28UL,
        0x00000000d13cc950UL,
        0x0000000ef84ead94UL,
        0x00000005b6ee0d50UL,
        0x00000000f4bec692UL,
        0x0000000de1b98881UL,
        0x000000055ccccd31UL,
        0x0000000086d9b84dUL,
        0x00000005ab736e3dUL,
        0x0000000167d2f005UL,
        0x0000000118ed1522UL,
        0x000000038bbdc903UL,
        0x000000039cd31ac2UL,
        0x000000031091bc51UL,
        0x0000000d66a87d3fUL,
        0x0000000afdade6d3UL,
        0x00000002bd1fe097UL,
        0x00000005cf545dd2UL,
        0x00000005e0af578eUL,
        0x00000006fe6dd4c9UL,
        0x0000000862bc8fcaUL,
        0x0000000cbce0b4c6UL,
        0x000000008b7fa8ddUL,
        0x00000003d108ae9fUL,
        0x0000000fed2d914aUL,
        0x0000000bab304bd8UL,
        0x0000000debe74f8dUL,
        0x00000001e857e3dcUL,
        0x0000000570340581UL,
        0x0000000114bbf4f5UL,
        0x0000000a3cfc0566UL,
        0x00000004026cd686UL,
        0x0000000266fb76cdUL,
        0x0000000b715773bbUL,
        0x00000002fd2785fdUL,
        0x0000000481b34cadUL,
        0x000000011c58d2baUL,
        0x00000003a5186f4dUL,
        0x0000000da55ab71cUL,
        0x0000000ac887db92UL,
        0x00000009bd6d5592UL,
        0x000000045857d12aUL,
        0x00000008c862f0b9UL,
        0x0000000870c88666UL,
        0x00000004a4f4901fUL,
        0x0000000774a993d0UL,
        0x0000000c9f16c81dUL,
        0x0000000eb415e9efUL,
        0x0000000307aa6302UL,
        0x0000000a246f21eeUL,
        0x00000001a4f8a9c2UL,
        0x00000000cf09f9b4UL,
        0x0000000db30dbb49UL,
        0x00000003581be36fUL,
        0x00000006919a4318UL,
        0x00000008ee677afdUL,
        0x00000005944b9d59UL,
        0x00000008d5fe61aaUL,
        0x000000077c174b1dUL,
        0x00000005cff8fa10UL,
        0x0000000c1ce82f48UL,
        0x00000007fbb18e65UL,
        0x00000000b6737103UL,
        0x0000000e2d30a9b6UL,
        0x00000006481ff469UL,
        0x00000005834b4d26UL,
        0x00000003bba517d5UL,
        0x0000000eee6e8080UL,
        0x00000005fe4fea5eUL,
        0x0000000e84e94c8cUL,
        0x0000000ba2ad0a2aUL,
        0x0000000a7f2aead0UL,
        0x000000063cecb46dUL,
        0x00000008943d7229UL,
        0x00000001d3878b2bUL,
        0x0000000f2b4efe94UL,
        0x0000000d9af1949dUL,
        0x0000000bb5824d39UL,
        0x0000000b8d8f5090UL,
        0x0000000ed5e19d08UL,
        0x000000060287437eUL,
        0x00000008fe6ae5c2UL,
        0x00000006c85ac058UL,
        0x0000000b906be1b8UL,
        0x0000000f9d423f65UL,
        0x00000006efed81d6UL,
        0x0000000781b67fa2UL,
        0x0000000e1dd437acUL,
        0x00000007a9201a8cUL,
        0x0000000fb444c819UL,
        0x0000000ce75af959UL,
        0x000000086df6e72bUL,
        0x0000000756695aa7UL,
        0x0000000b7b2bddf2UL,
        0x0000000f19a1b99eUL,
        0x00000009a5790e90UL,
        0x00000001d3b3eac0UL,
        0x0000000a5c5d9d2bUL,
        0x0000000152850218UL,
        0x0000000025c4ba6eUL,
        0x0000000d4a5f4bebUL,
        0x0000000709cec10eUL,
        0x000000094ddbdb6cUL,
        0x00000009d1218277UL,
        0x00000006190ca34aUL,
        0x0000000468ed6a3fUL,
        0x0000000801bda52eUL,
        0x0000000261b3f1a9UL,
        0x00000000b3494d9bUL,
        0x0000000583e2d7e5UL,
        0x00000009407a80f2UL,
        0x000000058e902456UL,
        0x00000009108c2273UL,
        0x000000059778ff8cUL,
        0x0000000d6ce05028UL,
        0x00000000286adc62UL,
        0x00000007ed3060dcUL,
        0x000000057b7e03edUL,
        0x00000003e3dce5c1UL,
        0x00000001bebc2295UL,
        0x0000000014a17c9aUL,
        0x0000000c7d90fbdaUL,
        0x00000008158ae35aUL,
        0x000000069d70a335UL,
        0x0000000d3ef97931UL,
        0x00000005793efb7aUL,
        0x0000000e6989ef43UL,
        0x0000000cd15f0116UL,
        0x0000000f9dbc6e25UL,
        0x0000000da4a91117UL,
        0x0000000054d0917aUL,
        0x000000060f2c3f15UL,
        0x00000007393b0a66UL,
        0x00000006630ed79bUL,
        0x0000000ed8589c60UL,
        0x00000007db37ab26UL,
        0x0000000c4631e80aUL,
        0x00000001badaf501UL,
        0x00000009bdef764dUL,
        0x0000000dd0949b4bUL,
        0x000000086f116771UL,
        0x0000000acd7ea109UL,
        0x00000007cc9d2f6bUL,
        0x00000003f5598822UL,
        0x00000004ba5a8d0cUL,
        0x000000066e7f9c42UL,
        0x000000033127fb36UL,
        0x00000000c85ff976UL,
        0x00000009dbb32ddfUL,
        0x00000003d06c7a56UL,
        0x0000000ac07601ddUL,
        0x00000005fda3d7e9UL,
        0x000000040a47aef0UL,
        0x0000000139928cd0UL,
        0x0000000183ab75ebUL,
        0x00000009dd6d1f4bUL,
        0x0000000954afec44UL,
        0x000000029953fe22UL,
        0x0000000f947e49b1UL,
        0x0000000a74266cb0UL,
        0x00000003bbb7fdabUL,
        0x00000008a72b63d1UL,
        0x00000008763e2fbbUL,
        0x00000008c9b4f9a2UL,
        0x0000000a35f5a861UL,
        0x000000099e54752cUL,
        0x00000002fdb8e16fUL,
        0x00000002d083ed68UL,
        0x0000000a05d36c5eUL,
        0x00000005460842feUL,
        0x0000000173ae0ee6UL,
        0x000000038b3c62e5UL,
        0x0000000476c1ae99UL,
        0x00000009a8cb898aUL,
        0x000000019d4032acUL,
        0x0000000a9c01d80bUL,
        0x0000000ca7d5e4deUL,
        0x0000000295d53115UL,
        0x0000000b26740e51UL,
        0x0000000bf21b0988UL,
        0x0000000167391c15UL,
        0x0000000d10af35c6UL,
        0x0000000d94750799UL,
        0x0000000cb986d117UL,
        0x000000001dddf588UL,
        0x000000071ed85f46UL,
        0x0000000a5437d58fUL,
        0x00000004029d1e25UL,
        0x0000000c580ec972UL,
        0x00000006847df8baUL,
        0x0000000e294d997bUL,
        0x0000000e2e8b10eeUL,
        0x00000001593103ddUL,
        0x0000000222103857UL,
        0x00000001e035591dUL,
        0x0000000b5c9ef2e9UL,
        0x00000009f815ec3eUL,
        0x0000000d1da2a021UL,
        0x000000054f171191UL,
        0x0000000e51f4a05eUL,
        0x0000000c15e7d603UL,
        0x0000000ba7f16b87UL,
        0x000000080b7a83e1UL,
        0x0000000720e2b18dUL,
        0x00000005ec0c069dUL,
        0x0000000a4f9f689cUL,
        0x00000005871cafdaUL,
        0x0000000c913140a2UL,
        0x00000007a8f2efd1UL,
        0x000000077064952cUL,
        0x00000004ea2d857fUL,
        0x0000000484523555UL,
        0x000000054971a9e3UL,
        0x0000000eb0694eb2UL,
        0x0000000b513c8e63UL,
        0x00000005c910db58UL,
        0x0000000ca87a4dd7UL,
        0x0000000b8ca63158UL,
        0x0000000b4b09431dUL,
        0x00000003dc9d50b7UL,
        0x00000007d57f02acUL,
        0x00000005c595b1b2UL,
        0x00000009e0caf698UL,
        0x0000000136b48555UL,
        0x0000000687dbcc2bUL,
        0x000000054bae2294UL,
        0x00000006899bbd7bUL,
        0x00000008108f46deUL,
        0x00000001dbe8cf08UL,
        0x0000000a02e1ae1dUL,
        0x00000000f5f26d59UL,
        0x0000000805cf202bUL,
        0x0000000afede5687UL,
        0x00000001583d5b30UL,
        0x0000000da9ed0620UL,
        0x0000000cf1237338UL,
        0x00000003a5a77bc4UL,
        0x0000000a17ffa0c6UL,
        0x000000029de4c387UL,
        0x000000007825d431UL,
        0x000000002d7b9b38UL,
        0x00000008ed0f26aaUL,
        0x000000056e54e30dUL,
        0x00000009620ab0e7UL,
        0x0000000c7e3ea94cUL,
        0x0000000d288a41e2UL,
        0x0000000f68884f1eUL,
        0x00000005ee02df09UL,
        0x0000000c02dbf645UL,
        0x0000000eac4c2424UL,
        0x0000000cab2d51e1UL,
        0x0000000037439577UL,
        0x00000005618ada43UL,
        0x00000002683b5859UL,
        0x00000008a607c1ceUL,
        0x0000000795fd9198UL,
        0x0000000b3edb11b8UL,
        0x0000000846939c5cUL,
        0x00000008b1f6fa23UL,
        0x0000000b1a2f2bfeUL,
        0x0000000b63a07ad7UL,
        0x00000005f8ea7b00UL,
        0x00000004ee9c6d0cUL,
        0x0000000990f2889bUL,
        0x0000000b7f7251d0UL,
        0x0000000ac3291369UL,
        0x00000009d8f36a7bUL,
        0x0000000d57342897UL,
        0x0000000efca98365UL,
        0x0000000dacc69f0eUL,
        0x00000003a70e4b3cUL,
        0x00000001e95c34c2UL,
        0x00000004caab6c06UL,
        0x00000007231f6ee1UL,
        0x000000037909aa04UL,
        0x0000000048c9a9ccUL,
        0x000000059cd081bcUL,
        0x00000004dd78c2e4UL,
        0x00000004979da10fUL,
        0x000000004749d0c5UL,
        0x0000000a17a4283bUL,
        0x0000000de7e1d52dUL,
        0x00000000e47cedf1UL,
        0x00000004fa48cbffUL,
        0x0000000545a932a0UL,
        0x00000006c2bd9eb8UL,
        0x0000000dd9bd3b8cUL,
        0x000000043332c1baUL,
        0x0000000501fa761dUL,
        0x00000007ec40adbbUL,
        0x00000004049f2b33UL,
        0x0000000cde28f57bUL,
        0x0000000f68c804b9UL,
        0x00000008f50fbd3eUL,
        0x000000054e1bc344UL,
        0x000000036b26e3a2UL,
        0x000000002e5ac9b1UL,
        0x000000010837858dUL,
        0x00000006ccac9e0bUL,
        0x0000000625ba8a52UL,
        0x0000000ac4c8b45cUL,
        0x0000000868678237UL,
        0x00000004187235feUL,
        0x0000000bd62663ceUL,
        0x0000000ea832dfb2UL,
        0x0000000d5a72f0a7UL,
        0x00000000659c855eUL,
        0x0000000bea7f5e48UL,
        0x0000000ff9566715UL,
        0x00000001bd06d99aUL,
        0x00000009666c578cUL,
        0x0000000c6527d3ecUL,
        0x0000000b541f3c61UL,
        0x0000000678a9ad70UL,
        0x000000036eaadfa3UL,
        0x0000000af74b01deUL,
        0x000000054cc3cdc3UL,
        0x0000000d2e587ce6UL,
        0x00000008694b9349UL,
        0x0000000d309898feUL,
        0x00000005c3250e09UL,
        0x000000084dcac28eUL,
        0x0000000f72add2dfUL,
        0x00000001901681a3UL,
        0x000000009e6a8fd4UL,
        0x000000012f614cd1UL,
        0x00000006d7801ac4UL,
        0x000000014cf1ca54UL,
        0x000000012a7eb608UL,
        0x00000005e7a3bf62UL,
        0x00000000ba5056a2UL,
        0x00000005bee44c9bUL,
        0x0000000819d7dc86UL,
        0x0000000062adc8fdUL,
        0x0000000bd3155d41UL,
        0x0000000cd8c6b38aUL,
        0x0000000e320fd50eUL,
        0x0000000e189d6655UL,
        0x00000006863c2831UL,
        0x00000000d2b9058fUL,
        0x000000023bfad8faUL,
        0x0000000199bd1216UL,
        0x000000056138afd7UL,
        0x0000000face83a93UL,
        0x00000009554da725UL,
        0x00000009b614dd91UL,
        0x000000098acbca3fUL,
        0x0000000d5f0d5f21UL,
        0x0000000eb59039e1UL,
        0x000000051d1ec82aUL,
        0x0000000a366ef3baUL,
        0x00000001ad0e01f0UL,
        0x00000007f038ad0bUL,
        0x00000003ee055321UL,
        0x00000003bf2dcbb7UL,
        0x0000000210e9856cUL,
        0x0000000e4fea8231UL,
        0x0000000b89444937UL,
        0x000000058852cc34UL,
        0x00000001ee29eea9UL,
        0x0000000b919c79f2UL,
        0x0000000ddc44d3adUL,
        0x0000000ddcbd4777UL,
        0x00000003c3982ba1UL,
        0x0000000dc8ebc45dUL,
        0x00000008b97712b1UL,
        0x00000009702ea21eUL,
        0x00000001f457e726UL,
        0x000000027c6f6e26UL,
        0x00000000a9797770UL,
        0x0000000d7615f53bUL,
        0x000000074f1cb6e1UL,
        0x0000000a32e4d7dcUL,
        0x00000002e89afd1dUL,
        0x00000000b03704d5UL,
        0x0000000cca58aab0UL,
        0x00000001e5749225UL,
        0x00000006e63a36baUL,
        0x0000000562992099UL,
        0x000000064701b950UL,
        0x0000000f94ed6196UL,
        0x0000000b3441b5f1UL,
        0x0000000c64fac247UL,
        0x0000000d72ebd98bUL,
        0x0000000fa1985b23UL,
        0x00000002df788358UL,
        0x000000088838b488UL,
        0x00000006091032b4UL,
        0x000000025ff2d736UL,
        0x0000000dce63d3d5UL,
        0x0000000bb5970414UL,
        0x000000044d8b5ffeUL,
        0x0000000e1a5666d8UL,
        0x0000000e34129125UL,
        0x00000000e23854b1UL,
        0x000000001b2a6dbeUL,
        0x0000000d11507bcdUL,
        0x0000000844531e6bUL,
        0x0000000d864a8611UL,
        0x0000000e2a5a7700UL,
        0x00000002d178962aUL,
        0x0000000156b07f01UL,
        0x000000048b59fec3UL,
        0x00000003d3d9d79cUL,
        0x00000001846fb339UL,
        0x0000000ddf1d03caUL,
        0x00000000998abaf9UL,
        0x0000000c9d76190bUL,
        0x000000067354a1a8UL,
        0x0000000cc89e2b09UL,
        0x0000000353356834UL,
        0x00000007ad97470eUL,
        0x0000000f4d560524UL,
        0x0000000534b7804eUL,
        0x000000014290c632UL,
        0x0000000b67d39d60UL,
        0x000000035b166febUL,
        0x000000088e6fb681UL,
        0x0000000a0f82ae1aUL,
        0x000000008460ce52UL,
        0x00000008b06a9012UL,
        0x0000000daf1299dcUL,
        0x0000000629ab696cUL,
        0x00000003113b448aUL,
        0x00000000db5ca215UL,
        0x00000003e00b1e2dUL,
        0x000000085a87f5abUL,
        0x0000000b3995ff20UL,
        0x000000085661554dUL,
        0x0000000e709c5384UL,
        0x00000000111ca99bUL,
        0x000000049e614279UL,
        0x0000000f14677ec4UL,
        0x00000008f6439bfbUL,
        0x0000000749faa461UL,
        0x00000001c4f9189aUL,
        0x0000000e8e9015caUL,
        0x0000000f6e68d510UL,
        0x0000000b3819319fUL,
        0x0000000da9f7119fUL,
        0x00000007787f40f8UL,
        0x0000000bc57f5716UL,
        0x000000060ff2897eUL,
        0x0000000b3a28a934UL,
        0x000000010b34c97cUL,
        0x0000000c14f53aedUL,
        0x0000000d3c4eaf5dUL,
        0x0000000b3148d39eUL,
        0x000000007874ea02UL,
        0x0000000f86692b4aUL,
        0x00000005b03a0e8dUL,
        0x0000000ce6db8cc6UL,
        0x00000008233d5908UL,
        0x0000000f163e3c06UL,
        0x0000000dff854cceUL,
        0x000000026706f1bcUL,
        0x000000094c358653UL,
        0x00000007384c9821UL,
        0x0000000e51b8e5d5UL,
        0x0000000eda32963bUL,
        0x0000000a073f392fUL,
        0x0000000c3ccfa213UL,
        0x000000034adf5216UL,
        0x0000000cb8da286bUL,
        0x00000003b5fbbf08UL,
        0x000000012812d1f8UL,
        0x0000000c97c54c39UL,
        0x0000000e1c3e36b9UL,
        0x0000000abb8dc0edUL,
        0x0000000019dcbbf6UL,
        0x000000025b0d7c4dUL,
        0x0000000045e6b5ceUL,
        0x000000017dc086caUL,
        0x0000000c3f425e6bUL,
        0x00000006fdee14f8UL,
        0x000000039155e6b4UL,
        0x00000000a191ec15UL,
        0x0000000398fcd7f4UL,
        0x0000000a6e2b0594UL,
        0x0000000fe5678d82UL,
        0x0000000e317eba1fUL,
        0x00000002c4f10ca1UL,
        0x0000000ae239c19eUL,
        0x000000018e663ed2UL,
        0x00000004a040b7e7UL,
        0x0000000bbca0849cUL,
        0x0000000ce05b3a74UL,
        0x00000007cee982fdUL,
        0x000000078ee54fa7UL,
        0x00000007b47bb0bdUL,
        0x00000007e8f19216UL,
        0x0000000d67d91cedUL,
        0x0000000ef5effe94UL,
        0x0000000ec1d1938dUL,
        0x00000004c05ef70eUL,
        0x00000000324442d9UL,
        0x0000000fb0183bb4UL,
        0x0000000fb7a0bd50UL,
        0x000000089aa17d87UL,
        0x0000000e4e6aed89UL,
        0x0000000dbecf68b4UL,
        0x0000000683770de4UL,
        0x0000000b9f41a136UL,
        0x0000000c7614caceUL,
        0x000000089c298386UL,
        0x0000000959cf09deUL,
        0x0000000ab30b19e3UL,
        0x0000000db2e4b614UL,
        0x000000026d30d39bUL,
        0x00000006ccefe452UL,
        0x0000000587c5035cUL,
        0x0000000ea73bbbe0UL,
        0x0000000dd9d91a11UL,
        0x0000000dd8c5e851UL,
        0x0000000e8b4aa077UL,
        0x00000008ccf8faddUL,
        0x000000047ddd3c0bUL,
        0x0000000635a92f19UL,
        0x0000000f0edfd1a3UL,
        0x00000001f760bf5eUL,
        0x0000000a83feb68aUL,
        0x00000004f74da9ddUL,
        0x000000052f759252UL,
        0x000000098bee689eUL,
        0x0000000c5fc8c3d5UL,
        0x00000008373d1286UL,
        0x0000000f5f1cdabdUL,
        0x0000000ada68d3e5UL,
        0x00000003bbb9eb5eUL,
        0x000000050cde8478UL,
        0x0000000f01f956e0UL,
        0x0000000a922f2842UL,
        0x0000000233a8b25aUL,
        0x000000071118b754UL,
        0x0000000b7f874552UL,
        0x000000044d757121UL,
        0x0000000b873b14ccUL,
        0x00000005bcc1db5cUL,
        0x0000000bf9b895ceUL,
        0x00000005e65bb620UL,
        0x0000000bbd1ed35cUL,
        0x0000000358e79973UL,
        0x000000062aa5a4a5UL,
        0x000000081715fc0fUL,
        0x00000008df03a76eUL,
        0x0000000376b7c6c7UL,
        0x0000000a07a49f2eUL,
        0x000000045e159b63UL,
        0x0000000dae5706b0UL,
        0x0000000b5e52c7ccUL,
        0x0000000206935e8eUL,
        0x000000039f0c5119UL,
        0x00000003cd58c574UL,
        0x0000000571986d35UL,
        0x0000000ad66da60fUL,
        0x000000002b1a6315UL,
        0x0000000d0131b533UL,
        0x0000000741a195c5UL,
        0x00000000b8663437UL,
        0x00000001cde52798UL,
        0x00000006b8e658b1UL,
        0x0000000b43c0d44dUL,
        0x000000045481d697UL,
        0x000000029de93df5UL,
        0x000000010549b874UL,
        0x0000000c056b5828UL,
        0x000000003fa830adUL,
        0x00000009496d14faUL,
        0x0000000f540592a0UL,
        0x0000000f31c8b855UL,
        0x000000064f2ba36bUL,
        0x0000000fe7c6e4f5UL,
        0x00000005e42a78b0UL,
        0x00000009c2b8b096UL,
        0x0000000dcb4a6e71UL,
        0x0000000d63b0e7edUL,
        0x0000000de1bcbcdaUL,
        0x000000068e7161f2UL,
        0x00000003e5ddf88dUL,
        0x0000000419a37501UL,
        0x0000000fad63e7abUL,
        0x0000000c6e81b4baUL,
        0x00000008329315d3UL,
        0x0000000c88d267e6UL,
        0x000000073a0ac25fUL,
        0x0000000e7b75690fUL,
        0x0000000dcbb95be2UL,
        0x00000007a1d2a059UL,
        0x0000000d8fac361eUL,
        0x00000006312ff5c9UL,
        0x0000000d2cf50d54UL,
        0x00000008c65fd00fUL,
        0x0000000aa1636532UL,
        0x0000000870c7285dUL,
        0x00000001894f0b84UL,
        0x00000004260cc5c3UL,
        0x0000000e9997b9ecUL,
        0x000000087a052144UL,
        0x00000008706babf6UL,
        0x0000000bd5f62ad3UL,
        0x00000001a7895439UL,
        0x0000000f7e294bbcUL,
        0x0000000bcc27ca26UL,
        0x00000003186a63d4UL,
        0x00000007f3ede4a4UL,
        0x0000000b64e32468UL,
        0x000000071f250d53UL,
        0x00000007c6513783UL,
        0x0000000b1778714aUL,
        0x000000094bf2c57fUL,
        0x000000064a9f893aUL,
        0x00000001305be654UL,
        0x0000000493e0c9f6UL,
        0x000000005ba6fed8UL,
        0x0000000c4a0c7a06UL,
        0x00000000cc2ec0ddUL,
        0x0000000d9a6769afUL,
        0x0000000724c78a49UL,
        0x0000000c85c981a4UL,
        0x000000012553c4cdUL,
        0x000000083cb892b1UL,
        0x0000000bc324ccc7UL,
        0x0000000ef43f6c1dUL,
        0x00000002d6748bb7UL,
        0x00000005efdce2d7UL,
        0x000000094af64f28UL,
        0x0000000f9d58feb3UL,
        0x0000000cf547ac63UL,
        0x0000000ceb309febUL,
        0x000000030beba8caUL,
        0x00000008ab2e486aUL,
        0x00000004a95d58adUL,
        0x000000025ce07c46UL,
        0x0000000712b93fd7UL,
        0x00000007f46acc81UL,
        0x000000064049d4beUL,
        0x000000065303aa09UL,
        0x0000000f3aad21b3UL,
        0x00000002903a6cd0UL,
        0x00000005a0e0467dUL,
        0x00000003c4fa64e4UL,
        0x00000005c6655126UL,
        0x0000000b40a2a67fUL,
        0x0000000b0c22c6e5UL,
        0x00000001507e039bUL,
        0x0000000b282b16b8UL,
        0x0000000c0e14a3d3UL,
        0x000000093d381427UL,
        0x00000006bb55bb87UL,
        0x0000000b675af72fUL,
        0x0000000fceb4f95eUL,
        0x000000066af6ebbdUL,
        0x000000020a44d1f2UL,
        0x00000006bc873916UL,
        0x0000000b8947bee8UL,
        0x00000004b6bed8a6UL,
        0x00000007012f7867UL,
        0x00000007eda3c150UL,
        0x0000000ab3ef1b8eUL,
        0x00000006d71466eeUL,
        0x0000000408c4e225UL,
        0x0000000e117838b1UL,
        0x00000000aef3a075UL,
        0x00000005a0779d4fUL,
        0x000000070a3b1d69UL,
        0x000000026ccd31fdUL,
        0x0000000ed64dd1b2UL,
        0x0000000981d4f60cUL,
        0x00000006a6e4fb61UL,
        0x000000052f15fc93UL,
        0x0000000032b3a64dUL,
        0x0000000ecb17d667UL,
        0x0000000a983fb935UL,
        0x000000037d23c88dUL,
        0x0000000b8590fbcbUL,
        0x0000000ec2f1a277UL,
        0x000000090d3053e6UL,
        0x0000000a36fa8ccdUL,
        0x000000044bd08eccUL,
        0x000000061dd197d9UL,
        0x0000000a307cfd82UL,
        0x00000001d09c2de4UL,
        0x00000005f6d74368UL,
        0x00000001327d1b2dUL,
        0x0000000594cc36b9UL,
        0x0000000fea1cba7cUL,
        0x000000050c31262dUL,
        0x0000000d99b1a6baUL,
        0x00000001bf789cd2UL,
        0x0000000e2f6f66f9UL,
        0x000000013d5edfc6UL,
        0x0000000bc3a9ab0cUL,
        0x00000001da5b2734UL,
        0x000000025ef4f2deUL,
        0x0000000dcb55a50aUL,
        0x00000009c6dbc6acUL,
        0x000000089a838853UL,
        0x0000000168f099eeUL,
        0x0000000d51601760UL,
        0x000000089f324f1aUL,
        0x00000002cb1ec1eaUL,
        0x00000006306de366UL,
        0x0000000012a2f11eUL,
        0x0000000b5c0bf797UL,
        0x00000005c5f02be4UL,
        0x00000005019f54beUL,
        0x00000006ae4a096aUL,
        0x00000004bce78778UL,
        0x000000094b65b97fUL,
        0x0000000d3f6e7bd2UL,
        0x00000001fbd2a84cUL,
        0x00000006d0127ab1UL,
        0x00000003e82799aaUL,
        0x00000004c1264dfeUL,
        0x0000000cf69c9360UL,
        0x00000004b43e5342UL,
        0x000000035d1f0372UL,
        0x0000000d78c18eb4UL,
        0x0000000262574101UL,
        0x0000000c2c5c7335UL,
        0x0000000bad04051aUL,
        0x00000001c481f94eUL,
        0x00000003285aa0deUL,
        0x00000008973e1f69UL,
        0x00000005d238c694UL,
        0x00000007b71847b9UL,
        0x0000000242f5675cUL,
        0x0000000cc5751c2dUL,
        0x0000000e09bc620bUL,
        0x00000000e4e904ddUL,
        0x000000007ca4f1a7UL,
        0x00000002ac79ae43UL,
        0x0000000e213d4250UL,
        0x0000000d4137c2b5UL,
        0x0000000ddfce11bcUL,
        0x0000000d1d658566UL,
        0x0000000213f5b1bbUL,
        0x0000000cd35be0a8UL,
        0x0000000cc67d7f91UL,
        0x0000000509bde098UL,
        0x000000074d3d8f46UL,
        0x000000051309c970UL,
        0x000000053e2bdf66UL,
        0x0000000a5dd3fed3UL,
        0x0000000a4e69b212UL,
        0x0000000b1d39936dUL,
        0x00000006b6c8926bUL,
        0x000000046540a7b0UL,
        0x00000002eebc599fUL,
        0x00000002e54a283eUL,
        0x0000000f9a328a9cUL,
        0x00000007ea9cfc53UL,
        0x00000005cffa2bdbUL,
        0x0000000464d16f8eUL,
        0x0000000eb09444bcUL,
        0x00000003f341b259UL,
        0x00000004d112b108UL,
        0x000000070cb94242UL,
        0x0000000974ed4ffdUL,
        0x00000001084da291UL,
        0x000000085673ca39UL,
        0x0000000d4d74766fUL,
        0x000000064a68e1deUL,
        0x0000000e35630caeUL,
        0x00000002073229dbUL,
        0x000000063d3a3902UL,
        0x000000031598ee06UL,
        0x0000000808d61126UL,
        0x0000000029957984UL,
        0x0000000d4f5f2649UL,
        0x00000009ec8a706bUL,
        0x0000000349981760UL,
        0x0000000c93ab23a6UL,
        0x00000002c7aa80daUL,
        0x0000000866f102baUL,
        0x0000000b15cff7bcUL,
        0x000000066a13a4caUL,
        0x000000054a755048UL,
        0x0000000d13fdb8d9UL,
        0x000000016ad5edf3UL,
        0x0000000e043bb154UL,
        0x0000000cc8755671UL,
        0x0000000cf9b2bfd5UL,
        0x00000003608890b4UL,
        0x0000000330fef315UL,
        0x0000000e3299ca65UL,
        0x00000000b60765e1UL,
        0x00000000e9bb17dcUL,
        0x000000095f474d8bUL,
        0x0000000e721d3d00UL,
        0x0000000d4679e565UL,
        0x0000000c80da6113UL,
        0x000000098deeff30UL,
        0x0000000c293bb871UL,
        0x0000000e79132f48UL,
        0x0000000b152dafbbUL,
        0x000000055f6a4386UL,
        0x0000000a1b8a4044UL,
        0x00000004f4187b05UL,
        0x00000000b17c2ed3UL,
        0x000000095d75ba04UL,
        0x0000000bbf12e96dUL,
        0x00000006abd1a52fUL,
        0x0000000f300bc991UL,
        0x0000000f0a7385d4UL,
        0x000000052964f82aUL,
        0x0000000a9962925fUL,
        0x0000000613b2eef1UL,
        0x00000005fd2c92a8UL,
        0x000000009ebecd05UL,
        0x000000036002b87aUL,
        0x0000000902c79eefUL,
        0x0000000394e63c7eUL,
        0x0000000133285064UL,
        0x0000000f7cfe2d4bUL,
        0x00000004f068522cUL,
        0x000000096fea1a0fUL,
        0x0000000c5a927b13UL,
        0x0000000e9a2c1994UL,
        0x00000005c53b3803UL,
        0x0000000f636b6188UL,
        0x0000000007c656e3UL,
        0x000000026af1fc5fUL,
        0x0000000ec2f40b78UL,
        0x0000000faa1921e5UL,
        0x00000006137a8b30UL,
        0x0000000028674f7bUL,
        0x00000003de184e35UL,
        0x0000000eeef093e6UL,
        0x0000000d44b3dae0UL,
        0x0000000bb7ab7d93UL,
        0x00000002ae18c956UL,
        0x0000000cde492bd6UL,
        0x00000001cee0216eUL,
        0x0000000f1e5830adUL,
        0x000000076f6c3299UL,
        0x0000000dea24af84UL,
        0x0000000277e75586UL,
        0x0000000a17318024UL,
        0x00000005c4739486UL,
        0x00000005e3de4725UL,
        0x00000006f67c9f6dUL,
        0x000000025f42791dUL,
        0x00000003c54d15b3UL,
        0x0000000ef98d9c32UL,
        0x000000042f64819dUL,
        0x000000016d5fd070UL,
        0x000000063cb98d4fUL,
        0x000000045a3ad27cUL,
        0x00000001b496b0acUL,
        0x0000000aa471c42dUL,
        0x00000000599346a2UL,
        0x00000000dc8d1c2dUL,
        0x00000007498928c1UL,
        0x0000000ea06e90ffUL,
        0x0000000b683baa32UL,
        0x0000000f93014e16UL,
        0x000000020575d56eUL,
        0x0000000794325589UL,
        0x00000001533e9935UL,
        0x000000086b8bcb70UL,
        0x0000000ce11faf5dUL,
        0x000000036c0bd318UL,
        0x0000000e5e8c1167UL,
        0x0000000e1831ba64UL,
        0x0000000e088dbfa4UL,
        0x0000000984479674UL,
        0x0000000afef02b29UL,
        0x000000048518c716UL,
        0x00000004301564ceUL,
        0x000000021cc88710UL,
        0x0000000d5c995278UL,
        0x0000000d8367de1cUL,
        0x00000004a51125e8UL,
        0x0000000113e1c226UL,
        0x0000000ef141e076UL,
        0x000000044097011dUL,
        0x00000004ca9d707cUL,
        0x000000040d8831f1UL,
        0x0000000bd9c3b1d8UL,
        0x0000000978364177UL,
        0x000000010f7606a9UL,
        0x000000046a64270aUL,
        0x000000042df1b22bUL,
        0x0000000e906cf2a0UL,
        0x0000000997da6fa5UL,
        0x0000000a5722c26fUL,
        0x0000000b14f58aaaUL,
        0x0000000afc167ad8UL,
        0x000000037be56e60UL,
        0x0000000de7f80d62UL,
        0x00000000c3fb0a64UL,
        0x0000000ce8ca802cUL,
        0x000000035032ed9dUL,
        0x0000000aa8ba3ee6UL,
        0x000000094b2e707cUL,
        0x00000002debbdae1UL,
        0x0000000f53e25fcfUL,
        0x0000000e935543ebUL,
        0x00000001462f0e90UL,
        0x000000054ce7d18cUL,
        0x00000002ddafdc5fUL,
        0x0000000700565deeUL,
        0x0000000fd408e0afUL,
        0x000000017d089decUL,
        0x0000000833ea2459UL,
        0x00000003c8d3776aUL,
        0x00000002e5eebac8UL,
        0x000000020cbf49b0UL,
        0x0000000c44675eb7UL,
        0x00000003a4b6beb1UL,
        0x0000000ce6f37c1eUL,
        0x000000063fba2e7cUL,
        0x00000005a05b553dUL,
        0x00000001286445b0UL,
        0x00000005e07a9b61UL,
        0x00000007d8397ea4UL,
        0x00000008084b7bbbUL,
        0x0000000b05b38097UL,
        0x000000029c3019eeUL,
        0x0000000ed1d2708bUL,
        0x00000009df8a4d47UL,
        0x0000000e4891e436UL,
        0x00000002a762ab72UL,
        0x000000092f70600fUL,
        0x000000092329a2cdUL,
        0x00000003e200c6edUL,
        0x00000008c0a7233eUL,
        0x000000060866806aUL,
        0x0000000f4fddd24aUL,
        0x0000000f78464c71UL,
        0x00000009c3d22242UL,
        0x00000003877ea6d1UL,
        0x0000000e2a6d54acUL,
        0x0000000497d2a5e7UL,
        0x0000000ca82f781eUL,
        0x0000000481524f4cUL,
        0x0000000dee088814UL,
        0x0000000b2a82d3a4UL,
        0x00000008e6afe6e5UL,
        0x0000000d6279a5daUL,
        0x00000004567cbc1aUL,
        0x00000005bec2b2fdUL,
        0x00000004ef452505UL,
        0x000000061d992cbaUL,
        0x0000000ab96be0cbUL,
        0x0000000708ef35d9UL,
        0x0000000b3f6f3623UL,
        0x000000036eb1801dUL,
        0x0000000badfee917UL,
        0x0000000a3db13cd0UL,
        0x00000001d1a12828UL,
        0x00000002500816ceUL,
        0x0000000cf7612148UL,
        0x00000000be6a3f4bUL,
        0x000000074142f3daUL,
        0x0000000ce5deed92UL,
        0x0000000f9530a786UL,
        0x0000000047c8bb38UL,
        0x0000000fcabfe88fUL,
        0x0000000bc83accb1UL,
        0x000000020cd9fb1fUL,
        0x0000000023dcceb3UL,
        0x00000009e969b8c4UL,
        0x00000006e28de934UL,
        0x000000080a399667UL,
        0x000000076a0b85adUL,
        0x000000021a84be3cUL,
        0x0000000a28d028b5UL,
        0x0000000c4e7690dfUL,
        0x0000000bfd9621e8UL,
        0x00000006f4bc0c24UL,
        0x0000000aa8e76bd7UL,
        0x0000000deb55dac9UL,
        0x0000000bb344fa8bUL,
        0x0000000fcaab4decUL,
        0x0000000146aba6cbUL,
        0x0000000f49ed6eb8UL,
        0x0000000dd57e9deaUL,
        0x0000000225d5d090UL,
        0x0000000d6e86c1c5UL,
        0x0000000639be5f39UL,
        0x0000000f5e7a6132UL,
        0x0000000d2968b09fUL,
        0x000000082b30ba1eUL,
        0x0000000803fa46ccUL,
        0x0000000c290fab00UL,
        0x000000010df59de5UL,
        0x000000051ae9dcfbUL,
        0x000000049af8516dUL,
        0x000000002b564ce6UL,
        0x0000000c615a1de0UL,
        0x0000000fef9864a4UL,
        0x0000000c16e27341UL,
        0x000000039e846736UL,
        0x00000001ecbb6746UL,
        0x0000000588d03a7cUL,
        0x000000010a0eaf9cUL,
        0x0000000671ccea6bUL,
        0x000000033a154603UL,
        0x0000000a7b003bc1UL,
        0x0000000c5fc3848dUL,
        0x000000078e50a9c7UL,
        0x000000017dbfb88eUL,
        0x00000004fd0ed541UL,
        0x000000084221debaUL,
        0x00000003132cf7e6UL,
        0x0000000b67e7ac53UL,
        0x0000000df6b28024UL,
        0x0000000785b9f7edUL,
        0x0000000e3d35320dUL,
        0x0000000159c06583UL,
        0x00000005c54a80a3UL,
        0x0000000ed4d4533bUL,
        0x0000000cf16c601aUL,
        0x00000005e94efbd1UL,
        0x00000005d587126eUL,
        0x0000000eef2f2807UL,
        0x000000009f3c558eUL,
        0x0000000736cfd539UL,
        0x0000000f5a922ae1UL,
        0x00000004e2ab9959UL,
        0x00000006a2dd34e7UL,
        0x00000008c9d30d23UL,
        0x0000000eba20b791UL,
        0x0000000d5c5095e3UL,
        0x0000000423d75a82UL,
        0x000000040cebaafeUL,
        0x000000065e08d288UL,
        0x00000002e4f6d767UL,
        0x0000000fe10d2f21UL,
        0x0000000110347bdaUL,
        0x0000000e43a9bfb3UL,
        0x0000000cdea483ccUL,
        0x0000000fb1e2d8c6UL,
        0x0000000d8a0af7a7UL,
        0x000000037d05b182UL,
        0x00000008d1241d83UL,
        0x0000000da1ea7b6eUL,
        0x000000065bea93dbUL,
        0x00000002a02f8753UL,
        0x0000000454243289UL,
        0x00000004150bc5a2UL,
        0x0000000bbabe5911UL,
        0x00000004cbcdbc59UL,
        0x0000000f0e61340bUL,
        0x000000030a2cdea8UL,
        0x00000005daecb091UL,
        0x00000005dc93d891UL,
        0x0000000c501b4051UL,
        0x0000000782cfba78UL,
        0x00000004c191b61eUL,
        0x0000000b7e27ef35UL,
        0x000000005a476838UL,
        0x00000009b0209574UL,
        0x0000000a775164cfUL,
        0x0000000d33d21701UL,
        0x00000003afcb7d45UL,
        0x00000004df2035cdUL,
        0x0000000498819a21UL,
        0x0000000293f9e506UL,
        0x00000009a35ff1c8UL,
        0x0000000c090ebe6bUL,
        0x0000000a4f0551d4UL,
        0x00000005dc0dc194UL,
        0x00000001388aeb31UL,
        0x0000000340b27bf4UL,
        0x00000003a0f320abUL,
        0x00000000996be75dUL,
        0x0000000b257ecf39UL,
        0x000000078d86f2f1UL,
        0x0000000673f5ff91UL,
        0x00000004538d7e3eUL,
        0x0000000de5bc4369UL
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "tag36h11"
////////////////////////////////////////////////////////////////////////////////////////////////////

const apriltag_family_t tag36h11 = {
    .ncodes = 587,
    .black_border = 1,
    .d = 6,
    .h = 11,
    .codes = {
        0x0000000d5d628584UL,
        0x0000000d97f18b49UL,
        0x0000000dd280910eUL,
        0x0000000e479e9c98UL,
        0x0000000ebcbca822UL,
        0x0000000f31dab3acUL,
        0x0000000056a5d085UL,
        0x000000010652e1d4UL,
        0x000000022b1dfeadUL,
        0x0000000265ad0472UL,
        0x000000034fe91b86UL,
        0x00000003ff962cd5UL,
        0x000000043a25329aUL,
        0x0000000474b4385fUL,
        0x00000004e9d243e9UL,
        0x00000005246149aeUL,
        0x00000005997f5538UL,
        0x0000000683bb6c4cUL,
        0x00000006be4a7211UL,
        0x00000007e3158eeaUL,
        0x000000081da494afUL,
        0x0000000858339a74UL,
        0x00000008cd51a5feUL,
        0x00000009f21cc2d7UL,
        0x0000000a2cabc89cUL,
        0x0000000adc58d9ebUL,
        0x0000000b16e7dfb0UL,
        0x0000000b8c05eb3aUL,
        0x0000000d25ef139dUL,
        0x0000000d607e1962UL,
        0x0000000e4aba3076UL,
        0x00000002dde6a3daUL,
        0x000000043d40c678UL,
        0x00000005620be351UL,
        0x000000064c47fa65UL,
        0x0000000686d7002aUL,
        0x00000006c16605efUL,
        0x00000006fbf50bb4UL,
        0x00000008d06d39dcUL,
        0x00000009f53856b5UL,
        0x0000000adf746dc9UL,
        0x0000000bc9b084ddUL,
        0x0000000d290aa77bUL,
        0x0000000d9e28b305UL,
        0x0000000e4dd5c454UL,
        0x0000000fad2fe6f2UL,
        0x0000000181a8151aUL,
        0x000000026be42c2eUL,
        0x00000002e10237b8UL,
        0x0000000405cd5491UL,
        0x00000007742eab1cUL,
        0x000000085e6ac230UL,
        0x00000008d388cdbaUL,
        0x00000009f853ea93UL,
        0x0000000c41ea2445UL,
        0x0000000cf1973594UL,
        0x000000014a34a333UL,
        0x000000031eacd15bUL,
        0x00000006c79d2dabUL,
        0x000000073cbb3935UL,
        0x000000089c155bd3UL,
        0x00000008d6a46198UL,
        0x000000091133675dUL,
        0x0000000a708d89fbUL,
        0x0000000ae5ab9585UL,
        0x0000000b9558a6d4UL,
        0x0000000b98743ab2UL,
        0x0000000d6cec68daUL,
        0x00000001506bcaefUL,
        0x00000004becd217aUL,
        0x00000004f95c273fUL,
        0x0000000658b649ddUL,
        0x0000000a76c4b1b7UL,
        0x0000000ecf621f56UL,
        0x00000001c8a56a57UL,
        0x00000003628e92baUL,
        0x000000053706c0e2UL,
        0x00000005e6b3d231UL,
        0x00000007809cfa94UL,
        0x0000000e97eead6fUL,
        0x00000005af40604aUL,
        0x00000007492988adUL,
        0x0000000ed5994712UL,
        0x00000005eceaf9edUL,
        0x00000007c1632815UL,
        0x0000000c1a0095b4UL,
        0x0000000e9e25d52bUL,
        0x00000003a6705419UL,
        0x0000000a8333012fUL,
        0x00000004ce5704d0UL,
        0x0000000508e60a95UL,
        0x0000000877476120UL,
        0x0000000a864e950dUL,
        0x0000000ea45cfce7UL,
        0x000000019da047e8UL,
        0x000000024d4d5937UL,
        0x00000006e079cc9bUL,
        0x000000099f2e11d7UL,
        0x000000033aa50429UL,
        0x0000000499ff26c7UL,
        0x000000050f1d3251UL,
        0x000000066e7754efUL,
        0x000000096ad633ceUL,
        0x00000009a5653993UL,
        0x0000000aca30566cUL,
        0x0000000c298a790aUL,
        0x00000008be44b65dUL,
        0x0000000dc68f354bUL,
        0x000000016f7f919bUL,
        0x00000004dde0e826UL,
        0x0000000d548cbd9fUL,
        0x0000000e0439ceeeUL,
        0x0000000fd8b1fd16UL,
        0x000000076521bb7bUL,
        0x0000000d92375742UL,
        0x0000000cab16d40cUL,
        0x0000000730c9dd72UL,
        0x0000000ad9ba39c2UL,
        0x0000000b14493f87UL,
        0x000000052b15651fUL,
        0x0000000185409cadUL,
        0x000000077ae2c68dUL,
        0x000000094f5af4b5UL,
        0x00000000a13bad55UL,
        0x000000061ea437cdUL,
        0x0000000a022399e2UL,
        0x0000000203b163d1UL,
        0x00000007bba8f40eUL,
        0x000000095bc9442dUL,
        0x000000041c0b5358UL,
        0x00000008e9c6cc81UL,
        0x00000000eb549670UL,
        0x00000009da3a0b51UL,
        0x0000000d832a67a1UL,
        0x0000000dcd4350bcUL,
        0x00000004aa05fdd2UL,
        0x000000060c7bb44eUL,
        0x00000004b358b96cUL,
        0x0000000067299b45UL,
        0x0000000b9c89b5faUL,
        0x00000006975acaeaUL,
        0x000000062b8f7afaUL,
        0x000000033567c3d7UL,
        0x0000000bac139950UL,
        0x0000000a5927c62aUL,
        0x00000005c916e6a4UL,
        0x0000000260ecb7d5UL,
        0x000000029b7bbd9aUL,
        0x0000000903205f26UL,
        0x0000000ae72270a4UL,
        0x00000003d2ec51a7UL,
        0x000000082ea55324UL,
        0x000000011a6f3427UL,
        0x00000001ca1c4576UL,
        0x0000000a40c81aefUL,
        0x0000000bddccd730UL,
        0x00000000e617561eUL,
        0x0000000969317b0fUL,
        0x000000067f781364UL,
        0x0000000610912f96UL,
        0x0000000b2549fdfcUL,
        0x000000006e5aaa6bUL,
        0x0000000b6c475339UL,
        0x0000000c56836a4dUL,
        0x0000000844e351ebUL,
        0x00000004647f83b4UL,
        0x00000000908a04f5UL,
        0x00000007f51034c9UL,
        0x0000000aee537fcaUL,
        0x00000005e92494baUL,
        0x0000000d445808f4UL,
        0x000000028d68b563UL,
        0x000000004d25374bUL,
        0x00000002bc065f65UL,
        0x000000096dc3ea0cUL,
        0x00000004b2ade817UL,
        0x000000007c3fd502UL,
        0x0000000e768b5cafUL,
        0x000000017605cf6cUL,
        0x0000000182741ee4UL,
        0x000000062846097cUL,
        0x000000072b5ebf80UL,
        0x0000000263da6e13UL,
        0x0000000fa841bcb5UL,
        0x00000007e45e8c69UL,
        0x0000000653c81fa0UL,
        0x00000007443b5e70UL,
        0x00000000a5234afdUL,
        0x000000074756f24eUL,
        0x0000000157ebf02aUL,
        0x000000082ef46939UL,
        0x000000080d420264UL,
        0x00000002aeed3e98UL,
        0x0000000b0a1dd4f8UL,
        0x0000000b5436be13UL,
        0x00000007b7b4b13bUL,
        0x00000001ce80d6d3UL,
        0x000000016c08427dUL,
        0x0000000ee54462ddUL,
        0x00000001f7644cceUL,
        0x00000009c7b5cc92UL,
        0x0000000e369138f8UL,
        0x00000005d5a66e91UL,
        0x0000000485d62f49UL,
        0x0000000e6e819e94UL,
        0x0000000b1f340eb5UL,
        0x000000009d198ce2UL,
        0x0000000d60717437UL,
        0x00000000196b856cUL,
        0x0000000f0a6173a5UL,
        0x000000012c0e1ec6UL,
        0x000000062b82d5cfUL,
        0x0000000ad154c067UL,
        0x0000000ce3778832UL,
        0x00000006b0a7b864UL,
        0x00000004c7686694UL,
        0x00000005058ff3ecUL,
        0x0000000d5e21ea23UL,
        0x00000009ff4a76eeUL,
        0x00000009dd981019UL,
        0x00000001bad4d30aUL,
        0x0000000c601896d1UL,
        0x0000000973439b48UL,
        0x00000001ce7431a8UL,
        0x000000057a8021d6UL,
        0x0000000f9dba96e6UL,
        0x000000083a2e4e7cUL,
        0x00000008ea585380UL,
        0x0000000af6c0e744UL,
        0x0000000875b73babUL,
        0x0000000da34ca901UL,
        0x00000002ab9727efUL,
        0x0000000d39f21b9aUL,
        0x00000008a10b742fUL,
        0x00000005f8952dbaUL,
        0x0000000f8da71ab0UL,
        0x0000000c25f9df96UL,
        0x000000006f8a5d94UL,
        0x0000000e42e63e1aUL,
        0x0000000b78409d1bUL,
        0x0000000792229addUL,
        0x00000005acf8c455UL,
        0x00000002fc29a9b0UL,
        0x0000000ea486237bUL,
        0x0000000b0c9685a0UL,
        0x00000001ad748a47UL,
        0x000000003b4712d5UL,
        0x0000000f29216d30UL,
        0x00000008dad65e49UL,
        0x00000000a2cf09ddUL,
        0x00000000b5f174c6UL,
        0x0000000e54f57743UL,
        0x0000000b9cf54d78UL,
        0x00000004a312a88aUL,
        0x000000027babc962UL,
        0x0000000b86897111UL,
        0x0000000f2ff6c116UL,
        0x000000082274bd8aUL,
        0x000000097023505eUL,
        0x000000052d46edd1UL,
        0x0000000585c1f538UL,
        0x0000000bddd00e43UL,
        0x00000005590b74dfUL,
        0x0000000729404a1fUL,
        0x000000065320855eUL,
        0x0000000d3d4b6956UL,
        0x00000007ae374f14UL,
        0x00000002d7a60e06UL,
        0x0000000315cd9b5eUL,
        0x0000000fd36b4eacUL,
        0x0000000f1df7642bUL,
        0x000000055db27726UL,
        0x00000008f15ebc19UL,
        0x0000000992f8c531UL,
        0x000000062dea2a40UL,
        0x0000000928275cabUL,
        0x000000069c263cb9UL,
        0x0000000a774cca9eUL,
        0x0000000266b2110eUL,
        0x00000001b14acbb8UL,
        0x0000000624b8a71bUL,
        0x00000001c539406bUL,
        0x00000003086d529bUL,
        0x00000000111dd66eUL,
        0x000000098cd630bfUL,
        0x00000008b9d1ffdcUL,
        0x000000072b2f61e7UL,
        0x00000009ed9d672bUL,
        0x000000096cdd15f3UL,
        0x00000006366c2504UL,
        0x00000006ca9df73aUL,
        0x0000000a066d60f0UL,
        0x0000000e7a4b8addUL,
        0x00000008264647efUL,
        0x0000000aa195bf81UL,
        0x00000009a3db8244UL,
        0x0000000014d2df6aUL,
        0x00000000b63265b7UL,
        0x00000002f010de73UL,
        0x000000097e774986UL,
        0x0000000248affc29UL,
        0x0000000fb57dcd11UL,
        0x00000000b1a7e4d9UL,
        0x00000004bfa2d07dUL,
        0x000000054e5cdf96UL,
        0x00000004c15c1c86UL,
        0x0000000cd9c61166UL,
        0x0000000499380b2aUL,
        0x0000000540308d09UL,
        0x00000008b63fe66fUL,
        0x0000000c81aeb35eUL,
        0x000000086fe0bd5cUL,
        0x0000000ce2480c2aUL,
        0x00000001ab29ee60UL,
        0x00000008048daa15UL,
        0x0000000dbfeb2d39UL,
        0x0000000567c9858cUL,
        0x00000002b6edc5bcUL,
        0x00000002078fca82UL,
        0x0000000adacc22aaUL,
        0x0000000b92486f49UL,
        0x000000051fac5964UL,
        0x0000000691ee6420UL,
        0x0000000f63b3e129UL,
        0x000000039be7e572UL,
        0x0000000da2ce6c74UL,
        0x000000020cf17a5cUL,
        0x0000000ee55f9b6eUL,
        0x0000000fb8572726UL,
        0x0000000b2c2de548UL,
        0x0000000caa9bce92UL,
        0x0000000ae9182db3UL,
        0x000000074b6e5bd1UL,
        0x0000000137b252afUL,
        0x000000051f686881UL,
        0x0000000d672f6c02UL,
        0x0000000654146ce4UL,
        0x0000000f944bc825UL,
        0x0000000e8327f809UL,
        0x000000076a73fd59UL,
        0x0000000f79da4cb4UL,
        0x0000000956f8099bUL,
        0x00000007b5f2655cUL,
        0x0000000d06b114a6UL,
        0x0000000d0697ca50UL,
        0x000000027c390797UL,
        0x0000000bc61ed9b2UL,
        0x0000000cc12dd19bUL,
        0x0000000eb7818d2cUL,
        0x0000000092fcecdaUL,
        0x000000089ded4ea1UL,
        0x0000000256a0ba34UL,
        0x0000000b6948e627UL,
        0x00000001ef6b1054UL,
        0x00000008639294a2UL,
        0x0000000eda3780a4UL,
        0x000000039ee2af1dUL,
        0x0000000cd257edc5UL,
        0x00000002d9d6bc22UL,
        0x0000000121d3b47dUL,
        0x000000037e23f8adUL,
        0x0000000119f31cf6UL,
        0x00000002c97f4f09UL,
        0x0000000d502abfe0UL,
        0x000000010bc3ca77UL,
        0x000000053d7190efUL,
        0x000000090c3e62a6UL,
        0x00000007e9ebf675UL,
        0x0000000979ce23d1UL,
        0x000000027f0c98e9UL,
        0x0000000eafb4ae59UL,
        0x00000007ca7fe2bdUL,
        0x00000001490ca8f6UL,
        0x00000009123387baUL,
        0x0000000b3bc73888UL,
        0x00000003ea87e325UL,
        0x00000004888964aaUL,
        0x0000000a0188a6b9UL,
        0x0000000cd383c666UL,
        0x000000040029a3fdUL,
        0x0000000e1c00ac5cUL,
        0x000000039e6f2b6eUL,
        0x0000000de664f622UL,
        0x0000000e979a75e8UL,
        0x00000007c6b4c86cUL,
        0x0000000fd492e071UL,
        0x00000008fbb35118UL,
        0x000000040b4a09b7UL,
        0x0000000af80bd6daUL,
        0x000000070e0b2521UL,
        0x00000002f5c54d93UL,
        0x00000003f4a118d5UL,
        0x000000009c1897b9UL,
        0x0000000079776eacUL,
        0x0000000084b00b17UL,
        0x00000003a95ad90eUL,
        0x000000028c544095UL,
        0x000000039d457c05UL,
        0x00000007a3791a78UL,
        0x0000000bb770e22eUL,
        0x00000009a822bd6cUL,
        0x000000068a4b1fedUL,
        0x0000000a5fd27b3bUL,
        0x00000000c3995b79UL,
        0x0000000d1519dff1UL,
        0x00000008e7eee359UL,
        0x0000000cd3ca50b1UL,
        0x0000000b73b8b793UL,
        0x000000057aca1c43UL,
        0x0000000ec2655277UL,
        0x0000000785a2c1b3UL,
        0x000000075a07985aUL,
        0x0000000a4b01eb69UL,
        0x0000000a18a11347UL,
        0x0000000db1f28ca3UL,
        0x0000000877ec3e25UL,
        0x000000031f6341b8UL,
        0x00000001363a3a4cUL,
        0x0000000075d8b9baUL,
        0x00000007ae0792a9UL,
        0x0000000a83a21651UL,
        0x00000007f08f9fb5UL,
        0x00000000d0cf73a9UL,
        0x0000000b04dcc98eUL,
        0x0000000f65c7b0f8UL,
        0x000000065ddaf69aUL,
        0x00000002cf9b86b3UL,
        0x000000014cb51e25UL,
        0x0000000f48027b5bUL,
        0x00000000ec26ea8bUL,
        0x000000044bafd45cUL,
        0x0000000b12c7c0c4UL,
        0x0000000959fd9d82UL,
        0x0000000c77c9725aUL,
        0x000000048a22d462UL,
        0x00000008398e8072UL,
        0x0000000ec89b05ceUL,
        0x0000000bb682d4c9UL,
        0x0000000e5a86d2ffUL,
        0x0000000358f01134UL,
        0x00000008556ddcf6UL,
        0x000000067584b6e2UL,
        0x000000011609439fUL,
        0x000000008488816eUL,
        0x0000000aaf1a2c46UL,
        0x0000000f879898cfUL,
        0x00000008bbe5e2f7UL,
        0x0000000101eee363UL,
        0x0000000690f69377UL,
        0x0000000f5bd93cd9UL,
        0x0000000cea4c2bf6UL,
        0x00000009550be706UL,
        0x00000002c5b38a60UL,
        0x0000000e72033547UL,
        0x00000004458b0629UL,
        0x0000000ee8d9ed41UL,
        0x0000000d2f918d72UL,
        0x000000078dc39fd3UL,
        0x00000008212636f6UL,
        0x00000007450a72a7UL,
        0x0000000c4f0cf4c6UL,
        0x0000000367bcddcdUL,
        0x0000000c1caf8cc6UL,
        0x0000000a7f5b853dUL,
        0x00000009d536818bUL,
        0x0000000535e021b0UL,
        0x0000000a7eb8729eUL,
        0x0000000422a67b49UL,
        0x0000000929e928a6UL,
        0x000000048e8aefccUL,
        0x0000000a9897393cUL,
        0x00000005eb81d37eUL,
        0x00000001e80287b7UL,
        0x000000034770d903UL,
        0x00000002eef86728UL,
        0x000000059266ccb6UL,
        0x00000000110bba61UL,
        0x00000001dfd284efUL,
        0x0000000447439d1bUL,
        0x0000000fece0e599UL,
        0x00000009309f3703UL,
        0x000000080764d1ddUL,
        0x0000000353f1e6a0UL,
        0x00000002c1c12dccUL,
        0x0000000c1d21b9d7UL,
        0x0000000457ee453eUL,
        0x0000000d66faf540UL,
        0x000000044831e652UL,
        0x0000000cfd49a848UL,
        0x00000009312d4133UL,
        0x00000003f097d3eeUL,
        0x00000008c9ebef7aUL,
        0x0000000a99e29e88UL,
        0x00000000e9fab22cUL,
        0x00000004e748f4fbUL,
        0x0000000ecdee4288UL,
        0x0000000abce5f1d0UL,
        0x0000000c42f6876cUL,
        0x00000007ed402ea0UL,
        0x0000000e5c4242c3UL,
        0x0000000d5b2c31aeUL,
        0x0000000286863be6UL,
        0x0000000160444d94UL,
        0x00000005f0f5808eUL,
        0x0000000ae3d44b2aUL,
        0x00000009f5c5d109UL,
        0x00000008ad9316d7UL,
        0x00000003422ba064UL,
        0x00000002fed11d56UL,
        0x0000000bea6e3e04UL,
        0x000000004b029eecUL,
        0x00000006deed7435UL,
        0x00000003718ce17cUL,
        0x000000055857f5e2UL,
        0x00000002edac7b62UL,
        0x0000000085d6c512UL,
        0x0000000d6ca88e0fUL,
        0x00000002b7e1fc69UL,
        0x0000000a699d5c1bUL,
        0x0000000f05ad74deUL,
        0x00000004cf5fb56dUL,
        0x00000005725e07e1UL,
        0x000000072f18a2deUL,
        0x00000001cec52609UL,
        0x000000048534243cUL,
        0x00000002523a4d69UL,
        0x000000035c1b80d1UL,
        0x0000000a4d7338a7UL,
        0x00000000db1af012UL,
        0x0000000e61a9475dUL,
        0x000000005df03f91UL,
        0x000000097ae260bbUL,
        0x000000032d627fefUL,
        0x0000000b640f73c2UL,
        0x000000045a1ac9c6UL,
        0x00000006a2202de1UL,
        0x000000057d3e25f2UL,
        0x00000005aa9f986eUL,
        0x00000000cc859d8aUL,
        0x0000000e3ec6cca8UL,
        0x000000054e95e1aeUL,
        0x0000000446887b06UL,
        0x00000007516732beUL,
        0x00000003817ac8f5UL,
        0x00000003e26d938cUL,
        0x0000000aa81bc235UL,
        0x0000000df387ca1bUL,
        0x00000000f3a3b3f2UL,
        0x0000000b4bf69677UL,
        0x0000000ae21868edUL,
        0x000000081e1d2d9dUL,
        0x0000000a0a9ea14cUL,
        0x00000008eee297a9UL,
        0x00000004740c0559UL,
        0x0000000e8b141837UL,
        0x0000000ac69e0a3dUL,
        0x00000009ed83a1e1UL,
        0x00000005edb55ecbUL,
        0x000000007340fe81UL,
        0x000000050dfbc6bfUL,
        0x00000004f583508aUL,
        0x0000000cb1fb78bcUL,
        0x00000004025ced2fUL,
        0x000000039791ebecUL,
        0x000000053ee388f1UL,
        0x00000007d6c0bd23UL,
        0x000000093a995fbeUL,
        0x00000008a41728deUL,
        0x00000002fe70e053UL,
        0x0000000ab3db443aUL,
        0x00000001364edb05UL,
        0x000000047b6eeed6UL,
        0x000000012e71af01UL,
        0x000000052ff83587UL,
        0x00000003a1575dd8UL,
        0x00000003feaa3564UL,
        0x0000000eacf78ba7UL,
        0x00000000872b94f8UL,
        0x0000000da8ddf9a2UL,
        0x00000009aa920d2bUL,
        0x00000001f350ed36UL,
        0x000000018a5e861fUL,
        0x00000002c35b89c3UL,
        0x00000003347ac48aUL,
        0x00000007f23e022eUL,
        0x00000002459068fbUL,
        0x0000000e83be4b73UL
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "artoolkit"
////////////////////////////////////////////////////////////////////////////////////////////////////

const apriltag_family_t artoolkit = {
    .ncodes = 512,
    .black_border = 1,
    .d = 6,
    .h = 7,
    .codes = {
        0x0006dc269c27UL,
        0x0006d4229e26UL,
        0x0006cc2e9825UL,
        0x0006c42a9a24UL,
        0x0006fc369423UL,
        0x0006f4329622UL,
        0x0006ec3e9021UL,
        0x0006e43a9220UL,
        0x00069c068c2fUL,
        0x000694028e2eUL,
        0x00068c0e882dUL,
        0x0006840a8a2cUL,
        0x0006bc16842bUL,
        0x0006b412862aUL,
        0x0006ac1e8029UL,
        0x0006a41a8228UL,
        0x00065c66bc37UL,
        0x00065462be36UL,
        0x00064c6eb835UL,
        0x0006446aba34UL,
        0x00067c76b433UL,
        0x00067472b632UL,
        0x00066c7eb031UL,
        0x0006647ab230UL,
        0x00061c46ac3fUL,
        0x00061442ae3eUL,
        0x00060c4ea83dUL,
        0x0006044aaa3cUL,
        0x00063c56a43bUL,
        0x00063452a63aUL,
        0x00062c5ea039UL,
        0x0006245aa238UL,
        0x0007dca6dc07UL,
        0x0007d4a2de06UL,
        0x0007ccaed805UL,
        0x0007c4aada04UL,
        0x0007fcb6d403UL,
        0x0007f4b2d602UL,
        0x0007ecbed001UL,
        0x0007e4bad200UL,
        0x00079c86cc0fUL,
        0x00079482ce0eUL,
        0x00078c8ec80dUL,
        0x0007848aca0cUL,
        0x0007bc96c40bUL,
        0x0007b492c60aUL,
        0x0007ac9ec009UL,
        0x0007a49ac208UL,
        0x00075ce6fc17UL,
        0x000754e2fe16UL,
        0x00074ceef815UL,
        0x000744eafa14UL,
        0x00077cf6f413UL,
        0x000774f2f612UL,
        0x00076cfef011UL,
        0x000764faf210UL,
        0x00071cc6ec1fUL,
        0x000714c2ee1eUL,
        0x00070ccee81dUL,
        0x000704caea1cUL,
        0x00073cd6e41bUL,
        0x000734d2e61aUL,
        0x00072cdee019UL,
        0x000724dae218UL,
        0x0004dd261c67UL,
        0x0004d5221e66UL,
        0x0004cd2e1865UL,
        0x0004c52a1a64UL,
        0x0004fd361463UL,
        0x0004f5321662UL,
        0x0004ed3e1061UL,
        0x0004e53a1260UL,
        0x00049d060c6fUL,
        0x000495020e6eUL,
        0x00048d0e086dUL,
        0x0004850a0a6cUL,
        0x0004bd16046bUL,
        0x0004b512066aUL,
        0x0004ad1e0069UL,
        0x0004a51a0268UL,
        0x00045d663c77UL,
        0x000455623e76UL,
        0x00044d6e3875UL,
        0x0004456a3a74UL,
        0x00047d763473UL,
        0x000475723672UL,
        0x00046d7e3071UL,
        0x0004657a3270UL,
        0x00041d462c7fUL,
        0x000415422e7eUL,
        0x00040d4e287dUL,
        0x0004054a2a7cUL,
        0x00043d56247bUL,
        0x00043552267aUL,
        0x00042d5e2079UL,
        0x0004255a2278UL,
        0x0005dda65c47UL,
        0x0005d5a25e46UL,
        0x0005cdae5845UL,
        0x0005c5aa5a44UL,
        0x0005fdb65443UL,
        0x0005f5b25642UL,
        0x0005edbe5041UL,
        0x0005e5ba5240UL,
        0x00059d864c4fUL,
        0x000595824e4eUL,
        0x00058d8e484dUL,
        0x0005858a4a4cUL,
        0x0005bd96444bUL,
        0x0005b592464aUL,
        0x0005ad9e4049UL,
        0x0005a59a4248UL,
        0x00055de67c57UL,
        0x000555e27e56UL,
        0x00054dee7855UL,
        0x000545ea7a54UL,
        0x00057df67453UL,
        0x000575f27652UL,
        0x00056dfe7051UL,
        0x000565fa7250UL,
        0x00051dc66c5fUL,
        0x000515c26e5eUL,
        0x00050dce685dUL,
        0x000505ca6a5cUL,
        0x00053dd6645bUL,
        0x000535d2665aUL,
        0x00052dde6059UL,
        0x000525da6258UL,
        0x0002de279ca7UL,
        0x0002d6239ea6UL,
        0x0002ce2f98a5UL,
        0x0002c62b9aa4UL,
        0x0002fe3794a3UL,
        0x0002f63396a2UL,
        0x0002ee3f90a1UL,
        0x0002e63b92a0UL,
        0x00029e078cafUL,
        0x000296038eaeUL,
        0x00028e0f88adUL,
        0x0002860b8aacUL,
        0x0002be1784abUL,
        0x0002b61386aaUL,
        0x0002ae1f80a9UL,
        0x0002a61b82a8UL,
        0x00025e67bcb7UL,
        0x00025663beb6UL,
        0x00024e6fb8b5UL,
        0x0002466bbab4UL,
        0x00027e77b4b3UL,
        0x00027673b6b2UL,
        0x00026e7fb0b1UL,
        0x0002667bb2b0UL,
        0x00021e47acbfUL,
        0x00021643aebeUL,
        0x00020e4fa8bdUL,
        0x0002064baabcUL,
        0x00023e57a4bbUL,
        0x00023653a6baUL,
        0x00022e5fa0b9UL,
        0x0002265ba2b8UL,
        0x0003dea7dc87UL,
        0x0003d6a3de86UL,
        0x0003ceafd885UL,
        0x0003c6abda84UL,
        0x0003feb7d483UL,
        0x0003f6b3d682UL,
        0x0003eebfd081UL,
        0x0003e6bbd280UL,
        0x00039e87cc8fUL,
        0x00039683ce8eUL,
        0x00038e8fc88dUL,
        0x0003868bca8cUL,
        0x0003be97c48bUL,
        0x0003b693c68aUL,
        0x0003ae9fc089UL,
        0x0003a69bc288UL,
        0x00035ee7fc97UL,
        0x000356e3fe96UL,
        0x00034eeff895UL,
        0x000346ebfa94UL,
        0x00037ef7f493UL,
        0x000376f3f692UL,
        0x00036efff091UL,
        0x000366fbf290UL,
        0x00031ec7ec9fUL,
        0x000316c3ee9eUL,
        0x00030ecfe89dUL,
        0x000306cbea9cUL,
        0x00033ed7e49bUL,
        0x000336d3e69aUL,
        0x00032edfe099UL,
        0x000326dbe298UL,
        0x0000df271ce7UL,
        0x0000d7231ee6UL,
        0x0000cf2f18e5UL,
        0x0000c72b1ae4UL,
        0x0000ff3714e3UL,
        0x0000f73316e2UL,
        0x0000ef3f10e1UL,
        0x0000e73b12e0UL,
        0x00009f070cefUL,
        0x000097030eeeUL,
        0x00008f0f08edUL,
        0x0000870b0aecUL,
        0x0000bf1704ebUL,
        0x0000b71306eaUL,
        0x0000af1f00e9UL,
        0x0000a71b02e8UL,
        0x00005f673cf7UL,
        0x000057633ef6UL,
        0x00004f6f38f5UL,
        0x0000476b3af4UL,
        0x00007f7734f3UL,
        0x0000777336f2UL,
        0x00006f7f30f1UL,
        0x0000677b32f0UL,
        0x00001f472cffUL,
        0x000017432efeUL,
        0x00000f4f28fdUL,
        0x0000074b2afcUL,
        0x00003f5724fbUL,
        0x0000375326faUL,
        0x00002f5f20f9UL,
        0x0000275b22f8UL,
        0x0001dfa75cc7UL,
        0x0001d7a35ec6UL,
        0x0001cfaf58c5UL,
        0x0001c7ab5ac4UL,
        0x0001ffb754c3UL,
        0x0001f7b356c2UL,
        0x0001efbf50c1UL,
        0x0001e7bb52c0UL,
        0x00019f874ccfUL,
        0x000197834eceUL,
        0x00018f8f48cdUL,
        0x0001878b4accUL,
        0x0001bf9744cbUL,
        0x0001b79346caUL,
        0x0001af9f40c9UL,
        0x0001a79b42c8UL,
        0x00015fe77cd7UL,
        0x000157e37ed6UL,
        0x00014fef78d5UL,
        0x000147eb7ad4UL,
        0x00017ff774d3UL,
        0x000177f376d2UL,
        0x00016fff70d1UL,
        0x000167fb72d0UL,
        0x00011fc76cdfUL,
        0x000117c36edeUL,
        0x00010fcf68ddUL,
        0x000107cb6adcUL,
        0x00013fd764dbUL,
        0x000137d366daUL,
        0x00012fdf60d9UL,
        0x000127db62d8UL,
        0x000ed8249d27UL,
        0x000ed0209f26UL,
        0x000ec82c9925UL,
        0x000ec0289b24UL,
        0x000ef8349523UL,
        0x000ef0309722UL,
        0x000ee83c9121UL,
        0x000ee0389320UL,
        0x000e98048d2fUL,
        0x000e90008f2eUL,
        0x000e880c892dUL,
        0x000e80088b2cUL,
        0x000eb814852bUL,
        0x000eb010872aUL,
        0x000ea81c8129UL,
        0x000ea0188328UL,
        0x000e5864bd37UL,
        0x000e5060bf36UL,
        0x000e486cb935UL,
        0x000e4068bb34UL,
        0x000e7874b533UL,
        0x000e7070b732UL,
        0x000e687cb131UL,
        0x000e6078b330UL,
        0x000e1844ad3fUL,
        0x000e1040af3eUL,
        0x000e084ca93dUL,
        0x000e0048ab3cUL,
        0x000e3854a53bUL,
        0x000e3050a73aUL,
        0x000e285ca139UL,
        0x000e2058a338UL,
        0x000fd8a4dd07UL,
        0x000fd0a0df06UL,
        0x000fc8acd905UL,
        0x000fc0a8db04UL,
        0x000ff8b4d503UL,
        0x000ff0b0d702UL,
        0x000fe8bcd101UL,
        0x000fe0b8d300UL,
        0x000f9884cd0fUL,
        0x000f9080cf0eUL,
        0x000f888cc90dUL,
        0x000f8088cb0cUL,
        0x000fb894c50bUL,
        0x000fb090c70aUL,
        0x000fa89cc109UL,
        0x000fa098c308UL,
        0x000f58e4fd17UL,
        0x000f50e0ff16UL,
        0x000f48ecf915UL,
        0x000f40e8fb14UL,
        0x000f78f4f513UL,
        0x000f70f0f712UL,
        0x000f68fcf111UL,
        0x000f60f8f310UL,
        0x000f18c4ed1fUL,
        0x000f10c0ef1eUL,
        0x000f08cce91dUL,
        0x000f00c8eb1cUL,
        0x000f38d4e51bUL,
        0x000f30d0e71aUL,
        0x000f28dce119UL,
        0x000f20d8e318UL,
        0x000cd9241d67UL,
        0x000cd1201f66UL,
        0x000cc92c1965UL,
        0x000cc1281b64UL,
        0x000cf9341563UL,
        0x000cf1301762UL,
        0x000ce93c1161UL,
        0x000ce1381360UL,
        0x000c99040d6fUL,
        0x000c91000f6eUL,
        0x000c890c096dUL,
        0x000c81080b6cUL,
        0x000cb914056bUL,
        0x000cb110076aUL,
        0x000ca91c0169UL,
        0x000ca1180368UL,
        0x000c59643d77UL,
        0x000c51603f76UL,
        0x000c496c3975UL,
        0x000c41683b74UL,
        0x000c79743573UL,
        0x000c71703772UL,
        0x000c697c3171UL,
        0x000c61783370UL,
        0x000c19442d7fUL,
        0x000c11402f7eUL,
        0x000c094c297dUL,
        0x000c01482b7cUL,
        0x000c3954257bUL,
        0x000c3150277aUL,
        0x000c295c2179UL,
        0x000c21582378UL,
        0x000dd9a45d47UL,
        0x000dd1a05f46UL,
        0x000dc9ac5945UL,
        0x000dc1a85b44UL,
        0x000df9b45543UL,
        0x000df1b05742UL,
        0x000de9bc5141UL,
        0x000de1b85340UL,
        0x000d99844d4fUL,
        0x000d91804f4eUL,
        0x000d898c494dUL,
        0x000d81884b4cUL,
        0x000db994454bUL,
        0x000db190474aUL,
        0x000da99c4149UL,
        0x000da1984348UL,
        0x000d59e47d57UL,
        0x000d51e07f56UL,
        0x000d49ec7955UL,
        0x000d41e87b54UL,
        0x000d79f47553UL,
        0x000d71f07752UL,
        0x000d69fc7151UL,
        0x000d61f87350UL,
        0x000d19c46d5fUL,
        0x000d11c06f5eUL,
        0x000d09cc695dUL,
        0x000d01c86b5cUL,
        0x000d39d4655bUL,
        0x000d31d0675aUL,
        0x000d29dc6159UL,
        0x000d21d86358UL,
        0x000ada259da7UL,
        0x000ad2219fa6UL,
        0x000aca2d99a5UL,
        0x000ac2299ba4UL,
        0x000afa3595a3UL,
        0x000af23197a2UL,
        0x000aea3d91a1UL,
        0x000ae23993a0UL,
        0x000a9a058dafUL,
        0x000a92018faeUL,
        0x000a8a0d89adUL,
        0x000a82098bacUL,
        0x000aba1585abUL,
        0x000ab21187aaUL,
        0x000aaa1d81a9UL,
        0x000aa21983a8UL,
        0x000a5a65bdb7UL,
        0x000a5261bfb6UL,
        0x000a4a6db9b5UL,
        0x000a4269bbb4UL,
        0x000a7a75b5b3UL,
        0x000a7271b7b2UL,
        0x000a6a7db1b1UL,
        0x000a6279b3b0UL,
        0x000a1a45adbfUL,
        0x000a1241afbeUL,
        0x000a0a4da9bdUL,
        0x000a0249abbcUL,
        0x000a3a55a5bbUL,
        0x000a3251a7baUL,
        0x000a2a5da1b9UL,
        0x000a2259a3b8UL,
        0x000bdaa5dd87UL,
        0x000bd2a1df86UL,
        0x000bcaadd985UL,
        0x000bc2a9db84UL,
        0x000bfab5d583UL,
        0x000bf2b1d782UL,
        0x000beabdd181UL,
        0x000be2b9d380UL,
        0x000b9a85cd8fUL,
        0x000b9281cf8eUL,
        0x000b8a8dc98dUL,
        0x000b8289cb8cUL,
        0x000bba95c58bUL,
        0x000bb291c78aUL,
        0x000baa9dc189UL,
        0x000ba299c388UL,
        0x000b5ae5fd97UL,
        0x000b52e1ff96UL,
        0x000b4aedf995UL,
        0x000b42e9fb94UL,
        0x000b7af5f593UL,
        0x000b72f1f792UL,
        0x000b6afdf191UL,
        0x000b62f9f390UL,
        0x000b1ac5ed9fUL,
        0x000b12c1ef9eUL,
        0x000b0acde99dUL,
        0x000b02c9eb9cUL,
        0x000b3ad5e59bUL,
        0x000b32d1e79aUL,
        0x000b2adde199UL,
        0x000b22d9e398UL,
        0x0008db251de7UL,
        0x0008d3211fe6UL,
        0x0008cb2d19e5UL,
        0x0008c3291be4UL,
        0x0008fb3515e3UL,
        0x0008f33117e2UL,
        0x0008eb3d11e1UL,
        0x0008e33913e0UL,
        0x00089b050defUL,
        0x000893010feeUL,
        0x00088b0d09edUL,
        0x000883090becUL,
        0x0008bb1505ebUL,
        0x0008b31107eaUL,
        0x0008ab1d01e9UL,
        0x0008a31903e8UL,
        0x00085b653df7UL,
        0x000853613ff6UL,
        0x00084b6d39f5UL,
        0x000843693bf4UL,
        0x00087b7535f3UL,
        0x0008737137f2UL,
        0x00086b7d31f1UL,
        0x0008637933f0UL,
        0x00081b452dffUL,
        0x000813412ffeUL,
        0x00080b4d29fdUL,
        0x000803492bfcUL,
        0x00083b5525fbUL,
        0x0008335127faUL,
        0x00082b5d21f9UL,
        0x0008235923f8UL,
        0x0009dba55dc7UL,
        0x0009d3a15fc6UL,
        0x0009cbad59c5UL,
        0x0009c3a95bc4UL,
        0x0009fbb555c3UL,
        0x0009f3b157c2UL,
        0x0009ebbd51c1UL,
        0x0009e3b953c0UL,
        0x00099b854dcfUL,
        0x000993814fceUL,
        0x00098b8d49cdUL,
        0x000983894bccUL,
        0x0009bb9545cbUL,
        0x0009b39147caUL,
        0x0009ab9d41c9UL,
        0x0009a39943c8UL,
        0x00095be57dd7UL,
        0x000953e17fd6UL,
        0x00094bed79d5UL,
        0x000943e97bd4UL,
        0x00097bf575d3UL,
        0x000973f177d2UL,
        0x00096bfd71d1UL,
        0x000963f973d0UL,
        0x00091bc56ddfUL,
        0x000913c16fdeUL,
        0x00090bcd69ddUL,
        0x000903c96bdcUL,
        0x00093bd565dbUL,
        0x000933d167daUL,
        0x00092bdd61d9UL,
        0x000923d963d8UL
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "union_find.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct unionfind unionfind_t;

struct unionfind
{
    struct ufrec *data;
};

struct ufrec
{
    // the parent of this node. If a node's parent is its own index,
    // then it is a root.
#ifdef IMLIB_ENABLE_HIGH_RES_APRILTAGS
    uint32_t parent;
#else
    uint16_t parent;
#endif
};

static inline unionfind_t *unionfind_create(uint32_t maxid)
{
    unionfind_t *uf = (unionfind_t*) fb_alloc(sizeof(unionfind_t), FB_ALLOC_NO_HINT);
    uf->data = (struct ufrec*) fb_alloc((maxid+1) * sizeof(struct ufrec), FB_ALLOC_NO_HINT);
    for (int i = 0; i <= maxid; i++) {
        uf->data[i].parent = i;
    }
    return uf;
}

static inline void unionfind_destroy()
{
    fb_free();
    fb_free();
}

/*
static inline uint32_t unionfind_get_representative(unionfind_t *uf, uint32_t id)
{
    // base case: a node is its own parent
    if (uf->data[id].parent == id)
        return id;

    // otherwise, recurse
    uint32_t root = unionfind_get_representative(uf, uf->data[id].parent);

    // short circuit the path. [XXX This write prevents tail recursion]
    uf->data[id].parent = root;

    return root;
}
*/

// this one seems to be every-so-slightly faster than the recursive
// version above.
static inline uint32_t unionfind_get_representative(unionfind_t *uf, uint32_t id)
{
    uint32_t root = id;

    // chase down the root
    while (uf->data[root].parent != root) {
        root = uf->data[root].parent;
    }

    // go back and collapse the tree.
    //
    // XXX: on some of our workloads that have very shallow trees
    // (e.g. image segmentation), we are actually faster not doing
    // this...
    while (uf->data[id].parent != root) {
        uint32_t tmp = uf->data[id].parent;
        uf->data[id].parent = root;
        id = tmp;
    }

    return root;
}

static inline uint32_t unionfind_connect(unionfind_t *uf, uint32_t aid, uint32_t bid)
{
    uint32_t aroot = unionfind_get_representative(uf, aid);
    uint32_t broot = unionfind_get_representative(uf, bid);

    if (aroot != broot)
        uf->data[broot].parent = aroot;

    return aroot;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "union_find.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "apriltag_quad_thresh.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

// limitation: image size must be <32768 in width and height. This is
// because we use a fixed-point 16 bit integer representation with one
// fractional bit.

static inline uint32_t u64hash_2(uint64_t x) {
    return (2654435761 * x) >> 32;
    return (uint32_t) x;
}

struct uint32_zarray_entry
{
    uint32_t id;
    zarray_t *cluster;

    struct uint32_zarray_entry *next;
};

#ifndef M_PI
# define M_PI 3.141592653589793238462643383279502884196
#endif

struct pt
{
    // Note: these represent 2*actual value.
    uint16_t x, y;
    float theta;
    int16_t gx, gy;
};

struct remove_vertex
{
    int i;           // which vertex to remove?
    int left, right; // left vertex, right vertex

    float err;
};

struct segment
{
    int is_vertex;

    // always greater than zero, but right can be > size, which denotes
    // a wrap around back to the beginning of the points. and left < right.
    int left, right;
};

struct line_fit_pt
{
    float Mx, My;
    float Mxx, Myy, Mxy;
    float W; // total weight
};

static inline void ptsort(struct pt *pts, int sz)
{
#define MAYBE_SWAP(arr,apos,bpos)                                   \
    if (arr[apos].theta > arr[bpos].theta) {                        \
        tmp = arr[apos]; arr[apos] = arr[bpos]; arr[bpos] = tmp;    \
    };

    if (sz <= 1)
        return;

    if (sz == 2) {
        struct pt tmp;
        MAYBE_SWAP(pts, 0, 1);
        return;
    }

    // NB: Using less-branch-intensive sorting networks here on the
    // hunch that it's better for performance.
    if (sz == 3) { // 3 element bubble sort is optimal
        struct pt tmp;
        MAYBE_SWAP(pts, 0, 1);
        MAYBE_SWAP(pts, 1, 2);
        MAYBE_SWAP(pts, 0, 1);
        return;
    }

    if (sz == 4) { // 4 element optimal sorting network.
        struct pt tmp;
        MAYBE_SWAP(pts, 0, 1); // sort each half, like a merge sort
        MAYBE_SWAP(pts, 2, 3);
        MAYBE_SWAP(pts, 0, 2); // minimum value is now at 0.
        MAYBE_SWAP(pts, 1, 3); // maximum value is now at end.
        MAYBE_SWAP(pts, 1, 2); // that only leaves the middle two.
        return;
    }

    if (sz == 5) {
        // this 9-step swap is optimal for a sorting network, but two
        // steps slower than a generic sort.
        struct pt tmp;
        MAYBE_SWAP(pts, 0, 1); // sort each half (3+2), like a merge sort
        MAYBE_SWAP(pts, 3, 4);
        MAYBE_SWAP(pts, 1, 2);
        MAYBE_SWAP(pts, 0, 1);
        MAYBE_SWAP(pts, 0, 3); // minimum element now at 0
        MAYBE_SWAP(pts, 2, 4); // maximum element now at end
        MAYBE_SWAP(pts, 1, 2); // now resort the three elements 1-3.
        MAYBE_SWAP(pts, 2, 3);
        MAYBE_SWAP(pts, 1, 2);
        return;
    }

#undef MAYBE_SWAP

    // a merge sort with temp storage.

    struct pt *tmp = fb_alloc(sizeof(struct pt) * sz, FB_ALLOC_NO_HINT);

    memcpy(tmp, pts, sizeof(struct pt) * sz);

    int asz = sz/2;
    int bsz = sz - asz;

    struct pt *as = &tmp[0];
    struct pt *bs = &tmp[asz];

    ptsort(as, asz);
    ptsort(bs, bsz);

#define MERGE(apos,bpos)                        \
    if (as[apos].theta < bs[bpos].theta)        \
        pts[outpos++] = as[apos++];             \
    else                                        \
        pts[outpos++] = bs[bpos++];

    int apos = 0, bpos = 0, outpos = 0;
    while (apos + 8 < asz && bpos + 8 < bsz) {
        MERGE(apos,bpos); MERGE(apos,bpos); MERGE(apos,bpos); MERGE(apos,bpos);
        MERGE(apos,bpos); MERGE(apos,bpos); MERGE(apos,bpos); MERGE(apos,bpos);
    }

    while (apos < asz && bpos < bsz) {
        MERGE(apos,bpos);
    }

    if (apos < asz)
        memcpy(&pts[outpos], &as[apos], (asz-apos)*sizeof(struct pt));
    if (bpos < bsz)
        memcpy(&pts[outpos], &bs[bpos], (bsz-bpos)*sizeof(struct pt));

    fb_free(); // tmp

#undef MERGE
}

// lfps contains *cumulative* moments for N points, with
// index j reflecting points [0,j] (inclusive).
//
// fit a line to the points [i0, i1] (inclusive). i0, i1 are both [0,
// sz) if i1 < i0, we treat this as a wrap around.
void fit_line(struct line_fit_pt *lfps, int sz, int i0, int i1, float *lineparm, float *err, float *mse)
{
    assert(i0 != i1);
    assert(i0 >= 0 && i1 >= 0 && i0 < sz && i1 < sz);

    float Mx, My, Mxx, Myy, Mxy, W;
    int N; // how many points are included in the set?

    if (i0 < i1) {
        N = i1 - i0 + 1;

        Mx  = lfps[i1].Mx;
        My  = lfps[i1].My;
        Mxx = lfps[i1].Mxx;
        Mxy = lfps[i1].Mxy;
        Myy = lfps[i1].Myy;
        W   = lfps[i1].W;

        if (i0 > 0) {
            Mx  -= lfps[i0-1].Mx;
            My  -= lfps[i0-1].My;
            Mxx -= lfps[i0-1].Mxx;
            Mxy -= lfps[i0-1].Mxy;
            Myy -= lfps[i0-1].Myy;
            W   -= lfps[i0-1].W;
        }

    } else {
        // i0 > i1, e.g. [15, 2]. Wrap around.
        assert(i0 > 0);

        Mx  = lfps[sz-1].Mx   - lfps[i0-1].Mx;
        My  = lfps[sz-1].My   - lfps[i0-1].My;
        Mxx = lfps[sz-1].Mxx  - lfps[i0-1].Mxx;
        Mxy = lfps[sz-1].Mxy  - lfps[i0-1].Mxy;
        Myy = lfps[sz-1].Myy  - lfps[i0-1].Myy;
        W   = lfps[sz-1].W    - lfps[i0-1].W;

        Mx  += lfps[i1].Mx;
        My  += lfps[i1].My;
        Mxx += lfps[i1].Mxx;
        Mxy += lfps[i1].Mxy;
        Myy += lfps[i1].Myy;
        W   += lfps[i1].W;

        N = sz - i0 + i1 + 1;
    }

    assert(N >= 2);

    float Ex = Mx / W;
    float Ey = My / W;
    float Cxx = Mxx / W - Ex*Ex;
    float Cxy = Mxy / W - Ex*Ey;
    float Cyy = Myy / W - Ey*Ey;

    float nx, ny;

    if (1) {
        // on iOS about 5% of total CPU spent in these trig functions.
        // 85 ms per frame on 5S, example.pnm
        //
        // XXX this was using the float-precision atan2. Was there a case where
        // we needed that precision? Seems doubtful.
        float normal_theta = .5 * atan2f(-2*Cxy, (Cyy - Cxx));
        nx = cosf(normal_theta);
        ny = sinf(normal_theta);
    } else {
        // 73.5 ms per frame on 5S, example.pnm
        float ty = -2*Cxy;
        float tx = (Cyy - Cxx);
        float mag = ty*ty + tx*tx;

        if (mag == 0) {
            nx = 1;
            ny = 0;
        } else {
            float norm = sqrtf(ty*ty + tx*tx);
            tx /= norm;

            // ty is now sin(2theta)
            // tx is now cos(2theta). We want sin(theta) and cos(theta)

            // due to precision err, tx could still have slightly too large magnitude.
            if (tx > 1) {
                ny = 0;
                nx = 1;
            } else if (tx < -1) {
                ny = 1;
                nx = 0;
            } else {
                // half angle formula
                ny = sqrtf((1 - tx)/2);
                nx = sqrtf((1 + tx)/2);

                // pick a consistent branch cut
                if (ty < 0)
                    ny = - ny;
            }
        }
    }

    if (lineparm) {
        lineparm[0] = Ex;
        lineparm[1] = Ey;
        lineparm[2] = nx;
        lineparm[3] = ny;
    }

    // sum of squared errors =
    //
    // SUM_i ((p_x - ux)*nx + (p_y - uy)*ny)^2
    // SUM_i  nx*nx*(p_x - ux)^2 + 2nx*ny(p_x -ux)(p_y-uy) + ny*ny*(p_y-uy)*(p_y-uy)
    //  nx*nx*SUM_i((p_x -ux)^2) + 2nx*ny*SUM_i((p_x-ux)(p_y-uy)) + ny*ny*SUM_i((p_y-uy)^2)
    //
    //  nx*nx*N*Cxx + 2nx*ny*N*Cxy + ny*ny*N*Cyy

    // sum of squared errors
    if (err)
        *err = nx*nx*N*Cxx + 2*nx*ny*N*Cxy + ny*ny*N*Cyy;

    // mean squared error
    if (mse)
        *mse = nx*nx*Cxx + 2*nx*ny*Cxy + ny*ny*Cyy;
}

int pt_compare_theta(const void *_a, const void *_b)
{
    struct pt *a = (struct pt*) _a;
    struct pt *b = (struct pt*) _b;

    return (a->theta < b->theta) ? -1 : 1;
}

int err_compare_descending(const void *_a, const void *_b)
{
    const float *a =  _a;
    const float *b =  _b;

    return ((*a) < (*b)) ? 1 : -1;
}

/*

  1. Identify A) white points near a black point and B) black points near a white point.

  2. Find the connected components within each of the classes above,
  yielding clusters of "white-near-black" and
  "black-near-white". (These two classes are kept separate). Each
  segment has a unique id.

  3. For every pair of "white-near-black" and "black-near-white"
  clusters, find the set of points that are in one and adjacent to the
  other. In other words, a "boundary" layer between the two
  clusters. (This is actually performed by iterating over the pixels,
  rather than pairs of clusters.) Critically, this helps keep nearby
  edges from becoming connected.
*/
int quad_segment_maxima(apriltag_detector_t *td, zarray_t *cluster, struct line_fit_pt *lfps, int indices[4])
{
    int sz = zarray_size(cluster);

    // ksz: when fitting points, how many points on either side do we consider?
    // (actual "kernel" width is 2ksz).
    //
    // This value should be about: 0.5 * (points along shortest edge).
    //
    // If all edges were equally-sized, that would give a value of
    // sz/8. We make it somewhat smaller to account for tags at high
    // aspects.

    // XXX Tunable. Maybe make a multiple of JPEG block size to increase robustness
    // to JPEG compression artifacts?
    int ksz = imin(20, sz / 12);

    // can't fit a quad if there are too few points.
    if (ksz < 2)
        return 0;

//    printf("sz %5d, ksz %3d\n", sz, ksz);

    float *errs = fb_alloc(sz * sizeof(float), FB_ALLOC_NO_HINT);

    for (int i = 0; i < sz; i++) {
        fit_line(lfps, sz, (i + sz - ksz) % sz, (i + ksz) % sz, NULL, &errs[i], NULL);
    }

    // apply a low-pass filter to errs
    if (1) {
        float *y = fb_alloc(sz * sizeof(float), FB_ALLOC_NO_HINT);

        // how much filter to apply?

        // XXX Tunable
        float sigma = 1; // was 3

        // cutoff = exp(-j*j/(2*sigma*sigma));
        // log(cutoff) = -j*j / (2*sigma*sigma)
        // log(cutoff)*2*sigma*sigma = -j*j;

        // how big a filter should we use? We make our kernel big
        // enough such that we represent any values larger than
        // 'cutoff'.

        // XXX Tunable (though not super useful to change)
        float cutoff = 0.05;
        int fsz = sqrt(-log(cutoff)*2*sigma*sigma) + 1;
        fsz = 2*fsz + 1;

        // For default values of cutoff = 0.05, sigma = 3,
        // we have fsz = 17.
        float *f = fb_alloc(fsz * sizeof(float), FB_ALLOC_NO_HINT);

        for (int i = 0; i < fsz; i++) {
            int j = i - fsz / 2;
            f[i] = exp(-j*j/(2*sigma*sigma));
        }

        for (int iy = 0; iy < sz; iy++) {
            float acc = 0;
#ifdef OPTIMIZED
            int index = (iy - fsz/2 + sz) % sz;
            for (int i = 0; i < fsz; i++) {
                acc += errs[index] * f[i];
                index++;
                if (index >= sz) // faster to compare than divide (%)
                   index -= sz;
            }
#else
            for (int i = 0; i < fsz; i++) {
                acc += errs[(iy + i - fsz / 2 + sz) % sz] * f[i];
            }
#endif
            y[iy] = acc;
        }

        fb_free(); // f
        memcpy(errs, y, sz * sizeof(float));
        fb_free(); // y
    }

    int *maxima = fb_alloc(sz * sizeof(int), FB_ALLOC_NO_HINT);
    float *maxima_errs = fb_alloc(sz * sizeof(float), FB_ALLOC_NO_HINT);
    int nmaxima = 0;

    for (int i = 0; i < sz; i++) {
        if (errs[i] > errs[(i+1)%sz] && errs[i] > errs[(i+sz-1)%sz]) {
            maxima[nmaxima] = i;
            maxima_errs[nmaxima] = errs[i];
            nmaxima++;
        }
    }

    // if we didn't get at least 4 maxima, we can't fit a quad.
    if (nmaxima < 4)
        return 0;

    // select only the best maxima if we have too many
    int max_nmaxima = td->qtp.max_nmaxima;

    if (nmaxima > max_nmaxima) {
        float *maxima_errs_copy = fb_alloc(nmaxima * sizeof(float), FB_ALLOC_NO_HINT);
        memcpy(maxima_errs_copy, maxima_errs, nmaxima * sizeof(float));

        // throw out all but the best handful of maxima. Sorts descending.
        qsort(maxima_errs_copy, nmaxima, sizeof(float), err_compare_descending);

        float maxima_thresh = maxima_errs_copy[max_nmaxima];
        int out = 0;
        for (int in = 0; in < nmaxima; in++) {
            if (maxima_errs[in] <= maxima_thresh)
                continue;
            maxima[out++] = maxima[in];
        }
        nmaxima = out;

        fb_free(); // maxima_errs_copy
    }

    fb_free(); // maxima_errs
    fb_free(); // maxima
    fb_free(); // errs

    int best_indices[4];
    float best_error = HUGE_VALF;

    float err01, err12, err23, err30;
    float mse01, mse12, mse23, mse30;
    float params01[4], params12[4], params23[4], params30[4];

    // disallow quads where the angle is less than a critical value.
    float max_dot = cos(td->qtp.critical_rad); //25*M_PI/180);

    for (int m0 = 0; m0 < nmaxima - 3; m0++) {
        int i0 = maxima[m0];

        for (int m1 = m0+1; m1 < nmaxima - 2; m1++) {
            int i1 = maxima[m1];

            fit_line(lfps, sz, i0, i1, params01, &err01, &mse01);

            if (mse01 > td->qtp.max_line_fit_mse)
                continue;

            for (int m2 = m1+1; m2 < nmaxima - 1; m2++) {
                int i2 = maxima[m2];

                fit_line(lfps, sz, i1, i2, params12, &err12, &mse12);
                if (mse12 > td->qtp.max_line_fit_mse)
                    continue;

                float dot = params01[2]*params12[2] + params01[3]*params12[3];
                if (fabs(dot) > max_dot)
                    continue;

                for (int m3 = m2+1; m3 < nmaxima; m3++) {
                    int i3 = maxima[m3];

                    fit_line(lfps, sz, i2, i3, params23, &err23, &mse23);
                    if (mse23 > td->qtp.max_line_fit_mse)
                        continue;

                    fit_line(lfps, sz, i3, i0, params30, &err30, &mse30);
                    if (mse30 > td->qtp.max_line_fit_mse)
                        continue;

                    float err = err01 + err12 + err23 + err30;
                    if (err < best_error) {
                        best_error = err;
                        best_indices[0] = i0;
                        best_indices[1] = i1;
                        best_indices[2] = i2;
                        best_indices[3] = i3;
                    }
                }
            }
        }
    }

    if (best_error == HUGE_VALF)
        return 0;

    for (int i = 0; i < 4; i++)
        indices[i] = best_indices[i];

    if (best_error / sz < td->qtp.max_line_fit_mse)
        return 1;
    return 0;
}

// return 1 if the quad looks okay, 0 if it should be discarded
int fit_quad(apriltag_detector_t *td, image_u8_t *im, zarray_t *cluster, struct quad *quad, bool overrideMode)
{
    int res = 0;

    int sz = zarray_size(cluster);
    if (sz < 4) // can't fit a quad to less than 4 points
        return 0;

    /////////////////////////////////////////////////////////////
    // Step 1. Sort points so they wrap around the center of the
    // quad. We will constrain our quad fit to simply partition this
    // ordered set into 4 groups.

    // compute a bounding box so that we can order the points
    // according to their angle WRT the center.
    int32_t xmax = 0, xmin = INT32_MAX, ymax = 0, ymin = INT32_MAX;

    for (int pidx = 0; pidx < zarray_size(cluster); pidx++) {
        struct pt *p;
        zarray_get_volatile(cluster, pidx, &p);

        xmax = imax(xmax, p->x);
        xmin = imin(xmin, p->x);

        ymax = imax(ymax, p->y);
        ymin = imin(ymin, p->y);
    }

    // add some noise to (cx,cy) so that pixels get a more diverse set
    // of theta estimates. This will help us remove more points.
    // (Only helps a small amount. The actual noise values here don't
    // matter much at all, but we want them [-1, 1]. (XXX with
    // fixed-point, should range be bigger?)
    float cx = (xmin + xmax) * 0.5 + 0.05118;
    float cy = (ymin + ymax) * 0.5 + -0.028581;

    float dot = 0;

    for (int pidx = 0; pidx < zarray_size(cluster); pidx++) {
        struct pt *p;
        zarray_get_volatile(cluster, pidx, &p);

        float dx = p->x - cx;
        float dy = p->y - cy;

        p->theta = atan2f(dy, dx);

        dot += dx*p->gx + dy*p->gy;
//        p->theta = terrible_atan2(dy, dx);
    }

    // Ensure that the black border is inside the white border.
    if ((!overrideMode) && (dot < 0))
        return 0;

    // we now sort the points according to theta. This is a prepatory
    // step for segmenting them into four lines.
    if (1) {
        //        zarray_sort(cluster, pt_compare_theta);
        ptsort((struct pt*) cluster->data, zarray_size(cluster));

        // remove duplicate points. (A byproduct of our segmentation system.)
        if (1) {
            int outpos = 1;

            struct pt *last;
            zarray_get_volatile(cluster, 0, &last);

            for (int i = 1; i < sz; i++) {

                struct pt *p;
                zarray_get_volatile(cluster, i, &p);

                if (p->x != last->x || p->y != last->y) {

                    if (i != outpos)  {
                        struct pt *out;
                        zarray_get_volatile(cluster, outpos, &out);
                        memcpy(out, p, sizeof(struct pt));
                    }

                    outpos++;
                }

                last = p;
            }

            cluster->size = outpos;
            sz = outpos;
        }

    } else {
        // This is a counting sort in which we retain at most one
        // point for every bucket; the bucket index is computed from
        // theta. Since a good quad completes a complete revolution,
        // there's reason to think that we should get a good
        // distribution of thetas.  We might "lose" a few points due
        // to collisions, but this shouldn't affect quality very much.

        // XXX tunable. Increase to reduce the likelihood of "losing"
        // points due to collisions.
        int nbuckets = 4*sz;

#define ASSOC 2
        struct pt v[nbuckets][ASSOC];
        memset(v, 0, sizeof(v));

        // put each point into a bucket.
        for (int i = 0; i < sz; i++) {
            struct pt *p;
            zarray_get_volatile(cluster, i, &p);

            assert(p->theta >= -M_PI && p->theta <= M_PI);

            int bucket = (nbuckets - 1) * (p->theta + M_PI) / (2*M_PI);
            assert(bucket >= 0 && bucket < nbuckets);

            for (int i = 0; i < ASSOC; i++) {
                if (v[bucket][i].theta == 0) {
                    v[bucket][i] = *p;
                    break;
                }
            }
        }

        // collect the points from the buckets and put them back into the array.
        int outsz = 0;
        for (int i = 0; i < nbuckets; i++) {
            for (int j = 0; j < ASSOC; j++) {
                if (v[i][j].theta != 0) {
                    zarray_set(cluster, outsz, &v[i][j], NULL);
                    outsz++;
                }
            }
        }

        zarray_truncate(cluster, outsz);
        sz = outsz;
    }

    if (sz < 4)
        return 0;

    /////////////////////////////////////////////////////////////
    // Step 2. Precompute statistics that allow line fit queries to be
    // efficiently computed for any contiguous range of indices.

    struct line_fit_pt *lfps = fb_alloc0(sz * sizeof(struct line_fit_pt), FB_ALLOC_NO_HINT);

    for (int i = 0; i < sz; i++) {
        struct pt *p;
        zarray_get_volatile(cluster, i, &p);

        if (i > 0) {
            memcpy(&lfps[i], &lfps[i-1], sizeof(struct line_fit_pt));
        }

        if (0) {
            // we now undo our fixed-point arithmetic.
            float delta = 0.5;
            float x = p->x * .5 + delta;
            float y = p->y * .5 + delta;
            float W;

            for (int dy = -1; dy <= 1; dy++) {
                int iy = y + dy;

                if (iy < 0 || iy + 1 >= im->height)
                    continue;

                for (int dx = -1; dx <= 1; dx++) {
                    int ix = x + dx;

                    if (ix < 0 || ix + 1 >= im->width)
                        continue;

                    int grad_x = im->buf[iy * im->stride + ix + 1] -
                        im->buf[iy * im->stride + ix - 1];

                    int grad_y = im->buf[(iy+1) * im->stride + ix] -
                        im->buf[(iy-1) * im->stride + ix];

                    W = sqrtf(grad_x*grad_x + grad_y*grad_y) + 1;

//                    float fx = x + dx, fy = y + dy;
                    float fx = ix + .5, fy = iy + .5;
                    lfps[i].Mx  += W * fx;
                    lfps[i].My  += W * fy;
                    lfps[i].Mxx += W * fx * fx;
                    lfps[i].Mxy += W * fx * fy;
                    lfps[i].Myy += W * fy * fy;
                    lfps[i].W   += W;
                }
            }
        } else {
            // we now undo our fixed-point arithmetic.
            float delta = 0.5; // adjust for pixel center bias
            float x = p->x * .5 + delta;
            float y = p->y * .5 + delta;
            int ix = x, iy = y;
            float W = 1;

            if (ix > 0 && ix+1 < im->width && iy > 0 && iy+1 < im->height) {
                int grad_x = im->buf[iy * im->stride + ix + 1] -
                    im->buf[iy * im->stride + ix - 1];

                int grad_y = im->buf[(iy+1) * im->stride + ix] -
                    im->buf[(iy-1) * im->stride + ix];

                // XXX Tunable. How to shape the gradient magnitude?
                W = sqrt(grad_x*grad_x + grad_y*grad_y) + 1;
            }

            float fx = x, fy = y;
            lfps[i].Mx  += W * fx;
            lfps[i].My  += W * fy;
            lfps[i].Mxx += W * fx * fx;
            lfps[i].Mxy += W * fx * fy;
            lfps[i].Myy += W * fy * fy;
            lfps[i].W   += W;
        }
    }

    int indices[4];
    if (1) {
        if (!quad_segment_maxima(td, cluster, lfps, indices))
            goto finish;
    }

//    printf("%d %d %d %d\n", indices[0], indices[1], indices[2], indices[3]);

    if (0) {
        // no refitting here; just use those points as the vertices.
        // Note, this is useful for debugging, but pretty bad in
        // practice since this code path also omits several
        // plausibility checks that save us tons of time in quad
        // decoding.
        for (int i = 0; i < 4; i++) {
            struct pt *p;
            zarray_get_volatile(cluster, indices[i], &p);

            quad->p[i][0] = .5*p->x; // undo fixed-point arith.
            quad->p[i][1] = .5*p->y;
        }

        res = 1;

    } else {
        float lines[4][4];

        for (int i = 0; i < 4; i++) {
            int i0 = indices[i];
            int i1 = indices[(i+1)&3];

            if (0) {
                // if there are enough points, skip the points near the corners
                // (because those tend not to be very good.)
                if (i1-i0 > 8) {
                    int t = (i1-i0)/6;
                    if (t < 0)
                        t = -t;

                    i0 = (i0 + t) % sz;
                    i1 = (i1 + sz - t) % sz;
                }
            }

            float err;
            fit_line(lfps, sz, i0, i1, lines[i], NULL, &err);

            if (err > td->qtp.max_line_fit_mse) {
                res = 0;
                goto finish;
            }
        }

        for (int i = 0; i < 4; i++) {
            // solve for the intersection of lines (i) and (i+1)&3.
            // p0 + lambda0*u0 = p1 + lambda1*u1, where u0 and u1
            // are the line directions.
            //
            // lambda0*u0 - lambda1*u1 = (p1 - p0)
            //
            // rearrange (solve for lambdas)
            //
            // [u0_x   -u1_x ] [lambda0] = [ p1_x - p0_x ]
            // [u0_y   -u1_y ] [lambda1]   [ p1_y - p0_y ]
            //
            // remember that lines[i][0,1] = p, lines[i][2,3] = NORMAL vector.
            // We want the unit vector, so we need the perpendiculars. Thus, below
            // we have swapped the x and y components and flipped the y components.

            float A00 =  lines[i][3],  A01 = -lines[(i+1)&3][3];
            float A10 =  -lines[i][2],  A11 = lines[(i+1)&3][2];
            float B0 = -lines[i][0] + lines[(i+1)&3][0];
            float B1 = -lines[i][1] + lines[(i+1)&3][1];

            float det = A00 * A11 - A10 * A01;

            // inverse.
            float W00 = A11 / det, W01 = -A01 / det;
            if (fabs(det) < 0.001) {
                res = 0;
                goto finish;
            }

            // solve
            float L0 = W00*B0 + W01*B1;

            // compute intersection
            quad->p[i][0] = lines[i][0] + L0*A00;
            quad->p[i][1] = lines[i][1] + L0*A10;

            if (0) {
                // we should get the same intersection starting
                // from point p1 and moving L1*u1.
                float W10 = -A10 / det, W11 = A00 / det;
                float L1 = W10*B0 + W11*B1;

                float x = lines[(i+1)&3][0] - L1*A10;
                float y = lines[(i+1)&3][1] - L1*A11;
                assert(fabs(x - quad->p[i][0]) < 0.001 &&
                       fabs(y - quad->p[i][1]) < 0.001);
            }

            res = 1;
        }
    }

    // reject quads that are too small
    if (1) {
        float area = 0;

        // get area of triangle formed by points 0, 1, 2, 0
        float length[3], p;
        for (int i = 0; i < 3; i++) {
            int idxa = i; // 0, 1, 2,
            int idxb = (i+1) % 3; // 1, 2, 0
            length[i] = sqrt(sq(quad->p[idxb][0] - quad->p[idxa][0]) +
                             sq(quad->p[idxb][1] - quad->p[idxa][1]));
        }
        p = (length[0] + length[1] + length[2]) / 2;

        area += sqrt(p*(p-length[0])*(p-length[1])*(p-length[2]));

        // get area of triangle formed by points 2, 3, 0, 2
        for (int i = 0; i < 3; i++) {
            int idxs[] = { 2, 3, 0, 2 };
            int idxa = idxs[i];
            int idxb = idxs[i+1];
            length[i] = sqrt(sq(quad->p[idxb][0] - quad->p[idxa][0]) +
                             sq(quad->p[idxb][1] - quad->p[idxa][1]));
        }
        p = (length[0] + length[1] + length[2]) / 2;

        area += sqrt(p*(p-length[0])*(p-length[1])*(p-length[2]));

        // we don't actually know the family yet (quad detection is generic.)
        // This threshold is based on a 6x6 tag (which is actually 8x8)
//        int d = fam->d + fam->black_border*2;
        int d = 8;
        if (area < d*d) {
            res = 0;
            goto finish;
        }
    }

    // reject quads whose cumulative angle change isn't equal to 2PI
    if (1) {
        float total = 0;

        for (int i = 0; i < 4; i++) {
            int i0 = i, i1 = (i+1)&3, i2 = (i+2)&3;

            float theta0 = atan2f(quad->p[i0][1] - quad->p[i1][1],
                                   quad->p[i0][0] - quad->p[i1][0]);
            float theta1 = atan2f(quad->p[i2][1] - quad->p[i1][1],
                                   quad->p[i2][0] - quad->p[i1][0]);

            float dtheta = theta0 - theta1;
            if (dtheta < 0)
                dtheta += 2*M_PI;

            if (dtheta < td->qtp.critical_rad || dtheta > (M_PI - td->qtp.critical_rad))
                res = 0;

            total += dtheta;
        }

        // looking for 2PI
        if (total < 6.2 || total > 6.4) {
            res = 0;
            goto finish;
        }
    }

    // adjust pixel coordinates; all math up 'til now uses pixel
    // coordinates in which (0,0) is the lower left corner. But each
    // pixel actually spans from to [x, x+1), [y, y+1) the mean value of which
    // is +.5 higher than x & y.
/*    float delta = .5;
      for (int i = 0; i < 4; i++) {
      quad->p[i][0] += delta;
      quad->p[i][1] += delta;
      }
*/
  finish:

    fb_free(); // lfps

    return res;
}

#ifdef OPTIMIZED
#define DO_UNIONFIND(dx, dy) if (im->buf[y*s + dy*s + x + dx] == v) { broot = unionfind_get_representative(uf, y*w + dy*w + x + dx); if (aroot != broot) uf->data[broot].parent = aroot; }

static void do_unionfind_line(unionfind_t *uf, image_u8_t *im, int h, int w, int s, int y)
{
    assert(y+1 < im->height);
    uint8_t v, *p;
    p = &im->buf[y*s + 1];
    for (int x = 1; x < w - 1; x++) {
        v = *p++; //im->buf[y*s + x];

        if (v == 127)
            continue;
        uint32_t broot;
        uint32_t aroot = unionfind_get_representative(uf, y*w+x);
        // (dx,dy) pairs for 8 connectivity:
        //          (REFERENCE) (1, 0)
        // (-1, 1)    (0, 1)    (1, 1)
        //
        DO_UNIONFIND(1, 0);
        DO_UNIONFIND(0, 1);
        if (v == 255) {
            DO_UNIONFIND(-1, 1);
            DO_UNIONFIND(1, 1);
        }
    }
}
#else // not optimized
#define DO_UNIONFIND(dx, dy) if (im->buf[y*s + dy*s + x + dx] == v) unionfind_connect(uf, y*w + x, y*w + dy*w + x + dx);

static void do_unionfind_line(unionfind_t *uf, image_u8_t *im, int h, int w, int s, int y)
{
    assert(y+1 < im->height);

    for (int x = 1; x < w - 1; x++) {
        uint8_t v = im->buf[y*s + x];

        if (v == 127)
            continue;

        // (dx,dy) pairs for 8 connectivity:
        //          (REFERENCE) (1, 0)
        // (-1, 1)    (0, 1)    (1, 1)
        //
        DO_UNIONFIND(1, 0);
        DO_UNIONFIND(0, 1);
        if (v == 255) {
            DO_UNIONFIND(-1, 1);
            DO_UNIONFIND(1, 1);
        }
    }
}
#undef DO_UNIONFIND
#endif // OPTIMIZED

image_u8_t *threshold(apriltag_detector_t *td, image_u8_t *im)
{
    int w = im->width, h = im->height, s = im->stride;
    assert(w < 32768);
    assert(h < 32768);

    image_u8_t *threshim = fb_alloc(sizeof(image_u8_t), FB_ALLOC_NO_HINT);
    threshim->width = w;
    threshim->height = h;
    threshim->stride = s;
    threshim->buf = fb_alloc(w * h, FB_ALLOC_NO_HINT);
    assert(threshim->stride == s);

    // The idea is to find the maximum and minimum values in a
    // window around each pixel. If it's a contrast-free region
    // (max-min is small), don't try to binarize. Otherwise,
    // threshold according to (max+min)/2.
    //
    // Mark low-contrast regions with value 127 so that we can skip
    // future work on these areas too.

    // however, computing max/min around every pixel is needlessly
    // expensive. We compute max/min for tiles. To avoid artifacts
    // that arise when high-contrast features appear near a tile
    // edge (and thus moving from one tile to another results in a
    // large change in max/min value), the max/min values used for
    // any pixel are computed from all 3x3 surrounding tiles. Thus,
    // the max/min sampling area for nearby pixels overlap by at least
    // one tile.
    //
    // The important thing is that the windows be large enough to
    // capture edge transitions; the tag does not need to fit into
    // a tile.

    // XXX Tunable. Generally, small tile sizes--- so long as they're
    // large enough to span a single tag edge--- seem to be a winner.
    const int tilesz = 4;

    // the last (possibly partial) tiles along each row and column will
    // just use the min/max value from the last full tile.
    int tw = w / tilesz;
    int th = h / tilesz;

    uint8_t *im_max = fb_alloc(tw*th*sizeof(uint8_t), FB_ALLOC_NO_HINT);
    uint8_t *im_min = fb_alloc(tw*th*sizeof(uint8_t), FB_ALLOC_NO_HINT);

    // first, collect min/max statistics for each tile
    for (int ty = 0; ty < th; ty++) {
        for (int tx = 0; tx < tw; tx++) {
#if defined( OPTIMIZED ) && (defined(ARM_MATH_CM7) || defined(ARM_MATH_CM4))
        uint32_t tmp, max32 = 0, min32 = 0xffffffff;
        for (int dy=0; dy < tilesz; dy++) {
            uint32_t v = *(uint32_t *)&im->buf[(ty*tilesz+dy)*s + tx*tilesz];
            tmp = __USUB8(v, max32);
            max32 = __SEL(v, max32);
            tmp = __USUB8(min32, v);
            min32 = __SEL(v, min32);
        }
        // find the min/max of the 4 remaining values
        tmp = max32 >> 16;
        __USUB8(max32, tmp); // 4->2
        max32 = __SEL(max32, tmp);
        tmp = max32 >> 8;
        __USUB8(max32, tmp); // 2->1
        max32 = __SEL(max32, tmp);
        tmp = min32 >> 16;
        __USUB8(min32, tmp);
        min32 = __SEL(tmp, min32); // 4-->2
        tmp = min32 >> 8;
        __USUB8(min32, tmp);
        min32 = __SEL(tmp, min32); // 2-->1
        im_max[ty*tw+tx] = (uint8_t)max32;
        im_min[ty*tw+tx] = (uint8_t)min32;
#else
        uint8_t max = 0, min = 255;
        for (int dy = 0; dy < tilesz; dy++) {
            for (int dx = 0; dx < tilesz; dx++) {
                uint8_t v = im->buf[(ty*tilesz+dy)*s + tx*tilesz + dx];
                if (v < min)
                    min = v;
                if (v > max)
                    max = v;
            }
        }
        im_max[ty*tw+tx] = max;
        im_min[ty*tw+tx] = min;
#endif
        }
    }

    // second, apply 3x3 max/min convolution to "blur" these values
    // over larger areas. This reduces artifacts due to abrupt changes
    // in the threshold value.
    if (1) {
        uint8_t *im_max_tmp = fb_alloc(tw*th*sizeof(uint8_t), FB_ALLOC_NO_HINT);
        uint8_t *im_min_tmp = fb_alloc(tw*th*sizeof(uint8_t), FB_ALLOC_NO_HINT);

#ifdef OPTIMIZED
        // Checking boundaries on every pixel wastes significant time; just break it into 5 pieces
        // (center, top, bottom, left right)
        // First pass does the entire center area
        int ty, tx, dy, dx;
        for (ty = 1; ty < th-1; ty++) {
            for (tx = 1; tx < tw-1; tx++) {
                uint8_t max = 0, min = 255;
                for (dy = -1; dy <= 1; dy++) {
                    for (dx = -1; dx <= 1; dx++) {
                        uint8_t m = im_max[(ty+dy)*tw+tx+dx];
                        if (m > max)
                            max = m;
                        m = im_min[(ty+dy)*tw+tx+dx];
                        if (m < min)
                            min = m;
                    }
                }
                im_max_tmp[ty*tw + tx] = max;
                im_min_tmp[ty*tw + tx] = min;
            }
        }
        // top edge
        ty = 0;
        for (tx = 1; tx < tw-1; tx++) {
            uint8_t max = 0, min = 255;
            for (dy = 0; dy <= 1; dy++) {
                for (dx = -1; dx <= 1; dx++) {
                    uint8_t m = im_max[(ty+dy)*tw+tx+dx];
                    if (m > max)
                        max = m;
                    m = im_min[(ty+dy)*tw+tx+dx];
                    if (m < min)
                        min = m;
                }
            }
            im_max_tmp[ty*tw + tx] = max;
            im_min_tmp[ty*tw + tx] = min;
        }
        // bottom edge
        ty = th-1;
        for (tx = 1; tx < tw-1; tx++) {
            uint8_t max = 0, min = 255;
            for (dy = -1; dy <= 0; dy++) {
                for (dx = -1; dx <= 1; dx++) {
                    uint8_t m = im_max[(ty+dy)*tw+tx+dx];
                    if (m > max)
                        max = m;
                    m = im_min[(ty+dy)*tw+tx+dx];
                    if (m < min)
                        min = m;
                }
            }
            im_max_tmp[ty*tw + tx] = max;
            im_min_tmp[ty*tw + tx] = min;
        }
        // left edge
        tx = 0;
        for (ty = 1; ty < th-1; ty++) {
            uint8_t max = 0, min = 255;
            for (dy = -1; dy <= 1; dy++) {
                for (dx = 0; dx <= 1; dx++) {
                    uint8_t m = im_max[(ty+dy)*tw+tx+dx];
                    if (m > max)
                        max = m;
                    m = im_min[(ty+dy)*tw+tx+dx];
                    if (m < min)
                        min = m;
                }
            }
            im_max_tmp[ty*tw + tx] = max;
            im_min_tmp[ty*tw + tx] = min;
        }
        // right edge
        tx = tw-1;
        for (ty = 1; ty < th-1; ty++) {
            uint8_t max = 0, min = 255;
            for (dy = -1; dy <= 1; dy++) {
                for (dx = -1; dx <= 0; dx++) {
                    uint8_t m = im_max[(ty+dy)*tw+tx+dx];
                    if (m > max)
                        max = m;
                    m = im_min[(ty+dy)*tw+tx+dx];
                    if (m < min)
                        min = m;
                }
            }
            im_max_tmp[ty*tw + tx] = max;
            im_min_tmp[ty*tw + tx] = min;
        }
#else
        for (int ty = 0; ty < th; ty++) {
            for (int tx = 0; tx < tw; tx++) {
                uint8_t max = 0, min = 255;

                for (int dy = -1; dy <= 1; dy++) {
                    if (ty+dy < 0 || ty+dy >= th)
                        continue;
                    for (int dx = -1; dx <= 1; dx++) {
                        if (tx+dx < 0 || tx+dx >= tw)
                            continue;

                        uint8_t m = im_max[(ty+dy)*tw+tx+dx];
                        if (m > max)
                            max = m;
                        m = im_min[(ty+dy)*tw+tx+dx];
                        if (m < min)
                            min = m;
                    }
                }

                im_max_tmp[ty*tw + tx] = max;
                im_min_tmp[ty*tw + tx] = min;
            }
        }
#endif
        memcpy(im_max, im_max_tmp, tw*th*sizeof(uint8_t));
        memcpy(im_min, im_min_tmp, tw*th*sizeof(uint8_t));
        fb_free(); // im_min_tmp
        fb_free(); // im_max_tmp
    }
#if defined( OPTIMIZED ) && (defined(ARM_MATH_CM7) || defined(ARM_MATH_CM4))
    if ((s & 0x3) == 0 && tilesz == 4) // if each line is a multiple of 4, we can do this faster
    {
        const uint32_t lowcontrast = 0x7f7f7f7f;
        const int s32 = s/4; // pitch for 32-bit values
        const int minmax = td->qtp.min_white_black_diff; // local var to avoid constant dereferencing of the pointer
        for (int ty = 0; ty < th; ty++) {
            for (int tx = 0; tx < tw; tx++) {

                int min = im_min[ty*tw + tx];
                int max = im_max[ty*tw + tx];

                // low contrast region? (no edges)
                if (max - min < minmax) {
                    uint32_t *d32 = (uint32_t *)&threshim->buf[ty*tilesz*s + tx*tilesz];
                    d32[0] = d32[s32] = d32[s32*2] = d32[s32*3] = lowcontrast;
                    continue;
                } // if low contrast
                    // otherwise, actually threshold this tile.

                    // argument for biasing towards dark; specular highlights
                    // can be substantially brighter than white tag parts
                    uint32_t thresh32 = (min + (max - min) / 2) + 1; // plus 1 to make GT become GE for the __USUB8 and __SEL instructions
                    uint32_t u32tmp;
                    thresh32 *= 0x01010101; // spread value to all 4 slots
                        for (int dy = 0; dy < tilesz; dy++) {
                        uint32_t *d32 = (uint32_t *)&threshim->buf[(ty*tilesz+dy)*s + tx*tilesz];
                            uint32_t *s32 = (uint32_t *)&im->buf[(ty*tilesz+dy)*s + tx*tilesz];
                            // process 4 pixels at a time
                            u32tmp = s32[0];
                            u32tmp = __USUB8(u32tmp, thresh32);
                            u32tmp = __SEL(0xffffffff, 0x00000000); // 4 thresholded pixels
                            d32[0] = u32tmp;
                    } // dy
            } // tx
        } // ty
    }
    else // need to do it the slow way
#endif // OPTIMIZED
    {
    for (int ty = 0; ty < th; ty++) {
        for (int tx = 0; tx < tw; tx++) {

            int min = im_min[ty*tw + tx];
            int max = im_max[ty*tw + tx];

            // low contrast region? (no edges)
            if (max - min < td->qtp.min_white_black_diff) {
                for (int dy = 0; dy < tilesz; dy++) {
                    int y = ty*tilesz + dy;

                    for (int dx = 0; dx < tilesz; dx++) {
                        int x = tx*tilesz + dx;

                        threshim->buf[y*s+x] = 127;
                    }
                }
                continue;
            }

            // otherwise, actually threshold this tile.

            // argument for biasing towards dark; specular highlights
            // can be substantially brighter than white tag parts
            uint8_t thresh = min + (max - min) / 2;

            for (int dy = 0; dy < tilesz; dy++) {
                int y = ty*tilesz + dy;

                for (int dx = 0; dx < tilesz; dx++) {
                    int x = tx*tilesz + dx;

                    uint8_t v = im->buf[y*s+x];
                    if (v > thresh)
                        threshim->buf[y*s+x] = 255;
                    else
                        threshim->buf[y*s+x] = 0;
                }
            }
        }
    }
    }

    // we skipped over the non-full-sized tiles above. Fix those now.
    if (1) {
        for (int y = 0; y < h; y++) {

            // what is the first x coordinate we need to process in this row?

            int x0;

            if (y >= th*tilesz) {
                x0 = 0; // we're at the bottom; do the whole row.
            } else {
                x0 = tw*tilesz; // we only need to do the right most part.
            }

            // compute tile coordinates and clamp.
            int ty = y / tilesz;
            if (ty >= th)
                ty = th - 1;

            for (int x = x0; x < w; x++) {
                int tx = x / tilesz;
                if (tx >= tw)
                    tx = tw - 1;

                int max = im_max[ty*tw + tx];
                int min = im_min[ty*tw + tx];
                int thresh = min + (max - min) / 2;

                uint8_t v = im->buf[y*s+x];
                if (v > thresh)
                    threshim->buf[y*s+x] = 255;
                else
                    threshim->buf[y*s+x] = 0;
            }
        }
    }

    fb_free(); // im_min
    fb_free(); // im_max

    // this is a dilate/erode deglitching scheme that does not improve
    // anything as far as I can tell.
    if (0 || td->qtp.deglitch) {
        image_u8_t *tmp = fb_alloc(sizeof(image_u8_t), FB_ALLOC_NO_HINT);
        tmp->width = w;
        tmp->height = h;
        tmp->stride = s;
        tmp->buf = fb_alloc(w * h, FB_ALLOC_NO_HINT);

        for (int y = 1; y + 1 < h; y++) {
            for (int x = 1; x + 1 < w; x++) {
                uint8_t max = 0;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        uint8_t v = threshim->buf[(y+dy)*s + x + dx];
                        if (v > max)
                            max = v;
                    }
                }
                tmp->buf[y*s+x] = max;
            }
        }

        for (int y = 1; y + 1 < h; y++) {
            for (int x = 1; x + 1 < w; x++) {
                uint8_t min = 255;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        uint8_t v = tmp->buf[(y+dy)*s + x + dx];
                        if (v < min)
                            min = v;
                    }
                }
                threshim->buf[y*s+x] = min;
            }
        }

        fb_free(); // tmp->buf
        fb_free(); // tmp
    }

    return threshim;
}

zarray_t *apriltag_quad_thresh(apriltag_detector_t *td, image_u8_t *im, bool overrideMode)
{
    ////////////////////////////////////////////////////////
    // step 1. threshold the image, creating the edge image.

    int w = im->width, h = im->height;

    image_u8_t *threshim = threshold(td, im);
    int ts = threshim->stride;

    ////////////////////////////////////////////////////////
    // step 2. find connected components.

    unionfind_t *uf = unionfind_create(w * h);

    for (int y = 0; y < h - 1; y++) {
        do_unionfind_line(uf, threshim, h, w, ts, y);
    }

    uint32_t nclustermap;
    struct uint32_zarray_entry **clustermap = fb_alloc0_all(&nclustermap, FB_ALLOC_PREFER_SPEED);
    nclustermap /= sizeof(struct uint32_zarray_entry*);
    if (!nclustermap) fb_alloc_fail();

    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {

            uint8_t v0 = threshim->buf[y*ts + x];
            if (v0 == 127)
                continue;

            // XXX don't query this until we know we need it?
            uint32_t rep0 = unionfind_get_representative(uf, y*w + x);

            // whenever we find two adjacent pixels such that one is
            // white and the other black, we add the point half-way
            // between them to a cluster associated with the unique
            // ids of the white and black regions.
            //
            // We additionally compute the gradient direction (i.e., which
            // direction was the white pixel?) Note: if (v1-v0) == 255, then
            // (dx,dy) points towards the white pixel. if (v1-v0) == -255, then
            // (dx,dy) points towards the black pixel. p.gx and p.gy will thus
            // be -255, 0, or 255.
            //
            // Note that any given pixel might be added to multiple
            // different clusters. But in the common case, a given
            // pixel will be added multiple times to the same cluster,
            // which increases the size of the cluster and thus the
            // computational costs.
            //
            // A possible optimization would be to combine entries
            // within the same cluster.

#define DO_CONN(dx, dy)                                                 \
            if (1) {                                                    \
                uint8_t v1 = threshim->buf[y*ts + dy*ts + x + dx];      \
                                                                        \
                while (v0 + v1 == 255) {                                   \
                    uint32_t rep1 = unionfind_get_representative(uf, y*w + dy*w + x + dx); \
                    uint32_t clusterid;                                 \
                    if (rep0 < rep1)                                    \
                        clusterid = (rep1 << 16) + rep0;                \
                    else                                                \
                        clusterid = (rep0 << 16) + rep1;                \
                                                                        \
                    /* XXX lousy hash function */                       \
                    uint32_t clustermap_bucket = u64hash_2(clusterid) % nclustermap; \
                    struct uint32_zarray_entry *entry = clustermap[clustermap_bucket]; \
                    while (entry && entry->id != clusterid)     {       \
                        entry = entry->next;                            \
                    }                                                   \
                                                                        \
                    if (!entry) {                                       \
                        entry = umm_calloc(1, sizeof(struct uint32_zarray_entry)); \
                        if (!entry) break;                              \
                        entry->id = clusterid;                          \
                        entry->cluster = zarray_create_fail_ok(sizeof(struct pt)); \
                        if (!entry->cluster) {                          \
                            free(entry);                                \
                            break;                                      \
                        }                                               \
                        entry->next = clustermap[clustermap_bucket];    \
                        clustermap[clustermap_bucket] = entry;          \
                    }                                                   \
                                                                        \
                    struct pt p = { .x = 2*x + dx, .y = 2*y + dy, .gx = dx*((int) v1-v0), .gy = dy*((int) v1-v0)}; \
                    zarray_add_fail_ok(entry->cluster, &p);             \
                    break;                                              \
                }                                                       \
            }

            // do 4 connectivity. NB: Arguments must be [-1, 1] or we'll overflow .gx, .gy
            DO_CONN(1, 0);
            DO_CONN(0, 1);

#ifdef IMLIB_ENABLE_FINE_APRILTAGS
            // do 8 connectivity
            DO_CONN(-1, 1);
            DO_CONN(1, 1);
#endif
        }
    }
#undef DO_CONN

    ////////////////////////////////////////////////////////
    // step 3. process each connected component.
    zarray_t *clusters = zarray_create_fail_ok(sizeof(zarray_t*)); //, uint32_zarray_hash_size(clustermap));
    if (clusters) {
        for (int i = 0; i < nclustermap; i++) {

            for (struct uint32_zarray_entry *entry = clustermap[i]; entry; entry = entry->next) {
                // XXX reject clusters here?
                zarray_add_fail_ok(clusters, &entry->cluster);
            }
        }
    }


    int sz = clusters ? zarray_size(clusters) : 0;

    if (1) {
      for (int i = 0; i < nclustermap; i++) {
        struct uint32_zarray_entry *entry = clustermap[i];
        while (entry) {
          // free any leaked cluster (zarray_add_fail_ok)
          bool leaked = true;
          for (int j = 0; j < sz && leaked; j++) {
              zarray_t *cluster;
              zarray_get(clusters, j, &cluster);
              leaked &= entry->cluster != cluster;
          }
          if (leaked) free(entry->cluster);
          struct uint32_zarray_entry *tmp = entry->next;
          free(entry);
          entry = tmp;
        }
      }
      fb_free(); // clustermap
    }

    unionfind_destroy();

    fb_free(); // threshim->buf
    fb_free(); // threshim

    zarray_t *quads = zarray_create_fail_ok(sizeof(struct quad));

    if (quads) {
        for (int i = 0; i < sz; i++) {

            zarray_t *cluster;
            zarray_get(clusters, i, &cluster);

            if (zarray_size(cluster) < td->qtp.min_cluster_pixels)
                continue;

            // a cluster should contain only boundary points around the
            // tag. it cannot be bigger than the whole screen. (Reject
            // large connected blobs that will be prohibitively slow to
            // fit quads to.) A typical point along an edge is added three
            // times (because it has 3 neighbors). The maximum perimeter
            // is 2w+2h.
            if (zarray_size(cluster) > 3*(2*w+2*h)) {
                continue;
            }

            struct quad quad;
            memset(&quad, 0, sizeof(struct quad));

            if (fit_quad(td, im, cluster, &quad, overrideMode)) {

                zarray_add_fail_ok(quads, &quad);
            }
        }
    }

    //        printf("  %d %d %d %d\n", indices[0], indices[1], indices[2], indices[3]);

    for (int i = 0; i < sz; i++) {
        zarray_t *cluster;
        zarray_get(clusters, i, &cluster);
        zarray_destroy(cluster);
    }

    if (clusters) zarray_destroy(clusters);


    if (!quads) {
        // we should have enough memory now
        quads = zarray_create(sizeof(struct quad));
    }
    return quads;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "apriltag.c"
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef M_PI
# define M_PI 3.141592653589793238462643383279502884196
#endif

// Regresses a model of the form:
// intensity(x,y) = C0*x + C1*y + CC2
// The J matrix is the:
//    J = [ x1 y1 1 ]
//        [ x2 y2 1 ]
//        [ ...     ]
//  The A matrix is J'J

struct graymodel
{
    float A[3][3];
    float B[3];
    float C[3];
};

void graymodel_init(struct graymodel *gm)
{
    memset(gm, 0, sizeof(struct graymodel));
}

void graymodel_add(struct graymodel *gm, float x, float y, float gray)
{
    // update upper right entries of A = J'J
    gm->A[0][0] += x*x;
    gm->A[0][1] += x*y;
    gm->A[0][2] += x;
    gm->A[1][1] += y*y;
    gm->A[1][2] += y;
    gm->A[2][2] += 1;

    // update B = J'gray
    gm->B[0] += x * gray;
    gm->B[1] += y * gray;
    gm->B[2] += gray;
}

void graymodel_solve(struct graymodel *gm)
{
    mat33_sym_solve((float*) gm->A, gm->B, gm->C);
}

float graymodel_interpolate(struct graymodel *gm, float x, float y)
{
    return gm->C[0]*x + gm->C[1]*y + gm->C[2];
}

struct quick_decode_entry
{
    uint64_t rcode;   // the queried code
    uint16_t id;      // the tag ID (a small integer)
    uint8_t hamming;  // how many errors corrected?
    uint8_t rotation; // number of rotations [0, 3]
    bool hmirror;
    bool vflip;
};

struct quick_decode
{
    int nentries;
    struct quick_decode_entry *entries;
};

/** if the bits in w were arranged in a d*d grid and that grid was
 * rotated, what would the new bits in w be?
 * The bits are organized like this (for d = 3):
 *
 *  8 7 6       2 5 8      0 1 2
 *  5 4 3  ==>  1 4 7 ==>  3 4 5    (rotate90 applied twice)
 *  2 1 0       0 3 6      6 7 8
 **/
static uint64_t rotate90(uint64_t w, uint32_t d)
{
    uint64_t wr = 0;

    for (int32_t r = d-1; r >=0; r--) {
        for (int32_t c = 0; c < d; c++) {
            int32_t b = r + d*c;

            wr = wr << 1;

            if ((w & (((uint64_t) 1) << b))!=0)
                wr |= 1;
        }
    }

    return wr;
}

static uint64_t hmirror_code(uint64_t w, uint32_t d)
{
    uint64_t wr = 0;

    for (int32_t r = d-1; r >=0; r--) {
        for (int32_t c = 0; c < d; c++) {
            int32_t b = c + d*r;

            wr = wr << 1;

            if ((w & (((uint64_t) 1) << b))!=0)
                wr |= 1;
        }
    }

    return wr;
}

static uint64_t vflip_code(uint64_t w, uint32_t d)
{
    uint64_t wr = 0;

    for (int32_t r = 0; r < d; r++) {
        for (int32_t c = d-1; c >=0; c--) {
            int32_t b = c + d*r;

            wr = wr << 1;

            if ((w & (((uint64_t) 1) << b))!=0)
                wr |= 1;
        }
    }

    return wr;
}

void quad_destroy(struct quad *quad)
{
    if (!quad)
        return;

    matd_destroy(quad->H);
    matd_destroy(quad->Hinv);
    free(quad);
}

struct quad *quad_copy(struct quad *quad)
{
    struct quad *q = calloc(1, sizeof(struct quad));
    memcpy(q, quad, sizeof(struct quad));
    if (quad->H)
        q->H = matd_copy(quad->H);
    if (quad->Hinv)
        q->Hinv = matd_copy(quad->Hinv);
    return q;
}

// http://en.wikipedia.org/wiki/Hamming_weight

//types and constants used in the functions below
//uint64_t is an unsigned 64-bit integer variable type (defined in C99 version of C language)
const uint64_t m1  = 0x5555555555555555; //binary: 0101...
const uint64_t m2  = 0x3333333333333333; //binary: 00110011..
const uint64_t m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
const uint64_t m8  = 0x00ff00ff00ff00ff; //binary:  8 zeros,  8 ones ...
const uint64_t m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...
const uint64_t m32 = 0x00000000ffffffff; //binary: 32 zeros, 32 ones
const uint64_t hff = 0xffffffffffffffff; //binary: all ones
const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...

//This is a naive implementation, shown for comparison,
//and to help in understanding the better functions.
//This algorithm uses 24 arithmetic operations (shift, add, and).
int popcount64a(uint64_t x)
{
    x = (x & m1 ) + ((x >>  1) & m1 ); //put count of each  2 bits into those  2 bits
    x = (x & m2 ) + ((x >>  2) & m2 ); //put count of each  4 bits into those  4 bits
    x = (x & m4 ) + ((x >>  4) & m4 ); //put count of each  8 bits into those  8 bits
    x = (x & m8 ) + ((x >>  8) & m8 ); //put count of each 16 bits into those 16 bits
    x = (x & m16) + ((x >> 16) & m16); //put count of each 32 bits into those 32 bits
    x = (x & m32) + ((x >> 32) & m32); //put count of each 64 bits into those 64 bits
    return x;
}

//This uses fewer arithmetic operations than any other known
//implementation on machines with slow multiplication.
//This algorithm uses 17 arithmetic operations.
int popcount64b(uint64_t x)
{
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
    x += x >>  8;  //put count of each 16 bits into their lowest 8 bits
    x += x >> 16;  //put count of each 32 bits into their lowest 8 bits
    x += x >> 32;  //put count of each 64 bits into their lowest 8 bits
    return x & 0x7f;
}

//This uses fewer arithmetic operations than any other known
//implementation on machines with fast multiplication.
//This algorithm uses 12 arithmetic operations, one of which is a multiply.
int popcount64c(uint64_t x)
{
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
    return (x * h01) >> 56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}

// returns an entry with hamming set to 255 if no decode was found.
static void quick_decode_codeword(apriltag_family_t *tf, uint64_t rcode,
                                  struct quick_decode_entry *entry)
{
    int threshold = imax(tf->h - tf->d - 1, 0);

    for (int ridx = 0; ridx < 4; ridx++) {

        for (int i = 0, j = tf->ncodes; i < j; i++) {
            int hamming = popcount64c(tf->codes[i] ^ rcode);
            if(hamming <= threshold) {
                entry->rcode = rcode;
                entry->id = i;
                entry->hamming = hamming;
                entry->rotation = ridx;
                entry->hmirror = false;
                entry->vflip = false;
                return;
            }
        }

        rcode = rotate90(rcode, tf->d);
    }

    rcode = hmirror_code(rcode, tf->d); // handle hmirror

    for (int ridx = 0; ridx < 4; ridx++) {

        for (int i = 0, j = tf->ncodes; i < j; i++) {
            int hamming = popcount64c(tf->codes[i] ^ rcode);
            if(hamming <= threshold) {
                entry->rcode = rcode;
                entry->id = i;
                entry->hamming = hamming;
                entry->rotation = ridx;
                entry->hmirror = true;
                entry->vflip = false;
                return;
            }
        }

        rcode = rotate90(rcode, tf->d);
    }

    rcode = vflip_code(rcode, tf->d); // handle hmirror+vflip

    for (int ridx = 0; ridx < 4; ridx++) {

        for (int i = 0, j = tf->ncodes; i < j; i++) {
            int hamming = popcount64c(tf->codes[i] ^ rcode);
            if(hamming <= threshold) {
                entry->rcode = rcode;
                entry->id = i;
                entry->hamming = hamming;
                entry->rotation = ridx;
                entry->hmirror = true;
                entry->vflip = true;
                return;
            }
        }

        rcode = rotate90(rcode, tf->d);
    }

    rcode = hmirror_code(rcode, tf->d); // handle vflip

    for (int ridx = 0; ridx < 4; ridx++) {

        for (int i = 0, j = tf->ncodes; i < j; i++) {
            int hamming = popcount64c(tf->codes[i] ^ rcode);
            if(hamming <= threshold) {
                entry->rcode = rcode;
                entry->id = i;
                entry->hamming = hamming;
                entry->rotation = ridx;
                entry->hmirror = false;
                entry->vflip = true;
                return;
            }
        }

        rcode = rotate90(rcode, tf->d);
    }

    entry->rcode = 0;
    entry->id = 65535;
    entry->hamming = 255;
    entry->rotation = 0;
    entry->hmirror = false;
    entry->vflip = false;
}

static inline int detection_compare_function(const void *_a, const void *_b)
{
    apriltag_detection_t *a = *(apriltag_detection_t**) _a;
    apriltag_detection_t *b = *(apriltag_detection_t**) _b;

    return a->id - b->id;
}

void apriltag_detector_remove_family(apriltag_detector_t *td, apriltag_family_t *fam)
{
    zarray_remove_value(td->tag_families, &fam, 0);
}

void apriltag_detector_add_family_bits(apriltag_detector_t *td, apriltag_family_t *fam, int bits_corrected)
{
    zarray_add(td->tag_families, &fam);
}

void apriltag_detector_clear_families(apriltag_detector_t *td)
{
    zarray_clear(td->tag_families);
}

apriltag_detector_t *apriltag_detector_create()
{
    apriltag_detector_t *td = (apriltag_detector_t*) calloc(1, sizeof(apriltag_detector_t));

    td->qtp.max_nmaxima = 10;
    td->qtp.min_cluster_pixels = 5;

    td->qtp.max_line_fit_mse = 10.0;
    td->qtp.critical_rad = 10 * M_PI / 180;
    td->qtp.deglitch = 0;
    td->qtp.min_white_black_diff = 5;

    td->tag_families = zarray_create(sizeof(apriltag_family_t*));

    td->refine_edges = 1;
    td->refine_pose = 0;
    td->refine_decode = 0;

    return td;
}

void apriltag_detector_destroy(apriltag_detector_t *td)
{
    apriltag_detector_clear_families(td);

    zarray_destroy(td->tag_families);
    free(td);
}

struct evaluate_quad_ret
{
    int64_t rcode;
    float  score;
    matd_t  *H, *Hinv;

    int decode_status;
    struct quick_decode_entry e;
};

// returns non-zero if an error occurs (i.e., H has no inverse)
int quad_update_homographies(struct quad *quad)
{
    zarray_t *correspondences = zarray_create(sizeof(float[4]));

    for (int i = 0; i < 4; i++) {
        float corr[4];

        // At this stage of the pipeline, we have not attempted to decode the
        // quad into an oriented tag. Thus, just act as if the quad is facing
        // "up" with respect to our desired corners. We'll fix the rotation
        // later.
        // [-1, -1], [1, -1], [1, 1], [-1, 1]
        corr[0] = (i==0 || i==3) ? -1 : 1;
        corr[1] = (i==0 || i==1) ? -1 : 1;

        corr[2] = quad->p[i][0];
        corr[3] = quad->p[i][1];

        zarray_add(correspondences, &corr);
    }

    if (quad->H)
        matd_destroy(quad->H);
    if (quad->Hinv)
        matd_destroy(quad->Hinv);

    // XXX Tunable
    quad->H = homography_compute(correspondences, HOMOGRAPHY_COMPUTE_FLAG_SVD);
    quad->Hinv = matd_inverse(quad->H);
    zarray_destroy(correspondences);

    if (quad->H && quad->Hinv)
        return 0;

    return -1;
}

// compute a "score" for a quad that is independent of tag family
// encoding (but dependent upon the tag geometry) by considering the
// contrast around the exterior of the tag.
float quad_goodness(apriltag_family_t *family, image_u8_t *im, struct quad *quad)
{
    // when sampling from the white border, how much white border do
    // we actually consider valid, measured in bit-cell units? (the
    // outside portions are often intruded upon, so it could be advantageous to use
    // less than the "nominal" 1.0. (Less than 1.0 not well tested.)

    // XXX Tunable
    float white_border = 1;

    // in tag coordinates, how big is each bit cell?
    float bit_size = 2.0 / (2*family->black_border + family->d);
//    float inv_bit_size = 1.0 / bit_size;

    int32_t xmin = INT32_MAX, xmax = 0, ymin = INT32_MAX, ymax = 0;

    for (int i = 0; i < 4; i++) {
        float tx = (i == 0 || i == 3) ? -1 - bit_size : 1 + bit_size;
        float ty = (i == 0 || i == 1) ? -1 - bit_size : 1 + bit_size;
        float x, y;

        homography_project(quad->H, tx, ty, &x, &y);
        xmin = imin(xmin, x);
        xmax = imax(xmax, x);
        ymin = imin(ymin, y);
        ymax = imax(ymax, y);
    }

    // clamp bounding box to image dimensions
    xmin = imax(0, xmin);
    xmax = imin(im->width-1, xmax);
    ymin = imax(0, ymin);
    ymax = imin(im->height-1, ymax);

//    int nbits = family->d * family->d;

    int32_t W1 = 0, B1 = 0, Wn = 0, Bn = 0; // int64_t W1 = 0, B1 = 0, Wn = 0, Bn = 0;

    float wsz = bit_size*white_border;
    float bsz = bit_size*family->black_border;

    matd_t *Hinv = quad->Hinv;
//    matd_t *H = quad->H;

    // iterate over all the pixels in the tag. (Iterating in pixel space)
    for (int y = ymin; y <= ymax; y++) {

        // we'll incrementally compute the homography
        // projections. Begin by evaluating the homogeneous position
        // [(xmin - .5f), y, 1]. Then, we'll update as we stride in
        // the +x direction.
        float Hx = MATD_EL(Hinv, 0, 0) * (.5 + (int) xmin) +
            MATD_EL(Hinv, 0, 1) * (y + .5) + MATD_EL(Hinv, 0, 2);
        float Hy = MATD_EL(Hinv, 1, 0) * (.5 + (int) xmin) +
            MATD_EL(Hinv, 1, 1) * (y + .5) + MATD_EL(Hinv, 1, 2);
        float Hh = MATD_EL(Hinv, 2, 0) * (.5 + (int) xmin) +
            MATD_EL(Hinv, 2, 1) * (y + .5) + MATD_EL(Hinv, 2, 2);

        for (int x = xmin; x <= xmax;  x++) {
            // project the pixel center.
            float tx, ty;

            // divide by homogeneous coordinate
            tx = Hx / Hh;
            ty = Hy / Hh;

            // if we move x one pixel to the right, here's what
            // happens to our three pre-normalized coordinates.
            Hx += MATD_EL(Hinv, 0, 0);
            Hy += MATD_EL(Hinv, 1, 0);
            Hh += MATD_EL(Hinv, 2, 0);

            float txa = fabsf((float) tx), tya = fabsf((float) ty);
            float xymax = fmaxf(txa, tya);

//            if (txa >= 1 + wsz || tya >= 1 + wsz)
            if (xymax >= 1 + wsz)
                continue;

            uint8_t v = im->buf[y*im->stride + x];

            // it's within the white border?
//            if (txa >= 1 || tya >= 1) {
            if (xymax >= 1) {
                W1 += v;
                Wn ++;
                continue;
            }

            // it's within the black border?
//            if (txa >= 1 - bsz || tya >= 1 - bsz) {
            if (xymax >= 1 - bsz) {
                B1 += v;
                Bn ++;
                continue;
            }

            // it must be a data bit. We don't do anything with these.
            continue;
        }
    }


    // score = average margin between white and black pixels near border.
    float margin = 1.0 * W1 / Wn - 1.0 * B1 / Bn;
//    printf("margin %f: W1 %f, B1 %f\n", margin, W1, B1);

    return margin;
}

// returns the decision margin. Return < 0 if the detection should be rejected.
float quad_decode(apriltag_family_t *family, image_u8_t *im, struct quad *quad, struct quick_decode_entry *entry, image_u8_t *im_samples)
{
    // decode the tag binary contents by sampling the pixel
    // closest to the center of each bit cell.

    int64_t rcode = 0;

    // how wide do we assume the white border is?
    float white_border = 1.0;

    // We will compute a threshold by sampling known white/black cells around this tag.
    // This sampling is achieved by considering a set of samples along lines.
    //
    // coordinates are given in bit coordinates. ([0, fam->d]).
    //
    // { initial x, initial y, delta x, delta y, WHITE=1 }
    float patterns[] = {
        // left white column
        0 - white_border / 2.0, 0.5,
        0, 1,
        1,

        // left black column
        0 + family->black_border / 2.0, 0.5,
        0, 1,
        0,

        // right white column
        2*family->black_border + family->d + white_border / 2.0, .5,
        0, 1,
        1,

        // right black column
        2*family->black_border + family->d - family->black_border / 2.0, .5,
        0, 1,
        0,

        // top white row
        0.5, -white_border / 2.0,
        1, 0,
        1,

        // top black row
        0.5, family->black_border / 2.0,
        1, 0,
        0,

        // bottom white row
        0.5, 2*family->black_border + family->d + white_border / 2.0,
        1, 0,
        1,

        // bottom black row
        0.5, 2*family->black_border + family->d - family->black_border / 2.0,
        1, 0,
        0

        // XXX float-counts the corners.
    };

    struct graymodel whitemodel, blackmodel;
    graymodel_init(&whitemodel);
    graymodel_init(&blackmodel);

    for (int pattern_idx = 0; pattern_idx < sizeof(patterns)/(5*sizeof(float)); pattern_idx ++) {
        float *pattern = &patterns[pattern_idx * 5];

        int is_white = pattern[4];

        for (int i = 0; i < 2*family->black_border + family->d; i++) {
            float tagx01 = (pattern[0] + i*pattern[2]) / (2*family->black_border + family->d);
            float tagy01 = (pattern[1] + i*pattern[3]) / (2*family->black_border + family->d);

            float tagx = 2*(tagx01-0.5);
            float tagy = 2*(tagy01-0.5);

            float px, py;
            homography_project(quad->H, tagx, tagy, &px, &py);

            // don't round
            int ix = px;
            int iy = py;
            if (ix < 0 || iy < 0 || ix >= im->width || iy >= im->height)
                continue;

            int v = im->buf[iy*im->stride + ix];

            if (im_samples) {
                im_samples->buf[iy*im_samples->stride + ix] = (1-is_white)*255;
            }

            if (is_white)
                graymodel_add(&whitemodel, tagx, tagy, v);
            else
                graymodel_add(&blackmodel, tagx, tagy, v);
        }
    }

    graymodel_solve(&whitemodel);
    graymodel_solve(&blackmodel);

    // XXX Tunable
    if (graymodel_interpolate(&whitemodel, 0, 0) - graymodel_interpolate(&blackmodel, 0, 0) < 0)
        return -1;

    // compute the average decision margin (how far was each bit from
    // the decision boundary?
    //
    // we score this separately for white and black pixels and return
    // the minimum average threshold for black/white pixels. This is
    // to penalize thresholds that are too close to an extreme.
    float black_score = 0, white_score = 0;
    float black_score_count = 1, white_score_count = 1;

    for (int bitidx = 0; bitidx < family->d * family->d; bitidx++) {
        int bitx = bitidx % family->d;
        int bity = bitidx / family->d;

        float tagx01 = (family->black_border + bitx + 0.5) / (2*family->black_border + family->d);
        float tagy01 = (family->black_border + bity + 0.5) / (2*family->black_border + family->d);

        // scale to [-1, 1]
        float tagx = 2*(tagx01-0.5);
        float tagy = 2*(tagy01-0.5);

        float px, py;
        homography_project(quad->H, tagx, tagy, &px, &py);

        rcode = (rcode << 1);

        // don't round.
        int ix = px;
        int iy = py;

        if (ix < 0 || iy < 0 || ix >= im->width || iy >= im->height)
            continue;

        int v = im->buf[iy*im->stride + ix];

        float thresh = (graymodel_interpolate(&blackmodel, tagx, tagy) + graymodel_interpolate(&whitemodel, tagx, tagy)) / 2.0;
        if (v > thresh) {
            white_score += (v - thresh);
            white_score_count ++;
            rcode |= 1;
        } else {
            black_score += (thresh - v);
            black_score_count ++;
        }

        if (im_samples)
            im_samples->buf[iy*im_samples->stride + ix] = (1 - (rcode & 1)) * 255;
    }

    quick_decode_codeword(family, rcode, entry);

    return fmin(white_score / white_score_count, black_score / black_score_count);
}

float score_goodness(apriltag_family_t *family, image_u8_t *im, struct quad *quad, void *user)
{
    return quad_goodness(family, im, quad);
}

float score_decodability(apriltag_family_t *family, image_u8_t *im, struct quad *quad, void *user)
{
    struct quick_decode_entry entry;

    float decision_margin = quad_decode(family, im, quad, &entry, NULL);

    // hamming trumps decision margin; maximum value for decision_margin is 255.
    return decision_margin - entry.hamming*1000;
}

// returns score of best quad
float optimize_quad_generic(apriltag_family_t *family, image_u8_t *im, struct quad *quad0,
                             float *stepsizes, int nstepsizes,
                             float (*score)(apriltag_family_t *family, image_u8_t *im, struct quad *quad, void *user),
                             void *user)
{
    struct quad *best_quad = quad_copy(quad0);
    float best_score = score(family, im, best_quad, user);

    for (int stepsize_idx = 0; stepsize_idx < nstepsizes; stepsize_idx++)  {

        int improved = 1;

        // when we make progress with a particular step size, how many
        // times will we try to perform that same step size again?
        // (max_repeat = 0 means ("don't repeat--- just move to the
        // next step size").
        // XXX Tunable
        int max_repeat = 1;

        for (int repeat = 0; repeat <= max_repeat && improved; repeat++) {

            improved = 0;

            // wiggle point i
            for (int i = 0; i < 4; i++) {

                float stepsize = stepsizes[stepsize_idx];

                // XXX Tunable (really 1 makes the best sense since)
                int nsteps = 1;

                struct quad *this_best_quad = NULL;
                float this_best_score = best_score;

                for (int sx = -nsteps; sx <= nsteps; sx++) {
                    for (int sy = -nsteps; sy <= nsteps; sy++) {
                        if (sx==0 && sy==0)
                            continue;

                        struct quad *this_quad = quad_copy(best_quad);
                        this_quad->p[i][0] = best_quad->p[i][0] + sx*stepsize;
                        this_quad->p[i][1] = best_quad->p[i][1] + sy*stepsize;
                        if (quad_update_homographies(this_quad))
                            continue;

                        float this_score = score(family, im, this_quad, user);

                        if (this_score > this_best_score) {
                            quad_destroy(this_best_quad);

                            this_best_quad = this_quad;
                            this_best_score = this_score;
                        } else {
                            quad_destroy(this_quad);
                        }
                    }
                }

                if (this_best_score > best_score) {
                    quad_destroy(best_quad);
                    best_quad = this_best_quad;
                    best_score = this_best_score;
                    improved = 1;
                }
            }
        }
    }

    matd_destroy(quad0->H);
    matd_destroy(quad0->Hinv);
    memcpy(quad0, best_quad, sizeof(struct quad)); // copy pointers
    free(best_quad);
    return best_score;
}

static void refine_edges(apriltag_detector_t *td, image_u8_t *im_orig, struct quad *quad)
{
    float lines[4][4]; // for each line, [Ex Ey nx ny]

    for (int edge = 0; edge < 4; edge++) {
        int a = edge, b = (edge + 1) & 3; // indices of the end points.

        // compute the normal to the current line estimate
        float nx = quad->p[b][1] - quad->p[a][1];
        float ny = -quad->p[b][0] + quad->p[a][0];
        float mag = sqrt(nx*nx + ny*ny);
        nx /= mag;
        ny /= mag;

        // we will now fit a NEW line by sampling points near
        // our original line that have large gradients. On really big tags,
        // we're willing to sample more to get an even better estimate.
        int nsamples = imax(16, mag / 8); // XXX tunable

        // stats for fitting a line...
        float Mx = 0, My = 0, Mxx = 0, Mxy = 0, Myy = 0, N = 0;

        for (int s = 0; s < nsamples; s++) {
            // compute a point along the line... Note, we're avoiding
            // sampling *right* at the corners, since those points are
            // the least reliable.
            float alpha = (1.0 + s) / (nsamples + 1);
            float x0 = alpha*quad->p[a][0] + (1-alpha)*quad->p[b][0];
            float y0 = alpha*quad->p[a][1] + (1-alpha)*quad->p[b][1];

            // search along the normal to this line, looking at the
            // gradients along the way. We're looking for a strong
            // response.
            float Mn = 0;
            float Mcount = 0;

            // XXX tunable: how far to search?  We want to search far
            // enough that we find the best edge, but not so far that
            // we hit other edges that aren't part of the tag. We
            // shouldn't ever have to search more than quad_decimate,
            // since otherwise we would (ideally) have started our
            // search on another pixel in the first place. Likewise,
            // for very small tags, we don't want the range to be too
            // big.
            float range = 1.0 + 1;

            // XXX tunable step size.
            for (float n = -range; n <= range; n +=  0.25) {
                // Because of the guaranteed winding order of the
                // points in the quad, we will start inside the white
                // portion of the quad and work our way outward.
                //
                // sample to points (x1,y1) and (x2,y2) XXX tunable:
                // how far +/- to look? Small values compute the
                // gradient more precisely, but are more sensitive to
                // noise.
                float grange = 1;
                int x1 = x0 + (n + grange)*nx;
                int y1 = y0 + (n + grange)*ny;
                if (x1 < 0 || x1 >= im_orig->width || y1 < 0 || y1 >= im_orig->height)
                    continue;

                int x2 = x0 + (n - grange)*nx;
                int y2 = y0 + (n - grange)*ny;
                if (x2 < 0 || x2 >= im_orig->width || y2 < 0 || y2 >= im_orig->height)
                    continue;

                int g1 = im_orig->buf[y1*im_orig->stride + x1];
                int g2 = im_orig->buf[y2*im_orig->stride + x2];

                if (g1 < g2) // reject points whose gradient is "backwards". They can only hurt us.
                    continue;

                float weight = (g2 - g1)*(g2 - g1); // XXX tunable. What shape for weight=f(g2-g1)?

                // compute weighted average of the gradient at this point.
                Mn += weight*n;
                Mcount += weight;
            }

            // what was the average point along the line?
            if (Mcount == 0)
                continue;

            float n0 = Mn / Mcount;

            // where is the point along the line?
            float bestx = x0 + n0*nx;
            float besty = y0 + n0*ny;

            // update our line fit statistics
            Mx += bestx;
            My += besty;
            Mxx += bestx*bestx;
            Mxy += bestx*besty;
            Myy += besty*besty;
            N++;
        }

        // fit a line
        float Ex = Mx / N, Ey = My / N;
        float Cxx = Mxx / N - Ex*Ex;
        float Cxy = Mxy / N - Ex*Ey;
        float Cyy = Myy / N - Ey*Ey;

        float normal_theta = .5 * atan2f(-2*Cxy, (Cyy - Cxx));
        nx = cosf(normal_theta);
        ny = sinf(normal_theta);
        lines[edge][0] = Ex;
        lines[edge][1] = Ey;
        lines[edge][2] = nx;
        lines[edge][3] = ny;
    }

    // now refit the corners of the quad
    for (int i = 0; i < 4; i++) {

        // solve for the intersection of lines (i) and (i+1)&3.
        float A00 =  lines[i][3],  A01 = -lines[(i+1)&3][3];
        float A10 =  -lines[i][2],  A11 = lines[(i+1)&3][2];
        float B0 = -lines[i][0] + lines[(i+1)&3][0];
        float B1 = -lines[i][1] + lines[(i+1)&3][1];

        float det = A00 * A11 - A10 * A01;

        // inverse.
        if (fabs(det) > 0.001) {
            // solve
            float W00 = A11 / det, W01 = -A01 / det;

            float L0 = W00*B0 + W01*B1;

            // compute intersection
            quad->p[i][0] = lines[i][0] + L0*A00;
            quad->p[i][1] = lines[i][1] + L0*A10;
        } else {
            // this is a bad sign. We'll just keep the corner we had.
//            printf("bad det: %15f %15f %15f %15f %15f\n", A00, A11, A10, A01, det);
        }
    }
}

void apriltag_detection_destroy(apriltag_detection_t *det)
{
    if (det == NULL)
        return;

    matd_destroy(det->H);
    free(det);
}

int prefer_smaller(int pref, float q0, float q1)
{
    if (pref)     // already prefer something? exit.
        return pref;

    if (q0 < q1)
        return -1; // we now prefer q0
    if (q1 < q0)
        return 1; // we now prefer q1

    // no preference
    return 0;
}

zarray_t *apriltag_detector_detect(apriltag_detector_t *td, image_u8_t *im_orig)
{
    if (zarray_size(td->tag_families) == 0) {
        zarray_t *s = zarray_create(sizeof(apriltag_detection_t*));
        printf("apriltag.c: No tag families enabled.");
        return s;
    }

    ///////////////////////////////////////////////////////////
    // Step 1. Detect quads according to requested image decimation
    // and blurring parameters.

//    zarray_t *quads = apriltag_quad_gradient(td, im_orig);
    zarray_t *quads = apriltag_quad_thresh(td, im_orig, false);

    zarray_t *detections = zarray_create(sizeof(apriltag_detection_t*));

    td->nquads = zarray_size(quads);

    ////////////////////////////////////////////////////////////////
    // Step 2. Decode tags from each quad.
    if (1) {
        for (int i = 0; i < zarray_size(quads); i++) {
            struct quad *quad_original;
            zarray_get_volatile(quads, i, &quad_original);

            // refine edges is not dependent upon the tag family, thus
            // apply this optimization BEFORE the other work.
            //if (td->quad_decimate > 1 && td->refine_edges) {
            if (td->refine_edges) {
                refine_edges(td, im_orig, quad_original);
            }

            // make sure the homographies are computed...
            if (quad_update_homographies(quad_original))
                continue;

            for (int famidx = 0; famidx < zarray_size(td->tag_families); famidx++) {
                apriltag_family_t *family;
                zarray_get(td->tag_families, famidx, &family);

                float goodness = 0;

                // since the geometry of tag families can vary, start any
                // optimization process over with the original quad.
                struct quad *quad = quad_copy(quad_original);

                // improve the quad corner positions by minimizing the
                // variance within each intra-bit area.
                if (td->refine_pose) {
                    // NB: We potentially step an integer
                    // number of times in each direction. To make each
                    // sample as useful as possible, the step sizes should
                    // not be integer multiples of each other. (I.e.,
                    // probably don't use 1, 0.5, 0.25, etc.)

                    // XXX Tunable
                    float stepsizes[] = { 1, .4, .16, .064 };
                    int nstepsizes = sizeof(stepsizes)/sizeof(float);

                    goodness = optimize_quad_generic(family, im_orig, quad, stepsizes, nstepsizes, score_goodness, NULL);
                }

                if (td->refine_decode) {
                    // this optimizes decodability, but we don't report
                    // that value to the user.  (so discard return value.)
                    // XXX Tunable
                    float stepsizes[] = { .4 };
                    int nstepsizes = sizeof(stepsizes)/sizeof(float);

                    optimize_quad_generic(family, im_orig, quad, stepsizes, nstepsizes, score_decodability, NULL);
                }

                struct quick_decode_entry entry;

                float decision_margin = quad_decode(family, im_orig, quad, &entry, NULL);

                if (entry.hamming < 255 && decision_margin >= 0) {
                    apriltag_detection_t *det = calloc(1, sizeof(apriltag_detection_t));

                    det->family = family;
                    det->id = entry.id;
                    det->hamming = entry.hamming;
                    det->goodness = goodness;
                    det->decision_margin = decision_margin;

                    float theta = -entry.rotation * M_PI / 2.0;
                    float c = cos(theta), s = sin(theta);

                    // Fix the rotation of our homography to properly orient the tag
                    matd_t *R = matd_create(3,3);
                    MATD_EL(R, 0, 0) = c;
                    MATD_EL(R, 0, 1) = -s;
                    MATD_EL(R, 1, 0) = s;
                    MATD_EL(R, 1, 1) = c;
                    MATD_EL(R, 2, 2) = 1;

                    matd_t *RHMirror = matd_create(3,3);
                    MATD_EL(RHMirror, 0, 0) = entry.hmirror ? -1 : 1;
                    MATD_EL(RHMirror, 1, 1) = 1;
                    MATD_EL(RHMirror, 2, 2) = entry.hmirror ? -1 : 1;

                    matd_t *RVFlip = matd_create(3,3);
                    MATD_EL(RVFlip, 0, 0) = 1;
                    MATD_EL(RVFlip, 1, 1) = entry.vflip ? -1 : 1;
                    MATD_EL(RVFlip, 2, 2) = entry.vflip ? -1 : 1;

                    det->H = matd_op("M*M*M*M", quad->H, R, RHMirror, RVFlip);

                    matd_destroy(R);
                    matd_destroy(RHMirror);
                    matd_destroy(RVFlip);

                    homography_project(det->H, 0, 0, &det->c[0], &det->c[1]);

                    // [-1, -1], [1, -1], [1, 1], [-1, 1], Desired points
                    // [-1, 1], [1, 1], [1, -1], [-1, -1], FLIP Y
                    // adjust the points in det->p so that they correspond to
                    // counter-clockwise around the quad, starting at -1,-1.
                    for (int i = 0; i < 4; i++) {
                        int tcx = (i == 1 || i == 2) ? 1 : -1;
                        int tcy = (i < 2) ? 1 : -1;

                        float p[2];

                        homography_project(det->H, tcx, tcy, &p[0], &p[1]);

                        det->p[i][0] = p[0];
                        det->p[i][1] = p[1];
                    }

                    zarray_add(detections, &det);
                }

                quad_destroy(quad);
            }
        }
    }

    ////////////////////////////////////////////////////////////////
    // Step 3. Reconcile detections--- don't report the same tag more
    // than once. (Allow non-overlapping duplicate detections.)
    if (1) {
        zarray_t *poly0 = g2d_polygon_create_zeros(4);
        zarray_t *poly1 = g2d_polygon_create_zeros(4);

        for (int i0 = 0; i0 < zarray_size(detections); i0++) {

            apriltag_detection_t *det0;
            zarray_get(detections, i0, &det0);

            for (int k = 0; k < 4; k++)
                zarray_set(poly0, k, det0->p[k], NULL);

            for (int i1 = i0+1; i1 < zarray_size(detections); i1++) {

                apriltag_detection_t *det1;
                zarray_get(detections, i1, &det1);

                if (det0->id != det1->id || det0->family != det1->family)
                    continue;

                for (int k = 0; k < 4; k++)
                    zarray_set(poly1, k, det1->p[k], NULL);

                if (g2d_polygon_overlaps_polygon(poly0, poly1)) {
                    // the tags overlap. Delete one, keep the other.

                    int pref = 0; // 0 means undecided which one we'll keep.
                    pref = prefer_smaller(pref, det0->hamming, det1->hamming);     // want small hamming
                    pref = prefer_smaller(pref, -det0->decision_margin, -det1->decision_margin);      // want bigger margins
                    pref = prefer_smaller(pref, -det0->goodness, -det1->goodness); // want bigger goodness

                    // if we STILL don't prefer one detection over the other, then pick
                    // any deterministic criterion.
                    for (int i = 0; i < 4; i++) {
                        pref = prefer_smaller(pref, det0->p[i][0], det1->p[i][0]);
                        pref = prefer_smaller(pref, det0->p[i][1], det1->p[i][1]);
                    }

                    if (pref == 0) {
                        // at this point, we should only be undecided if the tag detections
                        // are *exactly* the same. How would that happen?
                        // printf("uh oh, no preference for overlappingdetection\n");
                    }

                    if (pref < 0) {
                        // keep det0, destroy det1
                        apriltag_detection_destroy(det1);
                        zarray_remove_index(detections, i1, 1);
                        i1--; // retry the same index
                        goto retry1;
                    } else {
                        // keep det1, destroy det0
                        apriltag_detection_destroy(det0);
                        zarray_remove_index(detections, i0, 1);
                        i0--; // retry the same index.
                        goto retry0;
                    }
                }

              retry1: ;
            }

          retry0: ;
        }

        zarray_destroy(poly0);
        zarray_destroy(poly1);
    }

    for (int i = 0; i < zarray_size(quads); i++) {
        struct quad *quad;
        zarray_get_volatile(quads, i, &quad);
        matd_destroy(quad->H);
        matd_destroy(quad->Hinv);
    }

    zarray_destroy(quads);

    zarray_sort(detections, detection_compare_function);

    return detections;
}


// Call this method on each of the tags returned by apriltag_detector_detect
void apriltag_detections_destroy(zarray_t *detections)
{
    for (int i = 0; i < zarray_size(detections); i++) {
        apriltag_detection_t *det;
        zarray_get(detections, i, &det);

        apriltag_detection_destroy(det);
    }

    zarray_destroy(detections);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void imlib_find_apriltags(list_t *out, image_t *ptr, rectangle_t *roi, apriltag_families_t families,
                          float fx, float fy, float cx, float cy)
{
    // Frame Buffer Memory Usage...
    // -> GRAYSCALE Input Image = w*h*1
    // -> GRAYSCALE Threhsolded Image = w*h*1
    // -> UnionFind = w*h*2 (+w*h*1 for hash table)
    size_t resolution = roi->w * roi->h;
    size_t fb_alloc_need = resolution * (1 + 1 + 2 + 1); // read above...
    umm_init_x(((fb_avail() - fb_alloc_need) / resolution) * resolution);
    apriltag_detector_t *td = apriltag_detector_create();

    if (families & TAG16H5) {
        apriltag_detector_add_family(td, (apriltag_family_t *) &tag16h5);
    }

    if (families & TAG25H7) {
        apriltag_detector_add_family(td, (apriltag_family_t *) &tag25h7);
    }

    if (families & TAG25H9) {
        apriltag_detector_add_family(td, (apriltag_family_t *) &tag25h9);
    }

    if (families & TAG36H10) {
        apriltag_detector_add_family(td, (apriltag_family_t *) &tag36h10);
    }

    if (families & TAG36H11) {
        apriltag_detector_add_family(td, (apriltag_family_t *) &tag36h11);
    }

    if (families & ARTOOLKIT) {
        apriltag_detector_add_family(td, (apriltag_family_t *) &artoolkit);
    }

    uint8_t *grayscale_image = fb_alloc(roi->w * roi->h, FB_ALLOC_NO_HINT);

    image_u8_t im;
    im.width = roi->w;
    im.height = roi->h;
    im.stride = roi->w;
    im.buf = grayscale_image;

    switch(ptr->bpp) {
        case IMAGE_BPP_BINARY: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
                    *(grayscale_image++) = COLOR_BINARY_TO_GRAYSCALE(IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x));
                }
            }
            break;
        }
        case IMAGE_BPP_GRAYSCALE: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
                    *(grayscale_image++) = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x);
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = roi->y, yy = roi->y + roi->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(ptr, y);
                for (int x = roi->x, xx = roi->x + roi->w; x < xx; x++) {
                    *(grayscale_image++) = COLOR_RGB565_TO_GRAYSCALE(IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x));
                }
            }
            break;
        }
        default: {
            memset(grayscale_image, 0, roi->w * roi->h);
            break;
        }
    }

    zarray_t *detections = apriltag_detector_detect(td, &im);
    list_init(out, sizeof(find_apriltags_list_lnk_data_t));

    for (int i = 0, j = zarray_size(detections); i < j; i++) {
        apriltag_detection_t *det;
        zarray_get(detections, i, &det);

        find_apriltags_list_lnk_data_t lnk_data;
        rectangle_init(&(lnk_data.rect), fast_roundf(det->p[0][0]) + roi->x, fast_roundf(det->p[0][1]) + roi->y, 0, 0);

        for (size_t k = 1, l = (sizeof(det->p) / sizeof(det->p[0])); k < l; k++) {
            rectangle_t temp;
            rectangle_init(&temp, fast_roundf(det->p[k][0]) + roi->x, fast_roundf(det->p[k][1]) + roi->y, 0, 0);
            rectangle_united(&(lnk_data.rect), &temp);
        }

        // Add corners...
        lnk_data.corners[0].x = fast_roundf(det->p[3][0]) + roi->x; // top-left
        lnk_data.corners[0].y = fast_roundf(det->p[3][1]) + roi->y; // top-left
        lnk_data.corners[1].x = fast_roundf(det->p[2][0]) + roi->x; // top-right
        lnk_data.corners[1].y = fast_roundf(det->p[2][1]) + roi->y; // top-right
        lnk_data.corners[2].x = fast_roundf(det->p[1][0]) + roi->x; // bottom-right
        lnk_data.corners[2].y = fast_roundf(det->p[1][1]) + roi->y; // bottom-right
        lnk_data.corners[3].x = fast_roundf(det->p[0][0]) + roi->x; // bottom-left
        lnk_data.corners[3].y = fast_roundf(det->p[0][1]) + roi->y; // bottom-left

        lnk_data.id = det->id;
        lnk_data.family = 0;

        if(det->family == &tag16h5) {
            lnk_data.family |= TAG16H5;
        }

        if(det->family == &tag25h7) {
            lnk_data.family |= TAG25H7;
        }

        if(det->family == &tag25h9) {
            lnk_data.family |= TAG25H9;
        }

        if(det->family == &tag36h10) {
            lnk_data.family |= TAG36H10;
        }

        if(det->family == &tag36h11) {
            lnk_data.family |= TAG36H11;
        }

        if(det->family == &artoolkit) {
            lnk_data.family |= ARTOOLKIT;
        }

        lnk_data.hamming = det->hamming;
        lnk_data.centroid.x = fast_roundf(det->c[0]) + roi->x;
        lnk_data.centroid.y = fast_roundf(det->c[1]) + roi->y;
        lnk_data.goodness = det->goodness / 255.0; // scale to [0:1]
        lnk_data.decision_margin = det->decision_margin / 255.0; // scale to [0:1]

        matd_t *pose = homography_to_pose(det->H, -fx, fy, cx, cy);

        lnk_data.x_translation = MATD_EL(pose, 0, 3);
        lnk_data.y_translation = MATD_EL(pose, 1, 3);
        lnk_data.z_translation = MATD_EL(pose, 2, 3);
        lnk_data.x_rotation = fast_atan2f(MATD_EL(pose, 2, 1), MATD_EL(pose, 2, 2));
        lnk_data.y_rotation = fast_atan2f(-MATD_EL(pose, 2, 0), fast_sqrtf(sq(MATD_EL(pose, 2, 1)) + sq(MATD_EL(pose, 2, 2))));
        lnk_data.z_rotation = fast_atan2f(MATD_EL(pose, 1, 0), MATD_EL(pose, 0, 0));

        matd_destroy(pose);

        list_push_back(out, &lnk_data);
    }

    apriltag_detections_destroy(detections);
    fb_free(); // grayscale_image;
    apriltag_detector_destroy(td);
    fb_free(); // umm_init_x();
}

#ifdef IMLIB_ENABLE_FIND_RECTS
void imlib_find_rects(list_t *out, image_t *ptr, rectangle_t *roi, uint32_t threshold)
{
    // Frame Buffer Memory Usage...
    // -> GRAYSCALE Input Image = w*h*1
    // -> GRAYSCALE Threhsolded Image = w*h*1
    // -> UnionFind = w*h*4 (+w*h*2 for hash table)
    size_t resolution = roi->w * roi->h;
    size_t fb_alloc_need = resolution * (1 + 1 + 4 + 2); // read above...
    umm_init_x(((fb_avail() - fb_alloc_need) / resolution) * resolution);
    apriltag_detector_t *td = apriltag_detector_create();

    image_t img;
    img.w = roi->w;
    img.h = roi->h;
    img.bpp = IMAGE_BPP_GRAYSCALE;
    img.data = fb_alloc(image_size(&img), FB_ALLOC_NO_HINT);
    imlib_draw_image(&img, ptr, 0, 0, 1.f, 1.f, roi, -1, 256, NULL, NULL, 0, NULL, NULL);

    image_u8_t im;
    im.width = roi->w;
    im.height = roi->h;
    im.stride = roi->w;
    im.buf = img.data;

    ///////////////////////////////////////////////////////////
    // Detect quads according to requested image decimation
    // and blurring parameters.

//    zarray_t *detections = apriltag_quad_gradient(td, &im, true);
    zarray_t *detections = apriltag_quad_thresh(td, &im, true);

    td->nquads = zarray_size(detections);

    ////////////////////////////////////////////////////////////////
    // Decode tags from each quad.
    if (1) {
        for (int i = 0; i < zarray_size(detections); i++) {
            struct quad *quad_original;
            zarray_get_volatile(detections, i, &quad_original);

            // refine edges is not dependent upon the tag family, thus
            // apply this optimization BEFORE the other work.
            //if (td->quad_decimate > 1 && td->refine_edges) {
            if (td->refine_edges) {
                refine_edges(td, &im, quad_original);
            }

            // make sure the homographies are computed...
            if (quad_update_homographies(quad_original))
                continue;
        }
    }

    ////////////////////////////////////////////////////////////////
    // Reconcile detections--- don't report the same tag more
    // than once. (Allow non-overlapping duplicate detections.)
    if (1) {
        zarray_t *poly0 = g2d_polygon_create_zeros(4);
        zarray_t *poly1 = g2d_polygon_create_zeros(4);

        for (int i0 = 0; i0 < zarray_size(detections); i0++) {

            struct quad *det0;
            zarray_get_volatile(detections, i0, &det0);

            for (int k = 0; k < 4; k++)
                zarray_set(poly0, k, det0->p[k], NULL);

            for (int i1 = i0+1; i1 < zarray_size(detections); i1++) {

                struct quad *det1;
                zarray_get_volatile(detections, i1, &det1);

                for (int k = 0; k < 4; k++)
                    zarray_set(poly1, k, det1->p[k], NULL);

                if (g2d_polygon_overlaps_polygon(poly0, poly1)) {
                    // the tags overlap. Delete one, keep the other.

                    int pref = 0; // 0 means undecided which one we'll keep.

                    // if we STILL don't prefer one detection over the other, then pick
                    // any deterministic criterion.
                    for (int i = 0; i < 4; i++) {
                        pref = prefer_smaller(pref, det0->p[i][0], det1->p[i][0]);
                        pref = prefer_smaller(pref, det0->p[i][1], det1->p[i][1]);
                    }

                    if (pref == 0) {
                        // at this point, we should only be undecided if the tag detections
                        // are *exactly* the same. How would that happen?
                        // printf("uh oh, no preference for overlappingdetection\n");
                    }

                    if (pref < 0) {
                        // keep det0, destroy det1
                        matd_destroy(det1->H);
                        matd_destroy(det1->Hinv);
                        zarray_remove_index(detections, i1, 1);
                        i1--; // retry the same index
                        goto retry1;
                    } else {
                        // keep det1, destroy det0
                        matd_destroy(det0->H);
                        matd_destroy(det0->Hinv);
                        zarray_remove_index(detections, i0, 1);
                        i0--; // retry the same index.
                        goto retry0;
                    }
                }

              retry1: ;
            }

          retry0: ;
        }

        zarray_destroy(poly0);
        zarray_destroy(poly1);
    }

    list_init(out, sizeof(find_rects_list_lnk_data_t));

    const int r_diag_len = fast_roundf(fast_sqrtf((roi->w * roi->w) + (roi->h * roi->h))) * 2;
    int *theta_buffer = fb_alloc(sizeof(int) * r_diag_len, FB_ALLOC_NO_HINT);
    uint32_t *mag_buffer = fb_alloc(sizeof(uint32_t) * r_diag_len, FB_ALLOC_NO_HINT);
    point_t *point_buffer = fb_alloc(sizeof(point_t) * r_diag_len, FB_ALLOC_NO_HINT);

    for (int i = 0, j = zarray_size(detections); i < j; i++) {
        struct quad *det;
        zarray_get_volatile(detections, i, &det);

        line_t lines[4];
        lines[0].x1 = fast_roundf(det->p[0][0]) + roi->x; lines[0].y1 = fast_roundf(det->p[0][1]) + roi->y;
        lines[0].x2 = fast_roundf(det->p[1][0]) + roi->x; lines[0].y2 = fast_roundf(det->p[1][1]) + roi->y;
        lines[1].x1 = fast_roundf(det->p[1][0]) + roi->x; lines[1].y1 = fast_roundf(det->p[1][1]) + roi->y;
        lines[1].x2 = fast_roundf(det->p[2][0]) + roi->x; lines[1].y2 = fast_roundf(det->p[2][1]) + roi->y;
        lines[2].x1 = fast_roundf(det->p[2][0]) + roi->x; lines[2].y1 = fast_roundf(det->p[2][1]) + roi->y;
        lines[2].x2 = fast_roundf(det->p[3][0]) + roi->x; lines[2].y2 = fast_roundf(det->p[3][1]) + roi->y;
        lines[3].x1 = fast_roundf(det->p[3][0]) + roi->x; lines[3].y1 = fast_roundf(det->p[3][1]) + roi->y;
        lines[3].x2 = fast_roundf(det->p[0][0]) + roi->x; lines[3].y2 = fast_roundf(det->p[0][1]) + roi->y;

        uint32_t magnitude = 0;

        for (int i = 0; i < 4; i++) {
            if(!lb_clip_line(&lines[i], 0, 0, roi->w, roi->h)) {
                continue;
            }

            size_t index = trace_line(ptr, &lines[i], theta_buffer, mag_buffer, point_buffer);

            for (int j = 0; j < index; j++) {
                magnitude += mag_buffer[j];
            }
        }

        if (magnitude < threshold) {
            continue;
        }

        find_rects_list_lnk_data_t lnk_data;
        rectangle_init(&(lnk_data.rect), fast_roundf(det->p[0][0]) + roi->x, fast_roundf(det->p[0][1]) + roi->y, 0, 0);

        for (size_t k = 1, l = (sizeof(det->p) / sizeof(det->p[0])); k < l; k++) {
            rectangle_t temp;
            rectangle_init(&temp, fast_roundf(det->p[k][0]) + roi->x, fast_roundf(det->p[k][1]) + roi->y, 0, 0);
            rectangle_united(&(lnk_data.rect), &temp);
        }

        // Add corners...
        lnk_data.corners[0].x = fast_roundf(det->p[3][0]) + roi->x; // top-left
        lnk_data.corners[0].y = fast_roundf(det->p[3][1]) + roi->y; // top-left
        lnk_data.corners[1].x = fast_roundf(det->p[2][0]) + roi->x; // top-right
        lnk_data.corners[1].y = fast_roundf(det->p[2][1]) + roi->y; // top-right
        lnk_data.corners[2].x = fast_roundf(det->p[1][0]) + roi->x; // bottom-right
        lnk_data.corners[2].y = fast_roundf(det->p[1][1]) + roi->y; // bottom-right
        lnk_data.corners[3].x = fast_roundf(det->p[0][0]) + roi->x; // bottom-left
        lnk_data.corners[3].y = fast_roundf(det->p[0][1]) + roi->y; // bottom-left

        lnk_data.magnitude = magnitude;

        list_push_back(out, &lnk_data);
    }

    fb_free(); // point_buffer
    fb_free(); // mag_buffer
    fb_free(); // theta_buffer

    zarray_destroy(detections);
    fb_free(); // grayscale_image;
    apriltag_detector_destroy(td);
    fb_free(); // umm_init_x();
}
#endif //IMLIB_ENABLE_FIND_RECTS

#ifdef IMLIB_ENABLE_ROTATION_CORR
// http://jepsonsblog.blogspot.com/2012/11/rotation-in-3d-using-opencvs.html
void imlib_rotation_corr(image_t *img, float x_rotation, float y_rotation, float z_rotation,
                         float x_translation, float y_translation,
                         float zoom, float fov, float *corners)
{
    // Create a tmp copy of the image to pull pixels from.
    size_t size = image_size(img);
    void *data = fb_alloc(size, FB_ALLOC_NO_HINT);
    memcpy(data, img->data, size);
    memset(img->data, 0, size);

    umm_init_x(fb_avail());

    int w = img->w;
    int h = img->h;
    float z = (fast_sqrtf((w * w) + (h * h)) / 2) / tanf(fov / 2);
    float z_z = z * zoom;

    matd_t *A1 = matd_create(4, 3);
    MATD_EL(A1, 0, 0) = 1;  MATD_EL(A1, 0, 1) = 0;  MATD_EL(A1, 0, 2) = -w / 2;
    MATD_EL(A1, 1, 0) = 0;  MATD_EL(A1, 1, 1) = 1;  MATD_EL(A1, 1, 2) = -h / 2;
    MATD_EL(A1, 2, 0) = 0;  MATD_EL(A1, 2, 1) = 0;  MATD_EL(A1, 2, 2) = 0;
    MATD_EL(A1, 3, 0) = 0;  MATD_EL(A1, 3, 1) = 0;  MATD_EL(A1, 3, 2) = 1; // needed for z translation

    matd_t *RX = matd_create(4, 4);
    MATD_EL(RX, 0, 0) = 1;  MATD_EL(RX, 0, 1) = 0;                  MATD_EL(RX, 0, 2) = 0;                  MATD_EL(RX, 0, 3) = 0;
    MATD_EL(RX, 1, 0) = 0;  MATD_EL(RX, 1, 1) = +cosf(x_rotation);  MATD_EL(RX, 1, 2) = -sinf(x_rotation);  MATD_EL(RX, 1, 3) = 0;
    MATD_EL(RX, 2, 0) = 0;  MATD_EL(RX, 2, 1) = +sinf(x_rotation);  MATD_EL(RX, 2, 2) = +cosf(x_rotation);  MATD_EL(RX, 2, 3) = 0;
    MATD_EL(RX, 3, 0) = 0;  MATD_EL(RX, 3, 1) = 0;                  MATD_EL(RX, 3, 2) = 0;                  MATD_EL(RX, 3, 3) = 1;

    matd_t *RY = matd_create(4, 4);
    MATD_EL(RY, 0, 0) = +cosf(y_rotation);  MATD_EL(RY, 0, 1) = 0;  MATD_EL(RY, 0, 2) = -sinf(y_rotation);  MATD_EL(RY, 0, 3) = 0;
    MATD_EL(RY, 1, 0) = 0;                  MATD_EL(RY, 1, 1) = 1;  MATD_EL(RY, 1, 2) = 0;                  MATD_EL(RY, 1, 3) = 0;
    MATD_EL(RY, 2, 0) = +sinf(y_rotation);  MATD_EL(RY, 2, 1) = 0;  MATD_EL(RY, 2, 2) = +cosf(y_rotation);  MATD_EL(RY, 2, 3) = 0;
    MATD_EL(RY, 3, 0) = 0;                  MATD_EL(RY, 3, 1) = 0;  MATD_EL(RY, 3, 2) = 0;                  MATD_EL(RY, 3, 3) = 1;

    matd_t *RZ = matd_create(4, 4);
    MATD_EL(RZ, 0, 0) = +cosf(z_rotation);  MATD_EL(RZ, 0, 1) = -sinf(z_rotation);  MATD_EL(RZ, 0, 2) = 0;  MATD_EL(RZ, 0, 3) = 0;
    MATD_EL(RZ, 1, 0) = +sinf(z_rotation);  MATD_EL(RZ, 1, 1) = +cosf(z_rotation);  MATD_EL(RZ, 1, 2) = 0;  MATD_EL(RZ, 1, 3) = 0;
    MATD_EL(RZ, 2, 0) = 0;                  MATD_EL(RZ, 2, 1) = 0;                  MATD_EL(RZ, 2, 2) = 1;  MATD_EL(RZ, 2, 3) = 0;
    MATD_EL(RZ, 3, 0) = 0;                  MATD_EL(RZ, 3, 1) = 0;                  MATD_EL(RZ, 3, 2) = 0;  MATD_EL(RZ, 3, 3) = 1;

    matd_t *R = matd_op("M*M*M", RX, RY, RZ);

    matd_t *T = matd_create(4, 4);
    MATD_EL(T, 0, 0) = 1;   MATD_EL(T, 0, 1) = 0;   MATD_EL(T, 0, 2) = 0;   MATD_EL(T, 0, 3) = x_translation;
    MATD_EL(T, 1, 0) = 0;   MATD_EL(T, 1, 1) = 1;   MATD_EL(T, 1, 2) = 0;   MATD_EL(T, 1, 3) = y_translation;
    MATD_EL(T, 2, 0) = 0;   MATD_EL(T, 2, 1) = 0;   MATD_EL(T, 2, 2) = 1;   MATD_EL(T, 2, 3) = z;
    MATD_EL(T, 3, 0) = 0;   MATD_EL(T, 3, 1) = 0;   MATD_EL(T, 3, 2) = 0;   MATD_EL(T, 3, 3) = 1;

    matd_t *A2 = matd_create(3, 4);
    MATD_EL(A2, 0, 0) = z_z;    MATD_EL(A2, 0, 1) = 0;      MATD_EL(A2, 0, 2) = w / 2;   MATD_EL(A2, 0, 3) = 0;
    MATD_EL(A2, 1, 0) = 0;      MATD_EL(A2, 1, 1) = z_z;    MATD_EL(A2, 1, 2) = h / 2;   MATD_EL(A2, 1, 3) = 0;
    MATD_EL(A2, 2, 0) = 0;      MATD_EL(A2, 2, 1) = 0;      MATD_EL(A2, 2, 2) = 1;       MATD_EL(A2, 2, 3) = 0;

    matd_t *T1 = matd_op("M*M", R, A1);
    matd_t *T2 = matd_op("M*M", T, T1);
    matd_t *T3 = matd_op("M*M", A2, T2);
    matd_t *T4 = matd_inverse(T3);

    if (T4 && corners) {
        float corr[4];
        zarray_t *correspondences = zarray_create(sizeof(float[4]));

        corr[0] = 0;
        corr[1] = 0;
        corr[2] = corners[0];
        corr[3] = corners[1];
        zarray_add(correspondences, &corr);

        corr[0] = w - 1;
        corr[1] = 0;
        corr[2] = corners[2];
        corr[3] = corners[3];
        zarray_add(correspondences, &corr);

        corr[0] = w - 1;
        corr[1] = h - 1;
        corr[2] = corners[4];
        corr[3] = corners[5];
        zarray_add(correspondences, &corr);

        corr[0] = 0;
        corr[1] = h - 1;
        corr[2] = corners[6];
        corr[3] = corners[7];
        zarray_add(correspondences, &corr);

        matd_t *H = homography_compute(correspondences, HOMOGRAPHY_COMPUTE_FLAG_INVERSE);

        if (!H) { // try again...
            H = homography_compute(correspondences, HOMOGRAPHY_COMPUTE_FLAG_SVD);
        }

        if (H) {
            matd_t *T5 = matd_op("M*M", H, T4);
            matd_destroy(H);
            matd_destroy(T4);
            T4 = T5;
        }

        zarray_destroy(correspondences);
    }

    if (T4) {
        float T4_00 = MATD_EL(T4, 0, 0), T4_01 = MATD_EL(T4, 0, 1), T4_02 = MATD_EL(T4, 0, 2);
        float T4_10 = MATD_EL(T4, 1, 0), T4_11 = MATD_EL(T4, 1, 1), T4_12 = MATD_EL(T4, 1, 2);
        float T4_20 = MATD_EL(T4, 2, 0), T4_21 = MATD_EL(T4, 2, 1), T4_22 = MATD_EL(T4, 2, 2);

        if ((fast_fabsf(T4_20) < MATD_EPS) && (fast_fabsf(T4_21) < MATD_EPS)) { // warp affine
            T4_00 /= T4_22;
            T4_01 /= T4_22;
            T4_02 /= T4_22;
            T4_10 /= T4_22;
            T4_11 /= T4_22;
            T4_12 /= T4_22;
            switch(img->bpp) {
                case IMAGE_BPP_BINARY: {
                    uint32_t *tmp = (uint32_t *) data;

                    for (int y = 0, yy = h; y < yy; y++) {
                        uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                        for (int x = 0, xx = w; x < xx; x++) {
                            int sourceX = fast_roundf(T4_00*x + T4_01*y + T4_02);
                            int sourceY = fast_roundf(T4_10*x + T4_11*y + T4_12);

                            if ((0 <= sourceX) && (sourceX < w) && (0 <= sourceY) && (sourceY < h)) {
                                uint32_t *ptr = tmp + (((w + UINT32_T_MASK) >> UINT32_T_SHIFT) * sourceY);
                                int pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, sourceX);
                                IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, pixel);
                            }
                        }
                    }
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    uint8_t *tmp = (uint8_t *) data;

                    for (int y = 0, yy = h; y < yy; y++) {
                        uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                        for (int x = 0, xx = w; x < xx; x++) {
                            int sourceX = fast_roundf(T4_00*x + T4_01*y + T4_02);
                            int sourceY = fast_roundf(T4_10*x + T4_11*y + T4_12);

                            if ((0 <= sourceX) && (sourceX < w) && (0 <= sourceY) && (sourceY < h)) {
                                uint8_t *ptr = tmp + (w * sourceY);
                                int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(ptr, sourceX);
                                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, pixel);
                            }
                        }
                    }
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    uint16_t *tmp = (uint16_t *) data;

                    for (int y = 0, yy = h; y < yy; y++) {
                        uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                        for (int x = 0, xx = w; x < xx; x++) {
                            int sourceX = fast_roundf(T4_00*x + T4_01*y + T4_02);
                            int sourceY = fast_roundf(T4_10*x + T4_11*y + T4_12);

                            if ((0 <= sourceX) && (sourceX < w) && (0 <= sourceY) && (sourceY < h)) {
                                uint16_t *ptr = tmp + (w * sourceY);
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(ptr, sourceX);
                                IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, pixel);
                            }
                        }
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        } else { // warp persepective
            switch(img->bpp) {
                case IMAGE_BPP_BINARY: {
                    uint32_t *tmp = (uint32_t *) data;

                    for (int y = 0, yy = h; y < yy; y++) {
                        uint32_t *row_ptr = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, y);
                        for (int x = 0, xx = w; x < xx; x++) {
                            float xxx = T4_00*x + T4_01*y + T4_02;
                            float yyy = T4_10*x + T4_11*y + T4_12;
                            float zzz = T4_20*x + T4_21*y + T4_22;
                            int sourceX = fast_roundf(xxx / zzz);
                            int sourceY = fast_roundf(yyy / zzz);

                            if ((0 <= sourceX) && (sourceX < w) && (0 <= sourceY) && (sourceY < h)) {
                                uint32_t *ptr = tmp + (((w + UINT32_T_MASK) >> UINT32_T_SHIFT) * sourceY);
                                int pixel = IMAGE_GET_BINARY_PIXEL_FAST(ptr, sourceX);
                                IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, pixel);
                            }
                        }
                    }
                    break;
                }
                case IMAGE_BPP_GRAYSCALE: {
                    uint8_t *tmp = (uint8_t *) data;

                    for (int y = 0, yy = h; y < yy; y++) {
                        uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                        for (int x = 0, xx = w; x < xx; x++) {
                            float xxx = T4_00*x + T4_01*y + T4_02;
                            float yyy = T4_10*x + T4_11*y + T4_12;
                            float zzz = T4_20*x + T4_21*y + T4_22;
                            int sourceX = fast_roundf(xxx / zzz);
                            int sourceY = fast_roundf(yyy / zzz);

                            if ((0 <= sourceX) && (sourceX < w) && (0 <= sourceY) && (sourceY < h)) {
                                uint8_t *ptr = tmp + (w * sourceY);
                                int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(ptr, sourceX);
                                IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, pixel);
                            }
                        }
                    }
                    break;
                }
                case IMAGE_BPP_RGB565: {
                    uint16_t *tmp = (uint16_t *) data;

                    for (int y = 0, yy = h; y < yy; y++) {
                        uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, y);
                        for (int x = 0, xx = w; x < xx; x++) {
                            float xxx = T4_00*x + T4_01*y + T4_02;
                            float yyy = T4_10*x + T4_11*y + T4_12;
                            float zzz = T4_20*x + T4_21*y + T4_22;
                            int sourceX = fast_roundf(xxx / zzz);
                            int sourceY = fast_roundf(yyy / zzz);

                            if ((0 <= sourceX) && (sourceX < w) && (0 <= sourceY) && (sourceY < h)) {
                                uint16_t *ptr = tmp + (w * sourceY);
                                int pixel = IMAGE_GET_RGB565_PIXEL_FAST(ptr, sourceX);
                                IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, pixel);
                            }
                        }
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }

        matd_destroy(T4);
    }

    matd_destroy(T3);
    matd_destroy(T2);
    matd_destroy(T1);
    matd_destroy(A2);
    matd_destroy(T);
    matd_destroy(R);
    matd_destroy(RZ);
    matd_destroy(RY);
    matd_destroy(RX);
    matd_destroy(A1);

    fb_free(); // umm_init_x();

    fb_free();
}
#endif //IMLIB_ENABLE_ROTATION_CORR
#pragma GCC diagnostic pop

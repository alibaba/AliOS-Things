#ifndef KV_UTIL_H
#define KV_UTIL_H

// System includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifdef KV_SIM_DEBUG
    #include <pthread.h>
#else
    #include <aos/kernel.h>
#endif

// Logging functions
#ifdef KV_YES_TRACE
#define KV_TRACE(fmt, ...) \
    printf("kv_trace:%d: " fmt "\n", __LINE__, __VA_ARGS__)
#else
#define KV_TRACE(fmt, ...)
#endif

#ifndef KV_NO_DEBUG
#define KV_DEBUG(fmt, ...) \
    printf("kv_debug:%d: " fmt "\n", __LINE__, __VA_ARGS__)
#else
#define KV_DEBUG(fmt, ...)
#endif

#ifndef KV_NO_WARN
#define KV_WARN(fmt, ...) \
    printf("kv_warn:%d: " fmt "\n", __LINE__, __VA_ARGS__)
#else
#define KV_WARN(fmt, ...)
#endif

#ifndef KV_NO_ERROR
#define KV_ERROR(fmt, ...) \
    printf("kv_error:%d: " fmt "\n", __LINE__, __VA_ARGS__)
#else
#define KV_ERROR(fmt, ...)
#endif

// Runtime assertions
#ifndef KV_NO_ASSERT
    #define KV_ASSERT(test) assert(test)
#else
    #define KV_ASSERT(test)
#endif

// Convert between 32-bit little-endian and native order
static inline uint32_t kv_fromle32(uint32_t a)
{
    return (((uint8_t *)&a)[0] <<  0) |
           (((uint8_t *)&a)[1] <<  8) |
           (((uint8_t *)&a)[2] << 16) |
           (((uint8_t *)&a)[3] << 24);
}

static inline uint32_t kv_tole32(uint32_t a)
{
    return kv_fromle32(a);
}

// Convert between 16-bit little-endian and native order
static inline uint16_t kv_fromle16(uint16_t a)
{
    return (((uint8_t *)&a)[0] <<  0) |
           (((uint8_t *)&a)[1] <<  8);
}

static inline uint16_t kv_tole16(uint16_t a)
{
    return kv_fromle16(a);
}

// Min/max functions for unsigned 32-bit numbers
static inline uint32_t kv_max(uint32_t a, uint32_t b)
{
    return (a > b) ? a : b;
}

static inline uint32_t kv_min(uint32_t a, uint32_t b)
{
    return (a < b) ? a : b;
}

// Align to nearest multiple of a size
static inline uint32_t kv_aligndown(uint32_t a, uint32_t alignment)
{
    return a - (a % alignment);
}

static inline uint32_t kv_alignup(uint32_t a, uint32_t alignment)
{
    return kv_aligndown(a + alignment - 1, alignment);
}

// Find the sequence comparison of a and b, this is the distance
// between a and b ignoring overflow
static inline int kv_scmp(uint32_t a, uint32_t b)
{
    return (int)(unsigned)(a - b);
}

// calc CRC32
uint32_t kv_crc32(uint32_t init, const void *buffer, size_t size);

// mutex
#ifdef KV_SIM_DEBUG
    typedef pthread_mutex_t kv_lock_t;
    #define aos_malloc malloc
    #define aos_free free
#else
    typedef aos_mutex_t kv_lock_t;
#endif
void kv_lock_create(kv_lock_t *lock);
void kv_lock_destory(kv_lock_t *lock);
void kv_lock(kv_lock_t *lock);
void kv_unlock(kv_lock_t *lock);

#endif /* KV_UTIL_H */

#include <stdint.h>
#include <string.h>
#include "cmsis_gcc.h"
#include "unaligned_memcpy.h"

// ARM Cortex-M4/M7 Processors can access memory using unaligned 32-bit reads/writes.
void *unaligned_memcpy(void *dest, void *src, size_t n)
{
    #if (CPU==cortex-m4) || (CPU==cortex-m7)
    // TODO: Make this faster using only 32-bit aligned reads/writes with data shifting.
    uint32_t *dest32 = (uint32_t *) dest;
    uint32_t *src32 = (uint32_t *) src;

    for (; n > 4; n -= 4) {
        *dest32++ = *src32++;
    }

    uint8_t *dest8 = (uint8_t *) dest32;
    uint8_t *src8 = (uint8_t *) src32;

    for (; n > 0; n -= 1) {
        *dest8++ = *src8++;
    }

    return dest;
    #else
    return memcpy(dest, src, n);
    #endif
}

// ARM Cortex-M4/M7 Processors can access memory using unaligned 32-bit reads/writes.
void *unaligned_memcpy_rev16(void *dest, void *src, size_t n)
{
    uint32_t *dest32 = (uint32_t *) dest;
    uint32_t *src32 = (uint32_t *) src;

    #if (CPU==cortex-m4) || (CPU==cortex-m7)
    // TODO: Make this faster using only 32-bit aligned reads/writes with data shifting.
    for (; n > 2; n -= 2) {
        *dest32++ = __REV16(*src32++);
    }
    #endif

    uint16_t *dest16 = (uint16_t *) dest32;
    uint16_t *src16 = (uint16_t *) src32;

    for (; n > 0; n -= 1) {
        *dest16++ = __REV16(*src16++);
    }

    return dest;
}

void *unaligned_2_to_1_memcpy(void *dest, void *src, size_t n)
{
    uint32_t *dest32 = (uint32_t *) dest;
    uint32_t *src32 = (uint32_t *) src;

    #if (CPU==cortex-m4) || (CPU==cortex-m7)
    // TODO: Make this faster using only 32-bit aligned reads/writes with data shifting.
    for (; n > 4; n -= 4) {
        uint32_t tmp1 = *src32++;
        uint32_t tmp2 = *src32++;
        *dest32++ = (tmp1 & 0xff) | ((tmp1 >> 8) & 0xff00) | ((tmp2 & 0xff) << 16) | ((tmp2 & 0xff0000) << 8);
    }
    #endif

    uint8_t *dest8 = (uint8_t *) dest32;
    uint16_t *src16 = (uint16_t *) src32;

    for (; n > 0; n -= 1) {
        *dest8++ = *src16++;
    }

    return dest;
}

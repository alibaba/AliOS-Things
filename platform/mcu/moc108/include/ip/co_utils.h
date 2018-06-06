/**
 ****************************************************************************************
 *
 * @file co_utils.h
 *
 * @brief Common utilities definitions
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _CO_UTILS_H_
#define _CO_UTILS_H_

/**
 ****************************************************************************************
 * @defgroup CO_UTILS CO_UTILS
 * @ingroup COMMON
 * @brief  Common utilities
 *
 * This module contains the common utilities functions and macros
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "compiler.h"
#include <limits.h>
#include "co_math.h"

#ifndef CHAR_BIT
#define CHAR_BIT                     8
#endif

/*
 * MACROS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Converts a STA index in an Association Index.
 * @param[in] sta_idx The station index.
 * @return The association index
 ****************************************************************************************
 */
#define CO_STAIDX_TO_AID(sta_idx) ((sta_idx) + 1)

/**
 ****************************************************************************************
 * @brief Get the index of an element in an array.
 * @param[in] __element_ptr Pointer to the element
 * @param[in] __array_ptr Pointer to the array
 * @return The index of the element
 ****************************************************************************************
 */
#define CO_GET_INDEX(__element_ptr, __array_ptr) ((__element_ptr) - (__array_ptr))

/// Length of a char in bytes
#define CHAR_LEN    (CHAR_BIT/8)

/**
 ****************************************************************************************
 * @brief Converts a CPU pointer into a HW address
 * This macro is used to convert a SW pointer into the corresponding HW address. With CPUs
 * having native byte support, the value returned will be the same as the pointer passed.
 * With TL4, the value returned is the pointer multiplied by 2.
 * @param[in] ptr Pointer to be converted
 * @return The corresponding HW address
 ****************************************************************************************
 */
#define CPU2HW(ptr) (((uint32_t)(ptr)) * CHAR_LEN)

/**
 ****************************************************************************************
 * @brief Converts a HW address into a CPU pointer
 * This macro is doing the reverse operation as @ref CPU2HW.
 * @param[in] ptr Address to be converted
 * @return The corresponding CPU pointer
 ****************************************************************************************
 */
#define HW2CPU(ptr) ((void *)(((uint32_t)(ptr)) / CHAR_LEN))

/**
 ****************************************************************************************
 * @brief Return the size of a variable or type in bytes
 * @param[in] a Variable for which the size is computed
 * @return The size of the variable in bytes
 ****************************************************************************************
 */
#define sizeof_b(a) (sizeof(a) * CHAR_LEN)

/**
 ****************************************************************************************
 * @brief Return the offset (in bytes) of a structure element
 * @param[in] a Structure type
 * @param[in] b Field name
 * @return The of the field in bytes
 ****************************************************************************************
 */
#define offsetof_b(a, b)  (offsetof(a, b) * CHAR_LEN)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Read an aligned 16 bits word.
 * @param[in] ptr16 The address of the first byte of the 16 bits word.
 * @return The 16 bits value.
 ****************************************************************************************
 */
__INLINE uint16_t co_read16(void const *ptr16)
{
    return *((uint16_t*)ptr16);
}

/**
 ****************************************************************************************
 * @brief Write an aligned 16 bits word.
 * @param[in] ptr16 The address of the first byte of the 16 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__INLINE void co_write16(void const *ptr16, uint32_t value)
{
    *(uint16_t*)ptr16 = value;
}

/**
 ****************************************************************************************
 * @brief Read an aligned 32 bit word.
 * @param[in] ptr32 The address of the first byte of the 32 bit word.
 * @return The 32 bit value.
 ****************************************************************************************
 */
__INLINE uint32_t co_read32(void const *ptr32)
{
    return *((uint32_t*)ptr32);
}

/**
 ****************************************************************************************
 * @brief Write an aligned 32 bits word.
 * @param[in] ptr32 The address of the first byte of the 32 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__INLINE void co_write32(void const *ptr32, uint32_t value)
{
    *(uint32_t*)ptr32 = value;
}

/**
 ****************************************************************************************
 * @brief Copy a 32-bit aligned buffer into another one
 * The length in bytes is converted to the corresponding number of 32-bit words. If the
 * byte length is not a multiple of 4, then additional bytes will be copied at the end
 * of the buffer. It is the responsibility of the caller to ensure that these extra-byte
 * copy won't corrupt the memory at the end of the destination buffer.
 *
 * @param[in] dst Pointer to the destination buffer
 * @param[in] src Pointer to the source buffer
 * @param[in] len Length to be copied (in bytes)
 ****************************************************************************************
 */
__INLINE void co_copy32(uint32_t *dst, uint32_t *src, uint32_t len)
{
    len = CO_ALIGN4_HI(len)/4;
    while (len--)
    {
        *dst++ = *src++;
    }
}

/**
 ****************************************************************************************
 * @brief Read a 8 bits word.
 * @param[in] addr The address of the first byte of the 8 bits word.
 * @return The read value
 ****************************************************************************************
 */
__INLINE uint8_t co_read8p(uint32_t addr)
{
    return (*(uint8_t *)addr);
}

/**
 ****************************************************************************************
 * @brief Write a 8 bits word.
 * @param[in] addr The address of the first byte of the 8 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__INLINE void co_write8p(uint32_t addr, uint8_t value)
{    
    *(uint8_t *)addr = value;
}


/**
 ****************************************************************************************
 * @brief Read a packed 16 bits word.
 * @param[in] addr The address of the first byte of the 16 bits word.
 * @return The 16 bits value.
 ****************************************************************************************
 */
__INLINE uint16_t co_read16p(uint32_t addr)
{
    struct co_read16_struct
    {
        __PACKED16 uint16_t val;
    } *ptr = (struct co_read16_struct*)addr;
    return ptr->val;
}

/**
 ****************************************************************************************
 * @brief Write a packed 16 bits word.
 * @param[in] addr The address of the first byte of the 16 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__INLINE void co_write16p(uint32_t addr, uint16_t value)
{
    uint8_t *dest = (uint8_t *)addr;
    
    dest[0] = value & 0xff;
    dest[1] = (value >> 8) & 0xff;
}

/**
 ****************************************************************************************
 * @brief Read a packed 32 bits word.
 * @param[in] addr The address of the first byte of the 32 bits word.
 * @return The 32 bits value.
 ****************************************************************************************
 */
__INLINE uint32_t co_read32p(uint32_t addr)
{
    struct co_read32_struct
    {
        __PACKED uint32_t val;
    } *ptr = (struct co_read32_struct*) addr;
    return ptr->val;
}

/**
 ****************************************************************************************
 * @brief Write a packed 32 bits word.
 * @param[in] addr The address of the first byte of the 32 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__INLINE void co_write32p(uint32_t addr, uint32_t value)
{
    struct co_read32_struct
    {
        __PACKED uint32_t val;
    } *ptr = (struct co_read32_struct*) addr;
    ptr->val = value;
}

/**
 ****************************************************************************************
 * @brief Read a packed 64 bits word.
 * @param[in] addr The address of the first byte of the 64 bits word.
 * @return The 64 bits value.
 ****************************************************************************************
 */
__INLINE uint64_t co_read64p(uint32_t addr)
{
    struct co_read64_struct
    {
        __PACKED uint64_t val;
    } *ptr = (struct co_read64_struct*) addr;
    return ptr->val;
}

/**
 ****************************************************************************************
 * @brief Compare a packed byte buffer with a CPU byte array
 * @param[in] pkd The address of the packed buffer.
 * @param[in] ptr Pointer to the CPU byte array
 * @param[in] len Length to be compared
 *
 * @return true if the buffers are equal, false otherwise
 ****************************************************************************************
 */
__INLINE bool co_cmp8p(uint32_t pkd, uint8_t const *ptr, uint32_t len)
{
    while (len--)
    {
        if (co_read8p(pkd++) != (*ptr++ & 0xFF))
            return false;
    }
    return true;
}

/**
 ****************************************************************************************
 * @brief Copy and pack a byte array to another one
 * @param[in] dst The address of the first byte of the packed buffer in which the data has
 *                to be copied.
 * @param[in] src Pointer to the source buffer
 * @param[in] len Length to be copied
 ****************************************************************************************
 */
__INLINE void co_pack8p(uint32_t dst, uint8_t const *src, uint32_t len)
{
    while (len--)
    {
        co_write8p(dst++, *src++);
    }
}

/**
 ****************************************************************************************
 * @brief Copy and unpack a byte array to another one
 * @param[in] dst Pointer to the first byte of the unpacked buffer in which the data has
 *                to be copied.
 * @param[in] src Address of the packed source buffer
 * @param[in] len Length to be copied
 ****************************************************************************************
 */
__INLINE void co_unpack8p(uint8_t *dst, uint32_t src, uint32_t len)
{
    while (len--)
    {
        *dst++ = co_read8p(src++);
    }
}

/**
 ****************************************************************************************
 * @brief Copy a packed byte array to another packed byte array
 * @param[in] dst The address of the first byte of the packed buffer in which the data has
 *                to be copied.
 * @param[in] src Address of the source buffer
 * @param[in] len Length to be copied
 ****************************************************************************************
 */
__INLINE void co_copy8p(uint32_t dst, uint32_t src, uint32_t len)
{
    while (len--)
    {
        co_write8p(dst++, co_read8p(src++));
    }
}

/// @} end of group COUTILS

#endif // _CO_UTILS_H_

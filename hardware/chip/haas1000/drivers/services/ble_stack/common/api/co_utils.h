#ifndef _CO_UTILS_H_
#define _CO_UTILS_H_

/**
 ****************************************************************************************
 * @defgroup CO_UTILS Utilities
 * @ingroup COMMON
 * @brief  Common utilities
 *
 * This module contains the common utilities functions and macros.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>       // standard definitions
#include <stddef.h>       // standard definitions
#include "co_bt.h"        // common bt definitions
#include "rwip_config.h"  // SW configuration
#include "compiler.h"     // for inline functions


/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/// Number of '1' bits in a byte
#define NB_ONE_BITS(byte)   (one_bits[byte & 0x0F] + one_bits[byte >> 4])

/// Get the number of elements within an array, give also number of rows in a 2-D array
#define ARRAY_LEN(array)   (sizeof((array))/sizeof((array)[0]))

/// Get the number of columns within a 2-D array
#define ARRAY_NB_COLUMNS(array)  (sizeof((array[0]))/sizeof((array)[0][0]))


/// Macro for LMP message handler function declaration or definition
#define LMP_MSG_HANDLER(msg_name)   __STATIC int lmp_##msg_name##_handler(struct lmp_##msg_name const *param,  \
                                                                                ke_task_id_t const dest_id)
/// Macro for LMP message handler function declaration or definition
#define LLCP_MSG_HANDLER(msg_name)   __STATIC int llcp_##msg_name##_handler(struct llcp_##msg_name const *param,  \
                                                                                ke_task_id_t const dest_id)

/// Macro for HCI message handler function declaration or definition (for multi-instantiated tasks)
#define HCI_CMD_HANDLER_C(cmd_name, param_struct)   __STATIC int hci_##cmd_name##_cmd_handler(param_struct const *param,  \
                                                                                ke_task_id_t const dest_id,  \
                                                                                uint16_t opcode)

/// Macro for HCI message handler function declaration or definition (with parameters)
#define HCI_CMD_HANDLER(cmd_name, param_struct)   __STATIC int hci_##cmd_name##_cmd_handler(param_struct const *param,  \
                                                                                uint16_t opcode)

/// Macro for HCI message handler function declaration or definition (with parameters)
#define HCI_CMD_HANDLER_TAB(task)   __STATIC const struct task##_hci_cmd_handler task##_hci_command_handler_tab[] =


/// MACRO to build a subversion field from the Minor and Release fields
#define CO_SUBVERSION_BUILD(minor, release)     (((minor) << 8) | (release))


/// Macro to get a structure from one of its structure field
//#define CONTAINER_OF(ptr, type, member)    ((type *)( (char *)ptr - offsetof(type,member) ))


/*
 * ENUMERATIONS DEFINITIONS
 ****************************************************************************************
 */

/// Status returned by generic packer-unpacker
enum CO_UTIL_PACK_STATUS
{
    CO_UTIL_PACK_OK,
    CO_UTIL_PACK_IN_BUF_OVFLW,
    CO_UTIL_PACK_OUT_BUF_OVFLW,
    CO_UTIL_PACK_WRONG_FORMAT,
    CO_UTIL_PACK_ERROR,
};


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/*
 * CONSTANT DECLARATIONS
 ****************************************************************************************
 */

/// Number of '1' bits in values from 0 to 15, used to fasten bit counting
extern const unsigned char one_bits[16];

/// Conversion table Sleep Clock Accuracy to PPM
extern const uint16_t co_sca2ppm[];

/// NULL BD address
extern const struct bd_addr co_null_bdaddr;

/// Default BD address
extern const struct bd_addr co_default_bdaddr;


/*
 * OPERATIONS ON BT CLOCK
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Clocks addition with 2 operands
 *
 * @param[in]   clock_a   1st operand value (in BT slots)
 * @param[in]   clock_b   2nd operand value (in BT slots)
 * @return      result    operation result (in BT slots)
 ****************************************************************************************
 */
#define CLK_ADD_2(clock_a, clock_b)     ((uint32_t)(((clock_a) + (clock_b)) & RWIP_MAX_CLOCK_TIME))

/**
 ****************************************************************************************
 * @brief Clocks addition with 3 operands
 *
 * @param[in]   clock_a   1st operand value (in BT slots)
 * @param[in]   clock_b   2nd operand value (in BT slots)
 * @param[in]   clock_c   3rd operand value (in BT slots)
 * @return      result    operation result (in BT slots)
 ****************************************************************************************
 */
#define CLK_ADD_3(clock_a, clock_b, clock_c)     ((uint32_t)(((clock_a) + (clock_b) + (clock_c)) & RWIP_MAX_CLOCK_TIME))

/**
 ****************************************************************************************
 * @brief Clocks subtraction
 *
 * @param[in]   clock_a   1st operand value (in BT slots)
 * @param[in]   clock_b   2nd operand value (in BT slots)
 * @return      result    operation result (in BT slots)
 ****************************************************************************************
 */
#define CLK_SUB(clock_a, clock_b)     ((uint32_t)(((clock_a) - (clock_b)) & RWIP_MAX_CLOCK_TIME))

/**
 ****************************************************************************************
 * @brief Clocks time difference
 *
 * @param[in]   clock_a   1st operand value (in BT half-slots)
 * @param[in]   clock_b   2nd operand value (in BT half-slots)
 * @return      result    return the time difference from clock A to clock B
 *                           - result < 0  => clock_b is in the past
 *                           - result == 0 => clock_a is equal to clock_b
 *                           - result > 0  => clock_b is in the future
 ****************************************************************************************
 */
#define CLK_DIFF(clock_a, clock_b)     ( (CLK_SUB((clock_b), (clock_a)) > ((RWIP_MAX_CLOCK_TIME+1) >> 1)) ?                      \
                          ((int32_t)((-CLK_SUB((clock_a), (clock_b))))) : ((int32_t)((CLK_SUB((clock_b), (clock_a))))) )



/// macro to extract a field from a value containing several fields
/// @param[in] __r bit field value
/// @param[in] __f field name
/// @return the value of the register masked and shifted
#define GETF(__r, __f)                                                           \
    (( (__r) & (__f##_MASK) ) >> (__f##_LSB))

/// macro to set a field value into a value  containing several fields.
/// @param[in] __r bit field value
/// @param[in] __f field name
/// @param[in] __v value to put in field
#define SETF(__r, __f, __v)                                                      \
    do {                                                                         \
        ASSERT_INFO( ( ( ( (__v) << (__f##_LSB) ) & ( ~(__f##_MASK) ) ) ) == 0 ,(__f##_MASK), (__v)); \
        __r = (((__r) & ~(__f##_MASK)) | (__v) << (__f##_LSB));                  \
    } while (0)



/// macro to extract a bit field from a value containing several fields
/// @param[in] __r bit field value
/// @param[in] __b bit field name
/// @return the value of the register masked and shifted
#define GETB(__r, __b)                                                           \
    (( (__r) & (__b##_BIT) ) >> (__b##_POS))

/// macro to set a bit field value into a value containing several fields.
/// @param[in] __r bit field value
/// @param[in] __b bit field name
/// @param[in] __v value to put in field
#define SETB(__r, __b, __v)                                                      \
    do {                                                                         \
        ASSERT_ERR( ( ( ( (__v) << (__b##_POS) ) & ( ~(__b##_BIT) ) ) ) == 0 ); \
        __r = (((__r) & ~(__b##_BIT)) | (__v) << (__b##_POS));                  \
    } while (0)

#if (BLE_EMB_PRESENT)
/**
 ******************************************************************************
 * @brief Compare 2 BLE instants (connection event counter)
 * @param[in]   instant_a   1st operand value (connection event counter)
 * @param[in]   instant_b   2nd operand value (connection event counter)
 * @return      result      True: B is greater or equal to A | False: B is smaller than A
 ******************************************************************************
 */
#define CO_BLE_INSTANT_PASSED(instant_a, instant_b)    ((uint16_t)(instant_b - instant_a) < 32767)
#endif //BLE_EMB_PRESENT


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Read an aligned 32 bit word.
 * @param[in] ptr32 The address of the first byte of the 32 bit word.
 * @return The 32 bit value.
 ****************************************************************************************
 */
__STATIC __INLINE uint32_t co_read32(void const *ptr32)
{
    return *((uint32_t*)ptr32);
}

/**
 ****************************************************************************************
 * @brief Read an aligned 16 bits word.
 * @param[in] ptr16 The address of the first byte of the 16 bits word.
 * @return The 16 bits value.
 ****************************************************************************************
 */
__STATIC __INLINE uint16_t co_read16(void const *ptr16)
{
    return *((uint16_t*)ptr16);
}

/**
 ****************************************************************************************
 * @brief Write an aligned 32 bits word.
 * @param[in] ptr32 The address of the first byte of the 32 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__STATIC __INLINE void co_write32(void const *ptr32, uint32_t value)
{
    *(uint32_t*)ptr32 = value;
}

/**
 ****************************************************************************************
 * @brief Write an aligned 16 bits word.
 * @param[in] ptr16 The address of the first byte of the 16 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__STATIC __INLINE void co_write16(void const *ptr16, uint32_t value)
{
    *(uint16_t*)ptr16 = value;
}

/**
 ****************************************************************************************
 * @brief Write a 8 bits word.
 * @param[in] ptr8 The address of the first byte of the 8 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__STATIC __INLINE void co_write8(void const *ptr8, uint32_t value)
{
    *(uint8_t*)ptr8 = value;
}

/**
 ****************************************************************************************
 * @brief Read a packed 16 bits word.
 * @param[in] ptr16 The address of the first byte of the 16 bits word.
 * @return The 16 bits value.
 ****************************************************************************************
 */
__STATIC __INLINE uint16_t co_read16p(void const *ptr16)
{
    uint16_t value = ((uint8_t *)ptr16)[0] | ((uint8_t *)ptr16)[1] << 8;
    return value;
}

/**
 ****************************************************************************************
 * @brief Read a packed 24 bits word.
 * @param[in] ptr24 The address of the first byte of the 24 bits word.
 * @return The 24 bits value.
 ****************************************************************************************
 */
 
__STATIC __INLINE uint32_t co_read24p(void const *ptr24)
{
    uint16_t addr_l, addr_h;
    addr_l = co_read16p(ptr24);
    addr_h = *((uint8_t *)ptr24 + 2) & 0x00FF;
    return ((uint32_t)addr_l | (uint32_t)addr_h << 16);
}

/**
 ****************************************************************************************
 * @brief Write a packed 24 bits word.
 * @param[in] ptr24 The address of the first byte of the 24 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
 
__STATIC __INLINE void co_write24p(void const *ptr24, uint32_t value)
{
    uint8_t *ptr=(uint8_t*)ptr24;

    *ptr++ = (uint8_t)(value&0xff);
    *ptr++ = (uint8_t)((value&0xff00)>>8);
    *ptr++ = (uint8_t)((value&0xff0000)>>16);
}

/**
 ****************************************************************************************
 * @brief Read a packed 32 bits word.
 * @param[in] ptr32 The address of the first byte of the 32 bits word.
 * @return The 32 bits value.
 ****************************************************************************************
 */
__STATIC __INLINE uint32_t co_read32p(void const *ptr32)
{
    uint16_t addr_l, addr_h;
    addr_l = co_read16p(ptr32);
    addr_h = co_read16p((uint8_t *)ptr32 + 2);
    return ((uint32_t)addr_l | (uint32_t)addr_h << 16);
}
/**
 ****************************************************************************************
 * @brief Write a packed 32 bits word.
 * @param[in] ptr32 The address of the first byte of the 32 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__STATIC __INLINE void co_write32p(void const *ptr32, uint32_t value)
{
    uint8_t *ptr=(uint8_t*)ptr32;

    *ptr++ = (uint8_t)(value&0xff);
    *ptr++ = (uint8_t)((value&0xff00)>>8);
    *ptr++ = (uint8_t)((value&0xff0000)>>16);
    *ptr = (uint8_t)((value&0xff000000)>>24);
}

/**
 ****************************************************************************************
 * @brief Write a packed 16 bits word.
 * @param[in] ptr16 The address of the first byte of the 16 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__STATIC __INLINE void co_write16p(void const *ptr16, uint16_t value)
{
    uint8_t *ptr=(uint8_t*)ptr16;

    *ptr++ = value&0xff;
    *ptr = (value&0xff00)>>8;
}

#if RW_DEBUG
/**
 ****************************************************************************************
 * @brief Convert bytes to hexadecimal string
 *
 * @param[out] dest      Pointer to the destination string (must be 2x longer than input table)
 * @param[in]  src       Pointer to the bytes table
 * @param[in]  nb_bytes  Number of bytes to display in the string
 ****************************************************************************************
 */
void co_bytes_to_string(char* dest, uint8_t* src, uint8_t nb_bytes);
#endif //RW_DEBUG

/**
 ****************************************************************************************
 * @brief Compares two Bluetooth device addresses
 *
 * This function checks if the two bd address are equal.
 *
 * @param[in] bd_address1        Pointer on the first bd address to be compared.
 * @param[in] bd_address2        Pointer on the second bd address to be compared.
 *
 * @return result of the comparison (true: equal | false: different).
 ****************************************************************************************
 */
bool co_bdaddr_compare(struct bd_addr const *bd_address1, struct bd_addr const *bd_address2);

#if (BLE_EMB_PRESENT)
/**
 ******************************************************************************
 * @brief Count the number of good channels in a LE map
 * @param[in]  map  Channel Map (bit fields for the 40 BT RF channels)
 * @return  Number of good channels
 ******************************************************************************
 */
uint8_t co_nb_good_le_channels(const struct le_chnl_map* map);
#endif //BLE_EMB_PRESENT

#if (BT_EMB_PRESENT)

/**
 ******************************************************************************
 * @brief Convert an duration in baseband slot to a duration in number of ticks.
 * @param[in]  slot_cnt  Duration in number of baseband slot
 * @return  Duration (in number of ticks).
 ******************************************************************************
 */
uint32_t co_slot_to_duration(uint16_t slot_cnt);

/**
 ******************************************************************************
 * @brief Count the number of good channels in a map
 * @param[in]  map  Channel Map (bit fields for the 79 BT RF channels)
 * @return  Number of good channels
 ******************************************************************************
 */
uint8_t co_nb_good_channels(const struct chnl_map* map);

#endif //BT_EMB_PRESENT

/**
 ****************************************************************************************
 * @brief Pack parameters from a C structure to a packed buffer
 *
 * This function packs parameters according to a specific format. It takes care of the
 * endianess, padding, required by the compiler.
 *
 * Format strings are the mechanism used to specify the expected layout when packing and unpacking data. They are built
 * up from Format Characters, which specify the type of data being packed/unpacked.
 *     - B  : byte - 8bits value
 *     - H  : word - 16bits value
 *     - L  : long - 32-bits value
 *     - XXB: table of several bytes, where XX is the byte number, in decimal
 *
 * Example:   "BBLH12BL" => 1 byte | 1 byte | 1 long | 1 short | 12-bytes table | 1 long
 *
 * Note: the function works in the same buffer
 *
 * @param[out]     out         Output Data Buffer
 * @param[in]      in          Input Data Buffer
 * @param[out]     out_len     Output size of packed data (in bytes)
 * @param[in]      in_len      Input buffer size (in bytes)
 * @param[in]      format      Parameters format
 *
 * @return  Status of the packing operation
 *****************************************************************************************
 */
enum CO_UTIL_PACK_STATUS co_util_pack(uint8_t* out, uint8_t* in, uint16_t* out_len, uint16_t in_len, const char* format);

/**
 ****************************************************************************************
 * @brief Unpack parameters from an unpacked buffer to a C structure
 *
 * This function unpacks parameters according to a specific format. It takes care of the
 * endianess, padding, required by the compiler.
 *
 * Format strings are the mechanism used to specify the expected layout when packing and unpacking data. They are built
 * up from Format Characters, which specify the type of data being packed/unpacked.
 *     - B  : byte - 8bits value
 *     - H  : word - 16bits value
 *     - L  : long - 32-bits value
 *     - XXB: table of several bytes, where XX is the byte number, in decimal
 *
 * Example:   "BBLH12BL" => 1 byte | 1 byte | 1 long | 1 short | 12-bytes table | 1 long
 *
 * Note: the output buffer provided must be large enough to contain the unpacked data.
 * Note2: if a NULL output buffer is provided, the function does not copy the unpacked parameters. It still parses the
 *  format string and input buffer to return the number of unpacked bytes. Can be used to compute the expected unpacked
 *  buffer size.
 *
 * @param[out]    out         Unpacked parameters buffer
 * @param[in]     in          Packed parameters buffer
 * @param[inout]  out_len     Input: buffer size / Output: size of unpacked data (in bytes)
 * @param[in]     in_len      Size of the packed data (in bytes)
 * @param[in]     format      Parameters format
 *
 * @return  Status of the unpacking operation
 *****************************************************************************************
 */
enum CO_UTIL_PACK_STATUS co_util_unpack(uint8_t* out, uint8_t* in, uint16_t* out_len, uint16_t in_len, const char* format);

/// @} CO_UTILS

#endif // _CO_UTILS_H_

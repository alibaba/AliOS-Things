/**
 * @file reg_intc.h
 * @brief Definitions of the INTC HW block registers and register access functions.
 *
 * @defgroup REG_INTC REG_INTC
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the INTC HW block registers and register access functions.
 */
#ifndef _REG_INTC_H_
#define _REG_INTC_H_

#include "co_int.h"
#include "_reg_intc.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_INTC peripheral.
 */
#define REG_INTC_COUNT 17

/** @brief Decoding mask of the REG_INTC peripheral registers from the CPU point of view.
 */
#define REG_INTC_DECODING_MASK 0x0000007F

/**
 * @name IRQ_STATUS register definitions
 *
 * @{
 */

/// Address of the IRQ_STATUS register
#define INTC_IRQ_STATUS_ADDR   0x10910000
/// Offset of the IRQ_STATUS register from the base address
#define INTC_IRQ_STATUS_OFFSET 0x00000000
/// Index of the IRQ_STATUS register
#define INTC_IRQ_STATUS_INDEX  0x00000000
/// Reset value of the IRQ_STATUS register
#define INTC_IRQ_STATUS_RESET  0x00000000
/// Number of elements of the IRQ_STATUS register array
#define INTC_IRQ_STATUS_COUNT  2

/**
 * @brief Returns the current value of the IRQ_STATUS register.
 * The IRQ_STATUS register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the IRQ_STATUS register.
 */
__INLINE uint32_t intc_irq_status_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    return REG_PL_RD(INTC_IRQ_STATUS_ADDR + reg_idx * 4);
}

/// @}

/**
 * @name IRQ_RAW_STATUS register definitions
 *
 * @{
 */

/// Address of the IRQ_RAW_STATUS register
#define INTC_IRQ_RAW_STATUS_ADDR   0x10910008
/// Offset of the IRQ_RAW_STATUS register from the base address
#define INTC_IRQ_RAW_STATUS_OFFSET 0x00000008
/// Index of the IRQ_RAW_STATUS register
#define INTC_IRQ_RAW_STATUS_INDEX  0x00000002
/// Reset value of the IRQ_RAW_STATUS register
#define INTC_IRQ_RAW_STATUS_RESET  0x00000000
/// Number of elements of the IRQ_RAW_STATUS register array
#define INTC_IRQ_RAW_STATUS_COUNT  2

/**
 * @brief Returns the current value of the IRQ_RAW_STATUS register.
 * The IRQ_RAW_STATUS register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the IRQ_RAW_STATUS register.
 */
__INLINE uint32_t intc_irq_raw_status_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    return REG_PL_RD(INTC_IRQ_RAW_STATUS_ADDR + reg_idx * 4);
}

/// @}

/**
 * @name IRQ_UNMASK_SET register definitions
 *
 * @{
 */

/// Address of the IRQ_UNMASK_SET register
#define INTC_IRQ_UNMASK_SET_ADDR   0x10910010
/// Offset of the IRQ_UNMASK_SET register from the base address
#define INTC_IRQ_UNMASK_SET_OFFSET 0x00000010
/// Index of the IRQ_UNMASK_SET register
#define INTC_IRQ_UNMASK_SET_INDEX  0x00000004
/// Reset value of the IRQ_UNMASK_SET register
#define INTC_IRQ_UNMASK_SET_RESET  0x00000000
/// Number of elements of the IRQ_UNMASK_SET register array
#define INTC_IRQ_UNMASK_SET_COUNT  2

/**
 * @brief Returns the current value of the IRQ_UNMASK_SET register.
 * The IRQ_UNMASK_SET register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the IRQ_UNMASK_SET register.
 */
__INLINE uint32_t intc_irq_unmask_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    return REG_PL_RD(INTC_IRQ_UNMASK_SET_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the IRQ_UNMASK_SET register to a value.
 * The IRQ_UNMASK_SET register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void intc_irq_unmask_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 1);
    REG_PL_WR(INTC_IRQ_UNMASK_SET_ADDR + reg_idx * 4, value);
}

/// @}

/**
 * @name IRQ_UNMASK_CLEAR register definitions
 *
 * @{
 */

/// Address of the IRQ_UNMASK_CLEAR register
#define INTC_IRQ_UNMASK_CLEAR_ADDR   0x10910018
/// Offset of the IRQ_UNMASK_CLEAR register from the base address
#define INTC_IRQ_UNMASK_CLEAR_OFFSET 0x00000018
/// Index of the IRQ_UNMASK_CLEAR register
#define INTC_IRQ_UNMASK_CLEAR_INDEX  0x00000006
/// Reset value of the IRQ_UNMASK_CLEAR register
#define INTC_IRQ_UNMASK_CLEAR_RESET  0x00000000
/// Number of elements of the IRQ_UNMASK_CLEAR register array
#define INTC_IRQ_UNMASK_CLEAR_COUNT  2

/**
 * @brief Sets the IRQ_UNMASK_CLEAR register to a value.
 * The IRQ_UNMASK_CLEAR register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void intc_irq_unmask_clear(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 1);
    REG_PL_WR(INTC_IRQ_UNMASK_CLEAR_ADDR + reg_idx * 4, value);
}

/// @}

/**
 * @name IRQ_POLARITY register definitions
 *
 * @{
 */

/// Address of the IRQ_POLARITY register
#define INTC_IRQ_POLARITY_ADDR   0x10910020
/// Offset of the IRQ_POLARITY register from the base address
#define INTC_IRQ_POLARITY_OFFSET 0x00000020
/// Index of the IRQ_POLARITY register
#define INTC_IRQ_POLARITY_INDEX  0x00000008
/// Reset value of the IRQ_POLARITY register
#define INTC_IRQ_POLARITY_RESET  0x00000000
/// Number of elements of the IRQ_POLARITY register array
#define INTC_IRQ_POLARITY_COUNT  2

/**
 * @brief Returns the current value of the IRQ_POLARITY register.
 * The IRQ_POLARITY register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the IRQ_POLARITY register.
 */
__INLINE uint32_t intc_irq_polarity_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    return REG_PL_RD(INTC_IRQ_POLARITY_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the IRQ_POLARITY register to a value.
 * The IRQ_POLARITY register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void intc_irq_polarity_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 1);
    REG_PL_WR(INTC_IRQ_POLARITY_ADDR + reg_idx * 4, value);
}

/// @}

/**
 * @name IRQ_INDEX register definitions
 *
 * @{
 */

/// Address of the IRQ_INDEX register
#define INTC_IRQ_INDEX_ADDR   0x10910040
/// Offset of the IRQ_INDEX register from the base address
#define INTC_IRQ_INDEX_OFFSET 0x00000040
/// Index of the IRQ_INDEX register
#define INTC_IRQ_INDEX_INDEX  0x00000010
/// Reset value of the IRQ_INDEX register
#define INTC_IRQ_INDEX_RESET  0x00000000

/**
 * @brief Returns the current value of the IRQ_INDEX register.
 * The IRQ_INDEX register will be read and its value returned.
 * @return The current value of the IRQ_INDEX register.
 */
__INLINE uint32_t intc_irq_index_get(void)
{
    return REG_PL_RD(INTC_IRQ_INDEX_ADDR);
}

/// @}


#endif // _REG_INTC_H_

/// @}


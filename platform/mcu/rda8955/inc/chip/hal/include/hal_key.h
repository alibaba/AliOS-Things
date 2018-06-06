/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/



#ifndef _HAL_KEY_H_
#define _HAL_KEY_H_

#include "cs_types.h"
///
/// @defgroup key HAL Keypad Driver
/// This document describes the characteristics of the keypad
/// module and how to use it via its Hardware Abstraction Layer API
///
/// @par Keypad Operation
/// The keypad module is designed to scan keys from a keypad array, and return
///  bitfields where a pressed key is represented by a '1' and an unpressed
///  key by a '0'.
///
/// @par
/// Once configured, the driver will call a user function when an interrupt (key
/// pressed, released or held down) occurs on the keypad. The debouncing is
/// handled by the hardware. Up to eleven keys can be detected at the same time,
/// under certain condition. But when three keys forming a corner
/// (eg. "1", "2" and "5") are
/// pressed at the same time, the fourth key that completes the square is also
/// detected (in our previous example, the key "4"). This behavior is normal, it
/// is inherent with the way the keypad hardware module works.
///
/// @par Interruption Masks
/// For the interrupt control, please use the provided functions:
/// #hal_KeyIrqSetHandler() and #hal_KeyIrqSetMask(). See the definition of
/// those functions for details on how to use them.
///
/// @par Link to the physical world.
/// The key are arranged as a matrix whose size can range from 5x5 to 6x6,
/// the sixth line and column being optional. A detected key (pressed or released)
/// is returned by the HAL KEY driver as a number. The position in the matrix
/// is given by the formula:
///  - line number = key_number / NUMBER_OF_ROWS_IN_THE_MATRIX
///  - row number  = key_number mod NUMBER_OF_ROWS_IN_THE_MATRIX.
///
/// The key code, corresponding to the pressed key (is it a '0' or a '->' ?)
/// is provided by a function of the target API, whose internals hold
/// an arraw which gives the value of each key. (tgt_GetKeyCode())
/// @{

// ============================================================================
// DEFINES
// ============================================================================

// ============================================================================
// HAL_KEY_ON_OFF
// ----------------------------------------------------------------------------
/// Value used to define the row and column of the on-off key.
// ============================================================================
#define HAL_KEY_ON_OFF  255



// ============================================================================
// TYPES
// ============================================================================


// ============================================================================
// HAL_KEY_STATE_T
// ----------------------------------------------------------------------------
/// Type used to define the state of a key: pressed (down), hold down (pressed)
/// or released (up).
// ============================================================================
typedef enum
{
    HAL_KEY_DOWN = 0,
    HAL_KEY_PRESSED,
    HAL_KEY_UP
} HAL_KEY_STATE_T;





// ============================================================================
// HAL_KEY_IRQ_HANDLER_T
// ----------------------------------------------------------------------------
/// Type used to define the IRQ handler function that will be called by the
/// keypad driver when a key event occurs: it becomes pressed, it is hold down
/// or it is released. The parameters of the called function are: the id
/// (position in the g_tgtKeyMap [board dependent key mapping] of the involved
/// key and its state (down, pressed or up).
/// The value 255 for the id means the key generating the event is
/// the on-off button.
// ============================================================================
typedef VOID (*HAL_KEY_IRQ_HANDLER_T)(UINT8 id, HAL_KEY_STATE_T status);



// ============================================================================
// HAL_KEY_IRQ_MASK_T
// ----------------------------------------------------------------------------
/// Type used to define upon which keypad events the driver will call the
/// user callback function (configured by the #hal_KeyIrqSetHandler()
/// function).
///
/// onUp When 1, an interruption is generated each time a key is pressed.
/// onPressed When 1, an interruption is generated periodically, for each
/// key, as long as this key is held down (i.e. is pressed). The repetition
/// period is configured with the function #hal_KeyOpen().
/// onDown When 1, an interruption is generated each time a key is released.
// ============================================================================
typedef struct
{
    /// When 1, an interruption is generated each time a key is pressed.
    UINT32 onUp:1;

    /// When 1, an interruption is generated periodically, for each
    /// key, as long as this key is held down (i.e. is pressed). The repetition
    /// period is configured with the function #hal_KeyOpen.
    UINT32 onPressed:1;

    /// When 1, an interruption is generated each time a key is released.
    UINT32 onDown:1;
} HAL_KEY_IRQ_MASK_T;





// ============================================================================
// FUNCTIONS
// ============================================================================

// ============================================================================
// hal_KeyOpen
// ----------------------------------------------------------------------------
/// This function initializes the keypad driver. The configuration of the key
/// pins used is board dependent and can be got from the TARGET module.
/// Refer to its documentation for details.
///
/// @param debounceTime defines the time to wait for debounce, in number of
/// 16384Hz steps. Boundaries applied to the possible values of this time,
/// which are dependent on the board config, especially the number of
/// out line of the keypad: the value of this parameter must be chosen
/// between 5*Number of Enabled KeyOu and 1280*Number of Enabled
/// Please refer to the keypad documentation for
/// details.
/// @param kpItvTime defines the interval time between the generation of IRQ
/// when one or more keys are being held down. It defines a multiple of the
/// debounce time.
// ============================================================================
PUBLIC VOID hal_KeyOpen(UINT16 debounceTime, UINT16 kpItvTime);


// ============================================================================
// hal_KeyClose
// ----------------------------------------------------------------------------
/// This function closes the key driver.
// ============================================================================
PUBLIC VOID hal_KeyClose(VOID);


// ============================================================================
// hal_KeyIrqSetHandler
// ----------------------------------------------------------------------------
/// This function configures which user function will be called by the
/// keypad driver when an interruption is generated. See the function
/// #hal_KeyIrqSetMask() for details about when an interruption is generated.
///
/// @param handler Pointer to a user function called when an interruption
/// is generated by the keypad driver.
// ============================================================================
PUBLIC VOID hal_KeyIrqSetHandler(HAL_KEY_IRQ_HANDLER_T handler);


// ============================================================================
// hal_KeyIrqSetMask
// ----------------------------------------------------------------------------
/// This function controls under which conditions the keypad interrupt will
/// be generated. It can be when a key is pressed, when it is held down and
/// when it is released. Use the function #hal_KeyIrqSetHandler() to configure
/// which user function will be called in case of a keypad interruption.
///
/// @param mask Defines which event will generate a call to the user
/// callback function. See the documentation of the type for details.
// ============================================================================
PUBLIC VOID hal_KeyIrqSetMask(HAL_KEY_IRQ_MASK_T mask);


// ============================================================================
// hal_KeyIrqGetMask
// ----------------------------------------------------------------------------
/// This function returns the keypad IRQ mask. Refer to the type documentation
/// for its interpretation.
/// @return The keypad IRA mask.
// ============================================================================
PUBLIC HAL_KEY_IRQ_MASK_T hal_KeyIrqGetMask(VOID);


// ============================================================================
// hal_KeyResetIrq
// ----------------------------------------------------------------------------
/// This function clears the keypad interrupt status. It should only be called
/// to avoid unwanted Irq. In normal usage, Irq clearing is done automatically
/// by the interrupt driver.
// ============================================================================
PUBLIC VOID hal_KeyResetIrq(VOID);


// ============================================================================
// hal_KeyOnOffStateGet
// ----------------------------------------------------------------------------
/// Tell if the key ON key is pressed
/// @return \c TRUE if the ON/OFF button is pressed \n
///         \c FALSE otherwise
// ============================================================================
PUBLIC BOOL hal_KeyOnOffStateGet(VOID);


// ============================================================================
// hal_KeyOnOffStateAtPowerOn
// ----------------------------------------------------------------------------
/// Tell if the key ON key was pressed at power-on time.
/// @return \c TRUE if the ON/OFF button was pressed \n
///         \c FALSE otherwise
// ============================================================================
PUBLIC BOOL hal_KeyOnOffStateAtPowerOn(VOID);


// =============================================================================
// hal_KeyRemoteHandler
// -----------------------------------------------------------------------------
/// Simulate the press or release of a key. This function is aimed at being
/// executed remotely, by a keyboard emulator integrated in CoolWatcher.
///
/// @param keyCode Key code of the key, as defined in key_defs.h, to press,
/// release, ...
/// @param state State to set the key to: #HAL_KEY_DOWN, #HAL_KEY_UP, ...
// =============================================================================
PUBLIC VOID hal_KeyRemoteHandler(UINT32 keyCode, HAL_KEY_STATE_T state);


///  @} <- End of key group
///




#endif // _HAL_KEY_H_


/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file       crc16btx.h
* @brief     Cyclic Redundancy Check (CRC) Implementation for BTX,
*           Polynomial: X**0 + X**2 + X**15 + X16
*
* @author       gordon
* @date         2015-07-13
* @version  v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#if ! defined (__CRC16BTX_H)
#define       __CRC16BTX_H

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>


/** @defgroup CRC_FCS CRC Implementation
  * @brief CRC implementation for specified polynomial
  * @{
  */

/*============================================================================*
 *                              Macro
*============================================================================*/
/** @defgroup CRC_FCS_Exported_Macros CRC Implementation Exported Macros
    * @{
    */
/** defines and macros */
#define BTXFCS_INIT      0x0000  /**< Initial FCS value */
#define BTXFCS_GOOD      0x0000  /**< Good final FCS value */
/** @} */ /* End of group CRC_FCS_Exported_Macros */

/*============================================================================*
  *                                Functions
  *============================================================================*/
/** @defgroup CRC_FCS_Exported_Functions CRC Implementation Exported Functions
    * @brief
    * @{
    */
/**
* @brief  Calculate a new fcs given the current fcs and the new data.
*       Polynomial: X**0 + X**2 + X**15 + X16
*
* @param  fcs: init or good final
* @param  cp: data point
* @param  len: length
*
* @return
*
*/
uint16_t btxfcs(uint16_t fcs,
                uint8_t  *cp,
                uint32_t len);


/**
* @brief  proprietary inteface for Realtek internal handshake API .
* @param  data: input data buffer
* @param  len: input data buffer length
* @return true if handshare pass, fals othewise
*
*/
extern bool btx_vendor_get_response(uint8_t *data, uint8_t len);

/** @} */ /* End of group CRC_FCS_Exported_Functions */

/** @} */ /* End of group CRC_FCS */

#endif  /**< #if ! defined (__CRC16BTX_H) */

/** End of CRC16BTX.H */


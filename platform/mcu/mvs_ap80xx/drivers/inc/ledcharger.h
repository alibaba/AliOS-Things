/**
  *****************************************************************************
  * @file:			ledcharger.h
  * @author			YunWang
  * @version		V1.0.0
  * @data			4-May-2014
  * @Brief			LED charger driver header file.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
  * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
  */

/**
* @addtogroup Çý¶¯
* @{
* @defgroup LEDCharger LEDCharger
* @{
*/

#ifndef __LED_CHARGER_H__
#define __LED_CHARGER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief	Set led out the A0 pin
 * @param	LedWidth the width of led out high and low pulse.
 * @param	LedPolarity led output polarity.
 * @return	None
 * @note 	LedWidth range 0~15 according to 1Hz~16Hz
 */
void ChargerSetMode(uint8_t LedWidth, uint8_t LedPolarity);



#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */

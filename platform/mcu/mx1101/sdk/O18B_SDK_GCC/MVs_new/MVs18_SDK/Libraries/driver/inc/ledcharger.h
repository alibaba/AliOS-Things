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

#ifndef __LED_CHARGER_H__
#define __LED_CHARGER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @Brief	Set led out the A0 pin
 * @Param	LedWidth the width of led out high and low pulse.
 * @Param	LedPolarity led output polarity.
 * @Return	None
 * @Note@ 	LedWidth range 0~15 according to 1Hz~16Hz
 */
void ChargerSetMode(uint8_t LedWidth, uint8_t LedPolarity);



#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

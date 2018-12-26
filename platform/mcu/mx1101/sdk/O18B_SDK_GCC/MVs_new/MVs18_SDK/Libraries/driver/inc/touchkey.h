/******************************************************************************
 * @file    touchkey.h
 * @author  Sean
 * @version V1.0.0
 * @date    29-July-2013
 * @brief   touchkey function declare
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
/**************************************************************************************
 *@maintainer Fenston
 *@date 2014/05/08
 **************************************************************************************
 **/
#ifndef __TOUCHKEY_H__
#define __TOUCHKEY_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief  init the touchkey pins
 * @param:  TouchKeyPin which pins will work in touchkey mode,its valid bits are form bit0 to bit13.
 *          Bit0~bit5 correspond GPIOB[26]~GPIOB[31],Bit6~bit13 correspond GPIOC[1]~GPIOC[8].These pins will be configed in analog mode.
 *          Beyongd this,GPIOC[0] will be configed in digital mode.
 * @return None
 */
void TouchKeyPortInit(uint16_t TouchKeyPin);

/**
 * @brief  restore the touchkey pins to default status
 * @param:  TouchKeyPin which pins will be restored to default status,its valid bits are form bit0 to bit13.
 *          Bit0~bit5 correspond GPIOB[26]~GPIOB[31],Bit6~bit13 correspond GPIOC[1]~GPIOC[8].These pins will be configed in analog mode.
 *          Beyongd this,GPIOC[0] will also be restored to default status.
 * @return None
 */
void TouchKeyPortDeInit(uint16_t TouchKeyPin);

/**
 * @brief  reset touchkey module
 * @param:  None
 * @return None
 */
void TouchKeyModuleRst(void);

/**
 * @brief  start a channel to work
 * @param:  ChSel the current which you want to start to work
 * @return None
 */
void TouchKeyStartConvert(uint8_t ChSel);
/**
 * @brief  check if current channel conversion is complete
 * @param  None
 * @return TRUE:conversion is complete,FALSE:conversion is not complete
 */
bool TouchKeyIsConvertOK(void);

/**
 * @brief  get current channel Dout value
 * @param  None
 * @return current channel Dout value
 */
uint16_t TouchKeyGetDout(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

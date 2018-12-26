/**
  *****************************************************************************
  * @file:			pwc.h
  * @author			YunWang
  * @version		V1.0.0
  * @data			4-May-2014
  * @Brief			PWC module driver header file.
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

#ifndef __PWC_H__
#define	__PWC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/*
**********************************************************
*					TYPE DEFINE
**********************************************************
*/
#define PWC_MODE_EDGE_1_1			0		 /**< Pulse Capture mode posedge to posedge */
#define PWC_MODE_EDGE_1_0			1		 /**< Pulse Capture mode posedge to negedge */
#define PWC_MODE_EDGE_0_1			2		 /**< Pulse Capture mode negedge to posedge */
#define PWC_MODE_EDGE_0_0			3		 /**< Pulse Capture mode negedge to negedge */
#define PWC_MODE_EDGE_X_X			4		 /**< Pulse Capture mode anyedge to anyedge */


/*
**********************************************************
*					GLOBAL FUNCTION PROTOTYPE
**********************************************************
*/
/**
 * @Brief	PWC capture pulse mode select
 * @Param	PwcMode refer MACRO definition
 * @Return	None
 * @Note@	Support 5 kinds of PwcMode
 * 			posedge to posedge
 *			posedge to negedge
 *			negedge to posedge
 *			negedge to negedge
 *			anyedge to anyedge
 */
void PwcModeSet(uint8_t PwcMode);

/**
 * @Brief	Disable Pwc module
 * @Param	None
 * @Return	None
 * @Note@
 */
void PwcDeInit(void);
/**
 * @Brief	Get PWC interrupt stauts
 * @Param	None
 * @Return	None
 * @Note@
 */
bool PwcGetIntStatus(void);

/**
 * @Brief	Clear PWC interrupt stauts
 * @Param	None
 * @Return	None
 * @Note@
 */
void PwcIntClr(void);

/**
 * @Brief	Clear PWC Pulse count over status
 * @Param	None
 * @Return	None
 * @Note@
 */
void PwcOverFlowClr(void);

/**
 * @Brief	Get PWC captured value.
 * @Param	None
 * @Return	PWC captured value
 * @Note@	When PwcMode is posedge to posedge or negedge to negedge,
 *			The value is Frequency based on 12M, Fq = 12000000/(value + 1).
 *			When PwcMode is posedge to negedge or negedge to posedge,
 *			The value is high level or low level percent, duty = (value/(12000000/Fq))*100%
 */
uint32_t GetPulseValue(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 *******************************************************************************
 * @file    sdram.h
 * @author  Robert
 * @version V1.0.0
 * @date    24-July-2013
 * @brief	
 *					SDRMC(Synchronous Dynamic Random Access Memory Controller) is a controller to control
 *					and switch information between CPU & sdram memory chip.For the sake of driving most of
 *					SDRAM chips,function compatibility,efficiency reliability will be took into account.
 * @maintainer Halley
 * Changelog:
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef	__SDRAM_H__
#define	__SDRAM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief
 *   The SdramInit() function initialize the sdram chip and detect the SDRAM size automatically
 *
 * @param	NONE
 *
 * @return NONE
 *
 */
extern void SdramInit(void);

/**
 * @brief
 *   The CopyFlash2Sdram() function copy code from flash to SDRAM.
 *
 * @param			NONE
 * @return		NONE
 */
void CopyFlash2Sdram(void);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//_SDRAM_H_

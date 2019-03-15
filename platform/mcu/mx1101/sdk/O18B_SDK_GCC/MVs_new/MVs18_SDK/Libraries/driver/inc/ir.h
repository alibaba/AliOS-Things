/**
 *****************************************************************************
 * @file     ir.h
 * @author   Yancy
 * @version  V1.0.0
 * @date     19-June-2013
 * @maintainer Fenston
 * @brief    ir module driver head file
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __IR_H__
#define __IR_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * IR IO选择
 */
#define IR_USE_GPIOA10 0   /**< 使用GPIO A10作为Ir接收 */
#define IR_USE_GPIOB7  1   /**< 使用GPIO B7 作为Ir接收 */
#define IR_USE_GPIOC2  2   /**< 使用GPIO C2 作为Ir接收 */

/**
 * @brief  选择IR复用关系
 * @param  IrSel：指定IO口
 * @arg    IR_USE_GPIOA10
 * @arg    IR_USE_GPIOB7
 * @arg    IR_USE_GPIOC2
 * @return NONE
 */
void IrGpioSel(uint8_t IrSel);

/**
 * @brief  初始化IR模块
 * @param  NONE
 * @return NONE
 */
void IrInit(void);

/**
 * @brief  判断是否接收到了按键
 * @param  NONE
 * @return TRUE：接收到了 FALSE:没有接收到
 */
bool IrIsKeyCome(void);

/**
 * @brief  获取按键码值
 * @param  NONE
 * @return 接收到的按键码值
 */
uint32_t IrGetKeyCode(void);

/**
 * @brief  判断是否在长按状态
 * @param  NONE
 * @return TRUE：在长按状态  FALSE：没有在长按状态
 */
bool IrIsContinuePrs(void);

/**
 * @brief  设置检测模式
 * @param  IsIgnore
 * @arg    TRUE: 设置为无头检测模式
 * @arg    FALSE: 设置为一般测模式
 * @return NONE
 */
void IrIgnoreLeadHeader(bool IsIgnore);



#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__IR_KEY_H__

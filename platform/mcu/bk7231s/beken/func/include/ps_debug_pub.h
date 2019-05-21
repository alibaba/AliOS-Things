#ifndef _PS_DEBUG_H_
#define _PS_DEBUG_H_
#include "arm_arch.h"

#define PS_USE_GPIO_TRACE           0
#define PS_NO_USE_GPIO_WAKE         1
#define PS_NO_USE_UART1_WAKE        0

#if (CFG_SOC_NAME == SOC_BK7231)

#define PS_CK_GPIO                 (0x0802800 +(10*4))
#define PS_UP_GPIO                 (0x0802800 +(14*4))
#define PS_RX_GPIO                 (0x0802800 +(15*4))
#define PS_BCN_GPIO                (0x0802800 +(11*4))
#define PS_DOWN_GPIO               (0x0802800 +(17*4))
#define PS_PWM_GPIO                (0x0802800 +(16*4))
#elif (CFG_SOC_NAME == SOC_BK7231U)
#define PS_CK_GPIO                 (0x0802800 +(8*4))
#define PS_UP_GPIO                 (0x0802800 +(22*4))
#define PS_RX_GPIO                 (0x0802800 +(23*4))
#define PS_BCN_GPIO                (0x0802800 +(9*4))
#define PS_DOWN_GPIO               (0x0802800 +(20*4))
#define PS_PWM_GPIO                (0x0802800 +(21*4))
#elif (CFG_SOC_NAME == SOC_BK7221U)
#define PS_CK_GPIO                 (0x0802800 +(21*4))
#define PS_UP_GPIO                 (0x0802800 +(20*4))
#define PS_RX_GPIO                 (0x0802800 +(6*4))
#define PS_BCN_GPIO                (0x0802800 +(22*4))
#define PS_DOWN_GPIO               (0x0802800 +(23*4))
#define PS_PWM_GPIO                (0x0802800 +(7*4))
#endif

#if PS_USE_GPIO_TRACE
#if PS_NO_USE_GPIO_WAKE
#define PS_DEBUG_UP_OUT 	(REG_WRITE(PS_UP_GPIO, 0x00))
#define PS_DEBUG_RX_OUT 	(REG_WRITE(PS_RX_GPIO, 0x00))
#else
#define PS_DEBUG_UP_OUT 
#define PS_DEBUG_RX_OUT 
#endif

#if PS_NO_USE_UART1_WAKE
#define PS_DEBUG_CK_OUT 	(REG_WRITE(PS_CK_GPIO, 0x00))
#define PS_DEBUG_BCN_OUT 	(REG_WRITE(PS_BCN_GPIO, 0x00))
#else
#define PS_DEBUG_CK_OUT
#define PS_DEBUG_BCN_OUT
#endif
#define PS_DEBUG_DOWN_OUT 	(REG_WRITE(PS_DOWN_GPIO, 0x00))
#define PS_DEBUG_PWM_OUT 	(REG_WRITE(PS_PWM_GPIO, 0x00))

#if PS_NO_USE_GPIO_WAKE
#define PS_DEBUG_UP_TRIGER 	(REG_WRITE(PS_UP_GPIO,(REG_READ(PS_UP_GPIO))^(0x2)))
#define PS_DEBUG_RX_TRIGER 	(REG_WRITE(PS_RX_GPIO,(REG_READ(PS_RX_GPIO))^(0x2)))
#else
#define PS_DEBUG_UP_TRIGER 
#define PS_DEBUG_RX_TRIGER
#endif

#if PS_NO_USE_UART1_WAKE
#define PS_DEBUG_CK_TRIGER 	(REG_WRITE(PS_CK_GPIO,(REG_READ(PS_CK_GPIO))^(0x2)))
#define PS_DEBUG_BCN_TRIGER 	(REG_WRITE(PS_BCN_GPIO,(REG_READ(PS_BCN_GPIO))^(0x2)))
#define PS_DEBUG_BCN_HIGH 	(REG_WRITE(PS_BCN_GPIO,(REG_READ(PS_BCN_GPIO))|(0x2)))
#define PS_DEBUG_BCN_LOW 	(REG_WRITE(PS_BCN_GPIO,(REG_READ(PS_BCN_GPIO))&(~0x2)))

#else
#define PS_DEBUG_CK_TRIGER
#define PS_DEBUG_BCN_TRIGER
#endif
#define PS_DEBUG_DOWN_TRIGER 	(REG_WRITE(PS_DOWN_GPIO,(REG_READ(PS_DOWN_GPIO))^(0x2)))
#define PS_DEBUG_PWM_TRIGER 	(REG_WRITE(PS_PWM_GPIO,(REG_READ(PS_PWM_GPIO))^(0x2)))
#define PS_DEBUG_PWM_HIGH 	(REG_WRITE(PS_PWM_GPIO,(REG_READ(PS_PWM_GPIO))|(0x2)))
#define PS_DEBUG_PWM_LOW 	(REG_WRITE(PS_PWM_GPIO,(REG_READ(PS_PWM_GPIO))&(~0x2)))

#else
#define PS_DEBUG_UP_OUT 
#define PS_DEBUG_RX_OUT 
#define PS_DEBUG_CK_OUT
#define PS_DEBUG_BCN_OUT
#define PS_DEBUG_DOWN_OUT 	
#define PS_DEBUG_PWM_OUT 
#define PS_DEBUG_UP_TRIGER 
#define PS_DEBUG_RX_TRIGER
#define PS_DEBUG_CK_TRIGER
#define PS_DEBUG_BCN_TRIGER
#define PS_DEBUG_DOWN_TRIGER 	
#define PS_DEBUG_PWM_TRIGER 	
#endif

#endif // _PS_DEBUG_H_
// eof




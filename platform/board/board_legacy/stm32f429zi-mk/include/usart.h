#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "board.h"

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;

void MX_UART4_Init(void);
void MX_UART5_Init(void);
void MX_UART7_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART6_UART_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */


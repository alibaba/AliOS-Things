/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOARD_H
#define __BOARD_H
  /* Includes ------------------------------------------------------------------*/

#include "base_types.h"
/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LED_ON()    {M0P_GPIO->PDDIR_f.PD05 = 0;M0P_GPIO->PDOUT_f.PD05 = 1;}
#define LED_OFF()   {M0P_GPIO->PDDIR_f.PD05 = 0;M0P_GPIO->PDOUT_f.PD05 = 0;}

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
#ifdef __cplusplus
extern "C" {
#endif
void _Error_Handler(char *, int);

void board_init(void);
     
void UART0_Init(void);   
void HAL_IncTick(void);  
uint32_t HAL_GetTick(void);  
void HAL_Delay(volatile uint32_t Delay);     
uint32_t HAL_GetHalVersion(void);    
void UART0_SendChar(unsigned char temp);
void Print(unsigned char *pd);   

 
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __BOARD_H */

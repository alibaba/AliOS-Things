
#ifndef __BOARD_H__
#define __BOARD_H__

/* Includes */

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define*/


/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */


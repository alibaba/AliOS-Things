

/* Includes */
#include "main.h"
#include "HK32f1xx_hal.h"
#include "HK32f1xx_hal_uart.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/** The application entry point. */
int main(void)
{ 
	extern void test_certificate(void);
	test_certificate();
}


/** This function is executed in case of error occurrence.  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/*Reports the name of the source file and the source line number  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */



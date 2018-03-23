/**
  ******************************************************************************
  * @file    Cloud/AWS/Src/main.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "soc_init.h"
#include "hal/soc/soc.h"
#include "aos/kernel.h"
#include "k_api.h"
#include "errno.h"

#if defined(__ICCARM__)
#include "sys/errno.h"
#endif

/* Global variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;
RNG_HandleTypeDef hrng;

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile uint8_t button_flags = 0;

stm32_uart_t stm32_uart[COMn];

uart_dev_t   uart_0;

static uart_dev_t console_uart={
  .port=STDIO_UART,
};
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
//static void Console_UART_Init(void);
static int UART_Init(uart_dev_t *uart);
static void RTC_Init(void);
static void Button_ISR(void);
static int default_UART_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void stm32_soc_init(void)
{ 
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();
  
  BSP_LED_Init(LED_GREEN);
 // BSP_LED_Init(LED_RED);
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
//  Led_Off();
  /* RNG init function */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* RTC init */
  RTC_Init();
  /* UART console init */
  default_UART_Init(); 

}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 48000000
  *            PLL_M                          = 6
  *            PLL_N                          = 20
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                                       |RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_I2C2
                                       |RCC_PERIPHCLK_RNG;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_MSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  
  __HAL_RCC_PWR_CLK_ENABLE();
  
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Enable MSI PLL mode */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief Set LED GREEN ON 
  */
void Led_On(void)
{
  BSP_LED_On(LED_GREEN);
}

/**
  * @brief Set LED GREEN Off
  */
void Led_Off(void)
{
    BSP_LED_Off(LED_GREEN);
}

/**
  * @brief Update button ISR status 
  */
static void Button_ISR(void)
{
  button_flags++;
}

/**
  * @brief Waiting for button to be pushed 
  */
uint8_t Button_WaitForPush(uint32_t delay)
{
  uint32_t time_out = HAL_GetTick()+delay;
  do
  {
    if (button_flags > 1)  
    {   
      button_flags = 0;
      return BP_MULTIPLE_PUSH;
    }
   
    if (button_flags == 1)  
    {   
      button_flags = 0;
      return BP_SINGLE_PUSH;
    }
  }
  while( HAL_GetTick() < time_out);
  return BP_NOT_PUSHED;
}

/* bufferQueue for uart */
#define MAX_BUF_UART_BYTES  1800

kbuf_queue_t g_buf_queue_uart[COMn];
char g_buf_uart[COMn][MAX_BUF_UART_BYTES];
const char *g_pc_buf_queue_name[COMn] = {"buf_queue_uart0", "buf_queue_uart4"};

static int UART_Init(uart_dev_t *uart)
{

    switch(uart->port){
        case COM1:
          stm32_uart[uart->port].handle.Instance = USART1;
          break;
        case COM4:
          stm32_uart[uart->port].handle.Instance = UART4;
          break;
        default:
          return -1;
    }  
    
    switch(uart->config.data_width){
        case DATA_WIDTH_7BIT:
            stm32_uart[uart->port].handle.Init.WordLength = UART_WORDLENGTH_7B;
            break;
        case DATA_WIDTH_8BIT:
            stm32_uart[uart->port].handle.Init.WordLength = UART_WORDLENGTH_8B;
            break;
        case DATA_WIDTH_9BIT:
            stm32_uart[uart->port].handle.Init.WordLength = UART_WORDLENGTH_9B;
            break;
        case DATA_WIDTH_5BIT:
        case DATA_WIDTH_6BIT:
        default:
            return -1;
    }

    switch(uart->config.stop_bits){
        case STOP_BITS_1:
            stm32_uart[uart->port].handle.Init.StopBits = UART_STOPBITS_1;
            break;
        case STOP_BITS_2:
            stm32_uart[uart->port].handle.Init.StopBits = UART_STOPBITS_2;
            break;
        default:
            return -1;
    }

    switch(uart->config.parity){
        case NO_PARITY:
            stm32_uart[uart->port].handle.Init.Parity = UART_PARITY_NONE;
            break;
        case ODD_PARITY:
            stm32_uart[uart->port].handle.Init.Parity = UART_PARITY_ODD;
            break;
        case EVEN_PARITY:
            stm32_uart[uart->port].handle.Init.Parity = UART_PARITY_EVEN;
            break;
        default:
            return -1;
    }
    
    switch(uart->config.flow_control){
        case FLOW_CONTROL_DISABLED:
            stm32_uart[uart->port].handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
            break;
        case FLOW_CONTROL_CTS:
            stm32_uart[uart->port].handle.Init.HwFlowCtl = UART_HWCONTROL_CTS;
            break;
        case FLOW_CONTROL_RTS:
            stm32_uart[uart->port].handle.Init.HwFlowCtl = UART_HWCONTROL_RTS;
            break;
        case FLOW_CONTROL_CTS_RTS:
            stm32_uart[uart->port].handle.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
            break;
        default:
            return -1;
    }
    
    stm32_uart[uart->port].handle.Init.BaudRate = uart->config.baud_rate;
    stm32_uart[uart->port].handle.Init.Mode = UART_MODE_TX_RX;
    stm32_uart[uart->port].handle.Init.OverSampling = UART_OVERSAMPLING_16;
    stm32_uart[uart->port].handle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    stm32_uart[uart->port].handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    
    if(krhino_buf_queue_create(&g_buf_queue_uart[uart->port], g_pc_buf_queue_name[uart->port], g_buf_uart[uart->port], MAX_BUF_UART_BYTES, 1) != 0){
        return -2;
    }
    memset(g_buf_uart[uart->port], 0, MAX_BUF_UART_BYTES);
   
    stm32_uart[uart->port].handle.buffer_queue = &g_buf_queue_uart[uart->port];
    BSP_COM_Init((COM_TypeDef)uart->port,&stm32_uart[uart->port].handle);
    aos_mutex_new(&stm32_uart[uart->port].uart_tx_mutex);
    aos_mutex_new(&stm32_uart[uart->port].uart_rx_mutex);
    aos_sem_new(&stm32_uart[uart->port].uart_tx_sem, 0);
    aos_sem_new(&stm32_uart[uart->port].uart_rx_sem, 0);
    return 0;
}



#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  if (ch == '\n') {
    hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
  }
  hal_uart_send(&console_uart, &ch, 1, 30000);
  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
  uint8_t ch = 0;
  uint32_t recv_size;
  int32_t ret = 0;

  ret = hal_uart_recv_II(&console_uart, &ch, 1, &recv_size, HAL_WAIT_FOREVER);

  if (ret == 0) {
      return ch;
  } else {
      return -1;
  }
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout) {
    HAL_UART_StateTypeDef state = HAL_UART_STATE_BUSY_TX;
    if(uart==NULL||data==NULL) {
      return -EINVAL;
    }
    if(uart->port>COMn-1) {
      return -EINVAL;
    }
    aos_mutex_lock(&stm32_uart[uart->port].uart_tx_mutex, AOS_WAIT_FOREVER);

    if (stm32_uart[uart->port].handle.gState != HAL_UART_STATE_READY) {
        aos_sem_wait(&stm32_uart[uart->port].uart_tx_sem, timeout);
    } else {
        state = HAL_UART_STATE_READY;
    }
    if (HAL_UART_Transmit_IT(&stm32_uart[uart->port].handle, (uint8_t *)data, size) != HAL_OK) {
        Error_Handler();
      }

    if (HAL_UART_STATE_READY == state) {
        aos_sem_wait(&stm32_uart[uart->port].uart_tx_sem, AOS_WAIT_FOREVER);
    }

    aos_mutex_unlock(&stm32_uart[uart->port].uart_tx_mutex);

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout) 
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;

    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }

    aos_mutex_lock(&stm32_uart[uart->port].uart_rx_mutex, AOS_WAIT_FOREVER);

    for (i = 0; i < expect_size; i++)
    {
        ret = HAL_UART_Receive_IT_Buf_Queue_1byte(&stm32_uart[uart->port].handle, &pdata[i], timeout); 
        if (ret == 0) {
            rx_count++;
        } else {
            break;
        }
    }

    if (NULL != recv_size){
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    aos_mutex_unlock(&stm32_uart[uart->port].uart_rx_mutex);

    return ret;
}


/**
  * @brief RTC init function 
  */
static void RTC_Init(void)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

    /**Initialize RTC Only */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initialize RTC and set the Time and Date */
  sTime.Hours = 0x12;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x24;
  sDate.Year = 0x17;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case (BUTTON_EXTI13_Pin):
    {
      Button_ISR();
      break;
    }
    
    default:
    {
      break;
    }
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while(1)
  {
     BSP_LED_Toggle(LED_GREEN);
     HAL_Delay(200);
  }
}

/**
  * @brief Tx Transfer completed callback.
  * @param huart: UART handle.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    for(int i=0;i<COMn;i++){
      if(&stm32_uart[i].handle==huart){
          aos_sem_signal(&stm32_uart[i].uart_tx_sem);
          break;
      }
    }
}

/**
  * @brief Rx Transfer completed callback.
  * @param huart: UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  return;
#if 0
  for(int i=0;i<COMn;i++){
    if(&stm32_uart[i].handle==huart){
        aos_sem_signal(&stm32_uart[i].uart_rx_sem);
        break;
    }
  }
#endif
}

static int default_UART_Init()
{
    uart_0.port                = STDIO_UART;
    uart_0.config.baud_rate    = STDIO_UART_BUADRATE;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    return hal_uart_init(&uart_0);
}




int32_t hal_uart_init(uart_dev_t *uart)
{
    if(uart == NULL) {
        return -EINVAL;
    }  
    
    if(uart->port != COM1 && uart->port != COM4){
        return -EINVAL;
    }   
    
    return UART_Init(uart);
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    return 0;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

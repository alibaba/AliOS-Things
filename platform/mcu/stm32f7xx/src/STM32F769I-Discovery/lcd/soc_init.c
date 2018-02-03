/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f769i_discovery.h"
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "soc_init.h"
#include "k_config.h"
#include "image_320x240_argb8888.h"
#include "life_augmented_argb8888.h"
#include "stm32f769i_discovery_lcd.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

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

extern LTDC_HandleTypeDef hltdc_discovery;
static DMA2D_HandleTypeDef           hdma2d;
extern DSI_HandleTypeDef hdsi_discovery;
DSI_VidCfgTypeDef hdsivideo_handle;
DSI_CmdCfgTypeDef CmdCfg;
DSI_LPCmdTypeDef LPCmd;
DSI_PLLInitTypeDef dsiPllInit;
static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

/* Private define ------------------------------------------------------------*/

#define VSYNC               1 
#define VBP                 1 
#define VFP                 1
#define VACT                480
#define HSYNC               1
#define HBP                 1
#define HFP                 1
#define HACT                800

#define LAYER0_ADDRESS               (LCD_FB_START_ADDRESS)

static __IO int32_t  front_buffer   = 0;
static __IO int32_t  pend_buffer   = -1;

static uint32_t ImageIndex = 0;
static const uint32_t * Images[] = 
{
  image_320x240_argb8888,
  life_augmented_argb8888,  
};

static const uint32_t Buffers[] = 
{
  LAYER0_ADDRESS,
  LAYER0_ADDRESS + (800*480*4),  
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void OnError_Handler(uint32_t condition);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void CopyBuffer(uint32_t *pSrc, 
                           uint32_t *pDst, 
                           uint16_t x, 
                           uint16_t y, 
                           uint16_t xsize, 
                           uint16_t ysize);
static uint8_t LCD_Init(void);
void LTDC_Init(void);
static void LCD_BriefDisplay(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  On Error Handler on condition TRUE.
  * @param  condition : Can be TRUE or FALSE
  * @retval None
  */
static void OnError_Handler(uint32_t condition)
{
  if(condition)
  {
    BSP_LED_On(LED1);
    while(1) { ; } /* Blocking on error */
  }
}
/* USER CODE END 0 */

void stm32_soc_init(void)
{
  uint8_t  lcd_status = LCD_OK;

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable I-Cache-------------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache-------------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_On(LED1);
  /* Configure User push-button in Interrupt mode */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
  /* Initialize the SDRAM */
  BSP_SDRAM_Init();  
  MX_USART1_UART_Init();
  /* Initialize the LCD   */
  lcd_status = LCD_Init();
  OnError_Handler(lcd_status != LCD_OK); 
  
  BSP_LCD_LayerDefaultInit(0, LAYER0_ADDRESS);     
  BSP_LCD_SelectLayer(0); 

  /* Display example brief   */
  LCD_BriefDisplay();

  /* Copy Buffer 0 into buffer 1, so only image area to be redrawn later */
  CopyBuffer((uint32_t *)Buffers[0], (uint32_t *)Buffers[1], 0, 0, 800, 480);
  
  /*Draw first image */
  CopyBuffer((uint32_t *)Images[ImageIndex++], (uint32_t *)Buffers[front_buffer], 240, 160, 320, 240);
  pend_buffer = 0;
 
  /* Send Display On DCS Command to display */
  HAL_DSI_ShortWrite(&(hdsi_discovery),
                     0,
                     DSI_DCS_SHORT_PKT_WRITE_P1,
                     OTM8009A_CMD_DISPON,
                     0x00);
  
  /*Refresh the LCD display*/
  HAL_DSI_Refresh(&hdsi_discovery);

}

void LCDRatatePic(void)
{
    if(pend_buffer < 0)
    {
      /* Prepare back buffer */     
      CopyBuffer((uint32_t *)Images[ImageIndex++], (uint32_t *)Buffers[1- front_buffer], 240, 160, 320, 240);
      pend_buffer = 1- front_buffer;
      
      if(ImageIndex >= 2)
      {
        ImageIndex = 0;
      }

      /* Refresh the display */
      HAL_DSI_Refresh(&hdsi_discovery);
      
      /* Wait some time before switching to next stage */
      /*HAL_Delay(2000); */
    }     
    return;

}


/**
  * @brief  End of Refresh DSI callback.
  * @param  hdsi: pointer to a DSI_HandleTypeDef structure that contains
  *               the configuration information for the DSI.
  * @retval None
  */
void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef *hdsi)
{
  if(pend_buffer >= 0)
  { 
    /* Disable DSI Wrapper */
    __HAL_DSI_WRAPPER_DISABLE(&hdsi_discovery);
    /* Update LTDC configuaration */
    LTDC_LAYER(&hltdc_discovery, 0)->CFBAR = ((uint32_t)Buffers[pend_buffer]);
    __HAL_LTDC_RELOAD_CONFIG(&hltdc_discovery);
    /* Enable DSI Wrapper */
    __HAL_DSI_WRAPPER_ENABLE(&hdsi_discovery);
    
    front_buffer = pend_buffer;  
    pend_buffer = -1;
  }
}



/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Pinout Configuration
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == USER_BUTTON_PIN)
  {  
      BSP_LED_Toggle(LED1);
      BSP_LED_Toggle(LED2);
  }
  LCDRatatePic();
}


/**
  * @brief  Initializes the DSI LCD. 
  * The ititialization is done as below:
  *     - DSI PLL ititialization
  *     - DSI ititialization
  *     - LTDC ititialization
  *     - OTM8009A LCD Display IC Driver ititialization
  * @param  None
  * @retval LCD state
  */
static uint8_t LCD_Init(void)
{
  DSI_PHY_TimerTypeDef  PhyTimings;
  
  /* Toggle Hardware Reset of the DSI LCD using
  * its XRES signal (active low) */
  BSP_LCD_Reset();
  
  /* Call first MSP Initialize only in case of first initialization
  * This will set IP blocks LTDC, DSI and DMA2D
  * - out of reset
  * - clocked
  * - NVIC IRQ related to IP blocks enabled
  */
  BSP_LCD_MspInit();
  
  /* LCD clock configuration */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 417 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 417 MHz / 5 = 83.4 MHz */
  /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 83.4 / 2 = 41.7 MHz */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 417;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  
  /* Base address of DSI Host/Wrapper registers to be set before calling De-Init */
  hdsi_discovery.Instance = DSI;
  
  HAL_DSI_DeInit(&(hdsi_discovery));
  
  dsiPllInit.PLLNDIV  = 100;
  dsiPllInit.PLLIDF   = DSI_PLL_IN_DIV5;
  dsiPllInit.PLLODF   = DSI_PLL_OUT_DIV1;  

  hdsi_discovery.Init.NumberOfLanes = DSI_TWO_DATA_LANES;
  hdsi_discovery.Init.TXEscapeCkdiv = 0x4;
  HAL_DSI_Init(&(hdsi_discovery), &(dsiPllInit));
    
  /* Configure the DSI for Command mode */
  CmdCfg.VirtualChannelID      = 0;
  CmdCfg.HSPolarity            = DSI_HSYNC_ACTIVE_HIGH;
  CmdCfg.VSPolarity            = DSI_VSYNC_ACTIVE_HIGH;
  CmdCfg.DEPolarity            = DSI_DATA_ENABLE_ACTIVE_HIGH;
  CmdCfg.ColorCoding           = DSI_RGB888;
  CmdCfg.CommandSize           = HACT;
  CmdCfg.TearingEffectSource   = DSI_TE_DSILINK;
  CmdCfg.TearingEffectPolarity = DSI_TE_RISING_EDGE;
  CmdCfg.VSyncPol              = DSI_VSYNC_FALLING;
  CmdCfg.AutomaticRefresh      = DSI_AR_DISABLE;
  CmdCfg.TEAcknowledgeRequest  = DSI_TE_ACKNOWLEDGE_ENABLE;
  HAL_DSI_ConfigAdaptedCommandMode(&hdsi_discovery, &CmdCfg);
  
  LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_ENABLE;
  LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_ENABLE;
  LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_ENABLE;
  LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_ENABLE;
  LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_ENABLE;
  LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_ENABLE;
  LPCmd.LPGenLongWrite        = DSI_LP_GLW_ENABLE;
  LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_ENABLE;
  LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_ENABLE;
  LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_ENABLE;
  LPCmd.LPDcsLongWrite        = DSI_LP_DLW_ENABLE;
  HAL_DSI_ConfigCommand(&hdsi_discovery, &LPCmd);

  /* Initialize LTDC */
  LTDC_Init();
  
  /* Start DSI */
  HAL_DSI_Start(&(hdsi_discovery));

  /* Configure DSI PHY HS2LP and LP2HS timings */
  PhyTimings.ClockLaneHS2LPTime = 35;
  PhyTimings.ClockLaneLP2HSTime = 35;
  PhyTimings.DataLaneHS2LPTime = 35;
  PhyTimings.DataLaneLP2HSTime = 35;
  PhyTimings.DataLaneMaxReadTime = 0;
  PhyTimings.StopWaitTime = 10;
  HAL_DSI_ConfigPhyTimer(&hdsi_discovery, &PhyTimings);  
    
  /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)
  *  depending on configuration set in 'hdsivideo_handle'.
  */
  OTM8009A_Init(OTM8009A_COLMOD_RGB888, LCD_ORIENTATION_LANDSCAPE);
  
  LPCmd.LPGenShortWriteNoP    = DSI_LP_GSW0P_DISABLE;
  LPCmd.LPGenShortWriteOneP   = DSI_LP_GSW1P_DISABLE;
  LPCmd.LPGenShortWriteTwoP   = DSI_LP_GSW2P_DISABLE;
  LPCmd.LPGenShortReadNoP     = DSI_LP_GSR0P_DISABLE;
  LPCmd.LPGenShortReadOneP    = DSI_LP_GSR1P_DISABLE;
  LPCmd.LPGenShortReadTwoP    = DSI_LP_GSR2P_DISABLE;
  LPCmd.LPGenLongWrite        = DSI_LP_GLW_DISABLE;
  LPCmd.LPDcsShortWriteNoP    = DSI_LP_DSW0P_DISABLE;
  LPCmd.LPDcsShortWriteOneP   = DSI_LP_DSW1P_DISABLE;
  LPCmd.LPDcsShortReadNoP     = DSI_LP_DSR0P_DISABLE;
  LPCmd.LPDcsLongWrite        = DSI_LP_DLW_DISABLE;
  HAL_DSI_ConfigCommand(&hdsi_discovery, &LPCmd);
  
   HAL_DSI_ConfigFlowControl(&hdsi_discovery, DSI_FLOW_CONTROL_BTA);

  /* Send Display Off DCS Command to display */
  HAL_DSI_ShortWrite(&(hdsi_discovery),
                     0,
                     DSI_DCS_SHORT_PKT_WRITE_P1,
                     OTM8009A_CMD_DISPOFF,
                     0x00);
 
  /* Refresh the display */
  HAL_DSI_Refresh(&hdsi_discovery);
  
  return LCD_OK; 
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void LTDC_Init(void)
{
  /* DeInit */
  HAL_LTDC_DeInit(&hltdc_discovery);
  
  /* LTDC Config */
  /* Timing and polarity */
  hltdc_discovery.Init.HorizontalSync = HSYNC;
  hltdc_discovery.Init.VerticalSync = VSYNC;
  hltdc_discovery.Init.AccumulatedHBP = HSYNC+HBP;
  hltdc_discovery.Init.AccumulatedVBP = VSYNC+VBP;
  hltdc_discovery.Init.AccumulatedActiveH = VSYNC+VBP+VACT;
  hltdc_discovery.Init.AccumulatedActiveW = HSYNC+HBP+HACT;
  hltdc_discovery.Init.TotalHeigh = VSYNC+VBP+VACT+VFP;
  hltdc_discovery.Init.TotalWidth = HSYNC+HBP+HACT+HFP;
  
  /* background value */
  hltdc_discovery.Init.Backcolor.Blue = 0;
  hltdc_discovery.Init.Backcolor.Green = 0;
  hltdc_discovery.Init.Backcolor.Red = 0;
  
  /* Polarity */
  hltdc_discovery.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc_discovery.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc_discovery.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc_discovery.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc_discovery.Instance = LTDC;

  HAL_LTDC_Init(&hltdc_discovery);
}

/**
  * @brief  Display Example description.
  * @param  None
  * @retval None
  */
static void LCD_BriefDisplay(void)
{
  BSP_LCD_SetFont(&Font24);  
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE); 
  BSP_LCD_FillRect(0, 0, 800, 112);  
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillRect(0, 112, 800, 368);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);  
  BSP_LCD_DisplayStringAtLine(1, (uint8_t *)"           LCD_DSI_CmdMode_DoubleBuffering");
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAtLine(4, (uint8_t *)"This example shows how to display images on LCD DSI using two buffers");
  BSP_LCD_DisplayStringAtLine(5, (uint8_t *)"one for display and the other for draw");  
}

/**
  * @brief  Converts a line to an ARGB8888 pixel format.
  * @param  pSrc: Pointer to source buffer
  * @param  pDst: Output color
  * @param  xSize: Buffer width
  * @param  ColorMode: Input color mode   
  * @retval None
  */
static void CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, uint16_t xsize, uint16_t ysize)
{   
  
  uint32_t destination = (uint32_t)pDst + (y * 800 + x) * 4;
  uint32_t source      = (uint32_t)pSrc;
  
  /*##-1- Configure the DMA2D Mode, Color Mode and output offset #############*/ 
  hdma2d.Init.Mode         = DMA2D_M2M;
  hdma2d.Init.ColorMode    = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 800 - xsize;
  hdma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;  /* No Output Alpha Inversion*/  
  hdma2d.Init.RedBlueSwap   = DMA2D_RB_REGULAR;     /* No Output Red & Blue swap */    
  
  /*##-2- DMA2D Callbacks Configuration ######################################*/
  hdma2d.XferCpltCallback  = NULL;
  
  /*##-3- Foreground Configuration ###########################################*/
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0xFF;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */  

  hdma2d.Instance          = DMA2D; 
   
  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(&hdma2d, source, destination, xsize, ysize) == HAL_OK)
      {
        /* Polling For DMA transfer */  
        HAL_DMA2D_PollForTransfer(&hdma2d, 100);
      }
    }
  }   
}


/** System Clock Configuration
*/
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 7;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  /* Activate the OverDrive to reach the 216 MHz Frequency */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/RHINO_CONFIG_TICKS_PER_SECOND);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /*Configure the SysTick to have interrupt in 1ms time basis*/
  HAL_SYSTICK_Config(SystemCoreClock/RHINO_CONFIG_TICKS_PER_SECOND);

  /*Configure the SysTick IRQ priority */
  HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority ,0U);

   /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Retargets the C library printf/puts/putchar... function to the USART.
  * @param  char to put
  * @retval EOF or char
  */
int libc_putc(char ch)
{
  HAL_StatusTypeDef status;
  
  if (ch == '\n') 
  {
    status = HAL_UART_Transmit(&huart1, (void *)"\r", 1,30000);
    if (status != HAL_OK)
    {
      return EOF;
    }
  }
  status = HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  if (status != HAL_OK)
  {
    return EOF;
  }
  return ch;
}

/**
  * @brief  Retargets the C library gets/getc/getchar function from the USART.
  * @param  None
  * @retval EOF or char
  */
int libc_getc()
{
  HAL_StatusTypeDef status;
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
  uint8_t ch;
  
  status = HAL_UART_Receive(&huart1, &ch, 1,30000);
  if (status != HAL_OK)
  {
    return EOF;
  }
  return ch;
}

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

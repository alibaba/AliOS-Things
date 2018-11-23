

/**
	******************************************************************************
	* File Name 				 : main.c
	* Description 			 : Main program body
	******************************************************************************
	** This notice applies to any and all portions of this file
	* that are not between comment pairs USER CODE BEGIN and
	* USER CODE END. Other portions of this file, whether 
	* inserted by the user or by software development tools
	* are owned by their respective copyright owners.
	*
	* COPYRIGHT(c) 2017 STMicroelectronics
	*
	* Redistribution and use in source and binary forms, with or without modification,
	* are permitted provided that the following conditions are met:
	* 	1. Redistributions of source code must retain the above copyright notice,
	* 		 this list of conditions and the following disclaimer.
	* 	2. Redistributions in binary form must reproduce the above copyright notice,
	* 		 this list of conditions and the following disclaimer in the documentation
	* 		 and/or other materials provided with the distribution.
	* 	3. Neither the name of STMicroelectronics nor the names of its contributors
	* 		 may be used to endorse or promote products derived from this software
	* 		 without specific prior written permission.
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
#include "soc_init.h"
#include "k_config.h"
#include "hal/soc/soc.h"
#include "string.h"


#if defined 										(__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE 			int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE 			int fgetc(FILE *f)
#elif defined(__ICCARM__) 			
#define PUTCHAR_PROTOTYPE 			int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE 			int fgetc(FILE *f)

#else

/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
	 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE 			int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE 			int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */
uart_dev_t uart_0;

//cpu滴答定时器配置(软件延时用)
void Init_SysTick(uint32_t ticksPerSecond)
{
	SysTick_Config(SYSTEM_CLK / ticksPerSecond);
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |			//关闭中断
	SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

//IO模拟功能配置:LCD/ADC
void AnalogIO(GPIOx_Type * GPIOx, uint32_t PinNum)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructureRun;

	GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

	if ((GPIO_InitStructureRun.Pin != PinNum) ||
		 (GPIO_InitStructureRun.PxINEN != GPIO_IN_Dis) ||
		 (GPIO_InitStructureRun.PxODEN != GPIO_OD_En) ||
		 (GPIO_InitStructureRun.PxPUEN != GPIO_PU_Dis) ||
		 (GPIO_InitStructureRun.PxFCR != GPIO_FCR_ANA))
	{
		GPIO_InitStructure.Pin = PinNum;
		GPIO_InitStructure.PxINEN = GPIO_IN_Dis;
		GPIO_InitStructure.PxODEN = GPIO_OD_En;
		GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
		GPIO_InitStructure.PxFCR = GPIO_FCR_ANA;

		GPIO_Init(GPIOx, &GPIO_InitStructure);
	}
}

//IO输入口配置 
//type 0 = 普通 
//type 1 = 上拉
#define IN_NORMAL 							0
#define IN_PULLUP 							1

void InputIO(GPIOx_Type * GPIOx, uint32_t PinNum, uint8_t Type)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructureRun;

	GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

	if ((GPIO_InitStructureRun.Pin != PinNum) ||
		 (GPIO_InitStructureRun.PxINEN != GPIO_IN_En) ||
		 (GPIO_InitStructureRun.PxODEN != GPIO_OD_En) || ((Type == IN_NORMAL) &&
		 (GPIO_InitStructureRun.PxPUEN != GPIO_PU_Dis)) || ((Type == IN_PULLUP) &&
		 (GPIO_InitStructureRun.PxPUEN != GPIO_PU_En)) ||
		 (GPIO_InitStructureRun.PxFCR != GPIO_FCR_IN))
	{
		GPIO_InitStructure.Pin = PinNum;
		GPIO_InitStructure.PxINEN = GPIO_IN_En;
		GPIO_InitStructure.PxODEN = GPIO_OD_En;
		if (Type == IN_NORMAL) GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
		else GPIO_InitStructure.PxPUEN = GPIO_PU_En;
		GPIO_InitStructure.PxFCR = GPIO_FCR_IN;

		GPIO_Init(GPIOx, &GPIO_InitStructure);
	}
}

//IO输出口配置 
//type 0 = 普通 
//type 1 = OD
#define OUT_PUSHPULL						0
#define OUT_OPENDRAIN 					1

void OutputIO(GPIOx_Type * GPIOx, uint32_t PinNum, uint8_t Type)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructureRun;

	GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

	if ((GPIO_InitStructureRun.Pin != PinNum) ||
		 (GPIO_InitStructureRun.PxINEN != GPIO_IN_Dis) || ((Type == OUT_PUSHPULL) &&
		 (GPIO_InitStructureRun.PxODEN != GPIO_OD_Dis)) || ((Type == OUT_OPENDRAIN) &&
		 (GPIO_InitStructureRun.PxODEN != GPIO_OD_En)) ||
		 (GPIO_InitStructureRun.PxPUEN != GPIO_PU_Dis) ||
		 (GPIO_InitStructureRun.PxFCR != GPIO_FCR_OUT))
	{
		GPIO_InitStructure.Pin = PinNum;
		GPIO_InitStructure.PxINEN = GPIO_IN_Dis;
		if (Type == OUT_PUSHPULL) GPIO_InitStructure.PxODEN = GPIO_OD_Dis;
		else GPIO_InitStructure.PxODEN = GPIO_OD_En;
		GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
		GPIO_InitStructure.PxFCR = GPIO_FCR_OUT;

		GPIO_Init(GPIOx, &GPIO_InitStructure);
	}
}

//IO数字特殊功能口 
//type 0 = 普通 
//type 1 = OD (OD功能仅部分特殊功能支持)
//type 2 = 普通+上拉 
//type 3 = OD+上拉
#define ALTFUN_NORMAL 					0
#define ALTFUN_OPENDRAIN				1
#define ALTFUN_PULLUP 					2
#define ALTFUN_OPENDRAIN_PULLUP 3

void AltFunIO(GPIOx_Type * GPIOx, uint32_t PinNum, uint8_t Type)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructureRun;

	GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

	if ((GPIO_InitStructureRun.Pin != PinNum) ||
		 (GPIO_InitStructureRun.PxINEN != GPIO_IN_Dis) || (((Type & 0x01) == 0) &&
		 (GPIO_InitStructureRun.PxODEN != GPIO_OD_Dis)) || (((Type & 0x01) != 0) &&
		 (GPIO_InitStructureRun.PxODEN != GPIO_OD_En)) || (((Type & 0x02) == 0) &&
		 (GPIO_InitStructureRun.PxPUEN != GPIO_PU_Dis)) || (((Type & 0x02) != 0) &&
		 (GPIO_InitStructureRun.PxPUEN != GPIO_PU_En)) ||
		 (GPIO_InitStructureRun.PxFCR != GPIO_FCR_DIG))
	{
		GPIO_InitStructure.Pin = PinNum;
		GPIO_InitStructure.PxINEN = GPIO_IN_Dis;
		if ((Type & 0x01) == 0) GPIO_InitStructure.PxODEN = GPIO_OD_Dis;
		else GPIO_InitStructure.PxODEN = GPIO_OD_En;
		if ((Type & 0x02) == 0) GPIO_InitStructure.PxPUEN = GPIO_PU_Dis;
		else GPIO_InitStructure.PxPUEN = GPIO_PU_En;
		GPIO_InitStructure.PxFCR = GPIO_FCR_DIG;

		GPIO_Init(GPIOx, &GPIO_InitStructure);
	}
}

//IO关闭（od输出高）
//当输入使能有效时，如果外部信号浮空，也可能导致FM385管脚漏电；
//可以将FCR配置为01（GPIO输出），ODEN配置为1，即伪开漏输出，关闭上拉使能，并将输出数据设为1
//注意SWD接口的PG8,9如果程序改变它们的配置将无法仿真
void CloseIO(GPIOx_Type * GPIOx, uint32_t PinNum)
{
	GPIO_InitTypeDef GPIO_InitStructureRun;

	GPIO_Get_InitPara(GPIOx, PinNum, &GPIO_InitStructureRun);

	if ((GPIO_InitStructureRun.PxFCR != GPIO_FCR_OUT))
	{
		GPIO_SetBits(GPIOx, PinNum);
		OutputIO(GPIOx, PinNum, OUT_OPENDRAIN);
	}
	else 
	{
		OutputIO(GPIOx, PinNum, OUT_OPENDRAIN);
		GPIO_SetBits(GPIOx, PinNum);
	}
}

//IO口初始输出状态配置
void Init_Pad_Io(void)
{
	GPIOx_DO_Write(GPIOA, 0x0000);										//
	GPIOx_DO_Write(GPIOB, 0x0000);										//
	GPIOx_DO_Write(GPIOC, 0x0000);										//
	GPIOx_DO_Write(GPIOD, 0x0000);										//
	GPIOx_DO_Write(GPIOE, 0x0000);										//
	GPIOx_DO_Write(GPIOF, 0x0000);										//
	GPIOx_DO_Write(GPIOG, 0x0000);										//
}

void IO_AnalogFunSet(void)
{
	/* PE4模拟功能选择 */
	GPIO_ANASEL_PE4ANS_Set(GPIO_ANASEL_PE4ANS_ACMP1_INP1);

	/* PE3模拟功能选择 */
	GPIO_ANASEL_PE3ANS_Set(GPIO_ANASEL_PE3ANS_ACMP1_INN1);

	/* PC15模拟功能选择 */
	GPIO_ANASEL_PC15ANS_Set(GPIO_ANASEL_PC15ANS_ACMP1_INP0);

	/* PC14模拟功能选择 */
	GPIO_ANASEL_PC14ANS_Set(GPIO_ANASEL_PC14ANS_ACMP1_INN0);

	/* PC13模拟功能选择 */
	GPIO_ANASEL_PC13ANS_Set(GPIO_ANASEL_PC13ANS_ADC_IN2);

	/* PC12模拟功能选择 */
	GPIO_ANASEL_PC12ANS_Set(GPIO_ANASEL_PC12ANS_ADC_IN1);
}


//默认开启大部分外设时钟，用户可按需求关闭不需要的时钟
//时钟开启关闭对功耗影响不大
void Init_RCC_PERIPH_clk(void)
{
	//PERCLKCON1
	RCC_PERCLK_SetableEx(DCUCLK, ENABLE); 						//debug contro时钟使能，建议打开
	RCC_PERCLK_SetableEx(EXTI2CLK, ENABLE); 					//EXTI外部引脚中断采样时钟，IO数字滤波时钟使能
	RCC_PERCLK_SetableEx(EXTI1CLK, ENABLE); 					//EXTI外部引脚中断采样时钟，IO数字滤波时钟使能
	RCC_PERCLK_SetableEx(EXTI0CLK, ENABLE); 					//EXTI外部引脚中断采样时钟，IO数字滤波时钟使能
	RCC_PERCLK_SetableEx(PDCCLK, ENABLE); 						//IO控制时钟寄存器使能
	RCC_PERCLK_SetableEx(ANACCLK, ENABLE);						//模拟电路总线时钟使能
	RCC_PERCLK_SetableEx(IWDTCLK, ENABLE);						//IWDT总线时钟使能
	RCC_PERCLK_SetableEx(SCUCLK, ENABLE); 						//system control时钟使能，建议打开
	RCC_PERCLK_SetableEx(PMUCLK, ENABLE); 						//电源管理模块时钟使能
	RCC_PERCLK_SetableEx(RTCCLK, ENABLE); 						//RTC总线时钟使能
	RCC_PERCLK_SetableEx(LPTFCLK, ENABLE);						//LPTIM功能时钟使能
	RCC_PERCLK_SetableEx(LPTRCLK, ENABLE);						//LPTIM总线时钟使能

	//PERCLKCON2 SETTING
	RCC_PERCLK_SetableEx(ADCCLK, ENABLE); 						//ADC时钟使能
	RCC_PERCLK_SetableEx(WWDTCLK, ENABLE);						//WWDT时钟使能
	RCC_PERCLK_SetableEx(RAMBISTCLK, DISABLE);				//RAMBIST时钟使能，建议关闭
	RCC_PERCLK_SetableEx(FLSEPCLK, DISABLE);					//Flash擦写控制器时钟使能，用完就关
	RCC_PERCLK_SetableEx(DMACLK, ENABLE); 						//DMA时钟使能
	RCC_PERCLK_SetableEx(LCDCLK, ENABLE); 						//LCD时钟使能
	RCC_PERCLK_SetableEx(AESCLK, ENABLE); 						//AES时钟使能
	RCC_PERCLK_SetableEx(TRNGCLK, ENABLE);						//TRNG时钟使能
	RCC_PERCLK_SetableEx(CRCCLK, ENABLE); 						//CRC时钟使能

	//PERCLKCON3 SETTING
	RCC_PERCLK_SetableEx(I2CCLK, ENABLE); 						//I2C时钟使能
	RCC_PERCLK_SetableEx(U7816CLK1, ENABLE);					//78161时钟使能
	RCC_PERCLK_SetableEx(U7816CLK0, ENABLE);					//78160时钟使能
	RCC_PERCLK_SetableEx(UARTCOMCLK, ENABLE); 				//UART0~5共享寄存器时钟使能
	RCC_PERCLK_SetableEx(UART5CLK, ENABLE); 					//UART5时钟使能
	RCC_PERCLK_SetableEx(UART4CLK, ENABLE); 					//UART4时钟使能
	RCC_PERCLK_SetableEx(UART3CLK, ENABLE); 					//UART3时钟使能
	RCC_PERCLK_SetableEx(UART2CLK, ENABLE); 					//UART2时钟使能
	RCC_PERCLK_SetableEx(UART1CLK, ENABLE); 					//UART1时钟使能
	RCC_PERCLK_SetableEx(UART0CLK, ENABLE); 					//UART0时钟使能
	RCC_PERCLK_SetableEx(HSPICLK, ENABLE);						//HSPI时钟使能
	RCC_PERCLK_SetableEx(SPI2CLK, ENABLE);						//SPI2时钟使能
	RCC_PERCLK_SetableEx(SPI1CLK, ENABLE);						//SPI1时钟使能

	//PERCLKCON4 SETTING
	RCC_PERCLK_SetableEx(ET4CLK, ENABLE); 						//ET4时钟使能
	RCC_PERCLK_SetableEx(ET3CLK, ENABLE); 						//ET3时钟使能
	RCC_PERCLK_SetableEx(ET2CLK, ENABLE); 						//ET2时钟使能
	RCC_PERCLK_SetableEx(ET1CLK, ENABLE); 						//ET1时钟使能
	RCC_PERCLK_SetableEx(BT2CLK, ENABLE); 						//BT2时钟使能
	RCC_PERCLK_SetableEx(BT1CLK, ENABLE); 						//BT1时钟使能
}

void Init_PLL(void)
{
	RCC_PLL_InitTypeDef PLL_InitStruct;

	PLL_InitStruct.PLLDB = 499; 											//pll倍频数 = PLLDB + 1
	PLL_InitStruct.PLLINSEL = RCC_PLLCON_PLLINSEL_XTLF; //PLL时钟源选择XTLF
	PLL_InitStruct.PLLOSEL = RCC_PLLCON_PLLOSEL_MUL1; //默认选择1倍输出，当超出PLLDB的1023时，可使用2倍输出实现更高的倍频
	PLL_InitStruct.PLLEN = DISABLE; 									//默认关闭PLL

	RCC_PLL_Init(&PLL_InitStruct);
	RCC_PLLCON_PLLEN_Setable(DISABLE);								//关闭PLL
}

//系统时钟配置
//使用RCHF做主时钟,define_all.h 中SYSCLKdef宏控制系统时钟频率
void Init_SysClk(void)
{
	RCC_RCHF_InitTypeDef RCHF_InitStruct;
	RCC_SYSCLK_InitTypeDef SYSCLK_InitStruct;

	RCHF_InitStruct.FSEL = SYSCLKdef; 								//define_all.h 中SYSCLKdef宏控制系统时钟频率
	RCHF_InitStruct.RCHFEN = ENABLE;									//打开RCHF

	RCC_RCHF_Init(&RCHF_InitStruct);

	SYSCLK_InitStruct.SYSCLKSEL = RCC_SYSCLKSEL_SYSCLKSEL_RCHF; //选择RCHF做主时钟
	SYSCLK_InitStruct.AHBPRES = RCC_SYSCLKSEL_AHBPRES_DIV1; //AHB不分频
	SYSCLK_InitStruct.APBPRES = RCC_SYSCLKSEL_APBPRES_DIV1; //APB不分频
	SYSCLK_InitStruct.EXTICKSEL = RCC_SYSCLKSEL_EXTICKSEL_AHBCLK; //EXTI,数字滤波时钟使用AHB时钟
	SYSCLK_InitStruct.SLP_ENEXTI = ENABLE;						//休眠模式使能外部中断采样
	SYSCLK_InitStruct.LPM_RCLP_OFF = DISABLE; 				//休眠模式下开启RCLP	

	RCC_SysClk_Init(&SYSCLK_InitStruct);
}

//Mode:0 仿真模式下运行看门狗，运行所有定时器
//Mode:1 仿真模式关闭看门狗，运行所有定时器
void SCU_Init(uint8_t Mode)
{
	if (Mode == 1) //debug
	{
		SCU_MCUDBGCR_DBG_WWDT_STOP_Setable(ENABLE); 		//仿真模式下关闭WWDT
		SCU_MCUDBGCR_DBG_IWDT_STOP_Setable(ENABLE); 		//仿真模式下关闭IWDT
	}
	else //release
	{
		SCU_MCUDBGCR_DBG_WWDT_STOP_Setable(DISABLE);		//仿真模式下运行WWDT
		SCU_MCUDBGCR_DBG_IWDT_STOP_Setable(DISABLE);		//仿真模式下运行IWDT
	}

	SCU_MCUDBGCR_DBG_ET4_STOP_Setable(DISABLE); 			//仿真模式下运行ET4
	SCU_MCUDBGCR_DBG_ET3_STOP_Setable(DISABLE); 			//仿真模式下运行ET3
	SCU_MCUDBGCR_DBG_ET2_STOP_Setable(DISABLE); 			//仿真模式下运行ET2
	SCU_MCUDBGCR_DBG_ET1_STOP_Setable(DISABLE); 			//仿真模式下运行ET1
	SCU_MCUDBGCR_DBG_BT2_STOP_Setable(DISABLE); 			//仿真模式下运行BT2
	SCU_MCUDBGCR_DBG_BT1_STOP_Setable(DISABLE); 			//仿真模式下运行BT1
}

void Init_SysClk_Gen(void) //时钟选择相关
{

	/*系统时钟超过24M后需要打开wait1*/
	if (RCHFCLKCFG >= 24) FLASH_FLSRDCON_WAIT_Set(FLASH_FLSRDCON_WAIT_1CYCLE);
	else FLASH_FLSRDCON_WAIT_Set(FLASH_FLSRDCON_WAIT_0CYCLE);

	/*PLL配置*/
	Init_PLL(); 																			//默认关闭PLL

	/*系统时钟配置*/
	Init_SysClk();																		//默认使用RCHF做主时钟

	/*外设时钟使能配置*/
	Init_RCC_PERIPH_clk();														//默认开启大部分外设时钟

	/*DMA访问RAM优先级配置*/
	RCC_MPRIL_MPRIL_Set(RCC_MPRIL_MPRIL_DMA); 				//默认AHB Master优先级配置DMA优先


	/*下电复位配置*/
	//pdr和bor两个下电复位至少要打开一个
	//当电源电压低于下电复位电压时，芯片会被复位住		
	//pdr电压档位不准但是功耗极低（几乎无法测量）
	//bor电压档位准确但是需要增加2uA功耗
	ANAC_PDRCON_PDREN_Setable(ENABLE);								//打开PDR
	ANAC_BORCON_OFF_BOR_Setable(DISABLE); 						//打开PDR

	/*仿真控制寄存器配置*/
#ifdef __DEBUG
	SCU_Init(1);																			//仿真时运行定时器,关闭看门狗

#else

	SCU_Init(0);
#endif
}

void IWDT_Init(void)
{
	RCC_PERCLK_SetableEx(IWDTCLK, ENABLE);						//IWDT总线时钟使能
	IWDT_Clr(); 																			//清IWDT
	IWDT_IWDTCFG_IWDTOVP_Set(IWDT_IWDTCFG_IWDTOVP_2s); //配置IWDT溢出周期
	IWDT_IWDTCFG_IWDTSLP4096S_Setable(DISABLE); 			//配置休眠时是否启用4096s长周期
}

void Init_IO(void)
{
	OutputIO(LED1_Port, LED1_Pin, 0); 								//led1
	OutputIO(LED2_Port, LED2_Pin, 0); 								//led2
	OutputIO(LED3_Port, LED3_Pin, 0); 								//led3
	OutputIO(LED4_Port, LED4_Pin, 0); 								//led4
	AltFunIO(DBGCOMRX_Port, DBGCOMRX_Pin, 1); 				//debug uart rx
	AltFunIO(DBGCOMTX_Port, DBGCOMTX_Pin, 1); 				//debug uart tx
}

static void configDelay(void) //约40ms
{
	__IO uint32_t count;

	for (count = SYSTEM_CLK / 100; count > 0; count--,IWDT_Clr());
}

void Init_System(void)
{

	/*基础系统配置*/
	__disable_irq();																	//关闭全局中断使能

	IWDT_Init();																			//系统看门狗设置
	IWDT_Clr(); 																			//清系统看门狗	
	configDelay();																		//等待稳定
	Init_SysClk_Gen();																//系统时钟配置
	RCC_Init_RCHF_Trim(clkmode);											//RCHF振荡器校准值载入(芯片复位后自动载入8M的校准值)

	/*用户初始化代码*/

  uart_0.port = DBGUART;
  uart_0.config.baud_rate=9600;
  uart_0.config.data_width = DATA_WIDTH_8BIT;
  uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
  uart_0.config.parity = NO_PARITY;
  uart_0.config.stop_bits = STOP_BITS_1;
  uart_0.config.mode=MODE_TX_RX;
  hal_uart_init(&uart_0);

	__enable_irq(); 																	//打开全局中断使能
}


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
 
/* USER CODE END 0 */
//static uint8_t temp[64];
void soc_init(void)
{
//  int i;
	Init_System();
	Init_IO();
//  memset(temp,0,sizeof(temp));
	Init_SysTick(RHINO_CONFIG_TICKS_PER_SECOND);			//cpu滴答定时器配置(软件延时用)	
  RCC_PERCLK_SetableEx(FLSEPCLK, ENABLE);
  FLASH_Deinit();
  NVIC_SetPriority (NVMIF_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
//  for(i=0x30000;i<0x40000;i+=64)
//  {
//    FLASH_Erase_Sector(i);
//    FLASH_Prog_ByteString(i,temp,64);
//    printf("flash address=0x%08x flag=0x%08x\r\n",i,FLASH->FLSIF);
//  }
}

void SysTick_Handler(void)
{
	//	HAL_IncTick();
	krhino_intrpt_enter();
	krhino_tick_proc();
	krhino_intrpt_exit();
  IWDT_Clr();
	//HAL_SYSTICK_IRQHandler();
}


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
	* @brief	This function is executed in case of error occurrence.
	* @param	None
	* @retval None
	*/
__attribute__((weak)) void _Error_Handler(char * file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1)
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
void assert_failed(uint8_t * file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
		ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}

#endif


/**
	* @brief	Retargets the C library printf function to the USART.
	* @param	None
	* @retval None
	*/
PUTCHAR_PROTOTYPE
{
	if (ch == '\n')
	{
		//hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
//		while (UART_UARTIF_RxTxIF_ChkEx(DBGUART, TxInt) == 0);

//		UARTx_TXREG_Write(DBGUART, '\r');
    hal_uart_send(&uart_0,"\r",1,1);
	}
  hal_uart_send(&uart_0,&ch,1,1);

//	while (UART_UARTIF_RxTxIF_ChkEx(DBGUART, TxInt) == 0);

//	UARTx_TXREG_Write(DBGUART, ch);

	return ch;
}

/**
	* @brief	Retargets the C library scanf function to the USART.
	* @param	None
	* @retval None
	*/
GETCHAR_PROTOTYPE
{
	/* Place your implementation of fgetc here */
	/* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
//	if (UART_UARTIF_RxTxIF_ChkEx(DBGUART, RxInt) == 0)
//	{
//		return - 1;
//	}
//	else 
//	{
//		return UARTx_RXREG_Read(DBGUART);
//	}
  char c;
  if(hal_uart_recv(&uart_0,&c,1,1)==0)
  {
    return c;
  }
  return  -1;
}


void krhino_idle_hook()
{
  IWDT_Clr();
}


/**
	* @}
	*/

/**
	* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

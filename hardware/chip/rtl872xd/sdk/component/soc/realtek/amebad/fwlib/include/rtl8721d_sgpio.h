/**
  ******************************************************************************
  * @file    rtl8721_sgpio.h
  * @author
  * @version V1.0.0
  * @date    2017-10-16
  * @brief   This file contains all the functions prototypes for the SGPIO firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_SGPIO_H_
#define _RTL8721D_SGPIO_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup SGPIO 
  * @brief SGPIO driver modules
  * @{
  */

/** @addtogroup SGPIO
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * SGPIO:
  *		- SGPIO, which is 1-pin serial GPIO, combines standard GPIO and Timer fucntion. 
  *				It can easily realize the conversion between serial data and parallel data.
  *				Capture functionality is also added to measure the input waveform.
  *				SGPIO used for security element.
  *		- Base Address: SGPIO_DEV(0x4801A000)
  *		- SGPIO clock: 2MHz
  *		- IRQ: SGPIO_IRQ_LP
  *		- basic function:
  *			- Timer mode:
  *				16-bit RX and Multiple timer with 16-bit prescaler
  *			- Counter mode:
  *				Multiple counter can count RX trigger events
  *			- Capture mode:
  *				RX timer can be captured by capture events(RX trigger input or multiple match event)
  *				capture value can be transfeered to 0 or 1 by camparing the value
  *		- data communication:
  *			- RX receiving data:
  *				"implement the conversion from serial to parallel data by producing shift clock."
  *				extract the value on the data line at three RX match events
  *				set "0" or "1" input by capture compare event
  *			- TX output mode:
  *				"implement the conversion from parallel to serial data by producing shift clock."
  *				set external output level on three multiple match events and three RX match events
  *				send the "0" or "1" waveform by shifting the output value of a 32-bit FIFO.
  *		- support power saving mode
  *
  *****************************************************************************************     
  *  How to use SGPIO RX match event
  *****************************************************************************************
  *      To use the SGPIO RX match event to control RX or Multiple timer, 
  *         the following steps are mandatory:
  *
  *		 1. Configure the SGPIO pinmux and pull the SGPIO pin up.
  *			 Pinmux_Congfig(PinName, PINMUX_FUNCTION_SGPIO)
  *			 PAD_PullCtrl(PinName, GPIO_PuPd_UP)
  *
  *		 2. Initialize RX timer as default, 
  *			 SGPIO_RX_StructInit(SGPIO_RXInitTypeDef * SGPIO_RXInitStruct)
  *			Select the RX Timer trigger mode.(set the pre-scale value if needed)
  *
  *		 3. Initialize hardware using the value configured above.
  *			 SGPIO_RX_Init(SGPIO_TypeDef *SGPIOx, SGPIO_RXInitTypeDef *SGPIO_RXInitStruct)
  *          
  *		 4. Configure the value of RXMRx match register using the function 
  *			 SGPIO_RXMRxConfig(SGPIOx, SGPIO_RXMRxVal, SGPIO_RXMRxCtl)
  *				to control RX timer stop/reset/shift_clk/interrupt, where x can be 0\1\2.
  *			 SGPIO_RXMR0MULConfig(SGPIOx, SGPIO_RXMR0Val, SGPIO_RXMR0MULCtl)
  *				to control Multiple timer or counter to stop/reset.
  *
  *		 5. Waiting for the input trigger to start RX timer.			  
  *
  *		 @note if using interrput, remember to initialize SGPIO interrupt information
  *		 	and enable SGPIO interrupt.
  *			 InterruptEn(IRQn_Type IrqNum, Priority)
  *
  *****************************************************************************************     
  *  How to use SGPIO Multiple match event
  *****************************************************************************************
  *      To use the SGPIO multiple match event to control multiple or RX timer, 
  *         the following steps are mandatory:
  *
  *		 1. Configure the SGPIO pinmux and pull the SGPIO pin up.
  *			 Pinmux_Congfig(PinName, PINMUX_FUNCTION_SGPIO)
  *			 PAD_PullCtrl(PinName, GPIO_PuPd_UP)
  *
  *		 2. Initialize multiple timer as default, 
  *			 SGPIO_MUL_StructInit(SGPIO_MULInitTypeDef * SGPIO_MULInitStruct)
  *			Select the Multiple Timer mode or Multiple counter mode.
  *
  *		 3. Initialize hardware using the value configured above.
  *			 SGPIO_MUL_Init(SGPIO_TypeDef *SGPIOx, SGPIO_MULInitTypeDef *SGPIO_MULInitStruct)
  *         
  *		 4. Configure the value of MULMRx match register using the function 
  *			 SGPIO_MULMR0MulConfig(SGPIOx, SGPIO_MULMR0Val0, SGPIO_MULMR0Val1, SGPIO_MULMR0Ctl)
  *				to control Multiple timer or counter stop/reset/shift_clk/interrupt.
  *			 SGPIO_MULMR0RXConfig(SGPIOx, SGPIO_MULMR0Val0, SGPIO_MULMR0Val1, SGPIO_MULMR0RXCtl)
  *				to control RX timer stop/reset.
  *
  *		 5. Enable Multiple timer/counter.
  *			 SGPIO_MULTmr_Cmd(SGPIOx, ENABLE)
  *
  *		 @note if using interrput, remember to initialize SGPIO interrupt information
  *			and enable SGPIO interrupt.
  *			 InterruptEn(IRQn_Type IrqNum, Priority)
  *
  *****************************************************************************************     
  *  How to use SGPIO capture event
  *****************************************************************************************
  *      To use the SGPIO capture event to capture RX timer, the following steps are mandatory.
  *
  *		 1. Configure the SGPIO pinmux and pull the SGPIO pin up.
  *			 Pinmux_Congfig(Pin_Num, PINMUX_FUNCTION_SGPIO)
  *			 PAD_PullCtrl(PinName, GPIO_PuPd_UP)
  *
  *		 2. Initialize the SGPIO interrupt infomation and enable it.
  *			 InterruptRegister(IRQ_FUN IrqFun, IRQn_Type IrqNum, Data, Priority)
  *			 InterruptEn(IRQn_Type IrqNum, Priority)
  *
  *		 3. Initialize the RX timer and capture as default.
  *			 SGPIO_RX_StructInit(SGPIO_RXInitTypeDef * SGPIO_RXInitStruct)
  *			 SGPIO_CAP_StructInit(SGPIO_CAPInitTypeDef * SGPIO_CAPInitStruct)
  *      
  *		 4. Select the RX timer trigger edge and capture trigger edge.
  *			set RX timer pre-scale if needed. 
  *
  *		 5. Initialize hardware using the value configured above.
  *			 SGPIO_RX_Init(SGPIO_TypeDef *SGPIOx, SGPIO_RXInitTypeDef *SGPIO_RXInitStruct)
  *			 SGPIO_CAP_Init(SGPIO_TypeDef *SGPIOx, SGPIO_CAPInitTypeDef *SGPIO_CAPInitStruct)
  *
  *		 6. Enable capture command.
  *			 SGPIO_Cap_Cmd(SGPIOx, ENABLE)
  *
  *		 7. Waiting for RX input.
  *
  *****************************************************************************************     
  *  How to use SGPIO capture compare event
  *****************************************************************************************
  *      To use the SGPIO capture compare event at multiple counter match event, 
  *         the following steps are mandatory.
  *
  *		 1. Configure the SGPIO pinmux and pull the SGPIO pin up.
  *			 Pinmux_Congfig(Pin_Num, PINMUX_FUNCTION_SGPIO)
  *			 PAD_PullCtrl(PinName, GPIO_PuPd_UP)
  *
  *		 2. Initialize the SGPIO interrupt infomation and enable it.
  *			 InterruptRegister(IRQ_FUN IrqFun, IRQn_Type IrqNum, Data, Priority)
  *			 InterruptEn(IRQn_Type IrqNum, Priority)
  *
  *		 3. Initialize the RX timer, capture and multiple timer as default.
  *			 SGPIO_RX_StructInit(SGPIO_RXInitTypeDef * SGPIO_RXInitStruct)
  *			 SGPIO_CAP_StructInit(SGPIO_CAPInitTypeDef * SGPIO_CAPInitStruct)
  *			 SGPIO_MUL_StructInit(SGPIO_MULInitTypeDef * SGPIO_MULInitStruct)
  *
  *		 4. Set Rx data source to capture compare.
  *			 	SGPIO_RXInitStruct.RxDataSrc_Sel = RX_DATA_SRC_CAP_CAMPARE;
  *			Set Capture source to multiple counter match event.
  *				SGPIO_CAPInitStruct.CapSrc_Sel = CAPSRC_MUL_COUNTER.
  *			set RX timer pre-scale if needed.(optional)
  *
  *		 5. Select the RX trigger edge and capture trigger edge as well as mutiple counter edge.
  *
  *		 6. Initialize hardware using the value configured above.
  *			 SGPIO_RX_Init(SGPIO_TypeDef *SGPIOx, SGPIO_RXInitTypeDef *SGPIO_RXInitStruct)
  *			 SGPIO_CAP_Init(SGPIO_TypeDef *SGPIOx, SGPIO_CAPInitTypeDef *SGPIO_CAPInitStruct)
  *			 SGPIO_MUL_Init(SGPIO_TypeDef *SGPIOx, SGPIO_MULInitTypeDef *SGPIO_MULInitStruct)
  *
  *		 7. Configure the Multiple match event.
  *			 SGPIO_MULMR0MulConfig(SGPIOx, SGPIO_MULMR0Val0, SGPIO_MULMR0Val1, SGPIO_MULMR0Ctl)
  *
  *		 8. Configure the capture compare value and output value.
  *			 SGPIO_Cap_CompConfig(SGPIOx, CapComp_Val, CapComp_Mode)
  *
  *		 9. Enable SGPIO capture event and multiple counter mode.
  *			 SGPIO_Cap_Cmd(SGPIOx, ENABLE)
  *			 SGPIO_MULTmr_Cmd(SGPIOx, ENABLE)
  *
  *****************************************************************************************     
  *  How to use SGPIO output PWM 
  *****************************************************************************************
  *      To use the SGPIO output PWM, the following steps are mandatory.
  *
  *		 1. Configure the SGPIO pinmux and pull the SGPIO pin up.
  *			 Pinmux_Congfig(Pin_Num, PINMUX_FUNCTION_SGPIO)
  *			 PAD_PullCtrl(PinName, GPIO_PuPd_UP)
  *
  *		 2. Initialize the RX timer as default.
  *			 SGPIO_RX_StructInit(SGPIO_RXInitTypeDef * SGPIO_RXInitStruct
  *			Set Rx trigger edge mode if needed.
  *
  *		 3. Initialize hardware using the value configured above.
  *			 SGPIO_RX_Init(SGPIO_TypeDef *SGPIOx, SGPIO_RXInitTypeDef *SGPIO_RXInitStruct)
  *
  *		 4. Configure RX match event, whrer x can be 0\1\2.
  *			 SGPIO_RXMRxConfig(SGPIOx, SGPIO_RXMRxVal, SGPIO_RXMRxCtl)
  *
  *		 5. Configure the output level at RX match event.
  *			 SGPIO_RXMRxTXConfig(SGPIOx, SGPIO_RXMR0TXCtl, SGPIO_RXMR1TXCtl, SGPIO_RXMR2TXCtl)
  * 
  *		 6. Waiting for the RX input trigger.
  *
  *		 @note if using interrput, remember to initialize SGPIO interrupt information
  *			and enable SGPIO interrupt.
  *			 InterruptEn(IRQn_Type IrqNum, Priority)
  *
  *****************************************************************************************     
  *  How to use SGPIO transfer multiple data
  *****************************************************************************************
  *      To use the SGPIO transfer multiple data, the following steps are mandatory.
  *
  *		 1. Configure the SGPIO pinmux and pull the SGPIO pin up.
  *			 Pinmux_Congfig(Pin_Num, PINMUX_FUNCTION_SGPIO)
  *			 PAD_PullCtrl(PinName, GPIO_PuPd_UP)
  *
  *		 2. Initialize the SGPIO interrupt infomation and enable it.
  *			 InterruptRegister(IRQ_FUN IrqFun, IRQn_Type IrqNum, Data, Priority)
  *			 InterruptEn(IRQn_Type IrqNum, Priority)
  *
  *		 3. Initialize the Multiple timer as default.
  *			 SGPIO_MUL_StructInit(SGPIO_MULInitTypeDef * SGPIO_MULInitStruct)
  *			Set TX to bi-direction open-drain output.
  *			Configure the multiple positon register.
  *			select the multiple data direction if needed.
  *
  *		 4. Initialize hardware using the value configured above.
  *			 SGPIO_MUL_Init(SGPIO_TypeDef *SGPIOx, SGPIO_MULInitTypeDef *SGPIO_MULInitStruct)
  *
  *		 5. Configure MUL match event with the four multiple match registers.
  *			 SGPIO_MULMRxGP0ValConfig(SGPIOx, SGPIO_MULMR0GP0, SGPIO_MULMR1GP0, SGPIO_MULMR2GP0, SGPIO_MULMR3GP0)
  *			 SGPIO_MULMRxGP1ValConfig(SGPIOx, SGPIO_MULMR0GP1, SGPIO_MULMR1GP1, SGPIO_MULMR2GP1, SGPIO_MULMR3GP1)
  *			 SGPIO_MULMRxTXCtlConfig(SGPIOx, SGPIO_MULMR0TXCtl, SGPIO_MULMR1TXCtl, SGPIO_MULMR2TXCtl, SGPIO_MULMR3TXCtl)
  *
  *		 6. Configure the multiple data value.
  *			 SGPIO_SetTXData(SGPIOx, SGPIO_MulDataVal, SGPIO_MulDataDPVal)
  *
  *		 7. Configure the MCNT interrupt event to set the waveform length.
  *			 SGPIO_MULMCNTConfig(SGPIOx, SGPIO_MULMCNTVal, SGPIO_MULMCNTCtl)
  *
  *		 8. Enable the multiple shift_clk, multiple data load and MCNT interrupt at least.
  *			 SGPIO_INTConfig(SGPIOx, SGPIO_IT, ENABLE)
  *			 SGPIO_MULMCNT_Cmd(SGPIOx, ENABLE)
  * 
  *		 9. Enable the multiple timer/counter.
  *			 SGPIO_MULTmr_Cmd(SGPIOx, ENABLE)
  *
  *		 @note if using FIFO or DMA for data transfer, according to the related API.
  *			Configure FIFO register and GDMA transfer buffer, and enable the command related.
  *
  *****************************************************************************************     
  *  How to SGPIO RX pattern match 
  *****************************************************************************************
  *      To use the SGPIO RX pattern match event at multiple counter match capture event,
  *         the following steps are mandatory.
  *
  *		 1. Configure the SGPIO pinmux and pull the SGPIO pin up.
  *			 Pinmux_Congfig(Pin_Num, PINMUX_FUNCTION_SGPIO)
  *			 PAD_PullCtrl(PinName, GPIO_PuPd_UP)
  *
  *		 2. Initialize the SGPIO interrupt infomation and enable it.
  *			 InterruptRegister(IRQ_FUN IrqFun, IRQn_Type IrqNum, Data, Priority)
  *			 InterruptEn(IRQn_Type IrqNum, Priority)
  *
  *		 3. Initialize the RX timer, capture and multiple timer as default.
  *			 SGPIO_RX_StructInit(SGPIO_RXInitTypeDef * SGPIO_RXInitStruct)
  *			 SGPIO_CAP_StructInit(SGPIO_CAPInitTypeDef * SGPIO_CAPInitStruct)
  *			 SGPIO_MUL_StructInit(SGPIO_MULInitTypeDef * SGPIO_MULInitStruct)
  *
  *		 4. Set Rx data source to capture compare.
  *			 	SGPIO_RXInitStruct.RxDataSrc_Sel = RX_DATA_SRC_CAP_CAMPARE;
  *			Set Capture source to multiple counter match event.
  *				SGPIO_CAPInitStruct.CapSrc_Sel = CAPSRC_MUL_COUNTER.
  *			set RX timer pre-scale and RX positon register if needed.(optional)
  *
  *		 5. Select the RX trigger edge and capture trigger edge as well as mutiple counter edge.
  *
  *		 6. Initialize hardware using the value configured above.
  *			 SGPIO_RX_Init(SGPIO_TypeDef *SGPIOx, SGPIO_RXInitTypeDef *SGPIO_RXInitStruct)
  *			 SGPIO_CAP_Init(SGPIO_TypeDef *SGPIOx, SGPIO_CAPInitTypeDef *SGPIO_CAPInitStruct)
  *			 SGPIO_MUL_Init(SGPIO_TypeDef *SGPIOx, SGPIO_MULInitTypeDef *SGPIO_MULInitStruct)
  *
  *		 7. Configure the Multiple match event.
  *			 SGPIO_MULMR0MulConfig(SGPIOx, SGPIO_MULMR0Val0, SGPIO_MULMR0Val1, SGPIO_MULMR0Ctl)
  *
  *		 8. Configure the capture compare value and output value.
  *			 SGPIO_Cap_CompConfig(SGPIOx, CapComp_Val, CapComp_Mode)
  *
  *		 9. Configure RX match event to limit the sereaching time.
  *			 SGPIO_RXMRxConfig(SGPIOx, SGPIO_RXMRxVal, SGPIO_RXMRxCtl)
  *			 SGPIO_RXMR0MULConfig(SGPIOx, SGPIO_RXMR0Val, SGPIO_RXMR0MULCtl)
  *
  *		10. Configure the RX pattern match event, and enable this interrupt.
  *			 SGPIO_RXPatternMatchConfig(SGPIOx, RXDATADP, RXDATAMask, ENABLE)
  *			 SGPIO_INTConfig(SGPIOx, SGPIO_IT, ENABLE)
  *
  *		11. Enable SGPIO capture event and multiple counter mode.
  *			 SGPIO_Cap_Cmd(SGPIOx, ENABLE)
  *			 SGPIO_MULTmr_Cmd(SGPIOx, ENABLE)
  *
  *****************************************************************************************     
  *  How to use SGPIO monitor at power saving mode.
  *****************************************************************************************
  *      To use the SGPIO monitor mode, the following steps are mandatory.
  *
  *		 1. Configure the SGPIO pinmux and pull the SGPIO pin up.
  *			 Pinmux_Congfig(Pin_Num, PINMUX_FUNCTION_SGPIO)
  *			 PAD_PullCtrl(PinName, GPIO_PuPd_UP)
  *
  *		 2. Initialize the SGPIO interrupt infomation and enable it.
  *			 InterruptRegister(IRQ_FUN IrqFun, IRQn_Type IrqNum, Data, Priority)
  *			 InterruptEn(IRQn_Type IrqNum, Priority)
  *
  *		 3. Initialize the RX timer, multiple timer as default.
  *			 SGPIO_RX_StructInit(SGPIO_RXInitTypeDef * SGPIO_RXInitStruct)
  *			 SGPIO_MUL_StructInit(SGPIO_MULInitTypeDef * SGPIO_MULInitStruct)
  *			Configure the RX input trigger edge and multiple timer mode.
  *			Configure the RX or Multiple pre-scale if needed.
  *
  *		 4. Initialize hardware use the value configured above.
  *			 SGPIO_RX_Init(SGPIO_TypeDef *SGPIOx, SGPIO_RXInitTypeDef *SGPIO_RXInitStruct)
  *			 SGPIO_MUL_Init(SGPIO_TypeDef *SGPIOx, SGPIO_MULInitTypeDef *SGPIO_MULInitStr
  *
  *		 5. Configure the RX match event as monitor action for example,and
  *			Configure the multiple match event as timeout to get into power saving mode.
  *			 SGPIO_RXMRxConfig(SGPIOx, SGPIO_RXMRxVal, SGPIO_RXMRxCtl)
  *			 SGPIO_MULMR0MULConfig(SGPIOx, SGPIO_MULMR0Val, SGPIO_MULMR0MULCtl) 
  *
  *		 6. Enable multiple timer/counter event.
  *
  *		 @note this is a example of monitor mode. the specific implementation can differ.
  *
  *****************************************************************************************     
  * @endverbatim
  */


/* Exported types ------------------------------------------------------------*/

/** @defgroup SGPIO_Exported_Types SGPIO Exported Types
  * @{
  */

/** 
  * @brief  SGPIO RX Init structure definition  
  */
typedef struct {
	u32 RxTimerEdge_Sel;   /*!< Specifies the trigger mode of RXTC.
					This parameter can be a value of @ref RX_Timer_Trigger_Mode_definitions */

	u32 RxDataSrc_Sel;  /*!< Specifies the data source of RX.
					This parameter can be a value of @ref RX_Data_Source_definitions */

	u32 RxPRVal;  /*!< Specifies the maximum value of RX Prescale Counter.
			This parameter must be set to a value in the 0x0-0xffff range. */

	u32 RxPRTC;  /*!< Specifies the value of RX Prescale Counter.
			This parameter must be set to a value in the 0x0-0xffff range. */

	u32 RxTC;  /*!< Specifies the value of RX Timer counter.
			This parameter must be set to a value in the 0x0-0xffff range. */

	u32 RxData;  /*!< Specifies the value of RX Data.
			This parameter must be set to a value in the 0x0-0xffffffff range. */

	u32 RxData_DP;  /*!< Specifies the value of RX Data Duplicate.
			This parameter must be set to a value in the 0x0-0xffffffff range. */

	u32 RxPosTC;  /*!< Specifies the value of RX Position counter. RxPosTC counts down when generate a shift clock for RX Data Register
			This parameter must be set to a value in the 0x0-0x1f range. */

	u32 RxData_Dir;  /*!< Specifies the direction of RX DATA.
			This parameter can be a value of @ref RX_Data_Direction */
} SGPIO_RXInitTypeDef;

/** 
  * @brief  SGPIO CAPTURE Init structure definition  
  */
typedef struct {
  	u32 Cap_ValidEn; /*!< Specifies the valid match whether need to enable RXTMR_CEN.
			This parameter can be a value of @ref Cap_RX_Valid_Control */

	u32 Cap_RxTCStop_Ctrl;  /*!< Specifies the RX stop control when capture trigger event happens.
			This parameter can be a value of @ref Cap_RX_Stop_Control */

	u32 Cap_RxTCRst_Ctrl;  /*!< Specifies the RX reset control when capture trigger event happens.
			This parameter can be a value of @ref Cap_RX_Reset_Control  */

	u32 CapSrc_Sel;  /*!< Specifies the capture trigger source.
			This parameter can be a value of @ref CapSrc_Sel_definitions */

	u32 CapEdge_Sel;  /*!< Specifies the rx input trigger mode when the CAPR is loaded with the value of RXTC
			This parameter can be a value of @ref CapEdge_Sel_definitions */

	u32 CapI_En;  /*!< Specifies the interrupt enable control when capture load event happens.
			This parameter can be a value of @ref Cap_Interrupt_Control */
} SGPIO_CAPInitTypeDef;

/** 
  * @brief  SGPIO Multiple Init structure definition  
  */
typedef struct {
	u32 BiOut;  /*!< Specifies the TX to bi-direction open-drain output. When disable output, bi-direction open-drain output is high.
					This parameter can be a value of @ref MUL_BiOut_definitions */
	
	u32 MulMode_Sel;   /*!< Specifies the Increased mode of MULTC.
					This parameter can be a value of @ref MUL_Mode_definitions */

	u32 MulPRVal;  /*!< Specifies the maximum value of Multiple Prescale Counter.
			This parameter must be set to a value in the 0x0-0xffff range. */

	u32 MulPRTC;  /*!< Specifies the value of Multiple Prescale Counter.
			This parameter must be set to a value in the 0x0-0xffff range. */

	u32 MulTC;  /*!< Specifies the value of Multiple Timer counter.
			This parameter must be set to a value in the 0x0-0xffff range. */

	u32 MulPosTC;  /*!< Specifies the value of Multiple Position counter. MulPosTC counts down when generate a shift clock for Multiple Data Register
			This parameter must be set to a value in the 0x0-0x1f range. */

	u32 MulPosRST;  /*!< Specifies the reload value of RxPosTC.
			This parameter must be set to a value in the 0x0-0x1f range. */

	u32 MulData_Dir;  /*!< Specifies the direction of Multiple DATA.
			This parameter can be a value of @ref MUL_Data_Direction */
} SGPIO_MULInitTypeDef;
/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/

/** @defgroup SGPIO_Exported_Constants SGPIO Exported Constants
  * @{
  */ 

/** @defgroup SGPIO_Peripheral_definitions 
  * @{
  */
#define IS_SGPIO_ALL_PERIPH(PERIPH) ((PERIPH) == SGPIO_DEV)
/**
  * @}
  */

/** @defgroup RX_Data_Source_definitions
  * @{
  */
#define RX_DATA_SRC_EXTERNAL_INPUT				((u32)0x00000000 << 4)
#define RX_DATA_SRC_CAP_CAMPARE				((u32)0x00000001 << 4)
#define IS_RX_DATA_SRC(SOURCE)				(((SOURCE) == RX_DATA_SRC_EXTERNAL_INPUT) || \
												((SOURCE) == RX_DATA_SRC_CAP_CAMPARE))

/**
  * @}
  */
  
/** @defgroup RX_Timer_Trigger_Mode_definitions 
  * @{
  */
#define RX_TIMER_NO_TRIGGER				((u32)0x00000000 << 8)
#define RX_TIMER_RISING_EDGE			((u32)0x00000001 << 8)
#define RX_TIMER_FALLING_EDGE			((u32)0x00000002 << 8)
#define RX_TIMER_BOTH_EDGE				((u32)0x00000003 << 8)
#define IS_RX_TIMER_TRIGGER_MODE(MODE)	(((MODE) == RX_TIMER_NO_TRIGGER) || \
											((MODE) == RX_TIMER_RISING_EDGE) || \
											((MODE) == RX_TIMER_FALLING_EDGE) || \
											((MODE) == RX_TIMER_BOTH_EDGE))
/**
  * @}
  */

/** @defgroup RX_Data_Direction
  * @{
  */
#define RX_DATA_DIR_MSB				((u32)0x00000000 << 7)
#define RX_DATA_DIR_LSB				((u32)0x00000001 << 7)
#define IS_RX_DATA_DIR(DIRECTION)	(((DIRECTION) == RX_DATA_DIR_MSB) || \
										((DIRECTION) == RX_DATA_DIR_LSB))

/**
  * @}
  */

/** @defgroup RXMR0_Output_Control_Options 
  * @{
  */
#define RX_EMC0_NONE					((u32)0x00000000 << 6)
#define RX_EMC0_OUTPUT_LOW				((u32)0x00000001 << 6)
#define RX_EMC0_OUTPUT_HIGH				((u32)0x00000002 << 6)
#define RX_EMC0_OUTPUT_TOGGLE			((u32)0x00000003 << 6)
#define IS_RXMR0_OUTCTRL_OPTION(OPTION)	(((OPTION) == RX_EMC0_NONE) || \
												((OPTION) == RX_EMC0_OUTPUT_LOW) || \
												((OPTION) == RX_EMC0_OUTPUT_HIGH) || \
												((OPTION) == RX_EMC0_OUTPUT_TOGGLE))
/**
  * @}
  */

/** @defgroup RXMR1_Output_Control_Options 
  * @{
  */
#define RX_EMC1_NONE					((u32)0x00000000 << 8)
#define RX_EMC1_OUTPUT_LOW				((u32)0x00000001 << 8)
#define RX_EMC1_OUTPUT_HIGH				((u32)0x00000002 << 8)
#define RX_EMC1_OUTPUT_TOGGLE			((u32)0x00000003 << 8)
#define IS_RXMR1_OUTCTRL_OPTION(OPTION)	(((OPTION) == RX_EMC1_NONE) || \
												((OPTION) == RX_EMC1_OUTPUT_LOW) || \
												((OPTION) == RX_EMC1_OUTPUT_HIGH) || \
												((OPTION) == RX_EMC1_OUTPUT_TOGGLE))
/**
  * @}
  */

/** @defgroup RXMR2_Output_Control_Options 
  * @{
  */                             								 
#define RX_EMC2_NONE					((u32)0x00000000 << 10)
#define RX_EMC2_OUTPUT_LOW				((u32)0x00000001 << 10)
#define RX_EMC2_OUTPUT_HIGH				((u32)0x00000002 << 10)
#define RX_EMC2_OUTPUT_TOGGLE			((u32)0x00000003 << 10)
#define IS_RXMR2_OUTCTRL_OPTION(OPTION)	(((OPTION) == RX_EMC2_NONE) || \
												((OPTION) == RX_EMC2_OUTPUT_LOW) || \
												((OPTION) == RX_EMC2_OUTPUT_HIGH) || \
												((OPTION) == RX_EMC2_OUTPUT_TOGGLE))
/**
  * @}
  */

/** @defgroup Cap_RX_Valid_Control 
  * @{
  */
#define CAP_RX_VALID_ENABLE				((u32)0x00000000 << 8)		/*The valid match need to enable RXTMR_CEN*/
#define CAP_RX_VALID_DISABLE			((u32)0x00000001 << 8)		/*The valid match don't need to enable RXTMR_CEN*/
#define IS_CAP_RX_VALID_OPTION(OPTION)	(((OPTION) == CAP_RX_VALID_DISABLE) || \
												((OPTION) == CAP_RX_VALID_ENABLE))
/**
  * @}
  */
  
/** @defgroup Cap_RX_Stop_Control 
  * @{
  */
#define CAP_RX_STOP_DISABLE				((u32)0x00000000 << 6)
#define CAP_RX_STOP_ENABLE				((u32)0x00000001 << 6)
#define IS_CAP_RX_STOP_OPTION(OPTION)	(((OPTION) == CAP_RX_STOP_DISABLE) || \
												((OPTION) == CAP_RX_STOP_ENABLE))
/**
  * @}
  */

/** @defgroup Cap_RX_Reset_Control 
  * @{
  */
#define CAP_RX_RESET_DISABLE			((u32)0x00000000 << 5)
#define CAP_RX_RESET_ENABLE				((u32)0x00000001 << 5)
#define IS_CAP_RX_RESET_OPTION(OPTION)	(((OPTION) == CAP_RX_RESET_DISABLE) || \
												((OPTION) == CAP_RX_RESET_ENABLE))
/**
  * @}
  */

/** @defgroup Cap_CmpVal_Sel_definitions 
  * @{
  */
#define CAP_CMPVAL_SEL0					((u32)0x00000000 << 7)		/*RXDATA receives 0 when CAPR is bigger than CAP_CMPVAL*/
#define CAP_CMPVAL_SEL1					((u32)0x00000001 << 7)		/*RXDATA receives 1 when CAPR is bigger than CAP_CMPVAL*/
#define IS_CAP_CMPVAL_SEL_OPTION(OPTION)	(((OPTION) == CAP_CMPVAL_SEL0) || \
												((OPTION) == CAP_CMPVAL_SEL1))
/**
  * @}
  */

/** @defgroup CapSrc_Sel_definitions 
  * @{
  */
#define CAPSRC_RX_INPUT				((u32)0x00000000 << 2)
#define CAPSRC_MUL_COUNTER				((u32)0x00000001 << 2)
#define IS_CAPSRC_SEL_OPTION(OPTION)	(((OPTION) == CAPSRC_RX_INPUT) || \
												((OPTION) == CAPSRC_MUL_COUNTER))
/**
  * @}
  */

/** @defgroup CapEdge_Sel_definitions 
  * @{
  */
#define CAP_RX_FALLING_EDGE				((u32)0x00000001 << 3)
#define CAP_RX_RISING_EDGE				((u32)0x00000002 << 3)
#define IS_CAPEDGE_SEL_OPTION(OPTION)	(((OPTION) == CAP_RX_FALLING_EDGE) || \
												((OPTION) == CAP_RX_RISING_EDGE))
/**
  * @}
  */

/** @defgroup Cap_Interrupt_Control 
  * @{
  */
#define CAP_INTR_DISABLE				((u32)0x00000000 << 1)
#define CAP_INTR_ENABLE				((u32)0x00000001 << 1)
#define IS_CAP_INTR_OPTION(OPTION)		(((OPTION) == CAP_INTR_DISABLE) || \
												((OPTION) == CAP_INTR_ENABLE))
/**
  * @}
  */
  
/** @defgroup MUL_BiOut_definitions
  * @{
  */
#define MUL_DISABLE_BIOUT				((u32)0x00000000 << 16)
#define MUL_ENABLE_BIOUT				((u32)0x00000001 << 16)
#define IS_MUL_BIOUT_MODE(MODE)			(((MODE) == MUL_DISABLE_BIOUT) || \
												((MODE) == MUL_ENABLE_BIOUT))
/**
  * @}
  */
  
/** @defgroup MUL_Mode_definitions 
  * @{
  */
#define MUL_TIMER_MODE				((u32)0x00000000 << 8)
#define MUL_COUNTER_RISING_EDGE			((u32)0x00000001 << 8)
#define MUL_COUNTER_FALLING_EDGE			((u32)0x00000002 << 8)
#define MUL_COUNTER_BOTH_EDGE			((u32)0x00000003 << 8)
#define IS_MUL_MODE(MODE)				(((MODE) == MUL_TIMER_MODE) || \
												((MODE) == MUL_COUNTER_RISING_EDGE) || \
												((MODE) == MUL_COUNTER_FALLING_EDGE) || \
												((MODE) == MUL_COUNTER_BOTH_EDGE))
/**
  * @}
  */

/** @defgroup MUL_Data_Direction
  * @{
  */
#define MUL_DATA_DIR_LSB				((u32)0x00000000 << 8)
#define MUL_DATA_DIR_MSB				((u32)0x00000001 << 8)
#define IS_MUL_DATA_DIR(DIRECTION)		(((DIRECTION) == MUL_DATA_DIR_MSB) || \
												((DIRECTION) == MUL_DATA_DIR_LSB))
/**
  * @}
  */
  
/** @defgroup MULMR1_Output_Control_Options 
  * @{
  */
#define MUL_EMC1_NONE					((u32)0x00000000)
#define MUL_EMC1_OUTPUT_LOW					((u32)0x00000001)
#define MUL_EMC1_OUTPUT_HIGH				((u32)0x00000002)
#define MUL_EMC1_OUTPUT_TOGGLE				((u32)0x00000003)
#define IS_MULMR1_OUTCTRL_OPTION(OPTION)	(((OPTION) == MUL_EMC1_NONE) || \
												((OPTION) == MUL_EMC1_OUTPUT_LOW) || \
												((OPTION) == MUL_EMC1_OUTPUT_HIGH) || \
												((OPTION) == MUL_EMC1_OUTPUT_TOGGLE))
/**
  * @}
  */

/** @defgroup MULMR2_Output_Control_Options 
  * @{
  */                             								 
#define MUL_EMC2_NONE					((u32)0x00000000 << 2)
#define MUL_EMC2_OUTPUT_LOW					((u32)0x00000001 << 2)
#define MUL_EMC2_OUTPUT_HIGH				((u32)0x00000002 << 2)
#define MUL_EMC2_OUTPUT_TOGGLE				((u32)0x00000003 << 2)
#define IS_MULMR2_OUTCTRL_OPTION(OPTION)	(((OPTION) == MUL_EMC2_NONE) || \
													((OPTION) == MUL_EMC2_OUTPUT_LOW) || \
													((OPTION) == MUL_EMC2_OUTPUT_HIGH) || \
													((OPTION) == MUL_EMC2_OUTPUT_TOGGLE))
/**
  * @}
  */

/** @defgroup MULMR3_Output_Control_Options 
  * @{
  */
#define MUL_EMC3_NONE					((u32)0x00000000 << 4)
#define MUL_EMC3_OUTPUT_LOW					((u32)0x00000001 << 4)
#define MUL_EMC3_OUTPUT_HIGH				((u32)0x00000002 << 4)
#define MUL_EMC3_OUTPUT_TOGGLE				((u32)0x00000003 << 4)
#define IS_MULMR3_OUTCTRL_OPTION(OPTION)	(((OPTION) == MUL_EMC3_NONE) || \
													((OPTION) == MUL_EMC3_OUTPUT_LOW) || \
													((OPTION) == MUL_EMC3_OUTPUT_HIGH) || \
													((OPTION) == MUL_EMC3_OUTPUT_TOGGLE))
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup SGPIO_Exported_Functions SGPIO Exported Functions
  * @{
  */
void SGPIO_RX_StructInit(SGPIO_RXInitTypeDef* SGPIO_RXInitStruct);
void SGPIO_RX_Init(SGPIO_TypeDef *SGPIOx, SGPIO_RXInitTypeDef* SGPIO_RXInitStruct);
void SGPIO_RXMR0MULConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR0Val, u32 SGPIO_RXMR0MULCtl);
void SGPIO_RXPosConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_PosRstVal, u32 SGPIO_RXPOSCtl);
u32 SGPIO_GetRXData(SGPIO_TypeDef *SGPIOx);
void SGPIO_RXMR0Config(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR0Val, u32 SGPIO_RXMR0Ctl);
void SGPIO_RXMR1Config(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR1Val, u32 SGPIO_RXMR1Ctl);
void SGPIO_RXMR2Config(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR2Val, u32 SGPIO_RXMR2Ctl);
void SGPIO_RXMRxTXConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_RXMR0TXCtl, u32 SGPIO_RXMR1TXCtl, u32 SGPIO_RXMR2TXCtl);
void SGPIO_RXTmr_Reset(SGPIO_TypeDef *SGPIOx);
void SGPIO_RXTmr_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState);
void SGPIO_RXPatternMatchConfig(SGPIO_TypeDef *SGPIOx, u32 RXDATADP, u32 RXDATAMask, u8 NewState);

void SGPIO_MUL_StructInit(SGPIO_MULInitTypeDef* SGPIO_MULInitStruct);
void SGPIO_MUL_Init(SGPIO_TypeDef *SGPIOx, SGPIO_MULInitTypeDef* SGPIO_MULInitStruct);
void SGPIO_MULMR0MulConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0Val0, u32 SGPIO_MULMR0Val1, u32 SGPIO_MULMR0Ctl);
void SGPIO_MULMR0RXConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0Val0, u32 SGPIO_MULMR0Val1, u32 SGPIO_MULMR0RXCtl);
void SGPIO_MULMRxGP0ValConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0GP0, u32 SGPIO_MULMR1GP0, u32 SGPIO_MULMR2GP0, u32 SGPIO_MULMR3GP0);
void SGPIO_MULMRxGP1ValConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0GP1, u32 SGPIO_MULMR1GP1, u32 SGPIO_MULMR2GP1, u32 SGPIO_MULMR3GP1);
void SGPIO_MULMRxTXCtlConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMR0TXCtl, u32 SGPIO_MULMR1TXCtl, u32 SGPIO_MULMR2TXCtl, u32 SGPIO_MULMR3TXCtl);
void SGPIO_SetTXData(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MulDataVal, u32 SGPIO_MulDataDPVal);
void SGPIO_OutputConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_OutputCtl);
void SGPIO_MULTmr_Reset(SGPIO_TypeDef *SGPIOx);
void SGPIO_MULTmr_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState);
void SGPIO_MULMCNTConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULMCNTVal, u32 SGPIO_MULMCNTCtl);
void SGPIO_MULMCNT_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState);

void SGPIO_CAP_StructInit(SGPIO_CAPInitTypeDef* SGPIO_CAPInitStruct);
void SGPIO_CAP_Init(SGPIO_TypeDef *SGPIOx, SGPIO_CAPInitTypeDef* SGPIO_CAPInitStruct);
u32 SGPIO_GetCapVal(SGPIO_TypeDef *SGPIOx);
void SGPIO_Cap_CompConfig(SGPIO_TypeDef *SGPIOx, u32 CapComp_Val, u32 CapComp_Mode);
void SGPIO_Cap_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState);

void SGPIO_INTConfig(SGPIO_TypeDef *SGPIOx, u32 SGPIO_IT, u32 NewState);
void SGPIO_INTMask(SGPIO_TypeDef *SGPIOx, u32 SGPIO_IT, u32 NewState);
u32 SGPIO_GetRawINT(SGPIO_TypeDef *SGPIOx);
void SGPIO_ClearRawINT(SGPIO_TypeDef *SGPIOx, u32 SGPIO_IT);

void SGPIO_MULFIFO_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState);
void SGPIO_MULFIFO_Set(SGPIO_TypeDef *SGPIOx, u32 SGPIO_MULFIFOVal);
void SGPIO_MULDMA_Cmd(SGPIO_TypeDef *SGPIOx, u8 NewState);
BOOL SGPIO_MULGDMA_Init(SGPIO_TypeDef *SGPIOx, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u32 *pTxBuf, int TxCount);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup SGPIO_Register_Definitions SGPIO Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup SGPIO_RXTMR_CTRL
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXTMREDGE_SEL			((u32)0x00000003 << 8)		/*Bit[9:8], bits for RXTC start condition set*/
#define BIT_SGPIO_RXTMR_CRST			((u32)0x00000001 << 1)		/*Bit[1], bit for reset RXTC and RXPRTC*/
#define BIT_SGPIO_RXTMR_CEN				((u32)0x00000001)			/*Bit[0], bit for enable RXTC and RXPRTC*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXPRVAL
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXPRVAL				((u32)0x0000ffff)			/*Bit[15:0], bits for Maximum value of the RX Prescale Counter*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXPRTC
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXPRTC				((u32)0x0000ffff)			/*Bit[15:0], bits for RX Prescale Counter value*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXTC
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXTC				((u32)0x0000ffff)			/*Bit[15:0], bits for RX Timer Counter value*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXMC_CTRL
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RX_POS_CTRL			((u32)0x0000c000)			/*Bit[15:14], bits for RXTC control mask when RXPOSTC reaches 0*/
#define BIT_SGPIO_RX_POSSTOP_EN			((u32)0x00000001 << 15)	/*Bit[15], bit for RXTC stop control when RXPOSTC reaches 0*/
#define BIT_SGPIO_RX_POSRST_EN			((u32)0x00000001 << 14)	/*Bit[14], bit for RXTC reset control when RXPOSTC reaches 0*/
#define BIT_SGPIO_RX_MR0MUL_CTRL		((u32)0x00003000)			/*Bit[13:12], bits for MULTC control mask when RXMR0 matches RXTC*/
#define BIT_SGPIO_RX_MR0MULSTOP_EN		((u32)0x00000001 << 13)	/*Bit[13], bit for MULTC stop control when RXMR0 matches RXTC*/
#define BIT_SGPIO_RX_MR0MULRST_EN		((u32)0x00000001 << 12)	/*Bit[12], bit for MULTC reset control when RXMR0 matches RXTC*/
#define BIT_SGPIO_RX_MR2_CTRL			((u32)0x00000f00)			/*Bit[11:8], bits for RXTC control mask when RXMR2 matches RXTC*/
#define BIT_SGPIO_RX_MR2SCLK_EN			((u32)0x00000001 << 11)	/*Bit[11], bit for shift clock control when RXMR2 matches RXTC*/
#define BIT_SGPIO_RX_MR2STOP_EN			((u32)0x00000001 << 10)	/*Bit[10], bit for RXTC stop control when RXMR2 matches RXTC*/
#define BIT_SGPIO_RX_MR2RST_EN			((u32)0x00000001 << 9)		/*Bit[9], bit for RXTC reset control when RXMR2 matches RXTC*/
#define BIT_SGPIO_RX_MR2_IE				((u32)0x00000001 << 8)		/*Bit[8], bit for interrupt enable when RXMR2 matches RXTC*/
#define BIT_SGPIO_RX_MR1_CTRL			((u32)0x000000f0)			/*Bit[7:4], bits for RXTC control mask when RXMR1 matches RXTC*/
#define BIT_SGPIO_RX_MR1SCLK_EN			((u32)0x00000001 << 7)		/*Bit[7], bit for shift clock control when RXMR1 matches RXTC*/
#define BIT_SGPIO_RX_MR1STOP_EN			((u32)0x00000001 << 6)		/*Bit[6], bit for RXTC stop control when RXMR1 matches RXTC*/
#define BIT_SGPIO_RX_MR1RST_EN			((u32)0x00000001 << 5)		/*Bit[5], bit for RXTC reset control when RXMR1 matches RXTC*/
#define BIT_SGPIO_RX_MR1_IE				((u32)0x00000001 << 4)		/*Bit[4], bit for interrupt enable when RXMR1 matches RXTC*/
#define BIT_SGPIO_RX_MR0_CTRL			((u32)0x0000000f)			/*Bit[3:0], bits for RXTC control mask when RXMR0 matches RXTC*/
#define BIT_SGPIO_RX_MR0SCLK_EN			((u32)0x00000001 << 3)		/*Bit[3], bit for shift clock control when RXMR0 matches RXTC*/
#define BIT_SGPIO_RX_MR0STOP_EN			((u32)0x00000001 << 2)		/*Bit[2], bit for RXTC stop control when RXMR0 matches RXTC*/
#define BIT_SGPIO_RX_MR0RST_EN			((u32)0x00000001 << 1)		/*Bit[1], bit for RXTC reset control when RXMR0 matches RXTC*/
#define BIT_SGPIO_RX_MR0_IE				((u32)0x00000001)			/*Bit[0], bit for interrupt enable when RXMR0 matches RXTC*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXMR0
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXMR0					((u32)0x0000ffff)			/*Bit[15:0], bits for RX timer counter match register 0 set*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXMR1
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXMR1					((u32)0x0000ffff)			/*Bit[15:0], bits for RX timer counter match register 1 set*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXMR2
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXMR2					((u32)0x0000ffff)			/*Bit[15:0], bits for RX timer counter match register 2 set*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_CAP_CTRL
 * @{
 *****************************************************************************/
#define BIT_SGPIO_CAP_CMPVAL			((u32)0xffff0000)			/*Bit[31:16], bits for capture compare value set*/
#define BIT_SGPIO_CAPVALID_EN			((u32)0x00000001 << 8)		/*Bit[8], bit for whether valid match needs RXTMR_CEN is enabled*/
#define BIT_SGPIO_CAP_CMPVAL_SEL 		((u32)0x00000001 << 7)		/*Bit[7], bit for RXDATA receives value setwhen CAPR 
																	is bigger than CAP_CMPVAL*/
#define BIT_SGPIO_CAP_RXTCSTOP_EN		((u32)0x00000001 << 6)		/*Bit[6], bit for stop RXTC and clear RXTMR_CEN when 
																	the capture trigger event happens*/
#define BIT_SGPIO_CAP_RXTCRST_EN		((u32)0x00000001 << 5)		/*Bit[5], bit for reset RXTC when the capture trigger event happens*/
#define BIT_SGPIO_CAP_RXEDGE_SEL		((u32)0x00000003 << 3)		/*Bit[4:3], bits for capture edge select mask*/
#define BIT_SGPIO_CAPSRC_SEL			((u32)0x00000001 << 2)		/*Bit[2], bit for eelect the capture trigger source*/
#define BIT_SGPIO_CAP_IE				((u32)0x00000001 << 1)		/*Bit[1], bit for interrupt enable when capture load event happens*/
#define BIT_SGPIO_CAP_EN				((u32)0x00000001)			/*Bit[0], bit for ebalbe CAPR loaded with the value of RXTC*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_CAPR
 * @{
 *****************************************************************************/
#define BIT_SGPIO_CAPR					((u32)0x0000ffff)			/*Bit[15:0], bits for RX timer counter capture value*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULTMR_CTRL
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULMODE_SEL			((u32)0x00000003 << 8)		/*Bit[9:8], bits for MULTC work mode*/
#define BIT_SGPIO_MULTMR_CRST			((u32)0x00000001 << 1)		/*Bit[1], bit for reset MULTC and MULPRTC*/
#define BIT_SGPIO_MULTMR_CEN			((u32)0x00000001)			/*Bit[0], bit for enable MULTC and MULPRTC*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULPRVAL
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULPRVAL				((u32)0x0000ffff)			/*Bit[15:0], bits for Maximum value of the Multiple Prescale Counter*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULPRTC
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULPRTC				((u32)0x0000ffff)			/*Bit[15:0], bits for Multiple Prescale Counter value*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULTC
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULTC					((u32)0x0000ffff)			/*Bit[15:0], bits for Multiple Timer Counter value*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULMC_CTRL
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MUL_DMA_START			((u32)0x00000001 << 13)		/*Bit[13], bit for trigger mutiple match FIFO load with data*/
#define BIT_SGPIO_MUL_DMA_EN			((u32)0x00000001 << 12)		/*Bit[12], bit for DMA mode enable*/
#define BIT_SGPIO_MUL_FMODE_EN			((u32)0x00000001 << 11)		/*Bit[11], bit for Multiple FIFO mode enable*/
#define BIT_SGPIO_MUL_MCNT_CTRL			((u32)0x00000700)			/*Bit[10:8], bits for MULTC control mask when MUL_MCNT reaches zero*/
#define BIT_SGPIO_MUL_MCNTSTOP_EN		((u32)0x00000001 << 10)		/*Bit[10], bit for MULTC stop control when MUL_MCNT reaches zero*/
#define BIT_SGPIO_MUL_MCNTRST_EN		((u32)0x00000001 << 9)		/*Bit[9], bit for MULTC reset control when MUL_MCNT reaches zero*/
#define BIT_SGPIO_MUL_MCNT_IE			((u32)0x00000001 << 8)		/*Bit[8], bit for interrupt enable when MUL_MCNT reaches zero*/
#define BIT_SGPIO_MUL_MR0MCNT_EN		((u32)0x00000001 << 7)		/*Bit[7], bit for enable MUL_MCNT counts down when
																	MULMR0 matches MULTC*/
#define BIT_SGPIO_MUL_MR0CAPSRC_EN		((u32)0x00000001 << 6)		/*Bit[6], bit for change the capture trigger source*/
#define BIT_SGPIO_MUL_MR0RX_CTRL		((u32)0x00000030)			/*Bit[5:4], bits for RXTC control mask when MULMR0 matches MULTC*/
#define BIT_SGPIO_MUL_MR0RXSTOP_EN		((u32)0x00000001 << 5)		/*Bit[5], bit for RXTC STOP control when MULMR0 matches MULTC*/
#define BIT_SGPIO_MUL_MR0RXRST_EN		((u32)0x00000001 << 4)		/*Bit[4], bit for RXTC reset control when MULMR0 matches MULTC*/
#define BIT_SGPIO_MUL_MR0_CTRL			((u32)0x0000000f)			/*Bit[3:0], bits for MULTC control mask when MULMR0 matches MULTC*/
#define BIT_SGPIO_MUL_MR0SCLK_EN		((u32)0x00000001 << 3)		/*Bit[3], bit for shift clock control when MULMR0 matches MULTC*/
#define BIT_SGPIO_MUL_MR0STOP_EN		((u32)0x00000001 << 2)		/*Bit[2], bit for MULTC STOP control when MULMR0 matches MULTC*/
#define BIT_SGPIO_MUL_MR0RST_EN			((u32)0x00000001 << 1)		/*Bit[1], bit for MULTC reset control when MULMR0 matches MULTC*/
#define BIT_SGPIO_MUL_MR0_IE			((u32)0x00000001)			/*Bit[0], bit for interrupt enable when MULMR0 matches MULTC*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULMR01GP0
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULMR1_GP0			((u32)0xffff0000)			/*Bit[31:16], bits for Multiple timer counter match register 1 
																	when TXDATA[0] is 0*/
#define BIT_SGPIO_MULMR0_GP0			((u32)0x0000ffff)			/*Bit[15:0], bits for Multiple timer counter match register 0 
																	when TXDATA[0] is 0*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULMR23GP0
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULMR3_GP0			((u32)0xffff0000)			/*Bit[31:16], bits for Multiple timer counter match register 3 
																	when TXDATA[0] is 0*/
#define BIT_SGPIO_MULMR2_GP0			((u32)0x0000ffff)			/*Bit[15:0], bits for Multiple timer counter match register 2 
																	when TXDATA[0] is 0*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULMR01GP1
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULMR1_GP1			((u32)0xffff0000)			/*Bit[31:16], bits for Multiple timer counter match register 1 
																	when TXDATA[0] is 1*/
#define BIT_SGPIO_MULMR0_GP1			((u32)0x0000ffff)			/*Bit[15:0], bits for Multiple timer counter match register 0 
																	when TXDATA[0] is 1*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULMR23GP1
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULMR3_GP1			((u32)0xffff0000)			/*Bit[31:16], bits for Multiple timer counter match register 3 
																	when TXDATA[0] is 1*/
#define BIT_SGPIO_MULMR2_GP1			((u32)0x0000ffff)			/*Bit[15:0], bits for Multiple timer counter match register 2 
																	when TXDATA[0] is 1*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MUL_MCNT
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MUL_MCNT				((u32)0x000000ff)			/*Bit[3:0], bits for mask of Multiple Match Counter Register*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULEMR 
 * @{
 *****************************************************************************/
#define BIT_SGPIO_BIOUT				((u32)0x00000001 << 16)		/*Bit[16], bit for send TX to bi-direction open-drain output*/
#define BIT_SGPIO_RXEMC_CTRL			((u32)0x00000fc0)			/*Bit[11:6], bits for output control mask of RX match event*/
#define BIT_SGPIO_RXEMC2				((u32)0x00000003 << 10)		/*Bit[11:10], bits for output control mask of RXMR2 match event*/
#define BIT_SGPIO_RXEMC1				((u32)0x00000003 << 8)		/*Bit[9:8], bits for output control mask of RXMR1 match event*/
#define BIT_SGPIO_RXEMC0				((u32)0x00000003 << 6)		/*Bit[7:6], bits for output control mask of RXMR0 match event*/
#define BIT_SGPIO_MULEMC3				((u32)0x00000003 << 4)		/*Bit[5:4], bits for output control mask of MULMR3 match event*/
#define BIT_SGPIO_MULEMC2				((u32)0x00000003 << 2)		/*Bit[3:2], bits for output control mask of MULMR2 match event*/
#define BIT_SGPIO_MULEMC1				((u32)0x00000003)			/*Bit[1:0], bits for output control mask of MULMR1 match event*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_DATA_CTRL
 * @{
 *****************************************************************************/
#define BIT_SGPIO_TXDATA_DIR			((u32)0x00000001 << 8)		/*Bit[8], bit for output direction set of the TXDATA*/
#define BIT_SGPIO_RXDATA_DIR			((u32)0x00000001 << 7)		/*Bit[7], bit for output direction set of the RXDATA*/
#define BIT_SGPIO_PATTERN_MATCH_IE		((u32)0x00000001 << 6)		/*Bit[6], bit for interrupt enable of pattern match*/
#define BIT_SGPIO_PATTERN_MATCH_MODE	((u32)0x00000001 << 5)		/*Bit[5], bit for mode enable of pattern match*/
#define BIT_SGPIO_RXDATASRC_SEL 		((u32)0x00000001 << 4)		/*Bit[4], bit for select of RX data source*/
#define BIT_SGPIO_MULLOAD_IE			((u32)0x00000001 << 3)		/*Bit[3], bit for interrupt enable when MULDATA_DP is loaded with MULDATA*/
#define BIT_SGPIO_RXLOAD_IE				((u32)0x00000001 << 2)		/*Bit[2], bit for interrupt enable when RXDATA_DP is loaded with RXDATA*/
#define BIT_SGPIO_MULSFT_IE				((u32)0x00000001 << 1)		/*Bit[1], bit for interrupt enable when have a shift clock for Multiple Data Register*/
#define BIT_SGPIO_RXSFT_IE				((u32)0x00000001)			/*Bit[0], bit for interrupt enable when have a shift clock for RX Data Register*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXDATA
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXDATA				((u32)0xffffffff)			/*Bit[31:0], bits for RX Data mask*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXDATA_DP
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXDATA_DP				((u32)0xffffffff)			/*Bit[31:0], bits for RX Data Duplicate mask*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXPOSR
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXPOSRST				((u32)0x00001f00)			/*Bit[12:8], bits for reload value mask of RXPOSTC after 
																	RXPOSTC reaches 0x0*/
#define BIT_SGPIO_RXPOSTC				((u32)0x0000001f)			/*Bit[4:0], bits for RX Position counter mask*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_RXDATAMASK
 * @{
 *****************************************************************************/
#define BIT_SGPIO_RXDATAMASK			((u32)0xffffffff)			/*Bit[31:0], bits for mask of pattern match function mask*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULDATA
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULDATA				((u32)0xffffffff)			/*Bit[31:0], bits for Multiple Data mask*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULDATA_DP
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULDATA_DP			((u32)0xffffffff)			/*Bit[31:0], bits for Multiple Data Duplicate mask*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULPOSR
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULPOSRST				((u32)0x00001f00)			/*Bit[12:8], bits for reload value mask of MULPOSTC after 
																	MULPOSTC reaches 0x0*/
#define BIT_SGPIO_MULPOSTC				((u32)0x0000001f)			/*Bit[4:0], bits for Multiple Position counter mask*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_MULFDATA 
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULFDATA				((u32)0xffffffff)			/*Bit[31:0], bits for match value mask for FIFO mode*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_ISR 
 * @{
 *****************************************************************************/
#define BIT_SGPIO_MULMCNT_IS			((u32)0x00000001 << 10)		/*Bit[10], bit for interrupt raw status when MUL_MCNT counts to zero*/
#define BIT_SGPIO_PATTERN_MATCH_IS			((u32)0x00000001 << 9)		/*Bit[9], bit for interrupt raw status for matching pattern*/
#define BIT_SGPIO_MULLOAD_IS			((u32)0x00000001 << 8)		/*Bit[8], bit for interrupt raw status for loading multiple data*/
#define BIT_SGPIO_RXLOAD_IS				((u32)0x00000001 << 7)		/*Bit[7], bit for interrupt raw status for loading rx data*/
#define BIT_SGPIO_MULSFT_IS				((u32)0x00000001 << 6)		/*Bit[6], bit for interrupt raw status for multiple shift clock*/
#define BIT_SGPIO_RXSFT_IS				((u32)0x00000001 << 5)		/*Bit[5], bit for interrupt raw status for rx shift clock*/
#define BIT_SGPIO_CAPI_IS				((u32)0x00000001 << 4)		/*Bit[4], bit for interrupt raw status for capture load event*/
#define BIT_SGPIO_MUL_MR0I_IS			((u32)0x00000001 << 3)		/*Bit[3], bit for interrupt raw status for multiple MR0 match event*/
#define BIT_SGPIO_RX_MR2I_IS			((u32)0x00000001 << 2)		/*Bit[2], bit for interrupt raw status for RX MR2 match event*/
#define BIT_SGPIO_RX_MR1I_IS			((u32)0x00000001 << 1)		/*Bit[1], bit for interrupt raw status for RX MR1 match event*/
#define BIT_SGPIO_RX_MR0I_IS			((u32)0x00000001)			/*Bit[0], bit for interrupt raw status for RX MR0 match event*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_GF
 * @{
 *****************************************************************************/
#define BIT_SGPIO_GF					((u32)0x000000ff)			/*Bit[7:0], bits for set digital glitch filtering*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_OUTPUT
 * @{
 *****************************************************************************/
#define BIT_SGPIO_OUTPUT				((u32)0x00000001)			/*Bit[0], bit for drive the state of output*/
#define BIT_SGPIO_OUTPUT_LOW			((u32)0x00000000)			/*Bit[0], bit for drive the state low of output*/
#define BIT_SGPIO_OUTPUT_HIGH			((u32)0x00000001)			/*Bit[0], bit for drive the state high of output*/
/** @} */

/**************************************************************************//**
 * @defgroup SGPIO_IMR 
 * @{
 *****************************************************************************/
#define BIT_SGPIO_INTR_MASK				((u32)0x00000fff)			/*Bit[11:0], bits for all interrupt mask*/
#define BIT_SGPIO_MULMCNT_IM			((u32)0x00000001 << 10)		/*Bit[10], bit for interrupt mask when MUL_MCNT counts to zero*/
#define BIT_SGPIO_PATTERN_MATCH_IM			((u32)0x00000001 << 9)		/*Bit[9], bit for interrupt mask for matching pattern*/
#define BIT_SGPIO_MULLOAD_IM			((u32)0x00000001 << 8)		/*Bit[8], bit for interrupt mask for loading multiple data*/
#define BIT_SGPIO_RXLOAD_IM				((u32)0x00000001 << 7)		/*Bit[7], bit for interrupt mask for loading rx data*/
#define BIT_SGPIO_MULSFT_IM				((u32)0x00000001 << 6)		/*Bit[6], bit for interrupt mask for multiple shift clock*/
#define BIT_SGPIO_RXSFT_IM				((u32)0x00000001 << 5)		/*Bit[5], bit for interrupt mask for rx shift clock*/
#define BIT_SGPIO_CAPI_IM				((u32)0x00000001 << 4)		/*Bit[4], bit for interrupt mask for capture load event*/
#define BIT_SGPIO_MUL_MR0I_IM			((u32)0x00000001 << 3)		/*Bit[3], bit for interrupt mask for multiple MR0 match event*/
#define BIT_SGPIO_RX_MR2I_IM			((u32)0x00000001 << 2)		/*Bit[2], bit for interrupt mask for RX MR2 match event*/
#define BIT_SGPIO_RX_MR1I_IM			((u32)0x00000001 << 1)		/*Bit[1], bit for interrupt mask for RX MR1 match event*/
#define BIT_SGPIO_RX_MR0I_IM			((u32)0x00000001)			/*Bit[0], bit for interrupt mask for RX MR0 match event*/
/** @} */
/** @} */

/**
  * @}
  */ 

/**
  * @}
  */

#endif //_RTL8721D_SGPIO_H_

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/

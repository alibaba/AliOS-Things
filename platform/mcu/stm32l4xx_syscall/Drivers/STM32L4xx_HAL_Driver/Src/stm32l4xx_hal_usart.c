/**
  ******************************************************************************
  * @file    stm32l4xx_hal_usart.c
  * @author  MCD Application Team
  * @brief   USART HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Universal Synchronous/Asynchronous Receiver Transmitter
  *          Peripheral (USART).
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State and Error functions
  *
  @verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
    [..]
      The USART HAL driver can be used as follows:

      (#) Declare a USART_HandleTypeDef handle structure (eg. USART_HandleTypeDef husart).
      (#) Initialize the USART low level resources by implementing the HAL_USART_MspInit() API:
          (++) Enable the USARTx interface clock.
          (++) USART pins configuration:
            (+++) Enable the clock for the USART GPIOs.
            (+++) Configure these USART pins as alternate function pull-up.
          (++) NVIC configuration if you need to use interrupt process (HAL_USART_Transmit_IT(),
                HAL_USART_Receive_IT() and HAL_USART_TransmitReceive_IT() APIs):
            (+++) Configure the USARTx interrupt priority.
            (+++) Enable the NVIC USART IRQ handle.
            (++) USART interrupts handling:
              -@@-   The specific USART interrupts (Transmission complete interrupt,
                  RXNE interrupt and Error Interrupts) will be managed using the macros
                  __HAL_USART_ENABLE_IT() and __HAL_USART_DISABLE_IT() inside the transmit and receive process.
          (++) DMA Configuration if you need to use DMA process (HAL_USART_Transmit_DMA()
               HAL_USART_Receive_DMA() and HAL_USART_TransmitReceive_DMA() APIs):
            (+++) Declare a DMA handle structure for the Tx/Rx channel.
            (+++) Enable the DMAx interface clock.
            (+++) Configure the declared DMA handle structure with the required Tx/Rx parameters.
            (+++) Configure the DMA Tx/Rx channel.
            (+++) Associate the initialized DMA handle to the USART DMA Tx/Rx handle.
            (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA Tx/Rx channel.

      (#) Program the Baud Rate, Word Length, Stop Bit, Parity, and Mode
          (Receiver/Transmitter) in the husart handle Init structure.

      (#) Initialize the USART registers by calling the HAL_USART_Init() API:
          (++) This API configures also the low level Hardware GPIO, CLOCK, CORTEX...etc)
               by calling the customized HAL_USART_MspInit(&husart) API.

    [..]
     (@) To configure and enable/disable the USART to wake up the MCU from stop mode, resort to UART API's
        HAL_UARTEx_StopModeWakeUpSourceConfig(), HAL_UARTEx_EnableStopMode() and                 
        HAL_UARTEx_DisableStopMode() in casting the USART handle to UART type UART_HandleTypeDef.                

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
#include "stm32l4xx_hal.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @defgroup USART USART
  * @brief HAL USART Synchronous module driver
  * @{
  */

#ifdef HAL_USART_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup USART_Private_Constants USART Private Constants
  * @{
  */
#define USART_DUMMY_DATA          ((uint16_t) 0xFFFF)           /*!< USART transmitted dummy data                     */
#define USART_TEACK_REACK_TIMEOUT ((uint32_t) 1000)             /*!< USART TX or RX enable acknowledge time-out value */

#if defined(USART_CR1_FIFOEN)
#define USART_CR1_FIELDS          ((uint32_t)(USART_CR1_M |  USART_CR1_PCE | USART_CR1_PS    | \
                                              USART_CR1_TE | USART_CR1_RE  | USART_CR1_OVER8 | \
                                              USART_CR1_FIFOEN ))                                  /*!< USART CR1 fields of parameters set by USART_SetConfig API */

#define USART_CR2_FIELDS          ((uint32_t)(USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_CLKEN | \
                                              USART_CR2_LBCL | USART_CR2_STOP | USART_CR2_SLVEN | \
                                              USART_CR2_DIS_NSS))                                  /*!< USART CR2 fields of parameters set by USART_SetConfig API */

#define USART_CR3_FIELDS          ((uint32_t)(USART_CR3_TXFTCFG | USART_CR3_RXFTCFG ))             /*!< USART or USART CR3 fields of parameters set by USART_SetConfig API */
#else
#define USART_CR1_FIELDS          ((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | \
                                              USART_CR1_TE | USART_CR1_RE  | USART_CR1_OVER8))    /*!< USART CR1 fields of parameters set by USART_SetConfig API */
#define USART_CR2_FIELDS          ((uint32_t)(USART_CR2_CPHA | USART_CR2_CPOL | \
                                              USART_CR2_CLKEN | USART_CR2_LBCL | USART_CR2_STOP)) /*!< USART CR2 fields of parameters set by USART_SetConfig API */
#endif

#define USART_BRR_MIN    0x10U        /* USART BRR minimum authorized value */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup USART_Private_Functions
  * @{
  */
static void USART_EndTransfer(USART_HandleTypeDef *husart);
static void USART_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void USART_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void USART_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void USART_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void USART_DMAError(DMA_HandleTypeDef *hdma);
static void USART_DMAAbortOnError(DMA_HandleTypeDef *hdma);
static void USART_DMATxAbortCallback(DMA_HandleTypeDef *hdma);
static void USART_DMARxAbortCallback(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef USART_WaitOnFlagUntilTimeout(USART_HandleTypeDef *husart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout);
static HAL_StatusTypeDef USART_SetConfig(USART_HandleTypeDef *husart);
static HAL_StatusTypeDef USART_CheckIdleState(USART_HandleTypeDef *husart);
static void USART_TxISR_8BIT(USART_HandleTypeDef *husart);
static void USART_TxISR_16BIT(USART_HandleTypeDef *husart);
#if defined(USART_CR1_FIFOEN)
static void USART_TxISR_8BIT_FIFOEN(USART_HandleTypeDef *husart);
static void USART_TxISR_16BIT_FIFOEN(USART_HandleTypeDef *husart);
#endif
static void USART_EndTransmit_IT(USART_HandleTypeDef *husart);
static void USART_RxISR_8BIT(USART_HandleTypeDef *husart);
static void USART_RxISR_16BIT(USART_HandleTypeDef *husart);
#if defined(USART_CR1_FIFOEN)
static void USART_RxISR_8BIT_FIFOEN(USART_HandleTypeDef *husart);
static void USART_RxISR_16BIT_FIFOEN(USART_HandleTypeDef *husart);
#endif


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup USART_Exported_Functions USART Exported Functions
  * @{
  */

/** @defgroup USART_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to initialize the USART
    in asynchronous and in synchronous modes.
      (+) For the asynchronous mode only these parameters can be configured:
        (++) Baud Rate
        (++) Word Length
        (++) Stop Bit
        (++) Parity: If the parity is enabled, then the MSB bit of the data written
             in the data register is transmitted but is changed by the parity bit.
        (++) USART polarity
        (++) USART phase
        (++) USART LastBit
        (++) Receiver/transmitter modes

    [..]
    The HAL_USART_Init() function follows the USART  synchronous configuration
    procedure (details for the procedure are available in reference manual).

@endverbatim

  Depending on the frame length defined by the M1 and M0 bits (7-bit, 
  8-bit or 9-bit), the possible USART formats are listed in the 
  following table.
  
    Table 1. USART frame format.
    +-----------------------------------------------------------------------+
    |  M1 bit |  M0 bit |  PCE bit  |            USART frame                |
    |---------|---------|-----------|---------------------------------------|
    |    0    |    0    |    0      |    | SB |    8 bit data   | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    0    |    0    |    1      |    | SB | 7 bit data | PB | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    0    |    1    |    0      |    | SB |    9 bit data   | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    0    |    1    |    1      |    | SB | 8 bit data | PB | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    1    |    0    |    0      |    | SB |    7 bit data   | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    1    |    0    |    1      |    | SB | 6 bit data | PB | STB |     |
    +-----------------------------------------------------------------------+

  * @{
  */

/**
  * @brief  Initialize the USART mode according to the specified
  *         parameters in the USART_InitTypeDef and initialize the associated handle.
  * @param  husart USART handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Init(USART_HandleTypeDef *husart)
{
  /* Check the USART handle allocation */
  if(husart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_USART_INSTANCE(husart->Instance));

  if(husart->State == HAL_USART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    husart->Lock = HAL_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK */
    HAL_USART_MspInit(husart);
  }

  husart->State = HAL_USART_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_USART_DISABLE(husart);

  /* Set the Usart Communication parameters */
  if (USART_SetConfig(husart) == HAL_ERROR)
  {
    return HAL_ERROR;
  }

  /* In Synchronous mode, the following bits must be kept cleared:
  - LINEN bit in the USART_CR2 register
  - HDSEL, SCEN and IREN bits in the USART_CR3 register.*/
  husart->Instance->CR2 &= ~USART_CR2_LINEN;
  husart->Instance->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);

  /* Enable the Peripheral */
  __HAL_USART_ENABLE(husart);

  /* TEACK and/or REACK to check before moving husart->State to Ready */
  return (USART_CheckIdleState(husart));
}

/**
  * @brief DeInitialize the USART peripheral.
  * @param  husart USART handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_DeInit(USART_HandleTypeDef *husart)
{
   /* Check the USART handle allocation */
  if(husart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_USART_INSTANCE(husart->Instance));

  husart->State = HAL_USART_STATE_BUSY;

  husart->Instance->CR1 = 0x0;
  husart->Instance->CR2 = 0x0;
  husart->Instance->CR3 = 0x0;

  /* DeInit the low level hardware */
  HAL_USART_MspDeInit(husart);

  husart->ErrorCode = HAL_USART_ERROR_NONE;
  husart->State = HAL_USART_STATE_RESET;

  /* Process Unlock */
  __HAL_UNLOCK(husart);

  return HAL_OK;
}

/**
  * @brief Initialize the USART MSP.
  * @param husart USART handle.
  * @retval None
  */
 __weak void HAL_USART_MspInit(USART_HandleTypeDef *husart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);
    
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_USART_MspInit can be implemented in the user file
   */
}

/**
  * @brief DeInitialize the USART MSP.
  * @param husart USART handle.
  * @retval None
  */
 __weak void HAL_USART_MspDeInit(USART_HandleTypeDef *husart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);
    
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_USART_MspDeInit can be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup USART_Exported_Functions_Group2 IO operation functions
  *  @brief   USART Transmit and Receive functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..] This subsection provides a set of functions allowing to manage the USART synchronous
    data transfers.

    [..] The USART supports master mode only: it cannot receive or send data related to an input
         clock (SCLK is always an output).

    (#) There are two modes of transfer:
       (++) Blocking mode: The communication is performed in polling mode.
            The HAL status of all data processing is returned by the same function
            after finishing transfer.
       (++) No-Blocking mode: The communication is performed using Interrupts
           or DMA, These API's return the HAL status.
           The end of the data processing will be indicated through the
           dedicated USART IRQ when using Interrupt mode or the DMA IRQ when
           using DMA mode.
           The HAL_USART_TxCpltCallback(), HAL_USART_RxCpltCallback() and HAL_USART_TxRxCpltCallback() user callbacks
           will be executed respectively at the end of the transmit or Receive process
           The HAL_USART_ErrorCallback()user callback will be executed when a communication error is detected

    (#) Blocking mode API's are :
        (++) HAL_USART_Transmit()in simplex mode
        (++) HAL_USART_Receive() in full duplex receive only
        (++) HAL_USART_TransmitReceive() in full duplex mode

    (#) Non-Blocking mode API's with Interrupt are :
        (++) HAL_USART_Transmit_IT()in simplex mode
        (++) HAL_USART_Receive_IT() in full duplex receive only
        (++) HAL_USART_TransmitReceive_IT()in full duplex mode
        (++) HAL_USART_IRQHandler()

    (#) No-Blocking mode API's  with DMA are :
        (++) HAL_USART_Transmit_DMA()in simplex mode
        (++) HAL_USART_Receive_DMA() in full duplex receive only
        (++) HAL_USART_TransmitReceive_DMA() in full duplex mode
        (++) HAL_USART_DMAPause()
        (++) HAL_USART_DMAResume()
        (++) HAL_USART_DMAStop()

    (#) A set of Transfer Complete Callbacks are provided in Non_Blocking mode:
        (++) HAL_USART_TxCpltCallback()
        (++) HAL_USART_RxCpltCallback()
        (++) HAL_USART_TxHalfCpltCallback()
        (++) HAL_USART_RxHalfCpltCallback()
        (++) HAL_USART_ErrorCallback()
        (++) HAL_USART_TxRxCpltCallback()

    (#) Non-Blocking mode transfers could be aborted using Abort API's :
        (+) HAL_USART_Abort()
        (+) HAL_USART_Abort_IT()

    (#) For Abort services based on interrupts (HAL_USART_Abort_IT), a Abort Complete Callbacks is provided:
        (+) HAL_USART_AbortCpltCallback()

    (#) In Non-Blocking mode transfers, possible errors are split into 2 categories.
        Errors are handled as follows :
       (+) Error is considered as Recoverable and non blocking : Transfer could go till end, but error severity is 
           to be evaluated by user : this concerns Frame Error, Parity Error or Noise Error in Interrupt mode reception .
           Received character is then retrieved and stored in Rx buffer, Error code is set to allow user to identify error type,
           and HAL_USART_ErrorCallback() user callback is executed. Transfer is kept ongoing on USART side.
           If user wants to abort it, Abort services should be called by user.
       (+) Error is considered as Blocking : Transfer could not be completed properly and is aborted.
           This concerns Overrun Error In Interrupt mode reception and all errors in DMA mode.
           Error code is set to allow user to identify error type, and HAL_USART_ErrorCallback() user callback is executed.

@endverbatim
  * @{
  */

/**
  * @brief  Simplex send an amount of data in blocking mode.
  * @param  husart USART handle.
  * @param  pTxData Pointer to data buffer.
  * @param  Size Amount of data to be sent.
  * @param  Timeout Timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Transmit(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size, uint32_t Timeout)
{
  uint16_t* tmp;
  uint32_t tickstart = 0;

  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }
      
    /* Process Locked */
    __HAL_LOCK(husart);
    
    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX;
    
    /* Init tickstart for timeout managment*/
    tickstart = HAL_GetTick();
    
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;
    
    /* Check the remaining data to be sent */
    while(husart->TxXferCount > 0)
    {
      if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      if((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
      {
        tmp = (uint16_t*) pTxData;
        husart->Instance->TDR = (*tmp & (uint16_t)0x01FF);
        pTxData += 2;
      }
      else
      {
        husart->Instance->TDR = (*pTxData++ & (uint8_t)0xFF);
      }
      
      husart->TxXferCount--;
    }
    
    if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TC, RESET, tickstart, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
    
    /* Clear Transmission Complete Flag */
    __HAL_USART_CLEAR_FLAG(husart, USART_CLEAR_TCF);
    
    /* Clear overrun flag and discard the received data */
    __HAL_USART_CLEAR_OREFLAG(husart);
    __HAL_USART_SEND_REQ(husart, USART_RXDATA_FLUSH_REQUEST);
    __HAL_USART_SEND_REQ(husart, USART_TXDATA_FLUSH_REQUEST);
    
    /* At end of Tx process, restore husart->State to Ready */
    husart->State = HAL_USART_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(husart);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in blocking mode.
  * @note To receive synchronous data, dummy data are simultaneously transmitted.
  * @param husart USART handle.
  * @param pRxData Pointer to data buffer.
  * @param Size Amount of data to be received.
  * @param Timeout Timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Receive(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
  uint16_t* tmp;
  uint16_t uhMask;
  uint32_t tickstart = 0;
  
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pRxData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }
    
    /* Process Locked */
    __HAL_LOCK(husart);
    
    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;
    
    /* Init tickstart for timeout managment*/
    tickstart = HAL_GetTick();
    
    husart->RxXferSize = Size;
    husart->RxXferCount = Size;
    
    /* Computation of USART mask to apply to RDR register */
    USART_MASK_COMPUTATION(husart);
    uhMask = husart->Mask;
    
    /* as long as data have to be received */
    while(husart->RxXferCount > 0)
    {
#if defined(USART_CR2_SLVEN)
      if (husart->SlaveMode == USART_SLAVEMODE_DISABLE)
#endif
      {
        /* Wait until TXE flag is set to send dummy byte in order to generate the
        * clock for the slave to send data.
        * Whatever the frame length (7, 8 or 9-bit long), the same dummy value
        * can be written for all the cases. */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        husart->Instance->TDR = (USART_DUMMY_DATA & (uint16_t)0x0FF);
      }
      
      /* Wait for RXNE Flag */
      if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      
      if((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
      {
        tmp = (uint16_t*) pRxData ;
        *tmp = (uint16_t)(husart->Instance->RDR & uhMask);
        pRxData +=2;
      }
      else
      {
        *pRxData++ = (uint8_t)(husart->Instance->RDR & (uint8_t)uhMask);
      }
      
      husart->RxXferCount--;
      
    }
    
#if defined(USART_CR2_SLVEN)
    /* Clear SPI slave underrun flag and discard transmit data */
    if (husart->SlaveMode == USART_SLAVEMODE_ENABLE)
    {
      __HAL_USART_CLEAR_UDRFLAG(husart);
      __HAL_USART_SEND_REQ(husart, USART_TXDATA_FLUSH_REQUEST);
    }
#endif
    
    /* At end of Rx process, restore husart->State to Ready */
    husart->State = HAL_USART_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(husart);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Full-Duplex Send and Receive an amount of data in blocking mode.
  * @param  husart USART handle.
  * @param  pTxData pointer to TX data buffer.
  * @param  pRxData pointer to RX data buffer.
  * @param  Size amount of data to be sent (same amount to be received).
  * @param  Timeout Timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_TransmitReceive(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
  uint16_t* tmp;
  uint16_t uhMask;
  uint32_t tickstart = 0;

  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (pRxData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }
    
    /* Process Locked */
    __HAL_LOCK(husart);

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

    /* Init tickstart for timeout managment*/
    tickstart = HAL_GetTick();

    husart->RxXferSize = Size;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;
    husart->RxXferCount = Size;

    /* Computation of USART mask to apply to RDR register */
    USART_MASK_COMPUTATION(husart);
    uhMask = husart->Mask;

#if defined(USART_CR2_SLVEN)
    if ((husart->SlaveMode == USART_SLAVEMODE_ENABLE) || (husart->TxXferCount == 0x01U))
#else
    if (husart->TxXferCount == 0x01U)
#endif
    {
      /* Wait until TXE flag is set to send data */
      if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      if((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
      {
        tmp = (uint16_t*) pTxData;
        husart->Instance->TDR = (*tmp & uhMask);
        pTxData += 2U;
      }
      else
    {
        husart->Instance->TDR = (*pTxData++ & (uint8_t)uhMask);
      }

      husart->TxXferCount--;
    }
    
    /* Check the remain data to be sent */
    while ((husart->TxXferCount > 0) || (husart->RxXferCount > 0))
    {
      if (husart->TxXferCount > 0)
      {
        /* Wait until TXE flag is set to send data */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        if((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
        {
          tmp = (uint16_t*) pTxData;
          husart->Instance->TDR = (*tmp & uhMask);
          pTxData += 2;
        }
        else
        {
          husart->Instance->TDR = (*pTxData++ & (uint8_t)uhMask);
        }
        
        husart->TxXferCount--;
      }
      
      if (husart->RxXferCount > 0)
      {
        /* Wait for RXNE Flag */
        if(USART_WaitOnFlagUntilTimeout(husart, USART_FLAG_RXNE, RESET, tickstart, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        
        if((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
        {
          tmp = (uint16_t*) pRxData ;
          *tmp = (uint16_t)(husart->Instance->RDR & uhMask);
          pRxData +=2;
        }
        else
        {
          *pRxData++ = (uint8_t)(husart->Instance->RDR & (uint8_t)uhMask);
        }
        
        husart->RxXferCount--;
      }
    }

    /* At end of TxRx process, restore husart->State to Ready */
    husart->State = HAL_USART_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Send an amount of data in interrupt mode.
  * @param  husart USART handle.
  * @param  pTxData pointer to data buffer.
  * @param  Size amount of data to be sent.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Transmit_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size)
{
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL ) || (Size == 0))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pTxBuffPtr  = pTxData;
    husart->TxXferSize  = Size;
    husart->TxXferCount = Size;
    husart->TxISR       = NULL;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State     = HAL_USART_STATE_BUSY_TX;

    /* The USART Error Interrupts: (Frame error, noise error, overrun error)
    are not managed by the USART Transmit Process to avoid the overrun interrupt
    when the usart mode is configured for transmit and receive "USART_MODE_TX_RX"
    to benefit for the frame error and noise interrupts the usart mode should be
    configured only for transmit "USART_MODE_TX" */

#if defined(USART_CR1_FIFOEN)
    /* Configure Tx interrupt processing */
    if (husart->FifoMode == USART_FIFOMODE_ENABLE)
    {      
      /* Set the Tx ISR function pointer according to the data word length */
      if ((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
      {
        husart->TxISR = USART_TxISR_16BIT_FIFOEN;
      }
      else
      {
        husart->TxISR = USART_TxISR_8BIT_FIFOEN;
      } 
      
      /* Process Unlocked */
      __HAL_UNLOCK(husart);
      
      /* Enable the TX FIFO threshold interrupt */
      __HAL_USART_ENABLE_IT(husart, USART_IT_TXFT);
    }
    else
#endif
    {
      /* Set the Tx ISR function pointer according to the data word length */
      if ((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
      {
        husart->TxISR = USART_TxISR_16BIT;
      }
      else
      {
        husart->TxISR = USART_TxISR_8BIT;
      }  
      
      /* Process Unlocked */
      __HAL_UNLOCK(husart);
      
      /* Enable the USART Transmit Data Register Empty Interrupt */
      __HAL_USART_ENABLE_IT(husart, USART_IT_TXE);
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in blocking mode.
  * @note  To receive synchronous data, dummy data are simultaneously transmitted.
  * @param  husart USART handle.
  * @param  pRxData pointer to data buffer.
  * @param  Size amount of data to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Receive_IT(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size)
{
#if defined(USART_CR1_FIFOEN)
  uint8_t  nb_dummy_data;
#endif
  
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pRxData == NULL ) || (Size == 0))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr  = pRxData;
    husart->RxXferSize  = Size;
    husart->RxXferCount = Size;
    husart->RxISR       = NULL;

    USART_MASK_COMPUTATION(husart);

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

    /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(husart->Instance->CR3, USART_CR3_EIE);

#if defined(USART_CR1_FIFOEN)
    /* Configure Rx interrupt processing */
    if ((husart->FifoMode == USART_FIFOMODE_ENABLE) && (Size >= husart->NbRxDataToProcess))
    {
      /* Set the Rx ISR function pointer according to the data word length */
      if ((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
      {
        husart->RxISR = USART_RxISR_16BIT_FIFOEN;
      }
      else
      {
        husart->RxISR = USART_RxISR_8BIT_FIFOEN;
      }
      
      /* Process Unlocked */
      __HAL_UNLOCK(husart);
      
      /* Enable the USART Parity Error interrupt and RX FIFO Threshold interrupt */
      SET_BIT(husart->Instance->CR1, USART_CR1_PEIE);
      SET_BIT(husart->Instance->CR3, USART_CR3_RXFTIE);
    }
    else
#endif
    {
      /* Set the Rx ISR function pointer according to the data word length */
      if ((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
      {
        husart->RxISR = USART_RxISR_16BIT;
      }
      else
      {
        husart->RxISR = USART_RxISR_8BIT;
      }
      
      /* Process Unlocked */
      __HAL_UNLOCK(husart);
      
      /* Enable the USART Parity Error and Data Register not empty Interrupts */
#if defined(USART_CR1_FIFOEN)
      SET_BIT(husart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);
#else
      SET_BIT(husart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
#endif
    }

#if defined(USART_CR2_SLVEN)
    if (husart->SlaveMode == USART_SLAVEMODE_DISABLE)
#endif
    {
      /* Send dummy data in order to generate the clock for the Slave to send the next data.
         When FIFO mode is disabled only one data must be transferred.
         When FIFO mode is enabled data must be transmitted until the RX FIFO reaches its threshold.
      */
#if defined(USART_CR1_FIFOEN)
      if ((husart->FifoMode == USART_FIFOMODE_ENABLE) && (Size >= husart->NbRxDataToProcess))
      {
        for (nb_dummy_data = husart->NbRxDataToProcess ; nb_dummy_data > 0 ; nb_dummy_data--)
        {
          husart->Instance->TDR = (USART_DUMMY_DATA & (uint16_t)0x00FF);
        }
      }
      else
#endif
      {
        husart->Instance->TDR = (USART_DUMMY_DATA & (uint16_t)0x00FF);
      }
    }
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Full-Duplex Send and Receive an amount of data in interrupt mode.
  * @param  husart USART handle.
  * @param  pTxData pointer to TX data buffer.
  * @param  pRxData pointer to RX data buffer.                         
  * @param  Size amount of data to be sent (same amount to be received).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_TransmitReceive_IT(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData,  uint16_t Size)
{

  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (pRxData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->RxXferCount = Size;
    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;

    /* Computation of USART mask to apply to RDR register */
    USART_MASK_COMPUTATION(husart);

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX_RX;
    
#if defined(USART_CR1_FIFOEN)
    /* Configure TxRx interrupt processing */
    if ((husart->FifoMode == USART_FIFOMODE_ENABLE) && (Size >= husart->NbRxDataToProcess))
    {
      /* Set the Rx ISR function pointer according to the data word length */
      if ((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
      {
        husart->TxISR = USART_TxISR_16BIT_FIFOEN;
        husart->RxISR = USART_RxISR_16BIT_FIFOEN;
      }
      else
      {
        husart->TxISR = USART_TxISR_8BIT_FIFOEN;
        husart->RxISR = USART_RxISR_8BIT_FIFOEN;
      }
      
      /* Process Locked */
      __HAL_UNLOCK(husart);
      
      /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
      SET_BIT(husart->Instance->CR3, USART_CR3_EIE);
      
      /* Enable the USART Parity Error interrupt  */
      SET_BIT(husart->Instance->CR1, USART_CR1_PEIE);

      /* Enable the TX and  RX FIFO Threshold interrupts */
      SET_BIT(husart->Instance->CR3, (USART_CR3_TXFTIE | USART_CR3_RXFTIE));
    }
    else
#endif
    {
      if ((husart->Init.WordLength == USART_WORDLENGTH_9B) && (husart->Init.Parity == USART_PARITY_NONE))
      {
        husart->TxISR = USART_TxISR_16BIT;
        husart->RxISR = USART_RxISR_16BIT;
      }
      else
      {
        husart->TxISR = USART_TxISR_8BIT;
        husart->RxISR = USART_RxISR_8BIT;
      }

      /* Process Locked */
      __HAL_UNLOCK(husart);
      
      /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
      SET_BIT(husart->Instance->CR3, USART_CR3_EIE);
      
      /* Enable the USART Parity Error and USART Data Register not empty Interrupts */
#if defined(USART_CR1_FIFOEN)
      SET_BIT(husart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE_RXFNEIE);
#else
    SET_BIT(husart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
#endif
      
      /* Enable the USART Transmit Data Register Empty Interrupt */
#if defined(USART_CR1_FIFOEN)
      SET_BIT(husart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);
#else
      SET_BIT(husart->Instance->CR1, USART_CR1_TXEIE);
#endif
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Send an amount of data in DMA mode.
  * @param  husart USART handle.
  * @param  pTxData pointer to data buffer.
  * @param  Size amount of data to be sent.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Transmit_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint16_t Size)
{
  uint32_t *tmp;

  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL ) || (Size == 0))
    {
      return HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;
    husart->TxXferCount = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX;

    /* Set the USART DMA transfer complete callback */
    husart->hdmatx->XferCpltCallback = USART_DMATransmitCplt;

    /* Set the USART DMA Half transfer complete callback */
    husart->hdmatx->XferHalfCpltCallback = USART_DMATxHalfCplt;

    /* Set the DMA error callback */
    husart->hdmatx->XferErrorCallback = USART_DMAError;

    /* Enable the USART transmit DMA channel */
    tmp = (uint32_t*)&pTxData;
    HAL_DMA_Start_IT(husart->hdmatx, *(uint32_t*)tmp, (uint32_t)&husart->Instance->TDR, Size);

    /* Clear the TC flag in the ICR register */
    __HAL_USART_CLEAR_FLAG(husart, USART_CLEAR_TCF);

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the USART CR3 register */
    SET_BIT(husart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in DMA mode.
  * @param  husart USART handle.
  * @param  pRxData pointer to data buffer.
  * @param  Size amount of data to be received.
  * @note   When the USART parity is enabled (PCE = 1), the received data contain
  *         the parity bit (MSB position).
  * @note The USART DMA transmit channel must be configured in order to generate the clock for the slave.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_Receive_DMA(USART_HandleTypeDef *husart, uint8_t *pRxData, uint16_t Size)
{
  uint32_t *tmp;

  /* Check that a Rx process is not already ongoing */
  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pRxData == NULL ) || (Size == 0))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->pTxBuffPtr = pRxData;
    husart->TxXferSize = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_RX;

    /* Set the USART DMA Rx transfer complete callback */
    husart->hdmarx->XferCpltCallback = USART_DMAReceiveCplt;

    /* Set the USART DMA Half transfer complete callback */
    husart->hdmarx->XferHalfCpltCallback = USART_DMARxHalfCplt;

    /* Set the USART DMA Rx transfer error callback */
    husart->hdmarx->XferErrorCallback = USART_DMAError;

    /* Enable the USART receive DMA channel */
    tmp = (uint32_t*)&pRxData;
    HAL_DMA_Start_IT(husart->hdmarx, (uint32_t)&husart->Instance->RDR, *(uint32_t*)tmp, Size);

#if defined(USART_CR2_SLVEN)
    if (husart->SlaveMode == USART_SLAVEMODE_DISABLE)
    {
      /* Enable the USART transmit DMA channel: the transmit channel is used in order
      to generate in the non-blocking mode the clock to the slave device,
      this mode isn't a simplex receive mode but a full-duplex receive mode */
      tmp = (uint32_t*)&pRxData;
      /* Set the USART DMA Tx Complete and Error callback to Null */
      husart->hdmatx->XferErrorCallback = NULL;
      husart->hdmatx->XferHalfCpltCallback = NULL;
      husart->hdmatx->XferCpltCallback = NULL;
      HAL_DMA_Start_IT(husart->hdmatx, *(uint32_t*)tmp, (uint32_t)&husart->Instance->TDR, Size);
    }
#endif
    
    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    /* Enable the USART Parity Error Interrupt */
    SET_BIT(husart->Instance->CR1, USART_CR1_PEIE);

    /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(husart->Instance->CR3, USART_CR3_EIE);

    /* Enable the DMA transfer for the receiver request by setting the DMAR bit
       in the USART CR3 register */
    SET_BIT(husart->Instance->CR3, USART_CR3_DMAR);

    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the USART CR3 register */
    SET_BIT(husart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Full-Duplex Transmit Receive an amount of data in non-blocking mode.
  * @param  husart USART handle.
  * @param  pTxData pointer to TX data buffer.
  * @param  pRxData pointer to RX data buffer.
  * @param  Size amount of data to be received/sent.
  * @note   When the USART parity is enabled (PCE = 1) the data received contain the parity bit.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_TransmitReceive_DMA(USART_HandleTypeDef *husart, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  uint32_t *tmp;

  if(husart->State == HAL_USART_STATE_READY)
  {
    if((pTxData == NULL) || (pRxData == NULL) || (Size == 0))
    {
      return HAL_ERROR;
    }
    /* Process Locked */
    __HAL_LOCK(husart);

    husart->pRxBuffPtr = pRxData;
    husart->RxXferSize = Size;
    husart->pTxBuffPtr = pTxData;
    husart->TxXferSize = Size;

    husart->ErrorCode = HAL_USART_ERROR_NONE;
    husart->State = HAL_USART_STATE_BUSY_TX_RX;

    /* Set the USART DMA Rx transfer complete callback */
    husart->hdmarx->XferCpltCallback = USART_DMAReceiveCplt;

    /* Set the USART DMA Half transfer complete callback */
    husart->hdmarx->XferHalfCpltCallback = USART_DMARxHalfCplt;

    /* Set the USART DMA Tx transfer complete callback */
    husart->hdmatx->XferCpltCallback = USART_DMATransmitCplt;

    /* Set the USART DMA Half transfer complete callback */
    husart->hdmatx->XferHalfCpltCallback = USART_DMATxHalfCplt;

    /* Set the USART DMA Tx transfer error callback */
    husart->hdmatx->XferErrorCallback = USART_DMAError;

    /* Set the USART DMA Rx transfer error callback */
    husart->hdmarx->XferErrorCallback = USART_DMAError;

    /* Enable the USART receive DMA channel */
    tmp = (uint32_t*)&pRxData;
    HAL_DMA_Start_IT(husart->hdmarx, (uint32_t)&husart->Instance->RDR, *(uint32_t*)tmp, Size);

    /* Enable the USART transmit DMA channel */
    tmp = (uint32_t*)&pTxData;
    HAL_DMA_Start_IT(husart->hdmatx, *(uint32_t*)tmp, (uint32_t)&husart->Instance->TDR, Size);

    /* Process Unlocked */
    __HAL_UNLOCK(husart);

    /* Enable the USART Parity Error Interrupt */
    SET_BIT(husart->Instance->CR1, USART_CR1_PEIE);

    /* Enable the USART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(husart->Instance->CR3, USART_CR3_EIE);

    /* Clear the TC flag in the ICR register */
    __HAL_USART_CLEAR_FLAG(husart, USART_CLEAR_TCF);

    /* Enable the DMA transfer for the receiver request by setting the DMAR bit
       in the USART CR3 register */
    SET_BIT(husart->Instance->CR3, USART_CR3_DMAR);

    /* Enable the DMA transfer for transmit request by setting the DMAT bit
       in the USART CR3 register */
    SET_BIT(husart->Instance->CR3, USART_CR3_DMAT);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Pause the DMA Transfer.
  * @param  husart USART handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_DMAPause(USART_HandleTypeDef *husart)
{
  /* Process Locked */
  __HAL_LOCK(husart);

  if( (husart->State == HAL_USART_STATE_BUSY_TX) &&
      (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT)))
  {
    /* Disable the USART DMA Tx request */
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);
  }
  else if( (husart->State == HAL_USART_STATE_BUSY_RX) ||
           (husart->State == HAL_USART_STATE_BUSY_TX_RX) )
  {
    if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT))
    {
      /* Disable the USART DMA Tx request */
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);
  }
    if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR))
  {
      /* Disable PE and ERR (Frame error, noise error, overrun error) interrupts */
      CLEAR_BIT(husart->Instance->CR1, USART_CR1_PEIE);
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

    /* Disable the USART DMA Rx request */
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(husart);

  return HAL_OK;
}

/**
  * @brief Resume the DMA Transfer.
  * @param  husart USART handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_DMAResume(USART_HandleTypeDef *husart)
{
  /* Process Locked */
  __HAL_LOCK(husart);

  if(husart->State == HAL_USART_STATE_BUSY_TX)
  {
    /* Enable the USART DMA Tx request */
    SET_BIT(husart->Instance->CR3, USART_CR3_DMAT);
  }
  else if( (husart->State == HAL_USART_STATE_BUSY_RX) ||
           (husart->State == HAL_USART_STATE_BUSY_TX_RX) )
  {
    /* Clear the Overrun flag before resuming the Rx transfer*/
    __HAL_USART_CLEAR_FLAG(husart, USART_CLEAR_OREF);    

    /* Reenable PE and ERR (Frame error, noise error, overrun error) interrupts */
    SET_BIT(husart->Instance->CR1, USART_CR1_PEIE);
    SET_BIT(husart->Instance->CR3, USART_CR3_EIE);

    /* Enable the USART DMA Rx request  before the DMA Tx request */
    SET_BIT(husart->Instance->CR3, USART_CR3_DMAR);

    /* Enable the USART DMA Tx request */
    SET_BIT(husart->Instance->CR3, USART_CR3_DMAT);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(husart);

  return HAL_OK;
}

/**
  * @brief Stop the DMA Transfer.
  * @param  husart USART handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_USART_DMAStop(USART_HandleTypeDef *husart)
{
  /* The Lock is not implemented on this API to allow the user application
     to call the HAL USART API under callbacks HAL_USART_TxCpltCallback() / HAL_USART_RxCpltCallback() /
     HAL_USART_TxHalfCpltCallback / HAL_USART_RxHalfCpltCallback: 
     indeed, when HAL_DMA_Abort() API is called, the DMA TX/RX Transfer or Half Transfer complete  
     interrupt is generated if the DMA transfer interruption occurs at the middle or at the end of 
     the stream and the corresponding call back is executed. */  

  /* Disable the USART Tx/Rx DMA requests */
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);

  /* Abort the USART DMA tx channel */
  if(husart->hdmatx != NULL)
  {
    HAL_DMA_Abort(husart->hdmatx);
  }
  /* Abort the USART DMA rx channel */
  if(husart->hdmarx != NULL)
  {
    HAL_DMA_Abort(husart->hdmarx);
  }

  USART_EndTransfer(husart);
  husart->State = HAL_USART_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Abort ongoing transfers (blocking mode).
  * @param  husart USART handle.
  * @note   This procedure could be used for aborting any ongoing transfer started in Interrupt or DMA mode. 
  *         This procedure performs following operations :
  *           - Disable USART Interrupts (Tx and Rx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort (in case of transfer in DMA mode)
  *           - Set handle State to READY
  * @note   This procedure is executed in blocking mode : when exiting function, Abort is considered as completed.
  * @retval HAL status
*/
HAL_StatusTypeDef HAL_USART_Abort(USART_HandleTypeDef *husart)
{
#if defined(USART_CR1_FIFOEN)
  /* Disable TXEIE, TCIE, RXNE, RXFT, TXFT, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE | USART_CR1_TXEIE_TXFNFIE | USART_CR1_TCIE));
  CLEAR_BIT(husart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE | USART_CR3_TXFTIE));
#else
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);
#endif

  /* Disable the USART DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT))
  {
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);

    /* Abort the USART DMA Tx channel : use blocking DMA Abort API (no callback) */
    if(husart->hdmatx != NULL)
    {
      /* Set the USART DMA Abort callback to Null. 
         No call back execution at end of DMA abort procedure */
      husart->hdmatx->XferAbortCallback = NULL;

      HAL_DMA_Abort(husart->hdmatx);
    }
  }

  /* Disable the USART DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);

    /* Abort the USART DMA Rx channel : use blocking DMA Abort API (no callback) */
    if(husart->hdmarx != NULL)
    {
      /* Set the USART DMA Abort callback to Null. 
         No call back execution at end of DMA abort procedure */
      husart->hdmarx->XferAbortCallback = NULL;

      HAL_DMA_Abort(husart->hdmarx);
    }
  }

  /* Reset Tx and Rx transfer counters */
  husart->TxXferCount = 0; 
  husart->RxXferCount = 0; 

  /* Clear the Error flags in the ICR register */
  __HAL_USART_CLEAR_FLAG(husart, USART_CLEAR_OREF | USART_CLEAR_NEF | USART_CLEAR_PEF | USART_CLEAR_FEF);

#if defined(USART_CR1_FIFOEN)
  /* Flush the whole TX FIFO (if needed) */
  if (husart->FifoMode == USART_FIFOMODE_ENABLE)
  {
    __HAL_USART_SEND_REQ(husart, USART_TXDATA_FLUSH_REQUEST);
  }
#endif
  
  /* Discard the received data */
  __HAL_USART_SEND_REQ(husart, USART_RXDATA_FLUSH_REQUEST);

  /* Restore husart->State to Ready */
  husart->State  = HAL_USART_STATE_READY;

  /* Reset Handle ErrorCode to No Error */
  husart->ErrorCode = HAL_USART_ERROR_NONE;

  return HAL_OK;
}

/**
  * @brief  Abort ongoing transfers (Interrupt mode).
  * @param  husart USART handle.
  * @note   This procedure could be used for aborting any ongoing transfer started in Interrupt or DMA mode. 
  *         This procedure performs following operations :
  *           - Disable USART Interrupts (Tx and Rx)
  *           - Disable the DMA transfer in the peripheral register (if enabled)
  *           - Abort DMA transfer by calling HAL_DMA_Abort_IT (in case of transfer in DMA mode)
  *           - Set handle State to READY
  *           - At abort completion, call user abort complete callback
  * @note   This procedure is executed in Interrupt mode, meaning that abort procedure could be
  *         considered as completed only when user abort complete callback is executed (not when exiting function).
  * @retval HAL status
*/
HAL_StatusTypeDef HAL_USART_Abort_IT(USART_HandleTypeDef *husart)
{
  uint32_t abortcplt = 1;
  
#if defined(USART_CR1_FIFOEN)
  /* Disable TXEIE, TCIE, RXNE, RXFT, TXFT, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE | USART_CR1_TXEIE_TXFNFIE | USART_CR1_TCIE));
  CLEAR_BIT(husart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE | USART_CR3_TXFTIE));
#else
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);
#endif

  /* If DMA Tx and/or DMA Rx Handles are associated to USART Handle, DMA Abort complete callbacks should be initialised
     before any call to DMA Abort functions */
  /* DMA Tx Handle is valid */
  if(husart->hdmatx != NULL)
  {
    /* Set DMA Abort Complete callback if USART DMA Tx request if enabled.
       Otherwise, set it to NULL */
    if(HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT))
    {
      husart->hdmatx->XferAbortCallback = USART_DMATxAbortCallback;
    }
    else
    {
      husart->hdmatx->XferAbortCallback = NULL;
    }
  }
  /* DMA Rx Handle is valid */
  if(husart->hdmarx != NULL)
  {
    /* Set DMA Abort Complete callback if USART DMA Rx request if enabled.
       Otherwise, set it to NULL */
    if(HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR))
    {
      husart->hdmarx->XferAbortCallback = USART_DMARxAbortCallback;
    }
    else
    {
      husart->hdmarx->XferAbortCallback = NULL;
    }
  }
  
  /* Disable the USART DMA Tx request if enabled */
  if(HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAT))
  {
    /* Disable DMA Tx at USART level */
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);

    /* Abort the USART DMA Tx channel : use non blocking DMA Abort API (callback) */
    if(husart->hdmatx != NULL)
    {
      /* USART Tx DMA Abort callback has already been initialised : 
         will lead to call HAL_USART_AbortCpltCallback() at end of DMA abort procedure */

      /* Abort DMA TX */
      if(HAL_DMA_Abort_IT(husart->hdmatx) != HAL_OK)
      {
        husart->hdmatx->XferAbortCallback = NULL;
      }
      else
      {
        abortcplt = 0;
      }
    }
  }

  /* Disable the USART DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR))
  {
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);

    /* Abort the USART DMA Rx channel : use non blocking DMA Abort API (callback) */
    if(husart->hdmarx != NULL)
    {
      /* USART Rx DMA Abort callback has already been initialised : 
         will lead to call HAL_USART_AbortCpltCallback() at end of DMA abort procedure */

      /* Abort DMA RX */
      if(HAL_DMA_Abort_IT(husart->hdmarx) != HAL_OK)
      {
        husart->hdmarx->XferAbortCallback = NULL;
        abortcplt = 1;
      }
      else
      {
        abortcplt = 0;
      }
    }
  }

  /* if no DMA abort complete callback execution is required => call user Abort Complete callback */
  if (abortcplt == 1)
  {
    /* Reset Tx and Rx transfer counters */
    husart->TxXferCount = 0; 
    husart->RxXferCount = 0;

    /* Reset errorCode */
    husart->ErrorCode = HAL_USART_ERROR_NONE;

    /* Clear the Error flags in the ICR register */
    __HAL_USART_CLEAR_FLAG(husart, USART_CLEAR_OREF | USART_CLEAR_NEF | USART_CLEAR_PEF | USART_CLEAR_FEF);

#if defined(USART_CR1_FIFOEN)
    /* Flush the whole TX FIFO (if needed) */
    if (husart->FifoMode == USART_FIFOMODE_ENABLE)
    {
      __HAL_USART_SEND_REQ(husart, USART_TXDATA_FLUSH_REQUEST);
    }
    
#endif
    /* Discard the received data */
    __HAL_USART_SEND_REQ(husart, USART_RXDATA_FLUSH_REQUEST);
    
    /* Restore husart->State to Ready */
    husart->State  = HAL_USART_STATE_READY;

    /* As no DMA to be aborted, call directly user Abort complete callback */
    HAL_USART_AbortCpltCallback(husart);
  }

  return HAL_OK;
}

/**
  * @brief  Handle USART interrupt request.
  * @param  husart USART handle.
  * @retval None
  */
void HAL_USART_IRQHandler(USART_HandleTypeDef *husart)
{
  uint32_t isrflags   = READ_REG(husart->Instance->ISR);
  uint32_t cr1its     = READ_REG(husart->Instance->CR1);
  uint32_t cr3its     = READ_REG(husart->Instance->CR3);

  uint32_t errorflags;
  
  /* If no error occurs */
#if defined(USART_CR2_SLVEN)
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_UDR));
#else
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
#endif
  if (errorflags == RESET)
  {
    /* USART in mode Receiver ---------------------------------------------------*/
#if defined(USART_CR1_FIFOEN)
    if(((isrflags & USART_ISR_RXNE_RXFNE) != RESET) 
       && (   ((cr1its & USART_CR1_RXNEIE_RXFNEIE) != RESET)
           || ((cr3its & USART_CR3_RXFTIE) != RESET)) )
#else
    if(((isrflags & USART_ISR_RXNE) != RESET) 
       && ((cr1its & USART_CR1_RXNEIE) != RESET))
#endif
    {
      if (husart->RxISR != NULL) {husart->RxISR(husart);}
      return;
    }
  }
  
  /* If some errors occur */
#if defined(USART_CR1_FIFOEN)
  if(   (errorflags != RESET)
     && (   ((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != RESET)
         || ((cr1its & (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE)) != RESET)))
#else
  if(   (errorflags != RESET)
     && (   ((cr3its & USART_CR3_EIE) != RESET)
         || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
#endif
  {
    /* USART parity error interrupt occurred -------------------------------------*/
    if(((isrflags & USART_ISR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
    {
      __HAL_USART_CLEAR_IT(husart, USART_CLEAR_PEF);
      
      husart->ErrorCode |= HAL_USART_ERROR_PE;
    }
    
    /* USART frame error interrupt occurred --------------------------------------*/
    if(((isrflags & USART_ISR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      __HAL_USART_CLEAR_IT(husart, USART_CLEAR_FEF);
      
      husart->ErrorCode |= HAL_USART_ERROR_FE;
    }
    
    /* USART noise error interrupt occurred --------------------------------------*/
    if(((isrflags & USART_ISR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      __HAL_USART_CLEAR_IT(husart, USART_CLEAR_NEF);
      
      husart->ErrorCode |= HAL_USART_ERROR_NE;
    }
    
    /* USART Over-Run interrupt occurred -----------------------------------------*/
#if defined(USART_CR1_FIFOEN)
    if(   ((isrflags & USART_ISR_ORE) != RESET)
       &&(  ((cr1its & USART_CR1_RXNEIE_RXFNEIE) != RESET) ||
            ((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != RESET)))
#else
    if(   ((isrflags & USART_ISR_ORE) != RESET)
       &&(  ((cr1its & USART_CR1_RXNEIE) != RESET) ||
            ((cr3its & USART_CR3_EIE) != RESET)))
#endif
    {
      __HAL_USART_CLEAR_IT(husart, USART_CLEAR_OREF);
      
      husart->ErrorCode |= HAL_USART_ERROR_ORE;
    }
    
#if defined(USART_CR2_SLVEN)
    /* USART SPI slave underrun error interrupt occurred -------------------------*/
    if(((isrflags & USART_ISR_UDR) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
    {
      /* Ignore SPI slave underrun errors when reception is going on */
      if (husart->State == HAL_USART_STATE_BUSY_RX)
      {
        __HAL_USART_CLEAR_UDRFLAG(husart);
        return;
      }
      else
      {
        __HAL_USART_CLEAR_UDRFLAG(husart);
        husart->ErrorCode |= HAL_USART_ERROR_UDR;
      }
    }
#endif
    
    /* Call USART Error Call back function if need be --------------------------*/
    if(husart->ErrorCode != HAL_USART_ERROR_NONE)
    {
      /* USART in mode Receiver ---------------------------------------------------*/
#if defined(USART_CR1_FIFOEN)
      if(((isrflags & USART_ISR_RXNE_RXFNE) != RESET) 
         && (   ((cr1its & USART_CR1_RXNEIE_RXFNEIE) != RESET)
             || ((cr3its & USART_CR3_RXFTIE) != RESET)))
#else
      if(((isrflags & USART_ISR_RXNE) != RESET) 
         && ((cr1its & USART_CR1_RXNEIE) != RESET))
#endif
      {
        if (husart->RxISR != NULL) {husart->RxISR(husart);}
      }
      
      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
      consider error as blocking */
      if (((husart->ErrorCode & HAL_USART_ERROR_ORE) != RESET) ||
          (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR)))
      {  
        /* Blocking error : transfer is aborted
        Set the USART state ready to be able to start again the process,
        Disable Interrupts, and disable DMA requests, if ongoing */
        USART_EndTransfer(husart);
        
        /* Disable the USART DMA Rx request if enabled */
        if (HAL_IS_BIT_SET(husart->Instance->CR3, USART_CR3_DMAR))
        {
          CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR | USART_CR3_DMAR);
          
          /* Abort the USART DMA Tx channel */
          if(husart->hdmatx != NULL)
          {
            /* Set the USART Tx DMA Abort callback to NULL : no callback
            executed at end of DMA abort procedure */
            husart->hdmatx->XferAbortCallback = NULL;
            
            /* Abort DMA TX */
            HAL_DMA_Abort_IT(husart->hdmatx);
          }
          
          /* Abort the USART DMA Rx channel */
          if(husart->hdmarx != NULL)
          {
            /* Set the USART Rx DMA Abort callback : 
            will lead to call HAL_USART_ErrorCallback() at end of DMA abort procedure */
            husart->hdmarx->XferAbortCallback = USART_DMAAbortOnError;
            
            /* Abort DMA RX */
            if(HAL_DMA_Abort_IT(husart->hdmarx) != HAL_OK)
            {
              /* Call Directly husart->hdmarx->XferAbortCallback function in case of error */
              husart->hdmarx->XferAbortCallback(husart->hdmarx);
            }
          }
          else
          {
            /* Call user error callback */
            HAL_USART_ErrorCallback(husart);
          }
        }
        else
        {
          /* Call user error callback */
          HAL_USART_ErrorCallback(husart);
        }
      }
      else
      {
        /* Non Blocking error : transfer could go on. 
        Error is notified to user through user error callback */
        HAL_USART_ErrorCallback(husart);
        husart->ErrorCode = HAL_USART_ERROR_NONE;
      }
    }
    return;
    
  } /* End if some error occurs */
  
  
  /* USART in mode Transmitter ------------------------------------------------*/
#if defined(USART_CR1_FIFOEN)
  if(((isrflags & USART_ISR_TXE_TXFNF) != RESET) 
     && (   ((cr1its & USART_CR1_TXEIE_TXFNFIE) != RESET)
         || ((cr3its & USART_CR3_TXFTIE) != RESET)))
#else
  if(((isrflags & USART_ISR_TXE) != RESET) 
     && ((cr1its & USART_CR1_TXEIE) != RESET))
#endif
  {
    if (husart->TxISR != NULL) {husart->TxISR(husart);}
    return;
  }
  
  /* USART in mode Transmitter (transmission end) -----------------------------*/
  if(((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    USART_EndTransmit_IT(husart);
    return;
  }
  
#if defined(USART_CR1_FIFOEN)
  /* USART TX Fifo Empty occurred ----------------------------------------------*/
  if(((isrflags & USART_ISR_TXFE) != RESET) && ((cr1its & USART_CR1_TXFEIE) != RESET))
  {
    HAL_USARTEx_TxFifoEmptyCallback(husart);
    return;
  }
  
  /* USART RX Fifo Full occurred ----------------------------------------------*/
  if(((isrflags & USART_ISR_RXFF) != RESET) && ((cr1its & USART_CR1_RXFFIE) != RESET))
  {
    HAL_USARTEx_RxFifoFullCallback(husart);
    return;
  }
#endif
}

/**
  * @brief Tx Transfer completed callback.
  * @param husart USART handle.
  * @retval None
  */
__weak void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);
    
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_USART_TxCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  Tx Half Transfer completed callback.
  * @param husart USART handle.
  * @retval None
  */
 __weak void HAL_USART_TxHalfCpltCallback(USART_HandleTypeDef *husart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);
    
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_USART_TxHalfCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  Rx Transfer completed callback.
  * @param husart USART handle.
  * @retval None
  */
__weak void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);
    
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_USART_RxCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief Rx Half Transfer completed callback.
  * @param husart USART handle.
  * @retval None
  */
__weak void HAL_USART_RxHalfCpltCallback(USART_HandleTypeDef *husart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);
    
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_USART_RxHalfCpltCallback can be implemented in the user file
   */
}

/**
  * @brief Tx/Rx Transfers completed callback for the non-blocking process.
  * @param husart USART handle.
  * @retval None
  */
__weak void HAL_USART_TxRxCpltCallback(USART_HandleTypeDef *husart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);
    
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_USART_TxRxCpltCallback can be implemented in the user file
   */
}

/**
  * @brief USART error callback.
  * @param husart USART handle.
  * @retval None
  */
__weak void HAL_USART_ErrorCallback(USART_HandleTypeDef *husart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);
    
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_USART_ErrorCallback can be implemented in the user file.
   */
}

/**
  * @brief  USART Abort Complete callback.
  * @param  husart USART handle.
  * @retval None
  */
__weak void HAL_USART_AbortCpltCallback (USART_HandleTypeDef *husart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(husart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_USART_AbortCpltCallback can be implemented in the user file.
   */
}

/**
  * @}
  */

/** @defgroup USART_Exported_Functions_Group4 Peripheral State and Error functions
 *  @brief   USART Peripheral State and Error functions
 *
@verbatim
  ==============================================================================
            ##### Peripheral State and Error functions #####
  ==============================================================================
    [..]
    This subsection provides functions allowing to :
      (+) Return the USART handle state
      (+) Return the USART handle error code

@endverbatim
  * @{
  */


/**
  * @brief Return the USART handle state.
  * @param husart pointer to a USART_HandleTypeDef structure that contains
  *              the configuration information for the specified USART.
  * @retval USART handle state
  */
HAL_USART_StateTypeDef HAL_USART_GetState(USART_HandleTypeDef *husart)
{
  return husart->State;
}

/**
  * @brief Return the USART error code.
  * @param husart pointer to a USART_HandleTypeDef structure that contains
  *              the configuration information for the specified USART.
  * @retval USART handle Error Code
  */
uint32_t HAL_USART_GetError(USART_HandleTypeDef *husart)
{
  return husart->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup USART_Private_Functions USART Private Functions
 * @{
 */

/**
  * @brief  End ongoing transfer on USART peripheral (following error detection or Transfer completion).
  * @param  husart USART handle.
  * @retval None
  */
static void USART_EndTransfer(USART_HandleTypeDef *husart)
{
#if defined(USART_CR1_FIFOEN)
  /* Disable TXEIE, TCIE, RXNE, RXFT, TXFT, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE | USART_CR1_TXEIE_TXFNFIE | USART_CR1_TCIE));
  CLEAR_BIT(husart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE | USART_CR3_TXFTIE));
#else
  /* Disable TXEIE, TCIE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE | USART_CR1_TCIE));
  CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);
#endif

  /* At end of process, restore husart->State to Ready */
  husart->State = HAL_USART_STATE_READY;
}

/**
  * @brief DMA USART transmit process complete callback.
  * @param  hdma DMA handle.
  * @retval None
  */
static void USART_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef*)(hdma->Parent);

  /* DMA Normal mode */
  if ( HAL_IS_BIT_CLR(hdma->Instance->CCR, DMA_CCR_CIRC) )
  {  
    husart->TxXferCount = 0;

    if(husart->State == HAL_USART_STATE_BUSY_TX)
    {
      /* Disable the DMA transfer for transmit request by resetting the DMAT bit
         in the USART CR3 register */
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);

      /* Enable the USART Transmit Complete Interrupt */
      __HAL_USART_ENABLE_IT(husart, USART_IT_TC);
    }
  }
  /* DMA Circular mode */
  else
  {
    if(husart->State == HAL_USART_STATE_BUSY_TX)
    {
    HAL_USART_TxCpltCallback(husart);
   }
 }
}

/**
  * @brief DMA USART transmit process half complete callback.
  * @param  hdma DMA handle.
  * @retval None
  */
static void USART_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef*)(hdma->Parent);

  HAL_USART_TxHalfCpltCallback(husart);
}

/**
  * @brief DMA USART receive process complete callback.
  * @param  hdma DMA handle.
  * @retval None
  */
static void USART_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef*)(hdma->Parent);
  
  /* DMA Normal mode */
  if ( HAL_IS_BIT_CLR(hdma->Instance->CCR, DMA_CCR_CIRC) )
  {
    husart->RxXferCount = 0;
    
    /* Disable PE and ERR (Frame error, noise error, overrun error) interrupts */
    CLEAR_BIT(husart->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);
    
    /* Disable the DMA RX transfer for the receiver request by resetting the DMAR bit
    in USART CR3 register */
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAR);
    /* similarly, disable the DMA TX transfer that was started to provide the
    clock to the slave device */
    CLEAR_BIT(husart->Instance->CR3, USART_CR3_DMAT);
    
    if(husart->State == HAL_USART_STATE_BUSY_RX)
    {
      HAL_USART_RxCpltCallback(husart);
    }
    /* The USART state is HAL_USART_STATE_BUSY_TX_RX */
    else
    {
      HAL_USART_TxRxCpltCallback(husart);
    }
    husart->State= HAL_USART_STATE_READY;
  }
  /* DMA circular mode */
  else
  {
    if(husart->State == HAL_USART_STATE_BUSY_RX)
    {
      HAL_USART_RxCpltCallback(husart);
    }
    /* The USART state is HAL_USART_STATE_BUSY_TX_RX */
    else
    {
      HAL_USART_TxRxCpltCallback(husart);
    }
  }
}

/**
  * @brief DMA USART receive process half complete callback.
  * @param  hdma DMA handle.
  * @retval None
  */
static void USART_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef*)(hdma->Parent);

  HAL_USART_RxHalfCpltCallback(husart);
}

/**
  * @brief DMA USART communication error callback.
  * @param  hdma DMA handle.
  * @retval None
  */
static void USART_DMAError(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef*)(hdma->Parent);

  husart->RxXferCount = 0;
  husart->TxXferCount = 0;
  USART_EndTransfer(husart);

  husart->ErrorCode |= HAL_USART_ERROR_DMA;
  husart->State= HAL_USART_STATE_READY;

  HAL_USART_ErrorCallback(husart);
}

/**
  * @brief  DMA USART communication abort callback, when initiated by HAL services on Error
  *         (To be called at end of DMA Abort procedure following error occurrence).
  * @param  hdma DMA handle.
  * @retval None
  */
static void USART_DMAAbortOnError(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef*)(hdma->Parent);
  husart->RxXferCount = 0;
  husart->TxXferCount = 0;

  HAL_USART_ErrorCallback(husart);
}

/**
  * @brief  DMA USART Tx communication abort callback, when initiated by user
  *         (To be called at end of DMA Tx Abort procedure following user abort request).
  * @note   When this callback is executed, User Abort complete call back is called only if no
  *         Abort still ongoing for Rx DMA Handle.
  * @param  hdma DMA handle.
  * @retval None
  */
static void USART_DMATxAbortCallback(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef* )(hdma->Parent);
  
  husart->hdmatx->XferAbortCallback = NULL;

  /* Check if an Abort process is still ongoing */
  if(husart->hdmarx != NULL)
  {
    if(husart->hdmarx->XferAbortCallback != NULL)
    {
      return;
    }
  }
  
  /* No Abort process still ongoing : All DMA channels are aborted, call user Abort Complete callback */
  husart->TxXferCount = 0;
  husart->RxXferCount = 0;

  /* Reset errorCode */
  husart->ErrorCode = HAL_USART_ERROR_NONE;

  /* Clear the Error flags in the ICR register */
  __HAL_USART_CLEAR_FLAG(husart, USART_CLEAR_OREF | USART_CLEAR_NEF | USART_CLEAR_PEF | USART_CLEAR_FEF);

  /* Restore husart->State to Ready */
  husart->State = HAL_USART_STATE_READY;

  /* Call user Abort complete callback */
  HAL_USART_AbortCpltCallback(husart);
}


/**
  * @brief  DMA USART Rx communication abort callback, when initiated by user
  *         (To be called at end of DMA Rx Abort procedure following user abort request).
  * @note   When this callback is executed, User Abort complete call back is called only if no
  *         Abort still ongoing for Tx DMA Handle.
  * @param  hdma DMA handle.
  * @retval None
  */
static void USART_DMARxAbortCallback(DMA_HandleTypeDef *hdma)
{
  USART_HandleTypeDef* husart = (USART_HandleTypeDef* )(hdma->Parent);
  
  husart->hdmarx->XferAbortCallback = NULL;

  /* Check if an Abort process is still ongoing */
  if(husart->hdmatx != NULL)
  {
    if(husart->hdmatx->XferAbortCallback != NULL)
    {
      return;
    }
  }
  
  /* No Abort process still ongoing : All DMA channels are aborted, call user Abort Complete callback */
  husart->TxXferCount = 0;
  husart->RxXferCount = 0;

  /* Reset errorCode */
  husart->ErrorCode = HAL_USART_ERROR_NONE;

  /* Clear the Error flags in the ICR register */
  __HAL_USART_CLEAR_FLAG(husart, USART_CLEAR_OREF | USART_CLEAR_NEF | USART_CLEAR_PEF | USART_CLEAR_FEF);

  /* Restore husart->State to Ready */
  husart->State  = HAL_USART_STATE_READY;

  /* Call user Abort complete callback */
  HAL_USART_AbortCpltCallback(husart);
}


/**
  * @brief  Handle USART Communication Timeout.
  * @param  husart USART handle.
  * @param  Flag Specifies the USART flag to check.
  * @param  Status the Flag status (SET or RESET).
  * @param  Tickstart Tick start value
  * @param  Timeout timeout duration.
  * @retval HAL status
  */
static HAL_StatusTypeDef USART_WaitOnFlagUntilTimeout(USART_HandleTypeDef *husart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout)
{
  /* Wait until flag is set */
  while((__HAL_USART_GET_FLAG(husart, Flag) ? SET : RESET) == Status)
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0) || ((HAL_GetTick()-Tickstart) > Timeout))
      {
        husart->State= HAL_USART_STATE_READY;
        
        /* Process Unlocked */
        __HAL_UNLOCK(husart);
        
        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief Configure the USART peripheral.
  * @param husart USART handle.
  * @retval HAL status
  */
static HAL_StatusTypeDef USART_SetConfig(USART_HandleTypeDef *husart)
{
  uint32_t tmpreg                      = 0x0;
  USART_ClockSourceTypeDef clocksource = USART_CLOCKSOURCE_UNDEFINED;
  HAL_StatusTypeDef ret                = HAL_OK;
  uint16_t brrtemp                     = 0x0000;
  uint16_t usartdiv                    = 0x0000;  

  /* Check the parameters */
  assert_param(IS_USART_POLARITY(husart->Init.CLKPolarity));
  assert_param(IS_USART_PHASE(husart->Init.CLKPhase));
  assert_param(IS_USART_LASTBIT(husart->Init.CLKLastBit));
  assert_param(IS_USART_BAUDRATE(husart->Init.BaudRate));
  assert_param(IS_USART_WORD_LENGTH(husart->Init.WordLength));
  assert_param(IS_USART_STOPBITS(husart->Init.StopBits));
  assert_param(IS_USART_PARITY(husart->Init.Parity));
  assert_param(IS_USART_MODE(husart->Init.Mode));
#if defined(USART_PRESC_PRESCALER)
  assert_param(IS_USART_PRESCALER(husart->Init.ClockPrescaler));
#endif
  
  /*-------------------------- USART CR1 Configuration -----------------------*/
   /* Clear M, PCE, PS, TE and RE bits and configure
   *  the USART Word Length, Parity and Mode:
   *  set the M bits according to husart->Init.WordLength value
   *  set PCE and PS bits according to husart->Init.Parity value
   *  set TE and RE bits according to husart->Init.Mode value 
   *  force OVER8 to 1 to allow to reach the maximum speed (Fclock/8) */
  tmpreg = (uint32_t)husart->Init.WordLength | husart->Init.Parity | husart->Init.Mode | USART_CR1_OVER8;
  MODIFY_REG(husart->Instance->CR1, USART_CR1_FIELDS, tmpreg);

  /*---------------------------- USART CR2 Configuration ---------------------*/
  /* Clear and configure the USART Clock, CPOL, CPHA, LBCL STOP and SLVEN bits:
   * set CPOL bit according to husart->Init.CLKPolarity value
   * set CPHA bit according to husart->Init.CLKPhase value
   * set LBCL bit according to husart->Init.CLKLastBit value (used in SPI master mode only)
   * set STOP[13:12] bits according to husart->Init.StopBits value */
  tmpreg = 0;
  tmpreg = (uint32_t)(USART_CLOCK_ENABLE);
  tmpreg |= (uint32_t)husart->Init.CLKLastBit;
  tmpreg |= ((uint32_t)husart->Init.CLKPolarity | (uint32_t)husart->Init.CLKPhase);
  tmpreg |= (uint32_t)husart->Init.StopBits;
  MODIFY_REG(husart->Instance->CR2, USART_CR2_FIELDS, tmpreg);

#if defined(USART_PRESC_PRESCALER)
  /*-------------------------- USART PRESC Configuration -----------------------*/
  /* Configure
   * - USART Clock Prescaler : set PRESCALER according to husart->Init.ClockPrescaler value */
  MODIFY_REG(husart->Instance->PRESC, USART_PRESC_PRESCALER, husart->Init.ClockPrescaler);
#endif

  /*-------------------------- USART BRR Configuration -----------------------*/
  /* BRR is filled-up according to OVER8 bit setting which is forced to 1     */ 
  USART_GETCLOCKSOURCE(husart, clocksource);
  
  switch (clocksource)
  {
  case USART_CLOCKSOURCE_PCLK1:
#if defined(USART_PRESC_PRESCALER)
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(HAL_RCC_GetPCLK1Freq(), husart->Init.BaudRate, husart->Init.ClockPrescaler));
#else
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(HAL_RCC_GetPCLK1Freq(), husart->Init.BaudRate));
#endif
    break;
  case USART_CLOCKSOURCE_PCLK2:
#if defined(USART_PRESC_PRESCALER)
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(HAL_RCC_GetPCLK2Freq(), husart->Init.BaudRate, husart->Init.ClockPrescaler));
#else
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(HAL_RCC_GetPCLK2Freq(), husart->Init.BaudRate));
#endif
    break;
  case USART_CLOCKSOURCE_HSI:
#if defined(USART_PRESC_PRESCALER)
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(HSI_VALUE, husart->Init.BaudRate, husart->Init.ClockPrescaler));
#else
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(HSI_VALUE, husart->Init.BaudRate));
#endif
    break;
  case USART_CLOCKSOURCE_SYSCLK:
#if defined(USART_PRESC_PRESCALER)
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(HAL_RCC_GetSysClockFreq(), husart->Init.BaudRate, husart->Init.ClockPrescaler));
#else
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(HAL_RCC_GetSysClockFreq(), husart->Init.BaudRate));
#endif
    break;
  case USART_CLOCKSOURCE_LSE:
#if defined(USART_PRESC_PRESCALER)
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(LSE_VALUE, husart->Init.BaudRate, husart->Init.ClockPrescaler));
#else
    usartdiv = (uint16_t)(USART_DIV_SAMPLING8(LSE_VALUE, husart->Init.BaudRate));
#endif
    break;
  case USART_CLOCKSOURCE_UNDEFINED:
  default:
    ret = HAL_ERROR;
    break;
  }
  
  /* USARTDIV must be greater than or equal to 0d16 */
  if (usartdiv >= USART_BRR_MIN)
  {
    brrtemp = usartdiv & 0xFFF0U;
    brrtemp |= (uint16_t)((usartdiv & (uint16_t)0x000FU) >> 1U);
    husart->Instance->BRR = brrtemp;
  }
  else
  {
    ret = HAL_ERROR;
  }
  
#if defined(USART_CR1_FIFOEN)
  /* Initialize the number of data to process during RX/TX ISR execution */
  husart->NbTxDataToProcess = 1;
  husart->NbRxDataToProcess = 1;
#endif
  
  /* Clear ISR function pointers */
  husart->RxISR   = NULL;
  husart->TxISR   = NULL;
  
  return ret;
}

/**
  * @brief Check the USART Idle State.
  * @param husart: USART handle.
  * @retval HAL status
  */
static HAL_StatusTypeDef USART_CheckIdleState(USART_HandleTypeDef *husart)
{
  uint32_t tickstart = 0;

  /* Initialize the USART ErrorCode */
  husart->ErrorCode = HAL_USART_ERROR_NONE;

  /* Init tickstart for timeout managment*/
  tickstart = HAL_GetTick();

  /* Check if the Transmitter is enabled */
  if((husart->Instance->CR1 & USART_CR1_TE) == USART_CR1_TE)
  {
    /* Wait until TEACK flag is set */
    if(USART_WaitOnFlagUntilTimeout(husart, USART_ISR_TEACK, RESET, tickstart, USART_TEACK_REACK_TIMEOUT) != HAL_OK)
    {
      /* Timeout occurred */
      return HAL_TIMEOUT;
    }
  }
  /* Check if the Receiver is enabled */
  if((husart->Instance->CR1 & USART_CR1_RE) == USART_CR1_RE)
  {
    /* Wait until REACK flag is set */
    if(USART_WaitOnFlagUntilTimeout(husart, USART_ISR_REACK, RESET, tickstart, USART_TEACK_REACK_TIMEOUT) != HAL_OK)
    {
      /* Timeout occurred */
      return HAL_TIMEOUT;
    }
  }

  /* Initialize the USART state*/
  husart->State= HAL_USART_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(husart);

  return HAL_OK;
}

/**
  * @brief  Simplex send an amount of data in non-blocking mode.
  * @note   Function called under interruption only, once
  *         interruptions have been enabled by HAL_USART_Transmit_IT().
  * @note   The USART errors are not managed to avoid the overrun error.
  * @note   ISR function executed when FIFO mode is disabled and when the
  *         data word length is less than 9 bits long.
  * @param  husart USART handle.
  * @retval None
  */
static void USART_TxISR_8BIT(USART_HandleTypeDef *husart)
{
  /* Check that a Tx process is ongoing */
  if ((husart->State == HAL_USART_STATE_BUSY_TX) ||
      (husart->State == HAL_USART_STATE_BUSY_TX_RX))
  {
    if(husart->TxXferCount == 0)
    {
      /* Disable the USART Transmit data register empty interrupt */
      __HAL_USART_DISABLE_IT(husart, USART_IT_TXE);
      
      /* Enable the USART Transmit Complete Interrupt */
      __HAL_USART_ENABLE_IT(husart, USART_IT_TC);
    }
    else
    {
      husart->Instance->TDR = (uint8_t)(*husart->pTxBuffPtr++ & (uint8_t)0xFF);
      husart->TxXferCount--;
    }
  }
}

/**
  * @brief  Simplex send an amount of data in non-blocking mode.
  * @note   Function called under interruption only, once
  *         interruptions have been enabled by HAL_USART_Transmit_IT().
  * @note   The USART errors are not managed to avoid the overrun error.
  * @note   ISR function executed when FIFO mode is disabled and when the
  *         data word length is 9 bits long.
  * @param  husart USART handle.
  * @retval None
  */
static void USART_TxISR_16BIT(USART_HandleTypeDef *husart)
{
  uint16_t* tmp;
  
  if ((husart->State == HAL_USART_STATE_BUSY_TX) ||
      (husart->State == HAL_USART_STATE_BUSY_TX_RX))
  {
    if(husart->TxXferCount == 0)
    {
      /* Disable the USART Transmit data register empty interrupt */
      __HAL_USART_DISABLE_IT(husart, USART_IT_TXE);
      
      /* Enable the USART Transmit Complete Interrupt */
      __HAL_USART_ENABLE_IT(husart, USART_IT_TC);
    }
    else
    {
      tmp = (uint16_t*) husart->pTxBuffPtr;
      husart->Instance->TDR = (*tmp & (uint16_t)0x01FF);
      husart->pTxBuffPtr += 2;      
      husart->TxXferCount--;
    }
  }
}

#if defined(USART_CR1_FIFOEN)
/**
  * @brief  Simplex send an amount of data in non-blocking mode.
  * @note   Function called under interruption only, once
  *         interruptions have been enabled by HAL_USART_Transmit_IT().
  * @note   The USART errors are not managed to avoid the overrun error.
  * @note   ISR function executed when FIFO mode is enabled and when the
  *         data word length is less than 9 bits long.
  * @param  husart USART handle.
  * @retval None
  */
static void USART_TxISR_8BIT_FIFOEN(USART_HandleTypeDef *husart)
{
  uint8_t   nb_tx_data;
  /* Check that a Tx process is ongoing */
  if ((husart->State == HAL_USART_STATE_BUSY_TX) ||
      (husart->State == HAL_USART_STATE_BUSY_TX_RX))
  {
    for(nb_tx_data = husart->NbTxDataToProcess ; nb_tx_data > 0 ; nb_tx_data--)
    {
      if(husart->TxXferCount == 0)
      {
        /* Disable the TX FIFO threshold interrupt */
        __HAL_USART_DISABLE_IT(husart, USART_IT_TXFT);
        
        /* Enable the USART Transmit Complete Interrupt */
        __HAL_USART_ENABLE_IT(husart, USART_IT_TC);
        
        break; /* force exit loop */
      }
      else if (__HAL_USART_GET_FLAG(husart, USART_FLAG_TXFNF) == SET)
      {
        husart->Instance->TDR = (uint8_t)(*husart->pTxBuffPtr++ & (uint8_t)0xFF);
        husart->TxXferCount--;
      }
    }   
  }
}

/**
  * @brief  Simplex send an amount of data in non-blocking mode.
  * @note   Function called under interruption only, once
  *         interruptions have been enabled by HAL_USART_Transmit_IT().
  * @note   The USART errors are not managed to avoid the overrun error.
  * @note   ISR function executed when FIFO mode is enabled and when the
  *         data word length is 9 bits long.
  * @param  husart USART handle.
  * @retval None
  */
static void USART_TxISR_16BIT_FIFOEN(USART_HandleTypeDef *husart)
{
  uint16_t* tmp;
  uint8_t   nb_tx_data;
  
  /* Check that a Tx process is ongoing */
  if ((husart->State == HAL_USART_STATE_BUSY_TX) ||
      (husart->State == HAL_USART_STATE_BUSY_TX_RX))
  {
    for(nb_tx_data = husart->NbTxDataToProcess ; nb_tx_data > 0 ; nb_tx_data--)
    {    
      if(husart->TxXferCount == 0)
      {
        /* Disable the TX FIFO threshold interrupt */
        __HAL_USART_DISABLE_IT(husart, USART_IT_TXFT);
        
        /* Enable the USART Transmit Complete Interrupt */
        __HAL_USART_ENABLE_IT(husart, USART_IT_TC);
        
        break; /* force exit loop */
      }
      else if (__HAL_USART_GET_FLAG(husart, USART_FLAG_TXFNF) == SET)
      {
        tmp = (uint16_t*) husart->pTxBuffPtr;
        husart->Instance->TDR = (*tmp & (uint16_t)0x01FF);
        husart->pTxBuffPtr += 2;      
        husart->TxXferCount--;
      }
    }
  }
}
#endif


/**
  * @brief  Wraps up transmission in non-blocking mode.
  * @param  husart Pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
static void USART_EndTransmit_IT(USART_HandleTypeDef *husart)
{
  /* Disable the USART Transmit Complete Interrupt */
  __HAL_USART_DISABLE_IT(husart, USART_IT_TC);

  /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_USART_DISABLE_IT(husart, USART_IT_ERR);

  /* Clear TxISR function pointer */
  husart->TxISR = NULL;
  
  if (husart->State == HAL_USART_STATE_BUSY_TX)
  {
    /* Clear overrun flag and discard the received data */
    __HAL_USART_CLEAR_OREFLAG(husart);
    __HAL_USART_SEND_REQ(husart, USART_RXDATA_FLUSH_REQUEST);
    
    /* Tx process is completed, restore husart->State to Ready */
    husart->State = HAL_USART_STATE_READY;
    
    HAL_USART_TxCpltCallback(husart);
  }
  else if (husart->RxXferCount == 0)
  {
    /* TxRx process is completed, restore husart->State to Ready */
    husart->State = HAL_USART_STATE_READY;
    
    HAL_USART_TxRxCpltCallback(husart);
  }
}


/**
  * @brief  Simplex receive an amount of data in non-blocking mode.
  * @note   Function called under interruption only, once
  *         interruptions have been enabled by HAL_USART_Receive_IT().
  * @note   ISR function executed when FIFO mode is disabled and when the
  *         data word length is less than 9 bits long.
  * @param  husart USART handle
  * @retval None
  */
static void USART_RxISR_8BIT(USART_HandleTypeDef *husart)
{
  uint16_t uhMask = husart->Mask;
  
  if ((husart->State == HAL_USART_STATE_BUSY_RX) ||
      (husart->State == HAL_USART_STATE_BUSY_TX_RX))
  {
    *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->RDR & (uint8_t)uhMask);
    
    if(--husart->RxXferCount == 0)
    {
      /* Disable the USART Parity Error Interrupt and RXNE interrupt*/
#if defined(USART_CR1_FIFOEN)
      CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
#else
      CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
#endif
      
      /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

      /* Clear RxISR function pointer */
      husart->RxISR = NULL;
      
      if(husart->State == HAL_USART_STATE_BUSY_RX)
      {
#if defined(USART_CR2_SLVEN)
        /* Clear SPI slave underrun flag and discard transmit data */
        if (husart->SlaveMode == USART_SLAVEMODE_ENABLE)
        {
          __HAL_USART_CLEAR_UDRFLAG(husart);
          __HAL_USART_SEND_REQ(husart, USART_TXDATA_FLUSH_REQUEST);
        }
#endif
        
        /* Rx process is completed, restore husart->State to Ready */
        husart->State = HAL_USART_STATE_READY;
        
        HAL_USART_RxCpltCallback(husart);
      }
      else if ((husart->TxXferCount == 0) &&
               (READ_BIT(husart->Instance->CR1, USART_CR1_TCIE) != USART_CR1_TCIE))
      {
        /* TxRx process is completed, restore husart->State to Ready */
        husart->State = HAL_USART_STATE_READY;
        
        HAL_USART_TxRxCpltCallback(husart);
      }
    }
#if defined(USART_CR2_SLVEN)
    else if ( (husart->State == HAL_USART_STATE_BUSY_RX) &&
              (husart->SlaveMode == USART_SLAVEMODE_DISABLE))
#else
    else if (husart->State == HAL_USART_STATE_BUSY_RX)
#endif
    {
      /* Send dummy byte in order to generate the clock for the Slave to Send the next data */
      husart->Instance->TDR = (USART_DUMMY_DATA & (uint16_t)0x00FF);
    }    
  }
}

/**
  * @brief  Simplex receive an amount of data in non-blocking mode.
  * @note   Function called under interruption only, once
  *         interruptions have been enabled by HAL_USART_Receive_IT().
  * @note   ISR function executed when FIFO mode is disabled and when the
  *         data word length is 9 bits long.
  * @param  husart USART handle
  * @retval None
  */
static void USART_RxISR_16BIT(USART_HandleTypeDef *husart)
{
  uint16_t* tmp;
  uint16_t uhMask = husart->Mask;

  if ((husart->State == HAL_USART_STATE_BUSY_RX) ||
      (husart->State == HAL_USART_STATE_BUSY_TX_RX))
  {
    tmp = (uint16_t*) husart->pRxBuffPtr;
    *tmp = (uint16_t)(husart->Instance->RDR & uhMask);
    husart->pRxBuffPtr += 2;
    
    if(--husart->RxXferCount == 0)
    {
      /* Disable the USART Parity Error Interrupt and RXNE interrupt*/
#if defined(USART_CR1_FIFOEN)
      CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));
#else
      CLEAR_BIT(husart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
#endif

      /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) */
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_EIE);

      /* Clear RxISR function pointer */
      husart->RxISR = NULL;
      
      if(husart->State == HAL_USART_STATE_BUSY_RX)
      {
#if defined(USART_CR2_SLVEN)
        /* Clear SPI slave underrun flag and discard transmit data */
        if (husart->SlaveMode == USART_SLAVEMODE_ENABLE)
        {
          __HAL_USART_CLEAR_UDRFLAG(husart);
          __HAL_USART_SEND_REQ(husart, USART_TXDATA_FLUSH_REQUEST);
        }
#endif
        
        /* Rx process is completed, restore husart->State to Ready */
        husart->State = HAL_USART_STATE_READY;
        
        HAL_USART_RxCpltCallback(husart);
      }
      else if ((husart->TxXferCount == 0) &&
               (READ_BIT(husart->Instance->CR1, USART_CR1_TCIE) != USART_CR1_TCIE))
      {
        /* TxRx process is completed, restore husart->State to Ready */
        husart->State = HAL_USART_STATE_READY;
        
        HAL_USART_TxRxCpltCallback(husart);
      }
    }
#if defined(USART_CR2_SLVEN)
    else if ( (husart->State == HAL_USART_STATE_BUSY_RX) &&
              (husart->SlaveMode == USART_SLAVEMODE_DISABLE))
#else
    else if (husart->State == HAL_USART_STATE_BUSY_RX)
#endif
    {
      /* Send dummy byte in order to generate the clock for the Slave to Send the next data */
      husart->Instance->TDR = (USART_DUMMY_DATA & (uint16_t)0x00FF);
    }
  }
}

#if defined(USART_CR1_FIFOEN)
/**
  * @brief  Simplex receive an amount of data in non-blocking mode.
  * @note   Function called under interruption only, once
  *         interruptions have been enabled by HAL_USART_Receive_IT().
  * @note   ISR function executed when FIFO mode is enabled and when the
  *         data word length is less than 9 bits long.
  * @retval None
  * @retval HAL status
  */
static void USART_RxISR_8BIT_FIFOEN(USART_HandleTypeDef *husart)
{
  uint16_t uhMask = husart->Mask;
  uint8_t  nb_rx_data;
  
  /* Check that a Rx process is ongoing */
  if ((husart->State == HAL_USART_STATE_BUSY_RX) ||
      (husart->State == HAL_USART_STATE_BUSY_TX_RX))
  {
    for(nb_rx_data = husart->NbRxDataToProcess ; nb_rx_data > 0 ; nb_rx_data--)
    {    
      if (__HAL_USART_GET_FLAG(husart, USART_FLAG_RXFNE) == SET)
      {
        *husart->pRxBuffPtr++ = (uint8_t)(husart->Instance->RDR & (uint8_t)uhMask);
        
        if(--husart->RxXferCount == 0)
        {
          /* Disable the USART Parity Error Interrupt */
          CLEAR_BIT(husart->Instance->CR1, USART_CR1_PEIE);
          
          /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) and RX FIFO Threshold interrupt */
          CLEAR_BIT(husart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));
          
          /* Clear RxISR function pointer */
          husart->RxISR = NULL;
          
          if (husart->State == HAL_USART_STATE_BUSY_RX)
          {
#if defined(USART_CR2_SLVEN)
            /* Clear SPI slave underrun flag and discard transmit data */
            if (husart->SlaveMode == USART_SLAVEMODE_ENABLE)
            {
              __HAL_USART_CLEAR_UDRFLAG(husart);
              __HAL_USART_SEND_REQ(husart, USART_TXDATA_FLUSH_REQUEST);
            }
#endif
            
            /* Rx process is completed, restore husart->State to Ready */
            husart->State = HAL_USART_STATE_READY;
            
            HAL_USART_RxCpltCallback(husart);
          }
          else if ((husart->TxXferCount == 0) &&
                   (READ_BIT(husart->Instance->CR1, USART_CR1_TCIE) != USART_CR1_TCIE))
          {
            /* TxRx process is completed, restore husart->State to Ready */
            husart->State = HAL_USART_STATE_READY;
            
            HAL_USART_TxRxCpltCallback(husart);
          }     
        }
#if defined(USART_CR2_SLVEN)
        else if ((husart->State == HAL_USART_STATE_BUSY_RX) &&
                 (husart->SlaveMode == USART_SLAVEMODE_DISABLE))
#else
        else if (husart->State == HAL_USART_STATE_BUSY_RX)
#endif
        {
          /* Send dummy byte in order to generate the clock for the Slave to Send the next data */
          husart->Instance->TDR = (USART_DUMMY_DATA & (uint16_t)0x00FF);
        }
      }
    }
    
    /* When remaining number of bytes to receive is less than the RX FIFO 
    threshold, next incoming frames are processed as if FIFO mode was 
    disabled (i.e. one interrupt per received frame).
    */
    if (((husart->RxXferCount != 0U)) && (husart->RxXferCount < husart->NbRxDataToProcess))
    {
      /* Disable the USART RXFT interrupt*/
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_RXFTIE);
      
      /* Update the RxISR function pointer */
      husart->RxISR = USART_RxISR_8BIT;
      
      /* Enable the USART Data Register Not Empty interrupt */
      SET_BIT(husart->Instance->CR1, USART_CR1_RXNEIE_RXFNEIE);
      
#if defined(USART_CR2_SLVEN)
      if ( (husart->State == HAL_USART_STATE_BUSY_TX_RX) &&
           (husart->SlaveMode == USART_SLAVEMODE_DISABLE) &&
           (husart->TxXferCount == 0U))
#else
      if ( (husart->State == HAL_USART_STATE_BUSY_TX_RX) &&
           (husart->TxXferCount == 0U))
#endif
      {
        /* Send dummy byte in order to generate the clock for the Slave to Send the next data */
        husart->Instance->TDR = (USART_DUMMY_DATA & (uint16_t)0x00FF);
      }
    }
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_USART_SEND_REQ(husart, USART_RXDATA_FLUSH_REQUEST);
  }
}

/**
  * @brief  Simplex receive an amount of data in non-blocking mode.
  * @note   Function called under interruption only, once
  *         interruptions have been enabled by HAL_USART_Receive_IT().
  * @note   ISR function executed when FIFO mode is enabled and when the
  *         data word length is 9 bits long.
  * @param  husart USART handle
  * @retval None
  */
static void USART_RxISR_16BIT_FIFOEN(USART_HandleTypeDef *husart)
{
  uint16_t* tmp;
  uint16_t uhMask = husart->Mask;
  uint8_t  nb_rx_data;
  
  /* Check that a Tx process is ongoing */
  if ((husart->State == HAL_USART_STATE_BUSY_RX) ||
      (husart->State == HAL_USART_STATE_BUSY_TX_RX))
  {
    for(nb_rx_data = husart->NbRxDataToProcess ; nb_rx_data > 0 ; nb_rx_data--)
    { 
      if (__HAL_USART_GET_FLAG(husart, USART_FLAG_RXFNE) == SET)
      {
        tmp = (uint16_t*) husart->pRxBuffPtr;
        *tmp = (uint16_t)(husart->Instance->RDR & uhMask);
        husart->pRxBuffPtr += 2;
        
        if(--husart->RxXferCount == 0)
        {
          /* Disable the USART Parity Error Interrupt */
          CLEAR_BIT(husart->Instance->CR1, USART_CR1_PEIE);
          
          /* Disable the USART Error Interrupt: (Frame error, noise error, overrun error) and RX FIFO Threshold interrupt */
          CLEAR_BIT(husart->Instance->CR3, (USART_CR3_EIE | USART_CR3_RXFTIE));
          
          /* Clear RxISR function pointer */
          husart->RxISR = NULL;
          
          if(husart->State == HAL_USART_STATE_BUSY_RX)
          {
#if defined(USART_CR2_SLVEN)
            /* Clear SPI slave underrun flag and discard transmit data */
            if (husart->SlaveMode == USART_SLAVEMODE_ENABLE)
            {
              __HAL_USART_CLEAR_UDRFLAG(husart);
              __HAL_USART_SEND_REQ(husart, USART_TXDATA_FLUSH_REQUEST);
            }
#endif
            
            /* Rx process is completed, restore husart->State to Ready */
            husart->State = HAL_USART_STATE_READY;
            
            HAL_USART_RxCpltCallback(husart);
          } 
          else if ((husart->TxXferCount == 0) &&
                   (READ_BIT(husart->Instance->CR1, USART_CR1_TCIE) != USART_CR1_TCIE))
          {
            /* TxRx process is completed, restore husart->State to Ready */
            husart->State = HAL_USART_STATE_READY;
            
            HAL_USART_TxRxCpltCallback(husart);
          }
        }
#if defined(USART_CR2_SLVEN)
        else if ((husart->State == HAL_USART_STATE_BUSY_RX) &&
                 (husart->SlaveMode == USART_SLAVEMODE_DISABLE))
#else
        else if (husart->State == HAL_USART_STATE_BUSY_RX)
#endif
        {
          /* Send dummy byte in order to generate the clock for the Slave to Send the next data */
          husart->Instance->TDR = (USART_DUMMY_DATA & (uint16_t)0x00FF);
        }
      }
    }
    
    /* When remaining number of bytes to receive is less than the RX FIFO 
    threshold, next incoming frames are processed as if FIFO mode was 
    disabled (i.e. one interrupt per received frame).
    */
    if (((husart->RxXferCount != 0U)) && (husart->RxXferCount < husart->NbRxDataToProcess))
    {
      /* Disable the USART RXFT interrupt*/
      CLEAR_BIT(husart->Instance->CR3, USART_CR3_RXFTIE);
      
      /* Update the RxISR function pointer */
      husart->RxISR = USART_RxISR_16BIT;
      
      /* Enable the USART Data Register Not Empty interrupt */
      SET_BIT(husart->Instance->CR1, USART_CR1_RXNEIE_RXFNEIE);
      
#if defined(USART_CR2_SLVEN)
      if ( (husart->State == HAL_USART_STATE_BUSY_TX_RX) &&
           (husart->SlaveMode == USART_SLAVEMODE_DISABLE) &&
           (husart->TxXferCount == 0U))
#else
      if ( (husart->State == HAL_USART_STATE_BUSY_TX_RX) &&
           (husart->TxXferCount == 0U))
#endif
      {
        /* Send dummy byte in order to generate the clock for the Slave to Send the next data */
        husart->Instance->TDR = (USART_DUMMY_DATA & (uint16_t)0x00FF);
      }
    }
  }
  else
  {
    /* Clear RXNE interrupt flag */
    __HAL_USART_SEND_REQ(husart, USART_RXDATA_FLUSH_REQUEST);
  }
}
#endif

/**
  * @}
  */

#endif /* HAL_USART_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

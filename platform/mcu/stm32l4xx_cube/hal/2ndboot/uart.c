#include "main.h"
#include "bootloader.h"
#include "stm32l4xx_hal.h"

#define LPUARTx                           LPUART1
#define LPUARTx_CLK_ENABLE()              __HAL_RCC_LPUART1_CLK_ENABLE();
#define LPUARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define LPUARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define LPUARTx_FORCE_RESET()             __HAL_RCC_LPUART1_FORCE_RESET()
#define LPUARTx_RELEASE_RESET()           __HAL_RCC_LPUART1_RELEASE_RESET()

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Definition for LPUART Pins */
#define LPUARTx_TX_PIN                    GPIO_PIN_11
#define LPUARTx_TX_GPIO_PORT              GPIOB
#define LPUARTx_TX_AF                     GPIO_AF8_LPUART1
#define LPUARTx_RX_PIN                    GPIO_PIN_10
#define LPUARTx_RX_GPIO_PORT              GPIOB
#define LPUARTx_RX_AF                     GPIO_AF8_LPUART1

/* Definition for LPUART's NVIC */
#define LPUARTx_IRQn                      LPUART1_IRQn
#define LPUARTx_IRQHandler                LPUART1_IRQHandler

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE

#define USART_CR1_FIELDS  ((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | \
                                      USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8 ))
#define USART_CR3_FIELDS  ((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT))

#define LPUART_BRR_MIN  0x00000300U
#define LPUART_BRR_MAX  0x000FFFFFU

#define UART_BRR_MIN    0x10U
#define UART_BRR_MAX    0x0000FFFFU

#define UART_RECE_SIZE (64)
UART_HandleTypeDef hlpuart1;
uint8_t aRxByte = 0;
uint8_t aRxBuffer[UART_RECE_SIZE];
uint8_t aRx_ptr = 0;
uint8_t aRx_ctr = 0;

__IO ITStatus UartReady = RESET;

extern void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);

static void UART_TxISR_8BIT(UART_HandleTypeDef *huart)
{
    if (huart->gState == HAL_UART_STATE_BUSY_TX) {
        if (huart->TxXferCount == 0) {
            CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE);
            SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
        }
        else {
            huart->Instance->TDR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0xFF);
            huart->TxXferCount--;
        }
    }
}

static void UART_TxISR_16BIT(UART_HandleTypeDef *huart)
{
    uint16_t *tmp;
    if (huart->gState == HAL_UART_STATE_BUSY_TX) {
        if (huart->TxXferCount == 0) {
            CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE);
            SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
        }
        else {
            tmp = (uint16_t *) huart->pTxBuffPtr;
            huart->Instance->TDR = (*tmp & (uint16_t)0x01FF);
            huart->pTxBuffPtr += 2;
            huart->TxXferCount--;
        }
    }
}

HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    if (huart->gState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }
        __HAL_LOCK(huart);
        huart->pTxBuffPtr  = pData;
        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;
        huart->TxISR       = NULL;
        huart->ErrorCode = HAL_UART_ERROR_NONE;
        huart->gState = HAL_UART_STATE_BUSY_TX;
        if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE)) {
            huart->TxISR = UART_TxISR_16BIT;
        }
        else {
            huart->TxISR = UART_TxISR_8BIT;
        }
        __HAL_UNLOCK(huart);
        SET_BIT(huart->Instance->CR1, USART_CR1_TXEIE);
        return HAL_OK;
    }
    else {
        return HAL_BUSY;
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    LPUARTx_TX_GPIO_CLK_ENABLE();
    LPUARTx_RX_GPIO_CLK_ENABLE();
    LPUARTx_CLK_ENABLE();
    GPIO_InitStruct.Pin       = LPUARTx_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = LPUARTx_TX_AF;
    HAL_GPIO_Init(LPUARTx_TX_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin       = LPUARTx_RX_PIN;
    GPIO_InitStruct.Alternate = LPUARTx_RX_AF;
    HAL_GPIO_Init(LPUARTx_RX_GPIO_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(LPUARTx_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(LPUARTx_IRQn);
}

HAL_StatusTypeDef UART_SetConfig(UART_HandleTypeDef *huart)
{
    uint32_t tmpreg                     = 0x00000000U;
    UART_ClockSourceTypeDef clocksource = UART_CLOCKSOURCE_UNDEFINED;
    uint16_t brrtemp                    = 0x0000U;
    uint32_t usartdiv                   = 0x00000000U;
    HAL_StatusTypeDef ret               = HAL_OK;
    uint32_t lpuart_ker_ck_pres         = 0x00000000U;

    assert_param(IS_UART_BAUDRATE(huart->Init.BaudRate));
    assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
    if (UART_INSTANCE_LOWPOWER(huart)) {
        assert_param(IS_LPUART_STOPBITS(huart->Init.StopBits));
    }
    else {
        assert_param(IS_UART_STOPBITS(huart->Init.StopBits));
        assert_param(IS_UART_ONE_BIT_SAMPLE(huart->Init.OneBitSampling));
    }
    tmpreg = (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode | huart->Init.OverSampling ;
    MODIFY_REG(huart->Instance->CR1, USART_CR1_FIELDS, tmpreg);
    MODIFY_REG(huart->Instance->CR2, USART_CR2_STOP, huart->Init.StopBits);
    tmpreg = (uint32_t)huart->Init.HwFlowCtl;
    if (!(UART_INSTANCE_LOWPOWER(huart))) {
        tmpreg |= huart->Init.OneBitSampling;
    }
    MODIFY_REG(huart->Instance->CR3, USART_CR3_FIELDS, tmpreg);
    UART_GETCLOCKSOURCE(huart, clocksource);
    if (UART_INSTANCE_LOWPOWER(huart)) {
        lpuart_ker_ck_pres = HAL_RCC_GetPCLK1Freq();
        if (lpuart_ker_ck_pres != 0U) {
            if ((lpuart_ker_ck_pres < (3 * huart->Init.BaudRate)) ||
                (lpuart_ker_ck_pres > (4096 * huart->Init.BaudRate))) {
                ret = HAL_ERROR;
            }
            else {
                usartdiv = (uint32_t)(UART_DIV_LPUART(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate));
                if ((usartdiv >= LPUART_BRR_MIN) && (usartdiv <= LPUART_BRR_MAX)) {
                    huart->Instance->BRR = usartdiv;
                }
                else {
                    ret = HAL_ERROR;
                }
            }
        }
    }
    else if (huart->Init.OverSampling == UART_OVERSAMPLING_8) {
        usartdiv = (uint16_t)(UART_DIV_SAMPLING8(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate));
        if ((usartdiv >= UART_BRR_MIN) && (usartdiv <= UART_BRR_MAX)) {
            brrtemp = usartdiv & 0xFFF0U;
            brrtemp |= (uint16_t)((usartdiv & (uint16_t)0x000FU) >> 1U);
            huart->Instance->BRR = brrtemp;
        }
        else {
            ret = HAL_ERROR;
        }
    }
    else {
        usartdiv = (uint16_t)(UART_DIV_SAMPLING16(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate));
        if ((usartdiv >= UART_BRR_MIN) && (usartdiv <= UART_BRR_MAX)) {
            huart->Instance->BRR = usartdiv;
        }
        else {
            ret = HAL_ERROR;
        }
    }
    huart->RxISR = NULL;
    huart->TxISR = NULL;

    return ret;
}

void UART_AdvFeatureConfig(UART_HandleTypeDef *huart)
{
    assert_param(IS_UART_ADVFEATURE_INIT(huart->AdvancedInit.AdvFeatureInit));
    if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_TXINVERT_INIT)) {
        assert_param(IS_UART_ADVFEATURE_TXINV(huart->AdvancedInit.TxPinLevelInvert));
        MODIFY_REG(huart->Instance->CR2, USART_CR2_TXINV, huart->AdvancedInit.TxPinLevelInvert);
    }

    if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_RXINVERT_INIT)) {
        assert_param(IS_UART_ADVFEATURE_RXINV(huart->AdvancedInit.RxPinLevelInvert));
        MODIFY_REG(huart->Instance->CR2, USART_CR2_RXINV, huart->AdvancedInit.RxPinLevelInvert);
    }
    if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_DATAINVERT_INIT)) {
        assert_param(IS_UART_ADVFEATURE_DATAINV(huart->AdvancedInit.DataInvert));
        MODIFY_REG(huart->Instance->CR2, USART_CR2_DATAINV, huart->AdvancedInit.DataInvert);
    }
    if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_SWAP_INIT)) {
        assert_param(IS_UART_ADVFEATURE_SWAP(huart->AdvancedInit.Swap));
        MODIFY_REG(huart->Instance->CR2, USART_CR2_SWAP, huart->AdvancedInit.Swap);
    }
    if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_RXOVERRUNDISABLE_INIT)) {
        assert_param(IS_UART_OVERRUN(huart->AdvancedInit.OverrunDisable));
        MODIFY_REG(huart->Instance->CR3, USART_CR3_OVRDIS, huart->AdvancedInit.OverrunDisable);
    }
    if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_DMADISABLEONERROR_INIT)) {
        assert_param(IS_UART_ADVFEATURE_DMAONRXERROR(huart->AdvancedInit.DMADisableonRxError));
        MODIFY_REG(huart->Instance->CR3, USART_CR3_DDRE, huart->AdvancedInit.DMADisableonRxError);
    }
    if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_AUTOBAUDRATE_INIT)) {
        assert_param(IS_USART_AUTOBAUDRATE_DETECTION_INSTANCE(huart->Instance));
        assert_param(IS_UART_ADVFEATURE_AUTOBAUDRATE(huart->AdvancedInit.AutoBaudRateEnable));
        MODIFY_REG(huart->Instance->CR2, USART_CR2_ABREN, huart->AdvancedInit.AutoBaudRateEnable);
        if (huart->AdvancedInit.AutoBaudRateEnable == UART_ADVFEATURE_AUTOBAUDRATE_ENABLE) {
            assert_param(IS_UART_ADVFEATURE_AUTOBAUDRATEMODE(huart->AdvancedInit.AutoBaudRateMode));
            MODIFY_REG(huart->Instance->CR2, USART_CR2_ABRMODE, huart->AdvancedInit.AutoBaudRateMode);
        }
    }
    if (HAL_IS_BIT_SET(huart->AdvancedInit.AdvFeatureInit, UART_ADVFEATURE_MSBFIRST_INIT)) {
        assert_param(IS_UART_ADVFEATURE_MSBFIRST(huart->AdvancedInit.MSBFirst));
        MODIFY_REG(huart->Instance->CR2, USART_CR2_MSBFIRST, huart->AdvancedInit.MSBFirst);
    }
}

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
    if (huart == NULL) {
        return HAL_ERROR;
    }
    if (huart->Init.HwFlowCtl != UART_HWCONTROL_NONE) {
        assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
    }
    else {
        assert_param((IS_UART_INSTANCE(huart->Instance)) || (IS_LPUART_INSTANCE(huart->Instance)));
    }
    if (huart->gState == HAL_UART_STATE_RESET) {
        huart->Lock = HAL_UNLOCKED;
        HAL_UART_MspInit(huart);
    }
    huart->gState = HAL_UART_STATE_BUSY;
    __HAL_UART_DISABLE(huart);
    if (UART_SetConfig(huart) == HAL_ERROR) {
        return HAL_ERROR;
    }
    if (huart->AdvancedInit.AdvFeatureInit != UART_ADVFEATURE_NO_INIT) {
        UART_AdvFeatureConfig(huart);
    }
    CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
    CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));
    __HAL_UART_ENABLE(huart);
    return (UART_CheckIdleState(huart));
}

HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status, uint32_t Tickstart, uint32_t Timeout)
{
    while ((__HAL_UART_GET_FLAG(huart, Flag) ? SET : RESET) == Status) {
        if (Timeout != HAL_MAX_DELAY) {
            if ((Timeout == 0U) || ((HAL_GetTick() - Tickstart) > Timeout)) {
                CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE));
                CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
                huart->gState = HAL_UART_STATE_READY;
                huart->RxState = HAL_UART_STATE_READY;
                __HAL_UNLOCK(huart);
                return HAL_TIMEOUT;
            }
        }
    }
    return HAL_OK;
}

HAL_StatusTypeDef UART_CheckIdleState(UART_HandleTypeDef *huart)
{
    uint32_t tickstart = 0U;
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    tickstart = HAL_GetTick();
    if ((huart->Instance->CR1 & USART_CR1_TE) == USART_CR1_TE) {
        if (UART_WaitOnFlagUntilTimeout(huart, USART_ISR_TEACK, RESET, tickstart, HAL_UART_TIMEOUT_VALUE) != HAL_OK) {
            return HAL_TIMEOUT;
        }
    }
    if ((huart->Instance->CR1 & USART_CR1_RE) == USART_CR1_RE) {
        if (UART_WaitOnFlagUntilTimeout(huart, USART_ISR_REACK, RESET, tickstart, HAL_UART_TIMEOUT_VALUE) != HAL_OK) {
            return HAL_TIMEOUT;
        }
    }
    huart->gState = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
    __HAL_UNLOCK(huart);
    return HAL_OK;
}

static void UART_RxISR_8BIT(UART_HandleTypeDef *huart)
{
    uint16_t uhMask = huart->Mask;
    uint16_t  uhdata;
    if (huart->RxState == HAL_UART_STATE_BUSY_RX) {
        uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
        *huart->pRxBuffPtr++ = (uint8_t)(uhdata & (uint8_t)uhMask);
        if (--huart->RxXferCount == 0) {
            CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
            CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
            huart->RxState = HAL_UART_STATE_READY;
            huart->RxISR = NULL;
            HAL_UART_RxCpltCallback(huart);
        }
    }
    else {
        __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
    }
}

static void UART_RxISR_16BIT(UART_HandleTypeDef *huart)
{
    uint16_t *tmp;
    uint16_t uhMask = huart->Mask;
    uint16_t  uhdata;
    if (huart->RxState == HAL_UART_STATE_BUSY_RX) {
        uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
        tmp = (uint16_t *) huart->pRxBuffPtr ;
        *tmp = (uint16_t)(uhdata & uhMask);
        huart->pRxBuffPtr += 2;
        if (--huart->RxXferCount == 0) {
            CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
            CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
            huart->RxState = HAL_UART_STATE_READY;
            huart->RxISR = NULL;
            HAL_UART_RxCpltCallback(huart);
        }
    }
    else {
        __HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
    }
}

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    if (huart->RxState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return HAL_ERROR;
        }
        __HAL_LOCK(huart);
        huart->pRxBuffPtr  = pData;
        huart->RxXferSize  = Size;
        huart->RxXferCount = Size;
        huart->RxISR       = NULL;
        UART_MASK_COMPUTATION(huart);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
        huart->RxState = HAL_UART_STATE_BUSY_RX;
        SET_BIT(huart->Instance->CR3, USART_CR3_EIE);
        if ((huart->Init.WordLength == UART_WORDLENGTH_9B)
            && (huart->Init.Parity == UART_PARITY_NONE)) {
            huart->RxISR = UART_RxISR_16BIT;
        }
        else {
            huart->RxISR = UART_RxISR_8BIT;
        }
        __HAL_UNLOCK(huart);
        SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
        return HAL_OK;
    }
    else {
        return HAL_BUSY;
    }
}

__weak void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

static void UART_EndTransmit_IT(UART_HandleTypeDef *huart)
{
    CLEAR_BIT(huart->Instance->CR1, USART_CR1_TCIE);
    huart->gState = HAL_UART_STATE_READY;
    huart->TxISR = NULL;
    HAL_UART_TxCpltCallback(huart);
}

static void UART_EndRxTransfer(UART_HandleTypeDef *huart)
{
    CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
    huart->RxState = HAL_UART_STATE_READY;
    huart->RxISR = NULL;
}

__weak void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

__weak void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    uint32_t isrflags   = READ_REG(huart->Instance->ISR);
    uint32_t cr1its     = READ_REG(huart->Instance->CR1);
    uint32_t cr3its     = READ_REG(huart->Instance->CR3);
    uint32_t errorflags;

    errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
    if (errorflags == RESET) {
        if (((isrflags & USART_ISR_RXNE) != RESET)
            && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
            if (huart->RxISR != NULL) {
                huart->RxISR(huart);
            }
            return;
        }
    }

    if ((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET)
        || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET))) {
        if (((isrflags & USART_ISR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET)) {
            __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);
            huart->ErrorCode |= HAL_UART_ERROR_PE;
        }
        if (((isrflags & USART_ISR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET)) {
            __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);
            huart->ErrorCode |= HAL_UART_ERROR_FE;
        }

        if (((isrflags & USART_ISR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET)) {
            __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);
            huart->ErrorCode |= HAL_UART_ERROR_NE;
        }
        if (((isrflags & USART_ISR_ORE) != RESET) && (((cr1its & USART_CR1_RXNEIE) != RESET) ||
            ((cr3its & USART_CR3_EIE) != RESET))) {
            __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
            huart->ErrorCode |= HAL_UART_ERROR_ORE;
        }
        if (huart->ErrorCode != HAL_UART_ERROR_NONE) {
            if (((isrflags & USART_ISR_RXNE) != RESET)
                && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
                if (huart->RxISR != NULL) {
                    huart->RxISR(huart);
                }
            }
            if (((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) ||
                (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))) {
                UART_EndRxTransfer(huart);
                if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)) {
                    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
                }
                else {
                    HAL_UART_ErrorCallback(huart);
                }
            }
            else {
                HAL_UART_ErrorCallback(huart);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
            }
        }
        return;
    }

    if (((isrflags & USART_ISR_WUF) != RESET) && ((cr3its & USART_CR3_WUFIE) != RESET)) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_WUF);
        HAL_UARTEx_WakeupCallback(huart);
        return;
    }
    if (((isrflags & USART_ISR_TXE) != RESET)
        && ((cr1its & USART_CR1_TXEIE) != RESET)) {
        if (huart->TxISR != NULL) {
            huart->TxISR(huart);
        }
        return;
    }

    if (((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET)) {
        UART_EndTransmit_IT(huart);
        return;
    }
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    uint16_t *tmp;
    uint32_t tickstart = 0U;
    if (huart->gState == HAL_UART_STATE_READY) {
        if ((pData == NULL) || (Size == 0U)) {
            return  HAL_ERROR;
        }
        __HAL_LOCK(huart);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
        huart->gState = HAL_UART_STATE_BUSY_TX;
        tickstart = HAL_GetTick();
        huart->TxXferSize  = Size;
        huart->TxXferCount = Size;
        while (huart->TxXferCount > 0U) {
            if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, tickstart, Timeout) != HAL_OK) {
                return HAL_TIMEOUT;
            }
            if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE)) {
                tmp = (uint16_t *) pData;
                huart->Instance->TDR = (*tmp & (uint16_t)0x01FFU);
                pData += 2U;
            }
            else {
                huart->Instance->TDR = (*pData++ & (uint8_t)0xFFU);
            }
            huart->TxXferCount--;
        }
        if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, tickstart, Timeout) != HAL_OK) {
            return HAL_TIMEOUT;
        }
        huart->gState = HAL_UART_STATE_READY;
        __HAL_UNLOCK(huart);
        return HAL_OK;
    }
    else {
        return HAL_BUSY;
    }
}

void hal_uart_Init(void)
{

    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = 115200;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    hlpuart1.Init.OverSampling = UART_OVERSAMPLING_16;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    if (HAL_UART_Init(&hlpuart1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    if(HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)&aRxByte, 1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    /* Set transmission flag: trasfer complete*/
    UartReady = SET;
    if(HAL_UART_Receive_IT(UartHandle, (unsigned char *)&aRxByte, 1) != HAL_OK) {
        ;
    }
    aRxBuffer[aRx_ptr++] = aRxByte;
    aRx_ptr = aRx_ptr % UART_RECE_SIZE;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    HAL_StatusTypeDef ret;

    if (data == NULL) {
        return -1;
    }

    if (size == 0) {
        return -1;
    }

    ret = HAL_UART_Transmit(&hlpuart1, data, size, timeout);
    if (ret != HAL_OK) {
        return -1;
    }

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    unsigned char *buff = NULL;
    uint32_t       size = 0;
    unsigned char  c;

    if ((data == NULL) || (recv_size == NULL)) {
        return -1;
    }

    if (expect_size == 0) {
        return -1;
    }

    buff = (unsigned char *)data;

    unsigned char rx_done = 0;
    while(aRx_ctr != aRx_ptr) {
        *buff = aRxBuffer[aRx_ctr++];
        aRx_ctr = aRx_ctr % UART_RECE_SIZE;

        size++;
        buff++;
        if (size == expect_size) {
            break;
        }
    }

    *recv_size = size;
    return 0;
}


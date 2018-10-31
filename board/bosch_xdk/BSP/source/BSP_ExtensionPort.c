/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file
 * @brief  Implementation of Extension port BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_EXTENSION_PORT

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_EXTENSIONPORT

#include "BSP_ExtensionPort.h"
#include "BspExtensionPort.h"
#include "BSP_BoardExtSettings.h"
#include "BSP_BoardShared.h"
#include "BCDS_MCU_I2C_Handle.h"
#include "em_i2c.h"
#include "BCDS_MCU_SPI_Handle.h"
#include "BCDS_MCU_DMA_Handle.h"
#include "BCDS_MCU_UART_Handle.h"
#include "em_usart.h"
#include "BCDS_MCU_UART.h"

/* Put the type and macro definitions here */

/* Put constant and variable definitions here */

/* Handle instance for I2C driver */
static struct MCU_I2C_Handle_S Bsp_ExtensionPortI2cInstance;

static struct MCU_I2C_Handle_S *Bsp_ExtensionPortI2cHandle = NULL;

static I2C_IRQ_Callback_T Bsp_ExtensionPort_McuI2cIrqCallback = NULL;

struct MCU_SPI_Handle_S Bsp_ExtensionPortSpiInstance;

struct MCU_SPI_Handle_S *Bsp_ExtensionPortSpiHandle = NULL;

#if BCDS_FEATURE_DMA
struct MCU_DMA_Channel_S Bsp_ExtensionPortSpiDmaTxCh;
struct MCU_DMA_Channel_S Bsp_ExtensionPortSpiDmaRxCh;

static SPI_DMA_Callback_T Bsp_ExtensionPortSpiTxDmaCb = NULL;
static SPI_DMA_Callback_T Bsp_ExtensionPortSpiRxDmaCb = NULL;
#endif
static SPI_IRQ_Callback_T Bsp_ExtensionPortSpiRxIsrCb;
static SPI_IRQ_Callback_T Bsp_ExtensionPortSpiTxIsrCb;

static UART_IRQ_Callback_T Bsp_ExtensionPortUartRxIsrCallback;
static UART_IRQ_Callback_T Bsp_ExtensionPortUartTxIsrCallback;
static struct MCU_UART_Handle_S Bsp_ExtensionPortUartInstance;
static struct MCU_UART_Handle_S *Bsp_ExtensionPortUartHandle = NULL;

/* Table to map  the user input enumeration BSP_ExtensionPortGpioPinMode_T to efm32 mode enumeration GPIO_Mode_TypeDef
 * @note The order it is populated is one -to-one mapping and hence the implementation must always ensure the table
 *       is in same order as it is enumerated in  BSP_ExtensionPortGpioPinMode_T.
 *
 * @warning Failing to take care of the above note may result in wrong configuration */
const GPIO_Mode_TypeDef BSP_ExtensionPortModeTable[BSP_EXTENSIONPORT_MODE_OUT_OF_RANGE] =
{
    gpioModeInput,
    gpioModeInputPull,
    gpioModeInputPull,
    gpioModeInputPullFilter,
    gpioModePushPull,
    gpioModePushPullDrive,
    gpioModeWiredOr,
    gpioModeWiredOrPullDown,
    gpioModeWiredAnd,
    gpioModeWiredAndFilter,
    gpioModeWiredAndPullUp,
    gpioModeWiredAndPullUpFilter
};

const BSP_ExtensionPort_Table_T BSP_ExtensionPortTable[BSP_EXTENSIONPORT_GPIO_MAX] =
{
    {   EXTENSION_PORT_GPIO_PA0, EXTENSION_PIN_GPIO_PA0},
    {   EXTENSION_PORT_GPIO_PA1, EXTENSION_PIN_GPIO_PA1},
    {   EXTENSION_PORT_GPIO_PB2, EXTENSION_PIN_GPIO_PB2},
    {   EXTENSION_PORT_GPIO_PB3, EXTENSION_PIN_GPIO_PB3},
    {   EXTENSION_PORT_GPIO_PB4, EXTENSION_PIN_GPIO_PB4},
    {   EXTENSION_PORT_GPIO_PB5, EXTENSION_PIN_GPIO_PB5},
    {   EXTENSION_PORT_GPIO_PB9, EXTENSION_PIN_GPIO_PB9},
    {   EXTENSION_PORT_GPIO_PB10, EXTENSION_PIN_GPIO_PB10},
    {   EXTENSION_PORT_GPIO_PB11, EXTENSION_PIN_GPIO_PB11},
    {   EXTENSION_PORT_GPIO_PB12, EXTENSION_PIN_GPIO_PB12},
    {   EXTENSION_PORT_GPIO_PC0, EXTENSION_PIN_GPIO_PC0},
    {   EXTENSION_PORT_GPIO_PC1, EXTENSION_PIN_GPIO_PC1},
    {   EXTENSION_PORT_GPIO_PC2, EXTENSION_PIN_GPIO_PC2},
    {   EXTENSION_PORT_GPIO_PC3, EXTENSION_PIN_GPIO_PC3},
    {   EXTENSION_PORT_GPIO_PC4, EXTENSION_PIN_GPIO_PC4},
    {   EXTENSION_PORT_GPIO_PC8, EXTENSION_PIN_GPIO_PC8},
    {   EXTENSION_PORT_GPIO_PC9, EXTENSION_PIN_GPIO_PC9},
    {   EXTENSION_PORT_GPIO_PC10, EXTENSION_PIN_GPIO_PC10},
    {   EXTENSION_PORT_GPIO_PD5, EXTENSION_PIN_GPIO_PD5},
    {   EXTENSION_PORT_GPIO_PD6, EXTENSION_PIN_GPIO_PD6},
    {   EXTENSION_PORT_GPIO_PD8, EXTENSION_PIN_GPIO_PD8},
    {   EXTENSION_PORT_GPIO_PE2, EXTENSION_PIN_GPIO_PE2},
    {   EXTENSION_PORT_GPIO_PF6, EXTENSION_PIN_GPIO_PF6},
};

BSP_ExtensionPort_Handle_T BSP_ExtensionPortInstance[BSP_EXTENSIONPORT_GPIO_MAX] =
{
    {   EXTENSION_MODE_GPIO_PA0, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PA1, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PB2, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PB3, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PB4, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PB5, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PB9, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PB10, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PB11, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PB12, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PC0, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PC1, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PC2, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PC3, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PC4, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PC8, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PC9, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PC10, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PD5, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PD6, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PD8, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PE2, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false},
    {   EXTENSION_MODE_GPIO_PF6, (uint8_t) BSP_EXTENSIONPORT_RESET_STATE, false, 0, false, false, false, 0, false}
};

/* Put private function declarations here */

/*
 * @brief  This static function is used to set the ownership state for I2c pins
 *
 * @retval RETCODE_OK in the case I2C ownership is set properly
 * @retval RETCODE_INVALID_PARAM in case of error
 */
static Retcode_T SetI2cOwnership(BSP_ExtensionPort_gpioOwner_T pinOwner)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdI2cData, pinIdI2cClk;
    pinIdI2cData = BSP_EXTENSIONPORT_GPIO_PB11; /* I2C1 data pin */
    pinIdI2cClk = BSP_EXTENSIONPORT_GPIO_PB12; /* I2C1 clock pin */
    retVal = BSP_ExtensionPort_SetGpioOwner(pinIdI2cData, pinOwner);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioOwner(pinIdI2cClk, pinOwner);
    }
    return retVal;
}

/*
 * @brief  This static function is used to verify the ownership state of I2c pins
 *
 * @retval  RETCODE_OK if the pins are available for I2C connect
 * @retval  RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE  if the pins are already assigned for some other functions
 */
static Retcode_T IsI2cPortAvailable(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdI2cData, pinIdI2cClk;
    pinIdI2cData = BSP_EXTENSIONPORT_GPIO_PB11; /* I2C1 data pin */
    pinIdI2cClk = BSP_EXTENSIONPORT_GPIO_PB12; /* I2C1 clock pin */
    BSP_ExtensionPort_gpioOwner_T getgpioOwnerI2cData, getgpioOwnerI2cClk;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinIdI2cData, &getgpioOwnerI2cData);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_GetGpioOwner(pinIdI2cClk, &getgpioOwnerI2cClk);
    }
    if (RETCODE_OK == retVal)
    {
        if ((BSP_EXTENSIONPORT_RESET_STATE != getgpioOwnerI2cData) || (BSP_EXTENSIONPORT_RESET_STATE != getgpioOwnerI2cClk))
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE);
        }
    }
    return retVal;
}

static bool CheckIrqNotRegistred(BSP_ExtensionPort_PinId_T pinID)
{
    GPIOINT_IrqCallbackPtr_t callback;
    callback = Board_GPIOInterruptGetCallback(BSP_ExtensionPortTable[pinID].Pin);
    if (NULL != callback)
    {
        return false;
    }
    else
    {
        return true;
    }
}
static bool getPinDirectionFromMode(BSP_ExtensionPortGpioPinMode_T mode)
{

    bool result = false; // output pin
    if (BSP_EXTENSIONPORT_INPUTPULLFILTER >= mode)
    {
        result = true; // Input pin
    }
    return (result);
}

static void TristateAllExtensionPortGpios(void)
{
    uint32_t index = 0;
    do
    {
        GPIO_PinModeSet(BSP_ExtensionPortTable[index].Port, BSP_ExtensionPortTable[index].Pin, gpioModeDisabled, 0);
        index++;
    }while ((uint32_t) BSP_EXTENSIONPORT_GPIO_MAX > index);

}
#if BCDS_FEATURE_DMA
static void Bsp_ExtPortSpiRxDMAComplete(uint32_t channel, bool primary, void * user)
{
    BCDS_UNUSED(channel);
    BCDS_UNUSED(primary);
    BCDS_UNUSED(user);
    if (NULL != Bsp_ExtensionPortSpiRxDmaCb)
    {
        /* interrupt processing is to be done at the MCU level */
        Bsp_ExtensionPortSpiRxDmaCb((SPI_T) Bsp_ExtensionPortSpiHandle);
    }
}

static void Bsp_ExtPortSpiTxDMAComplete(uint32_t channel, bool primary, void * user)
{
    BCDS_UNUSED(channel);
    BCDS_UNUSED(primary);
    BCDS_UNUSED(user);
    if (NULL != Bsp_ExtensionPortSpiTxDmaCb)
    {
        /* interrupt processing is to be done at the MCU level */
        Bsp_ExtensionPortSpiTxDmaCb((SPI_T) Bsp_ExtensionPortSpiHandle);
    }
}
#endif

/*
 * @brief  This static function is used to verify the ownership state of SPI pins
 *
 * @retval  RETCODE_OK if the pins are available for SPI connect
 * @retval  RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE  if the pins are already assigned for some other functions
 */
static Retcode_T IsSpiPortAvailable(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_gpioOwner_T getgpioOwnerSpiMiso, getgpioOwnerSpiMosi, getgpioOwnerSpiClk;
    retVal = BSP_ExtensionPort_GetGpioOwner(BSP_EXTENSIONPORT_GPIO_PB3, &getgpioOwnerSpiMosi);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_GetGpioOwner(BSP_EXTENSIONPORT_GPIO_PB4, &getgpioOwnerSpiMiso);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_GetGpioOwner(BSP_EXTENSIONPORT_GPIO_PB5, &getgpioOwnerSpiClk);
    }
    if (RETCODE_OK == retVal)
    {
        if ((BSP_EXTENSIONPORT_RESET_STATE != getgpioOwnerSpiMiso) || (BSP_EXTENSIONPORT_RESET_STATE != getgpioOwnerSpiMosi) || (BSP_EXTENSIONPORT_RESET_STATE != getgpioOwnerSpiClk))
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE);
        }
    }
    return retVal;
}

/*
 * @brief  This static function is used to verify the ownership state of ADC pins
 *
 * @retval  RETCODE_OK if the pins are available for ADC connect
 * @retval  RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE  if the pins are already assigned for some other functions
 * @retval RETCODE_INVALID_PARAM if the pins are not BSP_EXTENSIONPORT_GPIO_PD5 either BSP_EXTENSIONPORT_GPIO_PD6
 */
static Retcode_T IsAdcPortAvailable(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    /* Check needs to verify the pinID should be either BSP_EXTENSIONPORT_GPIO_PD5 or BSP_EXTENSIONPORT_GPIO_PD6*/
    if((BSP_EXTENSIONPORT_GPIO_PD5 != pinID) && (BSP_EXTENSIONPORT_GPIO_PD6 != pinID))
    {
    	retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
	if (RETCODE_OK == retVal)
	{
		retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
	}
	if (RETCODE_OK == retVal)
	{
		if (BSP_EXTENSIONPORT_RESET_STATE != getgpioOwner)
		{
			retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE);
		}
	}
    return retVal;
}

/* This static function is used to set the ownership state for SPI pins
 *
 * @retval RETCODE_OK  Successfully Set the PinOwner State to the Spi pins.
 * @retval RETCODE_INVALID_PARAM in case the passing parameter Spi pins are  out the range of GPIO Pin Id Table.
 *
 */
static Retcode_T SetSpiOwnership(BSP_ExtensionPort_gpioOwner_T pinOwner)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_SetGpioOwner(BSP_EXTENSIONPORT_GPIO_PB3, pinOwner);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioOwner(BSP_EXTENSIONPORT_GPIO_PB4, pinOwner);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioOwner(BSP_EXTENSIONPORT_GPIO_PB5, pinOwner);
    }
    return retVal;
}

/* @brief This static function is used to set the ownership state for UART pins
 *
 * @retval RETCODE_OK if the I2C ownership is set properly and error code otherwise.
 *
 */
static Retcode_T SetUartOwnership(BSP_ExtensionPort_gpioOwner_T pinOwner)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdTx, pinIdRx;

    pinIdTx = BSP_EXTENSIONPORT_GPIO_PB9; /*Tx pin */
    pinIdRx = BSP_EXTENSIONPORT_GPIO_PB10; /*Rx pin */

    retVal = BSP_ExtensionPort_SetGpioOwner(pinIdTx, pinOwner);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioOwner(pinIdRx, pinOwner);
    }
    return retVal;
}

/*
 *  This static function is used to get the ownership state of UART pins
 *  ----------------------------------------------------------------------------------------------------------------------------------
 *  | pinIdTx                           |     pinIdTx                       |  API output                                            |
 *  |-----------------------------------|-----------------------------------|--------------------------------------------------------|
 *  |USED_BY_UART                       |     USED_BY_UART                  |  RETCODE_OK                                            |
 *  |USED_BY_UART                       |     USED_BY_UART                  |  RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE                 |
 *  |-----------------------------------|-----------------------------------|--------------------------------------------------------|
 */

static Retcode_T IsUartPortAvailable(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdTx, pinIdRx;

    pinIdTx = BSP_EXTENSIONPORT_GPIO_PB9; /*Tx pin */
    pinIdRx = BSP_EXTENSIONPORT_GPIO_PB10; /*Rx pin */
    BSP_ExtensionPort_gpioOwner_T getgpioOwnerUartTx, getgpioOwnerUartRx;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinIdRx, &getgpioOwnerUartRx);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_GetGpioOwner(pinIdTx, &getgpioOwnerUartTx);
    }
    if (RETCODE_OK == retVal )
    {
        if((BSP_EXTENSIONPORT_RESET_STATE != getgpioOwnerUartRx) || (BSP_EXTENSIONPORT_RESET_STATE != getgpioOwnerUartTx))
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE);
        }
    }
    return retVal;
}

/*
 *  Refer for description BspExtensionPort.h
 *
 */
Retcode_T BSP_ExtensionPort_SetGpioOwner(BSP_ExtensionPort_PinId_T pinID, BSP_ExtensionPort_gpioOwner_T setOwner)
{
    Retcode_T retVal = RETCODE_OK;
    uint8_t setOwnerInfo = (uint8_t) setOwner;
    if (BSP_EXTENSIONPORT_GPIO_MAX <= pinID)
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        BSP_ExtensionPortInstance[pinID].gpioUsedFor = setOwnerInfo;
    }
    return retVal;
}

/*
 * Refer for description BspExtensionPort.h
 */
Retcode_T BSP_ExtensionPort_GetGpioOwner(BSP_ExtensionPort_PinId_T pinID, BSP_ExtensionPort_gpioOwner_T *getOwner)
{
    Retcode_T retVal = RETCODE_OK;
    if ((BSP_EXTENSIONPORT_GPIO_MAX <= pinID) || (NULL == getOwner))
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        *getOwner = (BSP_ExtensionPort_gpioOwner_T) BSP_ExtensionPortInstance[pinID].gpioUsedFor;
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_ConnectGpio(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    if (BSP_EXTENSIONPORT_GPIO_MAX <= pinID)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_RESET_STATE != getgpioOwner)
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE);
        }
        else
        {
            GPIO_PinModeSet(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin, gpioModeDisabled, 0);
        }
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioOwner(pinID, BSP_EXTENSIONPORT_PIN_USEDBY_GPIO);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_DisconnectGpio(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    if (BSP_EXTENSIONPORT_GPIO_MAX <= pinID)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_PIN_USEDBY_GPIO == getgpioOwner)
        {
            GPIO_PinModeSet(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin, gpioModeDisabled, 0);
            retVal = BSP_ExtensionPort_SetGpioOwner(pinID, BSP_EXTENSIONPORT_RESET_STATE);
        }
        else if (BSP_EXTENSIONPORT_RESET_STATE == getgpioOwner)
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED);
        }
        else
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE);
        }
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_SetGpioConfig(BSP_ExtensionPort_PinId_T pinID, BSP_ExtensionPortGpioConfig_T configItem, uint32_t value, void * pvalue)
{
    BCDS_UNUSED(pvalue);
    Retcode_T retVal = RETCODE_OK;
    if (BSP_EXTENSIONPORT_GPIO_MAX <= pinID)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_PIN_USEDBY_GPIO != getgpioOwner)
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE);
        }
        else if (BSP_EXTENSIONPORT_RESET_STATE == getgpioOwner)
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED);
        }
    }
    if (RETCODE_OK == retVal)
    {
        switch (configItem)
        {
            case BSP_EXTENSIONPORT_GPIO_PINMODE:
            if (BSP_EXTENSIONPORT_MODE_OUT_OF_RANGE <= (BSP_ExtensionPortGpioPinMode_T) value)
            {
                retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            }
            else
            {
                BSP_ExtensionPortInstance[pinID].Mode = BSP_ExtensionPortModeTable[value];
                BSP_ExtensionPortInstance[pinID].gpioPinDirection = getPinDirectionFromMode((BSP_ExtensionPortGpioPinMode_T) value);
            }
            break;
            case BSP_EXTENSIONPORT_GPIO_PINVALUE:
            if ((uint32_t) BSP_EXTENSIONPORT_GPIO_PIN_LOW == value)
            {
                BSP_ExtensionPortInstance[pinID].gpioPinValue = false;
            }
            else if ((uint32_t) BSP_EXTENSIONPORT_GPIO_PIN_HIGH == value)
            {
                BSP_ExtensionPortInstance[pinID].gpioPinValue = true;
            }
            else
            {
                retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            }
            break;
            default:
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
            break;
        }
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_RegisterGpioInt(BSP_ExtensionPort_PinId_T pinID, BSP_ExtensionPort_GpioIntMode_T interruptMode, BSP_ExtensionPort_GpioCallback_T gpioAppCallBack)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    if ((BSP_EXTENSIONPORT_GPIO_MAX <= (BSP_ExtensionPort_PinId_T) pinID) || (BSP_EXTENSIONPORT_INT_OUT_OF_RANGE <= (BSP_ExtensionPort_GpioIntMode_T) interruptMode) || (NULL == gpioAppCallBack))
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM));
    }

    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_PIN_USEDBY_GPIO != getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE));
        }
        else if (BSP_EXTENSIONPORT_RESET_STATE == getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED));
        }
    }
    if (false == BSP_ExtensionPortInstance[pinID].gpioPinDirection)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_INT_REQ_ON_OUTPUT_PIN));
    }

    if ((BSP_EXTENSIONPORT_INT_LEVEL_LOW == interruptMode) || (BSP_EXTENSIONPORT_INT_LEVEL_HIGH == interruptMode))
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED));
    }
    else if (BSP_EXTENSIONPORT_INT_EDGE_RAISING == interruptMode)
    {
        BSP_ExtensionPortInstance[pinID].gpioInterruptFallingEdge = false;
        BSP_ExtensionPortInstance[pinID].gpioInterruptRaisingEdge = true;
        BSP_ExtensionPortInstance[pinID].gpioInterruptType = true; // Edge Trigger
    }
    else if (BSP_EXTENSIONPORT_INT_EDGE_FALLING == interruptMode)
    {
        BSP_ExtensionPortInstance[pinID].gpioInterruptFallingEdge = true;
        BSP_ExtensionPortInstance[pinID].gpioInterruptRaisingEdge = false;
        BSP_ExtensionPortInstance[pinID].gpioInterruptType = true; // Edge Trigger
    }
    else if (BSP_EXTENSIONPORT_INT_EDGE_BOTH == interruptMode)
    {
        BSP_ExtensionPortInstance[pinID].gpioInterruptFallingEdge = true;
        BSP_ExtensionPortInstance[pinID].gpioInterruptRaisingEdge = true;
        BSP_ExtensionPortInstance[pinID].gpioInterruptType = true; // Edge Trigger
    }
    if (CheckIrqNotRegistred(pinID))
    {
        Board_GPIOInterruptRegisterCallback((uint8_t) BSP_ExtensionPortTable[pinID].Pin, (GPIOINT_IrqCallbackPtr_t) gpioAppCallBack);
        BSP_ExtensionPortInstance[pinID].gpioEnableInterrupt = true;
    }
    else
    {
        retVal = (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_INT_BUSY));
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_ToggleGpio(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    if ((BSP_EXTENSIONPORT_GPIO_MAX <= pinID))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_PIN_USEDBY_GPIO != getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE));
        }
        else if (BSP_EXTENSIONPORT_RESET_STATE == getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED));
        }
    }
    GPIO_PinOutToggle(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_SetGpio(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    if ((BSP_EXTENSIONPORT_GPIO_MAX <= pinID))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_PIN_USEDBY_GPIO != getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE));
        }
        else if (BSP_EXTENSIONPORT_RESET_STATE == getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED));
        }
    }
    GPIO_PinOutSet(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_ClearGpio(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    if ((BSP_EXTENSIONPORT_GPIO_MAX <= pinID))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_PIN_USEDBY_GPIO != getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE));
        }
        else if (BSP_EXTENSIONPORT_RESET_STATE == getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED));
        }
    }
    GPIO_PinOutClear(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_ReadGpio(BSP_ExtensionPort_PinId_T pinID, uint8_t *currentPinVal)
{
    Retcode_T retVal = RETCODE_OK;
    if ((BSP_EXTENSIONPORT_GPIO_MAX <= pinID) || (NULL == currentPinVal))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_PIN_USEDBY_GPIO != getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE));
        }
        else if (BSP_EXTENSIONPORT_RESET_STATE == getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED));
        }
    }
    *currentPinVal = GPIO_PinInGet(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_EnableGpio(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    uint32_t pinVal;
    if (BSP_EXTENSIONPORT_GPIO_MAX <= pinID)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_PIN_USEDBY_GPIO != getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE));
        }
        else if (BSP_EXTENSIONPORT_RESET_STATE == getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED));
        }
    }
    if (BSP_ExtensionPortInstance[pinID].gpioPinValue == true)
    {
        pinVal = UINT32_C(1);
    }
    else
    {
        pinVal = UINT32_C(0);
    }
    GPIO_PinModeSet(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin, BSP_ExtensionPortInstance[pinID].Mode, pinVal);
    if ( true == BSP_ExtensionPortInstance[pinID].gpioEnableInterrupt)
    {
        NVIC_DisableIRQ(GPIO_ODD_IRQn);
        NVIC_DisableIRQ(GPIO_EVEN_IRQn);
        GPIO_IntConfig(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin, BSP_ExtensionPortInstance[pinID].gpioInterruptRaisingEdge,
                BSP_ExtensionPortInstance[pinID].gpioInterruptFallingEdge, BSP_ENABLE);
        NVIC_EnableIRQ(GPIO_ODD_IRQn);
        NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_DisableGpio(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    if (BSP_EXTENSIONPORT_GPIO_MAX <= pinID)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    BSP_ExtensionPort_gpioOwner_T getgpioOwner;
    retVal = BSP_ExtensionPort_GetGpioOwner(pinID, &getgpioOwner);
    if (RETCODE_OK == retVal)
    {
        if (BSP_EXTENSIONPORT_PIN_USEDBY_GPIO != getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE));
        }
        else if (BSP_EXTENSIONPORT_RESET_STATE == getgpioOwner)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED));
        }
        if ( true == BSP_ExtensionPortInstance[pinID].gpioEnableInterrupt)
        {
            GPIO_IntConfig(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin, BSP_ExtensionPortInstance[pinID].gpioInterruptRaisingEdge,
                    BSP_ExtensionPortInstance[pinID].gpioInterruptFallingEdge, BSP_DISABLE);
            /* Free the registered Interrupt callback slot, so that others can make use of it */
            Board_GPIOInterruptUnregisterCallback(BSP_ExtensionPortTable[pinID].Pin);
            BSP_ExtensionPortInstance[pinID].gpioEnableInterrupt = false;
        }
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_Connect(void)
{
    Retcode_T retVal = RETCODE_OK;
    TristateAllExtensionPortGpios();
    Board_WakeupPowerSupply2V5(EXTENSION_BOARD);
    Board_EnablePowerSupply3V3(EXTENSION_BOARD);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_Disconnect(void)
{
    Retcode_T retVal = RETCODE_OK;
    Board_SnoozePowerSupply2V5(EXTENSION_BOARD);
    Board_DisablePowerSupply3V3(EXTENSION_BOARD);
    TristateAllExtensionPortGpios();
    return retVal;
}

/*
 * Refer to interface header for description
 */
HWHandle_T BSP_ExtensionPort_GetI2cHandle(void)
{
    HWHandle_T Handle = (HWHandle_T) Bsp_ExtensionPortI2cHandle;
    return Handle;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_ConnectI2c(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdI2cData, pinIdI2cClk;
    pinIdI2cData = BSP_EXTENSIONPORT_GPIO_PB11; /* I2C1 data pin */
    pinIdI2cClk = BSP_EXTENSIONPORT_GPIO_PB12; /* I2C1 clock pin */

    retVal = IsI2cPortAvailable();
    if (RETCODE_OK == retVal)
    {
        retVal = SetI2cOwnership(BSP_EXTENSIONPORT_PIN_USEDBY_I2C);
        if (RETCODE_OK == retVal)
        {
            Bsp_ExtensionPortI2cInstance.instance = EXTENSION_PORT_I2C_BUS;
            Bsp_ExtensionPortI2cInstance.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;
            GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdI2cData].Port, BSP_ExtensionPortTable[pinIdI2cData].Pin, gpioModeDisabled, 0);
            GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdI2cClk].Port, BSP_ExtensionPortTable[pinIdI2cClk].Pin, gpioModeDisabled, 0);
            Bsp_ExtensionPortI2cHandle = &Bsp_ExtensionPortI2cInstance;
        }
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_SetI2cConfig(BSP_ExtensionPortI2cConfig_T configItem, uint32_t value, void * pvalue)
{
    BCDS_UNUSED(pvalue);
    Retcode_T retVal = RETCODE_OK;

    if (NULL == Bsp_ExtensionPortI2cHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_I2C_NOT_CONNECTED));
    }
    switch (configItem)
    {
        case BSP_EXTENSIONPORT_I2C_MODE:
        if (BSP_EXTENSIONPORT_I2C_FASTPLUS_MODE >= value)
        {
            Bsp_ExtensionPortI2cHandle->I2cClockFreq = value;
        }
        else
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
        break;
        default:
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        break;
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_EnableI2c(void)
{
    Retcode_T retVal = RETCODE_OK;
    I2C_Init_TypeDef i2cInit =
    {   false, true, 0, I2C_FREQ_FAST_MAX, i2cClockHLRStandard};
    BSP_ExtensionPort_PinId_T pinIdI2cData, pinIdI2cClk;
    pinIdI2cData = BSP_EXTENSIONPORT_GPIO_PB11; /* I2C1 data pin */
    pinIdI2cClk = BSP_EXTENSIONPORT_GPIO_PB12; /* I2C1 clock pin */

    if (NULL == Bsp_ExtensionPortI2cHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_I2C_NOT_CONNECTED));
    }
    /* Properly connected the GPIO pins (SDA, SCK)  */
    GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdI2cData].Port, BSP_ExtensionPortTable[pinIdI2cData].Pin, EXTENSION_I2C_SDA_MODE, EXTENSION_I2C_SDA_DOUT);
    GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdI2cClk].Port, BSP_ExtensionPortTable[pinIdI2cClk].Pin, EXTENSION_I2C_SCK_MODE, EXTENSION_I2C_SCK_DOUT);

    if (BSP_EXTENSIONPORT_I2C_STANDARD_MODE == Bsp_ExtensionPortI2cHandle->I2cClockFreq)
    {
        i2cInit.freq = I2C_FREQ_STANDARD_MAX;
        i2cInit.clhr = i2cClockHLRStandard;
    }
    else if (BSP_EXTENSIONPORT_I2C_FAST_MODE == Bsp_ExtensionPortI2cHandle->I2cClockFreq)
    {
        i2cInit.freq = I2C_FREQ_FAST_MAX;
        i2cInit.clhr = i2cClockHLRAsymetric;
    }
    else if (BSP_EXTENSIONPORT_I2C_FASTPLUS_MODE == Bsp_ExtensionPortI2cHandle->I2cClockFreq)
    {
        i2cInit.freq = I2C_FREQ_FASTPLUS_MAX;
        i2cInit.clhr = i2cClockHLRFast;
    }
    CMU_ClockEnable(EXTENSION_PORT_I2C_CLOCK, BSP_ENABLE);

    (Bsp_ExtensionPortI2cHandle->instance)->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN | I2C_ROUTE_LOCATION_LOC1;
    /* It is mandatory to  fill proper value for IRQCallback while initializing using MCU_I2C_Initialize() api call */
    if (NULL != Bsp_ExtensionPortI2cHandle->IRQCallback)
    {
        Bsp_ExtensionPort_McuI2cIrqCallback = Bsp_ExtensionPortI2cHandle->IRQCallback;
        NVIC_SetPriority(EXTENSION_PORT_I2C_IRQn, EXTENSION_PORT_I2C_NVIC_PRIORITY);
        NVIC_EnableIRQ(EXTENSION_PORT_I2C_IRQn);
        I2C_Init(Bsp_ExtensionPortI2cHandle->instance, &i2cInit);
        I2C_Enable(Bsp_ExtensionPortI2cHandle->instance, BSP_ENABLE);
    }
    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_I2C_MCU_IRQ_CALLBACK_NULL);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_DisableI2c(void)
{
    Retcode_T retVal = RETCODE_OK;

    if (NULL == Bsp_ExtensionPortI2cHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_I2C_NOT_CONNECTED));
    }
    NVIC_DisableIRQ(EXTENSION_PORT_I2C_IRQn);
    /* Make the internally copied MCU Irq callback address to NULL*/
    Bsp_ExtensionPort_McuI2cIrqCallback = NULL;
    /* Disable I2C on extension port */
    I2C_Enable(Bsp_ExtensionPortI2cHandle->instance, BSP_DISABLE);
    CMU_ClockEnable(EXTENSION_PORT_I2C_CLOCK, BSP_DISABLE);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_DisonnectI2c(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdI2cData, pinIdI2cClk;
    pinIdI2cData = BSP_EXTENSIONPORT_GPIO_PB11; /* I2C1 data pin */
    pinIdI2cClk = BSP_EXTENSIONPORT_GPIO_PB12; /* I2C1 clock pin */

    if (NULL == Bsp_ExtensionPortI2cHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_I2C_NOT_CONNECTED));
    }

    retVal = SetI2cOwnership(BSP_EXTENSIONPORT_RESET_STATE);

    if (RETCODE_OK == retVal)
    {
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdI2cData].Port, BSP_ExtensionPortTable[pinIdI2cData].Pin, gpioModeDisabled, 0);
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdI2cClk].Port, BSP_ExtensionPortTable[pinIdI2cClk].Pin, gpioModeDisabled, 0);
        /* Configure the Route for the I2C */
        CMU_ClockEnable(EXTENSION_PORT_I2C_CLOCK, BSP_ENABLE);
        Bsp_ExtensionPortI2cHandle->instance->ROUTE = _I2C_ROUTE_RESETVALUE;
        Bsp_ExtensionPortI2cHandle->instance = NULL;
        CMU_ClockEnable(EXTENSION_PORT_I2C_CLOCK, BSP_DISABLE);
        Bsp_ExtensionPortI2cHandle = NULL;
    }
    return retVal;
}

/*
 * Refer description in the header file BspExtensionPort.h
 */
void BSP_ExtensionPort_I2cIsr(void)
{
    Retcode_T retVal = RETCODE_OK;

    if ((NULL != Bsp_ExtensionPort_McuI2cIrqCallback) && (NULL != Bsp_ExtensionPortI2cHandle))
    {
        /* All I2C interrupt processing is to be done at the MCU level */
        Bsp_ExtensionPort_McuI2cIrqCallback((I2C_T) Bsp_ExtensionPortI2cHandle);
    }
    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        Retcode_RaiseErrorFromIsr(retVal);
    }
}

/*
 * Refer to interface header for description
 */

HWHandle_T BSP_ExtensionPort_GetSpiHandle(void)
{
    HWHandle_T Handle = (HWHandle_T) Bsp_ExtensionPortSpiHandle;
    return Handle;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_ConnectSpi(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdSpiMosi, pinIdSpiMiso, pinIdSpiClk;
    pinIdSpiMosi = BSP_EXTENSIONPORT_GPIO_PB3; /* SPI1 mosi pin */
    pinIdSpiMiso = BSP_EXTENSIONPORT_GPIO_PB4; /* SPI1 miso pin */
    pinIdSpiClk = BSP_EXTENSIONPORT_GPIO_PB5; /* SPI1 clock pin */

    retVal = IsSpiPortAvailable();
    if (RETCODE_OK == retVal)
    {
        retVal = SetSpiOwnership(BSP_EXTENSIONPORT_PIN_USEDBY_SPI);
    }
    if (RETCODE_OK == retVal)
    {
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdSpiMosi].Port, BSP_ExtensionPortTable[pinIdSpiMosi].Pin, gpioModeDisabled, 0);
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdSpiMiso].Port, BSP_ExtensionPortTable[pinIdSpiMiso].Pin, gpioModeDisabled, 0);
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdSpiClk].Port, BSP_ExtensionPortTable[pinIdSpiClk].Pin, gpioModeDisabled, 0);
        CMU_ClockEnable(cmuClock_USART2, true);
        Bsp_ExtensionPortSpiInstance.Instance = USART2;
        CMU_ClockEnable(cmuClock_USART2, false);
#if BCDS_FEATURE_DMA
        Bsp_ExtensionPortSpiDmaTxCh.ChannelId = EXTENSIONPORT_SPI_TX_DMA_CHANNEL;
        Bsp_ExtensionPortSpiDmaTxCh.Config.highPri = false;
        Bsp_ExtensionPortSpiDmaTxCh.Config.enableInt = true;
        Bsp_ExtensionPortSpiDmaTxCh.Config.select = DMAREQ_USART2_TXBL;
        Bsp_ExtensionPortSpiDmaTxCh.CallBack.cbFunc = (DMA_FuncPtr_TypeDef) Bsp_ExtPortSpiTxDMAComplete;
        Bsp_ExtensionPortSpiDmaTxCh.CallBack.userPtr = &Bsp_ExtensionPortSpiInstance;

        Bsp_ExtensionPortSpiDmaRxCh.ChannelId = EXTENSIONPORT_SPI_RX_DMA_CHANNEL;
        Bsp_ExtensionPortSpiDmaRxCh.Config.highPri = false;
        Bsp_ExtensionPortSpiDmaRxCh.Config.enableInt = true;
        Bsp_ExtensionPortSpiDmaRxCh.Config.select = DMAREQ_USART2_RXDATAV;
        Bsp_ExtensionPortSpiDmaRxCh.CallBack.cbFunc = (DMA_FuncPtr_TypeDef) Bsp_ExtPortSpiRxDMAComplete;
        Bsp_ExtensionPortSpiDmaRxCh.CallBack.userPtr = &Bsp_ExtensionPortSpiInstance;

        Bsp_ExtensionPortSpiInstance.Link1 = (void*) &Bsp_ExtensionPortSpiDmaTxCh;
        Bsp_ExtensionPortSpiInstance.Link2 = (void*) &Bsp_ExtensionPortSpiDmaRxCh;

        Bsp_ExtensionPortSpiInstance.TransferMode = BCDS_HAL_TRANSFER_MODE_DMA;
#else
        Bsp_ExtensionPortSpiInstance.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;
#endif
        Bsp_ExtensionPortSpiHandle = &Bsp_ExtensionPortSpiInstance;
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_SetSpiConfig(BSP_ExtensionPortSpiConfig_T configItem, uint32_t value, void * pvalue)
{
    Retcode_T retVal = RETCODE_OK;
    BCDS_UNUSED(pvalue);
    if (NULL == Bsp_ExtensionPortSpiHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_SPI_NOT_CONNECTED));
    }
    switch (configItem)
    {
        case BSP_EXTENSIONPORT_SPI_BAUDRATE:
        if((UINT32_C(0) < value) && (UINT32_MAX > value))
        {
            /* Setting baudrate */
            Bsp_ExtensionPortSpiHandle->Baudrate = (uint32_t) value;
        }
        else
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
        break;
        case BSP_EXTENSIONPORT_SPI_MODE:
        if(BSP_EXTENSIONPORT_SPI_MODE3 >= value)
        {
            Bsp_ExtensionPortSpiHandle->Mode = (uint8_t) value;
        }
        else
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
        break;
        case BSP_EXTENSIONPORT_SPI_BIT_ORDER:
        if(BSP_EXTENSIONPORT_SPI_LSB_FIRST >=value)
        {
            Bsp_ExtensionPortSpiHandle->BitOrder = (uint8_t) value;
        }
        else
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
        break;
        default:
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        break;
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_EnableSpi(void)
{
    Retcode_T retVal = RETCODE_OK;
    /* Default configurations for the Usart Handle if the BSP_ExtensionPort_ConnectSpi() is not called by user */
    USART_InitSync_TypeDef efmUsartHandle =
    {   EXTENSIONPORT_SPI_STATE, EXTENSIONPORT_SPI_REF_FREQ, EXTENSIONPORT_SPI_BAUDRATE, EXTENSIONPORT_SPI_DATABITS,
        EXTENSIONPORT_SPI_MASTERMODE_ENABLE, EXTENSIONPORT_SPI_MSB_ENABLE, EXTENSIONPORT_SPI_CLOCKMODE,EXTENSIONPORT_SPI_PRS_RX_ENABLE,
        EXTENSIONPORT_SPI_PRS_RX_CHANNEL, EXTENSIONPORT_SPI_AUTOTX_ENABLE};

    BSP_ExtensionPort_PinId_T pinIdSpiMosi, pinIdSpiMiso, pinIdSpiClk;
    pinIdSpiMosi = BSP_EXTENSIONPORT_GPIO_PB3; /* SPI1 mosi pin */
    pinIdSpiMiso = BSP_EXTENSIONPORT_GPIO_PB4; /* SPI1 miso pin */
    pinIdSpiClk = BSP_EXTENSIONPORT_GPIO_PB5; /* SPI1 clock pin */
    if (NULL == Bsp_ExtensionPortSpiHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_SPI_NOT_CONNECTED));
    }
    GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdSpiMosi].Port,BSP_ExtensionPortTable[pinIdSpiMosi].Pin, gpioModePushPull,0);
    GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdSpiMiso].Port,BSP_ExtensionPortTable[pinIdSpiMiso].Pin, gpioModeInput,0);
    GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdSpiClk].Port,BSP_ExtensionPortTable[pinIdSpiClk].Pin, gpioModePushPull,0);
    CMU_ClockEnable(cmuClock_USART2, true);

    /* Setting Baudrate */
    efmUsartHandle.baudrate = Bsp_ExtensionPortSpiHandle->Baudrate;
    /* Setting SPI Mode */
    if (BSP_EXTENSIONPORT_SPI_MODE0 == Bsp_ExtensionPortSpiHandle->Mode)
    {
        efmUsartHandle.clockMode = usartClockMode0;
    }
    else if (BSP_EXTENSIONPORT_SPI_MODE1 == Bsp_ExtensionPortSpiHandle->Mode)
    {
        efmUsartHandle.clockMode = usartClockMode1;

    }
    else if (BSP_EXTENSIONPORT_SPI_MODE2 == Bsp_ExtensionPortSpiHandle->Mode)
    {
        efmUsartHandle.clockMode = usartClockMode2;
    }
    else
    {
        efmUsartHandle.clockMode = usartClockMode3;
    }
    /* Setting Most Significant Bit */
    if (BSP_EXTENSIONPORT_SPI_LSB_FIRST >= Bsp_ExtensionPortSpiHandle->BitOrder)
    {
        if (BSP_EXTENSIONPORT_SPI_MSB_FIRST == Bsp_ExtensionPortSpiHandle->BitOrder)
        {
            /* Set Most Significant Bit First */
            efmUsartHandle.msbf = true;
        }
        else
        {
            /* Set Least Significant Bit First */
            efmUsartHandle.msbf = false;
        }
    }
    /* By default after the initialization Usart module is disabled */
    USART_InitSync(Bsp_ExtensionPortSpiHandle->Instance,&efmUsartHandle);
    Bsp_ExtensionPortSpiHandle->Instance->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_LOCATION_LOC1;
    USART_IntClear(Bsp_ExtensionPortSpiHandle->Instance,_USART_IFC_MASK);
    USART_IntDisable(Bsp_ExtensionPortSpiHandle->Instance,_USART_IEN_MASK);

    /* Set interrupt priorities */
    NVIC_SetPriority(EXTENSIONPORT_SPI_RX_IRQN, EXTENSIONPORT_SPI_RX_INT_PRIORITY);
    NVIC_SetPriority(EXTENSIONPORT_SPI_TX_IRQN, EXTENSIONPORT_SPI_TX_INT_PRIORITY);

    /* clear pending interrupts */
    NVIC_ClearPendingIRQ(EXTENSIONPORT_SPI_RX_IRQN);
    NVIC_ClearPendingIRQ(EXTENSIONPORT_SPI_TX_IRQN);

    NVIC_EnableIRQ(EXTENSIONPORT_SPI_RX_IRQN);
    NVIC_EnableIRQ(EXTENSIONPORT_SPI_TX_IRQN);

    USART_IntEnable(Bsp_ExtensionPortSpiHandle->Instance, USART_IEN_TXBL);
    USART_IntEnable(Bsp_ExtensionPortSpiHandle->Instance, (USART_IEN_RXDATAV | USART_IEN_PERR | USART_IEN_FERR | USART_IEN_RXOF));
    USART_Enable(Bsp_ExtensionPortSpiHandle->Instance, BSP_ENABLE);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_DisableSpi(void)
{
    Retcode_T retVal = RETCODE_OK;
    if (NULL == Bsp_ExtensionPortSpiHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_SPI_NOT_CONNECTED));
    }
    /* Disable the USART clock*/
    CMU_ClockEnable(cmuClock_USART2, false);
    NVIC_DisableIRQ(EXTENSIONPORT_SPI_RX_IRQN);
    NVIC_DisableIRQ(EXTENSIONPORT_SPI_TX_IRQN);

    return retVal;
}

/*
 * Refer interface description available in BspExtensionPort.h
 */

Retcode_T BSP_ExtensionPort_DisonnectSpi(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdSpiMosi, pinIdSpiMiso, pinIdSpiClk;
    pinIdSpiMosi = BSP_EXTENSIONPORT_GPIO_PB3; /* SPI1 mosi pin */
    pinIdSpiMiso = BSP_EXTENSIONPORT_GPIO_PB4; /* SPI1 miso pin */
    pinIdSpiClk = BSP_EXTENSIONPORT_GPIO_PB5; /* SPI1 clock pin */
    if (NULL == Bsp_ExtensionPortSpiHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_SPI_NOT_CONNECTED));
    }
    retVal = SetSpiOwnership(BSP_EXTENSIONPORT_RESET_STATE);
    if (retVal == RETCODE_OK)
    {
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdSpiMosi].Port,BSP_ExtensionPortTable[pinIdSpiMosi].Pin, gpioModeDisabled,0);
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdSpiMiso].Port,BSP_ExtensionPortTable[pinIdSpiMiso].Pin, gpioModeDisabled,0);
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdSpiClk].Port,BSP_ExtensionPortTable[pinIdSpiClk].Pin, gpioModeDisabled,0);
        CMU_ClockEnable(cmuClock_USART2, true);
        Bsp_ExtensionPortSpiHandle->Instance->ROUTE = _USART_ROUTE_RESETVALUE;
        Bsp_ExtensionPortSpiHandle->Instance = NULL;
        CMU_ClockEnable(cmuClock_USART2, false);
        Bsp_ExtensionPortSpiHandle = NULL;
    }
    return retVal;
}

/*
 * Refer interface description available in BspExtensionPort.h
 */
void BSP_ExtensionPort_SPIRxISR(void)
{
    /* All SPI interrupt processing is to be done at the MCU level */
    MCU_SPI_IRQ_RX_Callback((SPI_T) Bsp_ExtensionPortSpiHandle);
}

/*
 * Refer interface description available in BspExtensionPort.h
 */
void BSP_ExtensionPort_SPITxISR(void)
{
    /* All SPI interrupt processing is to be done at the MCU level */
    MCU_SPI_IRQ_TX_Callback((SPI_T) Bsp_ExtensionPortSpiHandle);
}

/*
 * Refer to interface header for description
 */

HWHandle_T BSP_ExtensionPort_GetUartHandle(void)
{
    HWHandle_T Handle = (HWHandle_T) Bsp_ExtensionPortUartHandle;
    return Handle;
}

/*
 * Refer to interface header for description
 */

Retcode_T BSP_ExtensionPort_ConnectUart(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdTx, pinIdRx;

    pinIdTx = BSP_EXTENSIONPORT_GPIO_PB9; /*Tx pin */
    pinIdRx = BSP_EXTENSIONPORT_GPIO_PB10; /*Rx pin */

    retVal = IsUartPortAvailable();
    if (RETCODE_OK == retVal)
    {
        retVal = SetUartOwnership(BSP_EXTENSIONPORT_PIN_USEDBY_UART);
        if (RETCODE_OK == retVal)
        {
            Bsp_ExtensionPortUartInstance.Uart_ptr = UART1;
            Bsp_ExtensionPortUartInstance.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;
            GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdTx].Port, BSP_ExtensionPortTable[pinIdTx].Pin, gpioModeDisabled, 0);
            GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdRx].Port, BSP_ExtensionPortTable[pinIdRx].Pin, gpioModeDisabled, 0);

            Bsp_ExtensionPortUartHandle = &Bsp_ExtensionPortUartInstance;
        }
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */

Retcode_T BSP_ExtensionPort_DisonnectUart(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdTx, pinIdRx;
    pinIdTx = BSP_EXTENSIONPORT_GPIO_PB9; /*Tx pin */
    pinIdRx = BSP_EXTENSIONPORT_GPIO_PB10; /*Rx pin */
    if (NULL == Bsp_ExtensionPortUartHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_UART_NOT_CONNECTED));
    }
    retVal = SetUartOwnership(BSP_EXTENSIONPORT_RESET_STATE);
    if (RETCODE_OK == retVal)
    {
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdTx].Port, BSP_ExtensionPortTable[pinIdTx].Pin, gpioModeDisabled, 0);
        GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdRx].Port, BSP_ExtensionPortTable[pinIdRx].Pin, gpioModeDisabled, 0);

        /* Configure the Route for the UART */
        CMU_ClockEnable(cmuClock_UART1, true);
        (Bsp_ExtensionPortUartHandle->Uart_ptr)->ROUTE = _UART_ROUTE_RESETVALUE;
        Bsp_ExtensionPortUartHandle->Uart_ptr=NULL;
        CMU_ClockEnable(cmuClock_UART1, false);
        Bsp_ExtensionPortUartHandle=NULL;
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_SetUartConfig(BSP_ExtensionPortUartConfig_T configItem, uint32_t value, void * pvalue)
{
    BCDS_UNUSED(pvalue);
    Retcode_T retVal = RETCODE_OK;

    if (NULL == Bsp_ExtensionPortUartHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_UART_NOT_CONNECTED));
    }

    switch (configItem)
    {
        case BSP_EXTENSIONPORT_UART_BAUDRATE:
        if((UINT32_C(0) < value) && (UINT32_MAX > value))
        {
            /* Setting baudrate */
            Bsp_ExtensionPortUartHandle->Baudrate = (uint32_t) value;
        }
        else
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
        break;

        case BSP_EXTENSIONPORT_UART_PARITY:
        if(BSP_EXTENSIONPORT_UART_ODD_PARITY >= value)
        {
            /* Setting parity */
            Bsp_ExtensionPortUartHandle->Parity = (uint8_t)value;
        }
        else
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
        break;

        case BSP_EXTENSIONPORT_UART_STOPBITS:
        if(BSP_EXTENSIONPORT_UART_STOPBITS_TWO >= value)
        {
            /* Setting stopbits */
            Bsp_ExtensionPortUartHandle->Stopbits = (uint8_t)value;
        }
        else
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
        break;

        default:
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        break;
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_EnableUart(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_ExtensionPort_PinId_T pinIdTx, pinIdRx;
    pinIdTx = BSP_EXTENSIONPORT_GPIO_PB9; /*Tx pin */
    pinIdRx = BSP_EXTENSIONPORT_GPIO_PB10; /*Rx pin */

    /* Default configurations for the Usart Handle if the BSP_ExtensionPort_ConnectUart() is not called by user */
    USART_InitAsync_TypeDef UartInitializer =
    {   UART1_STATE, UART1_REF_FREQ, UART1_BAUDRATE, UART1_OVERSAMPLING, UART1_DATABITS,UART1_PARITY
        ,UART1_STOPBITS,UART1_MAJORITY_VOTING,UART1_PRS_RX_ENABLE,UART1_PRS_RX_CHANNEL};

    if (NULL == Bsp_ExtensionPortUartHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_UART_NOT_CONNECTED));
    }

    GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdRx].Port,
            BSP_ExtensionPortTable[pinIdRx].Pin, gpioModeInput,
            0);

    GPIO_PinModeSet(BSP_ExtensionPortTable[pinIdTx].Port,
            BSP_ExtensionPortTable[pinIdTx].Pin,gpioModePushPullDrive ,
            0);

    /* check for parity and stopbits */
    switch(Bsp_ExtensionPortUartHandle->Parity)
    {
        case BSP_EXTENSIONPORT_UART_NO_PARITY:
        UartInitializer.parity = usartNoParity;
        break;
        case BSP_EXTENSIONPORT_UART_EVEN_PARITY:
        UartInitializer.parity = usartEvenParity;
        break;
        case BSP_EXTENSIONPORT_UART_ODD_PARITY:
        UartInitializer.parity = usartOddParity;
        break;
        default:
        break;
    }
    switch(Bsp_ExtensionPortUartHandle->Stopbits)
    {
        case BSP_EXTENSIONPORT_UART_STOPBITS_HALF:
            UartInitializer.stopbits = usartStopbits0p5;
        break;
        case BSP_EXTENSIONPORT_UART_STOPBITS_ONE:
            UartInitializer.stopbits = usartStopbits1;
        break;
        case BSP_EXTENSIONPORT_UART_STOPBITS_ONEANDAHALF:
            UartInitializer.stopbits = usartStopbits1p5;
        break;
        case BSP_EXTENSIONPORT_UART_STOPBITS_TWO:
            UartInitializer.stopbits = usartStopbits2;
        break;
        default:
        break;
    }

    UartInitializer.baudrate = Bsp_ExtensionPortUartHandle->Baudrate; /* Baud rate */
    CMU_ClockEnable(cmuClock_UART1, true);

    if (NULL != Bsp_ExtensionPortUartHandle->Uart_ptr)
    {
        USART_InitAsync(Bsp_ExtensionPortUartHandle->Uart_ptr, &UartInitializer);

        /* clear interrupt flags */
        Bsp_ExtensionPortUartHandle->Uart_ptr->IFC = _USART_IF_MASK;

        /* By default disable the interrupts, send/and or receive will take care  */
        Bsp_ExtensionPortUartHandle->Uart_ptr->IEN = 0;
        Bsp_ExtensionPortUartRxIsrCallback = Bsp_ExtensionPortUartHandle->IrqRxCallback;
        Bsp_ExtensionPortUartTxIsrCallback = Bsp_ExtensionPortUartHandle->IrqTxCallback;

        (Bsp_ExtensionPortUartHandle->Uart_ptr)->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC2;
        NVIC_SetPriority(UART1_RX_IRQn, EXTENSION_PORT_UART_RX_NVIC_PRIORITY);
        NVIC_SetPriority(UART1_TX_IRQn, EXTENSION_PORT_UART_TX_NVIC_PRIORITY);

        /*clear pending interrupts */
        NVIC_ClearPendingIRQ(UART1_RX_IRQn);
        NVIC_ClearPendingIRQ(UART1_TX_IRQn);

        /*enable interrupts */
        NVIC_EnableIRQ(UART1_RX_IRQn);
        NVIC_EnableIRQ(UART1_TX_IRQn);

        /* Enable the UART */
        USART_Enable(Bsp_ExtensionPortUartHandle->Uart_ptr, (USART_Enable_TypeDef) (USART_CMD_RXEN | USART_CMD_TXEN));
    }
    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_UART_MCU_IRQ_CALLBACK_NULL);
    }

    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_DisableUart(void)
{
    Retcode_T retVal = RETCODE_OK;
    if (NULL == Bsp_ExtensionPortUartHandle)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXTENSIONPORT_UART_NOT_CONNECTED));
    }
    NVIC_DisableIRQ(UART1_RX_IRQn);
    NVIC_DisableIRQ(UART1_TX_IRQn);
    Bsp_ExtensionPortUartRxIsrCallback = NULL;
    Bsp_ExtensionPortUartTxIsrCallback = NULL;
    (Bsp_ExtensionPortUartHandle->Uart_ptr)->CMD = USART_CMD_RXDIS | USART_CMD_TXDIS | USART_CMD_MASTERDIS;
    CMU_ClockEnable(cmuClock_UART1, BSP_DISABLE);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_ConnectAdc(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
		retVal = IsAdcPortAvailable(pinID);
		if (RETCODE_OK == retVal)
		{
			retVal = BSP_ExtensionPort_SetGpioOwner(pinID,BSP_EXTENSIONPORT_PIN_USEDBY_ADC);
			if (RETCODE_OK == retVal)
			{
				GPIO_PinModeSet(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin, gpioModeDisabled, 0);
			}
		}
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_DisconnectAdc(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    /* Check needs to verify the pinID should be either BSP_EXTENSIONPORT_GPIO_PD5 or BSP_EXTENSIONPORT_GPIO_PD6*/
    if((BSP_EXTENSIONPORT_GPIO_PD5 != pinID) && (BSP_EXTENSIONPORT_GPIO_PD6 != pinID))
    {
    	retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    if(RETCODE_OK == retVal)
    {
		retVal = BSP_ExtensionPort_SetGpioOwner(pinID,BSP_EXTENSIONPORT_RESET_STATE);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_EnableAdc(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    /* Check needs to verify the pinID should be either BSP_EXTENSIONPORT_GPIO_PD5 or BSP_EXTENSIONPORT_GPIO_PD6*/
    if((BSP_EXTENSIONPORT_GPIO_PD5 != pinID) && (BSP_EXTENSIONPORT_GPIO_PD6 != pinID))
    {
    	retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    if(RETCODE_OK == retVal)
    {
		/* Properly connected the GPIO pins (channel 1, channel 2)  */
		GPIO_PinModeSet(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin, EXTENSION_ADC_CHANNEL_PIN_MODE, EXTENSION_ADC_CHANNEL_PIN_DOUT);
		/*Enable Clocks for the ADC */
		CMU_ClockEnable(cmuClock_HFPER, BSP_ENABLE);
		CMU_ClockEnable(cmuClock_ADC0, BSP_ENABLE);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_ExtensionPort_DisableAdc(BSP_ExtensionPort_PinId_T pinID)
{
    Retcode_T retVal = RETCODE_OK;
    /* Check needs to verify the pinID should be either BSP_EXTENSIONPORT_GPIO_PD5 or BSP_EXTENSIONPORT_GPIO_PD6*/
    if((BSP_EXTENSIONPORT_GPIO_PD5 != pinID) && (BSP_EXTENSIONPORT_GPIO_PD6 != pinID))
    {
    	retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
	GPIO_PinModeSet(BSP_ExtensionPortTable[pinID].Port, BSP_ExtensionPortTable[pinID].Pin, gpioModeDisabled, 0);
    return retVal;
}
/*
 * Refer description in the header file BspExtensionPort.h
 */
void BSP_ExtensionPort_UARTRxIsr(void)
{
    Retcode_T retVal = RETCODE_OK;
    if ((NULL != Bsp_ExtensionPortUartRxIsrCallback) && (NULL != Bsp_ExtensionPortUartHandle))
    {
        /* All UART interrupt processing is to be done at the MCU level */
        Bsp_ExtensionPortUartHandle->IrqRxCallback((UART_T) Bsp_ExtensionPortUartHandle);
    }
    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        Retcode_RaiseErrorFromIsr(retVal);
    }
}

/*
 * Refer description in the header file BspExtensionPort.h
 */
void BSP_ExtensionPort_UARTTxIsr(void)
{
    Retcode_T retVal = RETCODE_OK;
    if ((NULL != Bsp_ExtensionPortUartTxIsrCallback) && (NULL != Bsp_ExtensionPortUartHandle))
    {
        /* All UART interrupt processing is to be done at the MCU level */
        Bsp_ExtensionPortUartHandle->IrqTxCallback((UART_T) Bsp_ExtensionPortUartHandle);
    }
    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        Retcode_RaiseErrorFromIsr(retVal);
    }
}

#endif /* BCDS_FEATURE_BSP_EXTENSIONPORT */

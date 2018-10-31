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
 * @ingroup  BCDS_HAL_BSP_IF
 *
 * @defgroup BSP_EXTENSIONPORT  BSP_ExtensionPort
 * @{
 *
 * @brief This interface module contains necessary Application Programming Interfaces to enable user to do
 *        external interfacing via the standard XDK extension board delivered along with the XDK Development Kit.
 *
 * @details The main purpose of this interface is for XDK users to configure and use the XDK extension board effectively.
 *        This module provides ease of use API's for the novice programmer to start prototyping with XDK extension board.
 *        Currently the support is provided to configure the extension board I/O pins as GPIO, also providing support to
 *        configure and establish serial communication namely UART, SPI and I2C.
 * @file
 */
#ifndef BSP_EXTENSIONPORT_H_
#define BSP_EXTENSIONPORT_H_

/* Include all headers which are needed by this file. */
#include "BCDS_HAL.h"

#if BCDS_FEATURE_BSP_EXTENSIONPORT

/* Put the type and macro definitions here */

/**
 * @ingroup BSP_EXTENSIONPORT
 * @defgroup EXTENSIONPORT_GPIO BSP_EXTENSIONPORT_GPIO enums
 * @{
 * @brief List of Enums and macros used for the extension port module for GPIO interfaces
 *
 */

/**
 * Enumeration representing list of possible error code applicable for GPIO Extension port module
 */
enum Retcode_ExtensionPort_E
{
    RETCODE_BSP_EXTENSIONPORT_GPIO_INT_BUSY = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE,
    RETCODE_BSP_EXTENSIONPORT_INT_REQ_ON_OUTPUT_PIN,
    RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED,
    RETCODE_BSP_EXTENSIONPORT_I2C_MCU_IRQ_CALLBACK_NULL,
	RETCODE_BSP_EXTENSIONPORT_SPI_MCU_IRQ_CALLBACK_NULL,
	RETCODE_BSP_EXTENSIONPORT_UART_NOT_CONNECTED,
    RETCODE_BSP_EXTENSIONPORT_UART_MCU_IRQ_CALLBACK_NULL,
	RETCODE_BSP_EXTENSIONPORT_I2C_NOT_CONNECTED,
	RETCODE_BSP_EXTENSIONPORT_SPI_NOT_CONNECTED,
	RETCODE_BSP_EXTENSIONPORT_ADC_NOT_CONNECTED
};

/**
 * Function Type for the Gpio Application callback if the interrupt is registered for the gpio pin
 *
 * @ Note: On event the user needs to read the pin value using BSP_ExtensionPort_ReadGpio() inside the callback function implementation
 */
typedef void (*BSP_ExtensionPort_GpioCallback_T)(void);

/**
 * Enumeration representing list of Gpio pin ID provided for GPIO Extension port
 */
enum BSP_ExtensionPort_PinId_E
{
    BSP_EXTENSIONPORT_GPIO_PA0 = 0, /**< Alternate function shall be Timer0 channel 0 compare operations */
    BSP_EXTENSIONPORT_GPIO_PA1, /**< General purpose I/O  */
    BSP_EXTENSIONPORT_GPIO_PB2, /**< General purpose I/O  */
    BSP_EXTENSIONPORT_GPIO_PB3, /**< General purpose I/O ,SPI MOSI operations  */
    BSP_EXTENSIONPORT_GPIO_PB4, /**< General purpose I/O ,SPI MISO operations  */
    BSP_EXTENSIONPORT_GPIO_PB5, /**< General purpose I/O ,SPI clock operations */
    BSP_EXTENSIONPORT_GPIO_PB9, /**< General purpose I/O , UART1_TX operations */
    BSP_EXTENSIONPORT_GPIO_PB10, /**< General purpose I/O , UART1_RX operations */
    BSP_EXTENSIONPORT_GPIO_PB11, /**< General purpose I/O , I2C1 data line      */
    BSP_EXTENSIONPORT_GPIO_PB12, /**< General purpose I/O , I2C1 clock line     */
    BSP_EXTENSIONPORT_GPIO_PC0, /**< General purpose I/O , Timer0 channel 1 compare operations  */
    BSP_EXTENSIONPORT_GPIO_PC1, /**< General purpose I/O , Timer0 channel 2 compare operations  */
    BSP_EXTENSIONPORT_GPIO_PC2, /**< General purpose I/O , Timer0 Channel 0 Complementary Dead-Time Insertion operations  */
    BSP_EXTENSIONPORT_GPIO_PC3, /**< General purpose I/O ,Timer0 Channel 1 Complementary Dead-Time Insertion operations   */
    BSP_EXTENSIONPORT_GPIO_PC4, /**< General purpose I/O , Timer0 Channel 2 Complementary Dead-Time Insertion operations  */
    BSP_EXTENSIONPORT_GPIO_PC8, /**< General purpose I/O ,Timer2 channel 0 capture operations   */
    BSP_EXTENSIONPORT_GPIO_PC9, /**< General purpose I/O ,Timer2 channel 1 capture operations   */
    BSP_EXTENSIONPORT_GPIO_PC10, /**< General purpose I/O ,Timer2 channel 2 capture operations   */
    BSP_EXTENSIONPORT_GPIO_PD5, /**< General purpose I/O , ADC0 operations */
    BSP_EXTENSIONPORT_GPIO_PD6, /**< General purpose I/O , ADC0 operations */
    BSP_EXTENSIONPORT_GPIO_PD8, /**< General purpose I/O  */
    BSP_EXTENSIONPORT_GPIO_PE2, /**< General purpose I/O  */
    BSP_EXTENSIONPORT_GPIO_PF6, /**< General purpose I/O  */
    BSP_EXTENSIONPORT_GPIO_MAX
};
typedef enum BSP_ExtensionPort_PinId_E BSP_ExtensionPort_PinId_T;

/**
 * Enumeration representing list of Gpio Interrupt modes supported for GPIO Extension port
 */
enum BSP_ExtensionPort_GpioIntMode_E
{
    BSP_EXTENSIONPORT_INT_EDGE_RAISING = 0,
    BSP_EXTENSIONPORT_INT_EDGE_FALLING,
    BSP_EXTENSIONPORT_INT_EDGE_BOTH,
    BSP_EXTENSIONPORT_INT_LEVEL_LOW,
    BSP_EXTENSIONPORT_INT_LEVEL_HIGH,
    BSP_EXTENSIONPORT_INT_OUT_OF_RANGE, /**< Gpio Interrupt mode out of range */
};
typedef enum BSP_ExtensionPort_GpioIntMode_E BSP_ExtensionPort_GpioIntMode_T;

/**
 * Enumeration representing list of Gpio pin Value supported for GPIO Extension port
 */
enum BSP_ExtensionPortGpioPinValue_E
{
    BSP_EXTENSIONPORT_GPIO_PIN_LOW = 0,
    BSP_EXTENSIONPORT_GPIO_PIN_HIGH = 1,
};

/**
 * Enumeration representing list of Gpio pin modes supported for GPIO Extension port
 *
 * @note If the enumeration item needs to be extended, then please add the items at the last in the list, to avoid any side effects.
 */
enum BSP_ExtensionPortGpioPinMode_E
{
    BSP_EXTENSIONPORT_INPUT_NOPULL = 0, /**< Input enabled */
    BSP_EXTENSIONPORT_INPUT_PULLUP, /**< Input enabled with internal pull-up */
    BSP_EXTENSIONPORT_INPUT_PULLDOWN, /**< Input enabled with internal pull-down */
    BSP_EXTENSIONPORT_INPUTPULLFILTER, /**< Input enabled with internal pull-up and glitch filter enabled  */
    BSP_EXTENSIONPORT_PUSHPULL, /**< Push-pull output */
    BSP_EXTENSIONPORT_PUSHPULL_DRIVE, /**< Push-pull output */
    BSP_EXTENSIONPORT_OPEN_SOURCE,/**< open-source output */
    BSP_EXTENSIONPORT_OPEN_SOURCEPULLDOWN, /**< open-source output with pull-down*/
    BSP_EXTENSIONPORT_OPEN_DRAIN, /**< Open-drain output */
    BSP_EXTENSIONPORT_OPEN_DRAINFILTER, /**< Open-drain output with filter */
    BSP_EXTENSIONPORT_OPEN_DRAINPULLUP, /**< Open-drain output with pullup */
    BSP_EXTENSIONPORT_OPEN_DRAINPULLUP_FILTER, /**< Open-drain output with pullup and filter */
    BSP_EXTENSIONPORT_MODE_OUT_OF_RANGE, /**< Gpio pin mode out of range */
};
typedef enum BSP_ExtensionPortGpioPinMode_E BSP_ExtensionPortGpioPinMode_T;

/**
 * Enumeration representing the Configuration items for the GPIO interface provided on the extension port
 *
 * @brief The below table gives the possible configuration settings for ease of user.
 *
 * @note The Open Drain and Open source configuration has special purpose, it is mainly used for bidirectional signaling and other use case is for shared pin access in case of Peripheral Bus pin
 *
 * @note Gpio Interrupt capability is applicable only for the pin configured for the following
 *       modes BSP_EXTENSIONPORT_INPUT_NOPULL,BSP_EXTENSIONPORT_INPUT_PULLUP,BSP_EXTENSIONPORT_INPUT_PULLDOWN,BSP_EXTENSIONPORT_INPUTPULLFILTER
 *
 *  Pin Mode                                    | Suggested Pin Value for proper gpio I/O operation
 *----------------------------------------------|------------------
 * BSP_EXTENSIONPORT_INPUT_NOPULL               | BSP_EXTENSIONPORT_GPIO_PIN_LOW|
 * BSP_EXTENSIONPORT_INPUT_PULLUP               | BSP_EXTENSIONPORT_GPIO_PIN_HIGH |
 * BSP_EXTENSIONPORT_INPUT_PULLDOWN             | BSP_EXTENSIONPORT_GPIO_PIN_LOW |
 * BSP_EXTENSIONPORT_INPUTPULLFILTER            | BSP_EXTENSIONPORT_GPIO_PIN_LOW - Turns OFF the internal Filter , BSP_EXTENSIONPORT_GPIO_PIN_HIGH - Turns ON the internal filter , Pulse width of pulses to be removed by the glitch suppression filter is 10 to 50 nano seconds|
 * BSP_EXTENSIONPORT_PUSHPULL                   | (X) - Don't care,set initial Output pin value to either BSP_EXTENSIONPORT_GPIO_PIN_HIGH or BSP_EXTENSIONPORT_GPIO_PIN_LOW based on application need|
 * BSP_EXTENSIONPORT_OPEN_SOURCE                | (X) - Don't care,Check for presence of external Pull-down in Interfacing HW circuitry if not set initial Output pin value to BSP_EXTENSIONPORT_GPIO_PIN_LOW for safer side|
 * BSP_EXTENSIONPORT_OPEN_SOURCEPULLDOWN        | (X) - Don't care, set value default value as BSP_EXTENSIONPORT_GPIO_PIN_LOW for safer side
 * BSP_EXTENSIONPORT_OPEN_DRAIN                 | (X) - Don't care, Check for presence of external Pull-up in Interfacing HW circuitry, if not set initial Output pin value to set value BSP_EXTENSIONPORT_GPIO_PIN_HIGH for safer side|
 * BSP_EXTENSIONPORT_OPEN_DRAINFILTER           | (X) - Don't care, set value default value as BSP_EXTENSIONPORT_GPIO_PIN_HIGH for safer side                                              |
 * BSP_EXTENSIONPORT_OPEN_DRAINPULLUP           | (X) - Don't care, set value default value as BSP_EXTENSIONPORT_GPIO_PIN_HIGH for safer side , this enables internal pull up on this output pin
 * BSP_EXTENSIONPORT_OPEN_DRAINPULLUP_FILTER    | (X) - Don't care, set value default value as BSP_EXTENSIONPORT_GPIO_PIN_HIGH for safer side , this enables internal pull up on this output pin and also enables input filter
 *
 */
enum BSP_ExtensionPortGpioConfig_E
{
    BSP_EXTENSIONPORT_GPIO_PINVALUE, /**< Gpio Configuration Item representing Pin value when configured as Input or Output pin  */
    BSP_EXTENSIONPORT_GPIO_PINMODE, /**< Gpio Configuration Item representing Pin mode value when configured as Input or Output pin */
};
typedef enum BSP_ExtensionPortGpioConfig_E BSP_ExtensionPortGpioConfig_T;

/**@}*/

/**
 * @ingroup BSP_EXTENSIONPORT
 * @defgroup EXTENSIONPORT_SPI BSP_EXTENSIONPORT_SPI enums
 * @{
 * @brief List of Enums and macros used for the extension port module for SPI interfaces
 *
 */

/**
 * Enumeration representing list of SPI modes supported for SPI Extension port
 */
enum BSP_ExtensionPortSpiMode_E
{
    BSP_EXTENSIONPORT_SPI_MODE0, /**< Clock Polarity 0, Clock Edge 1 */
    BSP_EXTENSIONPORT_SPI_MODE1, /**< Clock Polarity 0, Clock Edge 0 */
    BSP_EXTENSIONPORT_SPI_MODE2, /**< Clock Polarity 1, Clock Edge 1 */
    BSP_EXTENSIONPORT_SPI_MODE3, /**< Clock Polarity 1, Clock Edge 0 */
};
typedef enum BSP_ExtensionPortSpiMode_E BSP_ExtensionPortSpiMode_T;

/**
 * Enumeration representing list of SPI Bit Order MSB/LSB supported for SPI Extension port
 */
enum BSP_ExtensionPortSpiBitOrder_E
{
    BSP_EXTENSIONPORT_SPI_MSB_FIRST, /**< configuration for handling Most Significant bit first */
    BSP_EXTENSIONPORT_SPI_LSB_FIRST, /**< configuration for handling Least Significant bit first */
};

/**
 * Enumeration representing the Configuration items for the SPI interface provided on the extension port */
enum  BSP_ExtensionPortSpiConfig_E
{
   BSP_EXTENSIONPORT_SPI_BAUDRATE,
   BSP_EXTENSIONPORT_SPI_MODE,
   BSP_EXTENSIONPORT_SPI_BIT_ORDER,
};
typedef enum BSP_ExtensionPortSpiConfig_E BSP_ExtensionPortSpiConfig_T;

/**@}*/

/**
 * @ingroup BSP_EXTENSIONPORT
 * @defgroup EXTENSIONPORT_I2C BSP_EXTENSIONPORT_I2C enums
 * @{
 * @brief List of Enums and macros used for the extension port module for I2C interfaces
 *
 */

/**
 * Enumeration representing list of I2C modes supported for I2C Extension port
 */
enum BSP_EXTENSIONPORT_I2C_MODE_E
{
    BSP_EXTENSIONPORT_I2C_STANDARD_MODE, /**< Standard mode max frequency(93458Hz), assuming using 4:4 ratio for Nlow:Nhigh. */
    BSP_EXTENSIONPORT_I2C_FAST_MODE, /**< Fast mode max frequency(392157Hz),assuming using 6:3 ratio for Nlow:Nhigh */
    BSP_EXTENSIONPORT_I2C_FASTPLUS_MODE, /**< Fast plus mode max frequency(987167Hz) assuming using 11:6 ratio for Nlow:Nhigh */
};

/**
 * Enumeration representing the Configuration items for the SPI interface provided on the extension port */
enum BSP_ExtensionPortI2cConfig_E
{
    BSP_EXTENSIONPORT_I2C_MODE,
};
typedef enum BSP_ExtensionPortI2cConfig_E BSP_ExtensionPortI2cConfig_T;

/**@}*/

/**
 * @ingroup BSP_EXTENSIONPORT
 * @defgroup EXTENSIONPORT_UART BSP_EXTENSIONPORT_UART enums
 * @{
 * @brief List of Enums and macros used for the extension port module for UART interfaces
 *
 */

/**
 * Enumeration representing list of parity options supported for UART Extension port
 */
enum BSP_EXTENSIONPORT_UART_PARTIY_E
{
    BSP_EXTENSIONPORT_UART_NO_PARITY, /**< No parity. */
    BSP_EXTENSIONPORT_UART_EVEN_PARITY, /**< Even parity. */
    BSP_EXTENSIONPORT_UART_ODD_PARITY, /**< Odd parity. */
};

/**
 * Enumeration representing list of StopBits options supported for UART Extension port
 */
enum BSP_EXTENSIONPORT_UART_STOPBITS_E
{
    BSP_EXTENSIONPORT_UART_STOPBITS_HALF, /**< 0.5 stop bits. */
    BSP_EXTENSIONPORT_UART_STOPBITS_ONE, /**< 1 stop bits. */
    BSP_EXTENSIONPORT_UART_STOPBITS_ONEANDAHALF, /**< 1.5 stop bits. */
    BSP_EXTENSIONPORT_UART_STOPBITS_TWO, /**< 2 stop bits. */
};

/**
 * Enumeration representing the Configuration items for the SPI interface provided on the extension port */
enum BSP_ExtensionPortUartConfig_E
{
    BSP_EXTENSIONPORT_UART_BAUDRATE,
    BSP_EXTENSIONPORT_UART_PARITY,
    BSP_EXTENSIONPORT_UART_STOPBITS,
};

typedef enum BSP_ExtensionPortUartConfig_E BSP_ExtensionPortUartConfig_T;

/**@}*/

/* Put the function declarations here */

/**
 * @brief Maps the GPIO pins,(MOSI,MISO,CLK) to their desired function.
 *
 * @details This function once called will map the GPIO pins,(MOSI,MISO,CLK) of the SPI interface
 *          of the extension port.
 *
 *          Prerequisite: It is necessary to call this function before calling BSP_ExtensionPort_EnableSpi().
 *
 * @retVal RETCODE_OK in the case of success or an error code otherwise.
 * @retVal RETCODE_INVALID_PARAM if the invalid spi pin passed to set the gpio pin as BSP_EXTENSIONPORT_PIN_USEDBY_SPI.
 */
Retcode_T BSP_ExtensionPort_ConnectSpi(void);

/**
 * @brief Returns the  handle for the SPI communication interfaces provided on the extension port
 *
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 *
 * @return HWHandle_T: Handle for the used MCU resource, in this case it is SPI present on the extension port.
 */
HWHandle_T BSP_ExtensionPort_GetSpiHandle(void);

/**
 * @brief Enables the control over the SPI peripheral connected via extension port to their desired operating mode necessary for proper operation.
 *
 * @details A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate. This function once called will prepare the
 * SPI peripheral interfaced with extension port to a functional state such that
 * user can start the SPI transfers after successful execution of this API
 *
 *  * Prerequisite: Before calling BSP_ExtensionPort_EnableSpi() API user needs to call the following API's
 *                BSP_ExtensionPort_ConnectSpi(),BSP_ExtensionPort_SetSpiConfig(), MCU_SPI_Initialize() for proper operation.
 *
 * @retVal RETCODE_OK in the case of success or an error code otherwise.
 * @retVal RETCODE_NULL_POINTER in the case of Handle is NULL.
 * @retVal RETCODE_BSP_EXTENSIONPORT_SPI_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectSpi()
 * @retVal RETCODE_BSP_EXTENSIONPORT_SPI_MCU_IRQ_CALLBACK_NULL if the MCU_SPI_Initialize() was not called before calling this API
 */
Retcode_T BSP_ExtensionPort_EnableSpi(void);

/**
 * @brief Disables the control over the SPI peripheral connected via extension port
 *
 * @details This function once called will disable control over the
 * SPI peripheral interfaced with extension port to a Non-functional state. After this call the
 * SPI transfers must not be initiated from the application.
 *
 * @retVal RETCODE_OK in the case of success.
 * @retVal RETCODE_NULL_POINTER in the case of Handle is NULL
 * @retVal RETCODE_BSP_EXTENSIONPORT_SPI_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectSpi() 
 *
 */
Retcode_T BSP_ExtensionPort_DisableSpi(void);

/**
 * @brief Disconnects the SPI interface of the extension port
 *
 * @details This function disconnects the GPIOs dedicated for the proper operation of SPI.
 * Effectively, it puts those pins into low power consumption state.
 *
 * @retVal RETCODE_OK in the case of success or an error code otherwise.
 * @retVal RETCODE_INVALID_PARAM if the invalid spi pin passed to set the gpio pin as BSP_EXTENSIONPORT_PIN_USEDBY_SPI.
 * @retVal RETCODE_BSP_EXTENSIONPORT_SPI_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectSpi() 
 */
Retcode_T BSP_ExtensionPort_DisonnectSpi(void);

/**
 * @brief Configures the extension port SPI communication interfaces as per the user provided configuration items
 *
 * @details This interface shall be used to set the appropriate SPI interface settings. User needs to provide
 *          proper configuration to have proper function.
 * Prerequisite: Before calling BSP_ExtensionPort_SetSpiConfig() API user needs to call BSP_ExtensionPort_ConnectSpi() for successful configuration
 *
 * @param [in] configItem : configuration item which needs to be configured
 *
 * @param [in] value :  value for the configuration item
 *
 * @param [in] pvalue : Reserved for future use , user needs to pass NULL value to this parameter by default
 *
 * @return A combined error code including the package ID and severity in the
 * upper 16 bits and the error value in the lower 16 bits.
 *
 * @retval  RETCODE_OK upon successful configuration of SPI configuration item.
 * @retval  RETCODE_INVALID_PARAM in case either configuration item or value  is incorrect
 * @retval  RETCODE_NULL_POINTER if either there is a pointer corruption.
 * @retVal RETCODE_BSP_EXTENSIONPORT_SPI_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectSpi() 
 *
 * @note The set configuration will come to effect only upon calling the BSP_ExtensionPort_EnableSpi() API
 * @note If BSP_ExtensionPort_SetSpiConfig() is not called in the application then it will be using default configuration for SPI.
 * @note <b>Default Configuration</b>:
 * @note <b>TX and/or RX enable</b> = usartDisable,
 * @note <b>USART reference clock frequency</b> = 48MHz,
 * @note <b>baudrate</b> = 1Mbps,
 * @note <b>databits</b> = usartDatabits8,
 * @note <b>master</b> = true : operates in master mode,
 * @note <b>msbf</b> = false : operates in least significant bit first,
 * @note <b>clockMode</b> = usartClockMode0 : Clock polarity/phase mode,
 * @note <b>prsRxEnable</b> = false : USART Rx via PRS,
 * @note <b>prsRxCh</b> = usartPrsRxCh0 : PRS channel for USART Rx,
 * @note <b>autoTx</b> = false : Enable AUTOTX mode,
 *
 * Example Usage to configure the SPI interface in SPI mode 0 , baudrate 2000000 and data word length 8 bits:
 * @code
 * #include "BSP_ExtensionPort.h"
 * Retcode_T retVal = RETCODE_OK;
 * retVal = BSP_ExtensionPort_SetSpiConfig(BSP_EXTENSIONPORT_SPI_BAUDRATE,EXT_SPI2_SPI_BAUDRATE,NULL);
 * if(RETCODE_OK == retVal)
 * {
 * retVal = BSP_ExtensionPort_SetSpiConfig(BSP_EXTENSIONPORT_SPI_MODE,EXT_SPI2_SPI_MODE,NULL);
 * }
 * if(RETCODE_OK == retVal)
 * {
 * retVal = BSP_ExtensionPort_SetSpiConfig(BSP_EXTENSIONPORT_SPI_BIT_ORDER,EXT_SPI2_SPI_MOST_SIGNIFICANT_BIT,NULL);
 * }
 * @endcode
 */
Retcode_T BSP_ExtensionPort_SetSpiConfig(BSP_ExtensionPortSpiConfig_T configItem, uint32_t value, void * pvalue);

/**
 * @brief Maps the GPIO pins,(SDA, SCK) to their desired function.
 *
 * @details This function once called will map the GPIO pins,(SDA, SCK) of the I2C interface
 *          of the extension port to their desired operating mode necessary for proper operation.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE , it is currently used
 *
 * @note It is necessary to call this function before calling any other API related to Extension port I2C
 *
 * @note If the error code is  RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE , means these either one or both of these GPIO pins (SDA, SCK) are already
 *         programmed as gpio pin. If user wants to use this those pins needs to be properly uninitialized, then call this  BSP_ExtensionPort_ConnectI2c() API
 */
Retcode_T BSP_ExtensionPort_ConnectI2c(void);

/**
 * @brief Returns the  handle for the I2C communication interfaces provided on the extension port
 *
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 *
 * @return HWHandle_T: Handle for the used MCU resource, in this case it is I2C interface of the extension port.
 */
HWHandle_T BSP_ExtensionPort_GetI2cHandle(void);

/**
 * @brief Enables the control over the I2C peripheral connected via extension port
 *
 * @details A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate. This function once called will prepare the
 * I2C peripheral interfaced with extension port to a functional state such that
 * user can start the I2C transfers after successful execution of this API
 *
 * Prerequisite: Before calling BSP_ExtensionPort_EnableI2c() API user needs to call the following API's
 *                BSP_ExtensionPort_ConnectI2c(),BSP_ExtensionPort_SetI2cConfig(), MCU_I2C_Initialize() for proper operation.
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_NULL_POINTER if either there is a pointer corruption.
 * @retval RETCODE_BSP_EXTENSIONPORT_I2C_MCU_IRQ_CALLBACK_NULL, if the MCU_I2C_Initialize() was not called before calling this API
 * @retVal RETCODE_BSP_EXTENSIONPORT_I2C_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectI2c() 
 */
Retcode_T BSP_ExtensionPort_EnableI2c(void);

/**
 * @brief Disables the control over the I2C peripheral connected via extension port
 * @details This function once called will disable control over the
 * I2C peripheral interfaced with extension port to a Non-functional state. After this call the
 * I2C transfers must not be initiated from the application.
 *
 * Prerequisite: User must have called BSP_ExtensionPort_ConnectI2c() before calling this API
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_NULL_POINTER if either there is a pointer corruption
 * @retVal RETCODE_BSP_EXTENSIONPORT_I2C_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectI2c()  
 */
Retcode_T BSP_ExtensionPort_DisableI2c(void);

/**
 * @brief Disconnects the I2C interface of the extension port
 *
 * @details This function disconnects the GPIOs dedicated for the proper operation of I2C.
 * Effectively, it puts those pins into low power consumption state.
 *
 * Prerequisite: User must have called BSP_ExtensionPort_ConnectI2c() before calling this API
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_NULL_POINTER if either there is a pointer corruption.
 * @retVal RETCODE_BSP_EXTENSIONPORT_I2C_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectI2c()  
 */
Retcode_T BSP_ExtensionPort_DisonnectI2c(void);

/**
 * @brief Configures the extension port I2C communication interfaces as per the user provided configuration items
 *
 * @details This interface shall be used to set the appropriate I2C interface settings. User needs to provide
 *          proper configuration to have proper function.
 *
 *          Prerequisite: Before calling BSP_ExtensionPort_SetI2cConfig() API user needs to call BSP_ExtensionPort_ConnectI2c()
 *          for successful configuration.
 *
 * @param [in] configItem : configuration item which needs to be configured
 *
 * @param [in] value :  value for the configuration item
 *
 * @param [in] pvalue : Reserved for future use , user needs to pass NULL value to this parameter by default
 *
 * @return A combined error code including the package ID and severity in the
 * upper 16 bits and the error value in the lower 16 bits.
 *
 * @retval  RETCODE_OK upon successful configuration of I2C configuration item.
 * @retval  RETCODE_INVALID_PARAM in case either configuration item or value  is incorrect
 * @retVal RETCODE_BSP_EXTENSIONPORT_I2C_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectI2c()  
 *
 * @note The set configuration will come to effect only upon calling the BSP_ExtensionPort_EnableI2c() API.
 * @note If BSP_ExtensionPort_SetI2cConfig() is not called in the application then it will be using default configuration for I2C.
 * @note <b>Default Configuration</b>:
 * @note <b>I2cClockFreq</b> = I2C_FREQ_FAST_MAX,
 *
 * Example Usage to configure the I2C interface in I2C standard Mode
 * @code
 * #include "BSP_ExtensionPort.h"
 * Retcode_T retVal = RETCODE_OK;
 * retVal = BSP_ExtensionPort_SetI2cConfig(BSP_EXTENSIONPORT_I2C_MODE, BSP_EXTENSIONPORT_I2C_STANDARD_MODE, NULL);
 * if(RETCODE_OK == retVal )
 * {
 * // Take further action
 * }
 * @endcode
 */
Retcode_T BSP_ExtensionPort_SetI2cConfig(BSP_ExtensionPortI2cConfig_T configItem, uint32_t value, void * pvalue);

/**
 * @brief Maps the GPIO pins(TX,RX) to their desired function.
 *
 * @details This function once called will map the GPIO pins(TX,RX) of the UART interface
 *          of the extension port to their desired operating mode necessary for proper operation.
 *
 *          Prerequisite: It is necessary to call this function before calling BSP_ExtensionPort_EnableUart().
 *
 * @retval RETCODE_OK in the case of success.
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE If either one or both of these GPIO pins (TX, RX) are already
 *         programmed as gpio pin.
 */
Retcode_T BSP_ExtensionPort_ConnectUart(void);

/**
 * @brief Returns the  handle for the Uart communication interfaces provided on the extension port
 *
 * @details After successful execution of BSP_Board_Initialize(param1, param2), the
 * application is free to call this function in order to get the handle needed
 * by the resources drivers. This handle will be used by the MCU drivers for the
 * intended resource.
 *
 * @return HWHandle_T: Handle for the used MCU resource, in this case it is Uart interface of the extension port.
 */
HWHandle_T BSP_ExtensionPort_GetUartHandle(void);

/**
 * @brief Enables the control over the Uart peripheral connected via extension port
 *
 * @details A prior call to MCU_<RESOURCE>_Initialize() is necessary for this
 * function in order to operate. This function once called will prepare the
 * Uart peripheral interfaced with extension port to a functional state such that
 * user can start the Uart transfers after successful execution of this API
 *
 * Prerequisite: Before calling BSP_ExtensionPort_EnableUart() API user needs to call the following API's
 *               BSP_ExtensionPort_ConnectUart(),BSP_ExtensionPort_SetUartConfig(), MCU_UART_Initialize() for proper operation.
 *
 * @retval RETCODE_OK in the case of success.
 * @retval RETCODE_NULL_POINTER if either there is a pointer corruption
 * @retval RETCODE_BSP_EXTENSIONPORT_UART_MCU_IRQ_CALLBACK_NULL when UART MCU IRQ Callback is NULL.
 * @retVal RETCODE_BSP_EXTENSIONPORT_UART_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectUart()  
 */
Retcode_T BSP_ExtensionPort_EnableUart(void);

/**
 * @brief Disables the control over the Uart peripheral connected via extension port
 * @details This function once called will disable control over the
 * Uart peripheral interfaced with extension port to a Non-functional state. After this call the
 * Uart transmission must not be initiated from the application.
 *
 * @retval RETCODE_OK in the case of success.
 * @retval RETCODE_NULL_POINTER in the case uart handle is NULL
 * @retVal RETCODE_BSP_EXTENSIONPORT_UART_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectUart() 
 *
 */
Retcode_T BSP_ExtensionPort_DisableUart(void);

/**
 * @brief Disconnects the Uart interface of the extension port
 *
 * @details This function disconnects the GPIOs dedicated for the proper operation of Uaer.
 * Effectively, it puts those pins into low power consumption state.
 *
 * @retval RETCODE_OK in the case of success.
 * @retval RETCODE_NULL_POINTER if either there is a pointer corruption
 * @retVal RETCODE_BSP_EXTENSIONPORT_UART_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectUart() 
 *
 */
Retcode_T BSP_ExtensionPort_DisonnectUart(void);

/**
 * @brief Configures the extension port UART communication interfaces as per the user provided configuration items
 *
 * @details This interface shall be used to set the appropriate UART interface settings. User needs to provide
 *          proper configuration to have proper function.
 *
 * Prerequisite: Before calling BSP_ExtensionPort_SetUartConfig() API user needs to call BSP_ExtensionPort_ConnectUart()
 *               for successful configuration.
 *
 * @param [in] configItem : configuration item which needs to be configured
 *
 * @param [in] value :  value for the configuration item
 *
 * @param [in] pvalue : Reserved for future use , user needs to pass NULL value to this parameter by default
 *
 * @return A combined error code including the package ID and severity in the
 * upper 16 bits and the error value in the lower 16 bits.
 *
 * @retval  RETCODE_OK upon successful configuration of UART configuration item.
 * @retval  RETCODE_INVALID_PARAM in case the input parameter is invalid.
 * @retval  RETCODE_NULL_POINTER if either there is a pointer corruption
 * @retVal RETCODE_BSP_EXTENSIONPORT_UART_NOT_CONNECTED in the case of this API is called without calling BSP_ExtensionPort_ConnectUart() 
 *
 * @note The set configuration will come to effect only upon calling the BSP_ExtensionPort_EnableUart() API
 * @note If BSP_ExtensionPort_SetUartConfig() is not called in the application then it will be using default configuration for UART.
 * @note <b>Default Configuration</b>:
 * @note <b>TX and/or RX enable</b> = usartDisable,
 * @note <b>USART reference clock frequency</b> = 0,
 * @note <b>baudrate</b> = 115200bps,
 * @note <b>oversampling</b> = usartOVS16,
 * @note <b>databits</b> = usartDatabits8,
 * @note <b>parity</b> = usartNoParity,
 * @note <b>stopbits</b> = usartStopbits1,
 * @note <b>mvdis</b> = false,
 * @note <b>prsRxEnable</b> = false : USART Rx via PRS,
 * @note <b>prsRxCh</b> = usartPrsRxCh0 : PRS channel for USART Rx.
 *
 * Example Usage to configure the UART interface with 9600 baudrate, No parity and 1 stop bits
 * @code
 * #include "BSP_ExtensionPort.h"
 * Retcode_T retVal = RETCODE_OK;
 * retVal = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_BAUDRATE,9600, NULL);
 * if(RETCODE_OK == retVal )
 * {
 * retVal = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_PARITY, BSP_EXTENSIONPORT_UART_NO_PARITY, NULL);
 * }
 * if(RETCODE_OK == retVal )
 * {
 * retVal = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_STOPBITS,BSP_EXTENSIONPORT_UART_STOPBITS_ONE,NULL);
 * }
 *  * if(RETCODE_OK == retVal )
 * {
 * // Take further action
 * }
 * @endcode
 */
Retcode_T BSP_ExtensionPort_SetUartConfig(BSP_ExtensionPortUartConfig_T configItem, uint32_t value, void * pvalue);


/**
 * @brief SConfigure the ADC peripheral connected via extension port
 *
 * @param [in] pinID :  Identity of the pin, which needs to be used as a ADC pin.
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use.
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 */
Retcode_T BSP_ExtensionPort_ConnectAdc(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Enables the control over the ADC peripheral connected via extension port
 *
 * @param [in] pinID :  Identity of the pin, which needs to be enabled as ADC input, Once this API called we can use pin as ADC.
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 */
Retcode_T BSP_ExtensionPort_EnableAdc(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Disables the control over the ADC peripheral connected via extension port
 *
 * @param [in] pinID :  Identity of the pin, which needs to be disabled from the ADC functionality.
 *
 * @retval RETCODE_OK in the case of success
 */
Retcode_T BSP_ExtensionPort_DisableAdc(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Disconnects the ADC interface of the extension port
 *
 * @param [in] pinID :  Identity of the pin, which needs to disconnect an ADC pin.
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 */
Retcode_T BSP_ExtensionPort_DisconnectAdc(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Maps the GPIO pin to their desired function.
 *
 * @details This function once called will map the GPIO pin of the extension port. *
 *          Prerequisite: It is necessary to call this function before calling BSP_ExtensionPort_EnableGpio().
 *
 * @param [in] pinID :  Identity of the pin, which needs to be connected to the extension port
 *
 * @retval  RETCODE_OK in the case of success.
 * @ retval RETCODE_INVALID_PARAM in case the input parameter pinID is out of limit
 */
Retcode_T BSP_ExtensionPort_ConnectGpio(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Disconnects the GPIO pin of the extension port
 *
 * @details This function disconnects the GPIO pin. i.e effectively, it puts those pins into low power consumption state.
 *
 * @param [in] pinID :  Identity of the pin, which needs to be connected to the extension port
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use.
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED if user tries to disconnect gpio pin without connecting it before using BSP_ExtensionPort_ConnectGpio() or an error code otherwise.
 */
Retcode_T BSP_ExtensionPort_DisconnectGpio(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Toggles the Gpio pin previous state which is connected via extension port
 *
 * @param [in] pinID :  Identity of the pin, which needs to be set to Toggled from the previous state
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use.
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED if user tries to disconnect gpio pin without connecting it before using BSP_ExtensionPort_ConnectGpio()
 */
Retcode_T BSP_ExtensionPort_ToggleGpio(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Sets the Gpio pin connected via extension port
 *
 * @param [in] pinID :  Identity of the pin, which needs to be set to logic High
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use.
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED if user tries to disconnect gpio pin without connecting it before using BSP_ExtensionPort_ConnectGpio()
 */
Retcode_T BSP_ExtensionPort_SetGpio(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Clears the Gpio pin connected via extension port
 *
 * @param [in] pinID :  Identity of the pin, which needs to be set to logic low
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use.
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED if user tries to disconnect gpio pin without connecting it before using BSP_ExtensionPort_ConnectGpio()
 */
Retcode_T BSP_ExtensionPort_ClearGpio(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Reads the Gpio pin connected via extension port
 *
 * @param [in] pinID :  Identity of the pin, which needs to be read
 *
 * @param [in/out] currentPinVal :  read value on the pin mentioned in the input parameter
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use.
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_ENABLED ,  Gpio is not yet configured and enabled
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED if user tries to disconnect gpio pin without connecting it before using BSP_ExtensionPort_ConnectGpio()
 */
Retcode_T BSP_ExtensionPort_ReadGpio(BSP_ExtensionPort_PinId_T pinID, uint8_t *currentPinVal);

/**
 * @brief Configures the extension port GPIO  as per the user provided configuration items
 *
 * @details This interface shall be used to set the appropriate GPIO interface settings. User needs to provide
 *          proper configuration to have proper function.
 *          Prerequisite: This API needs to be called before calling BSP_ExtensionPort_EnableGpio() API.
 *
 * @param [in] pinID :  Identity of the pin, which needs to be configured
 *
 * @param [in] configItem : configuration item which needs to be configured
 *
 * @param [in] value :  value for the configuration item
 *
 * @param [in] pvalue : Reserved for future use , user needs to pass NULL value to this parameter by default
 *
 * @return A combined error code including the package ID and severity in the
 * upper 16 bits and the error value in the lower 16 bits.
 *
 * @retval RETCODE_OK upon successful configuration of UART configuration item.
 * @retval RETCODE_NOT_SUPPORTED in case either configuration item or value is not supported by the MCU
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use hence not configured.
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED if user tries to disconnect gpio pin without connecting it before using BSP_ExtensionPort_ConnectGpio()
 *
 * @note   If user needs to re-configure gpio in run time then he needs to call the following API calls in order,
 *         1. BSP_ExtensionPort_DisableGpio()
 *         2. BSP_ExtensionPort_DisconnectGpio()
 *         On successful completion of the above APIcalls, user can call BSP_ExtensionPort_ConnectGpio() ,BSP_ExtensionPort_SetGpioConfig() and
 *         BSP_ExtensionPort_EnableGpio() in order to correctly operate the gpio with re-configured operation.
 *
 * Example Usage to configure the Gpio pin PA0 as Input pin, Pullup with filter
 * @code
 * #include "BSP_ExtensionPort.h"
 * Retcode_T retVal = RETCODE_OK;
 * retVal = BSP_ExtensionPort_ConnectGpio(BSP_EXTENSIONPORT_GPIO_PA0);
 * if(RETCODE_OK == retVal )
 * {
 * retVal = BSP_ExtensionPort_SetGpioConfig(BSP_EXTENSIONPORT_GPIO_PA0,BSP_EXTENSIONPORT_GPIO_PINVALUE,BSP_EXTENSIONPORT_GPIO_PIN_HIGH,NULL);
 * }
 * if(RETCODE_OK == retVal )
 * {
 * retVal = BSP_ExtensionPort_SetGpioConfig(BSP_EXTENSIONPORT_GPIO_PA0,BSP_EXTENSIONPORT_GPIO_PINVALUE,BSP_EXTENSIONPORT_GPIO_PIN_HIGH,NULL);
 * }
 * if(RETCODE_OK == retVal )
 * {
 * retVal = BSP_ExtensionPort_EnableGpio(BSP_EXTENSIONPORT_GPIO_PA0);
 * }
 *  * if(RETCODE_OK == retVal )
 * {
 * // Take further action
 * }
 * @endcode
 *
 * Example Usage to configure the Gpio pin PA1 as Output pin, push-pull and initial value as Logic High
 * @code
 * #include "BSP_ExtensionPort.h"
 * Retcode_T retVal = RETCODE_OK;
 * retVal = BSP_ExtensionPort_ConnectGpio(BSP_EXTENSIONPORT_GPIO_PA1);
 * if(RETCODE_OK == retVal )
 * {
 * retVal = BSP_ExtensionPort_SetGpioConfig(BSP_EXTENSIONPORT_GPIO_PA1,BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_PUSHPULL, NULL);
 * }
 * if(RETCODE_OK == retVal )
 * {
 * retVal = BSP_ExtensionPort_SetGpioConfig(BSP_EXTENSIONPORT_GPIO_PA1,BSP_EXTENSIONPORT_GPIO_PINVALUE,BSP_EXTENSIONPORT_GPIO_PIN_HIGH,NULL);
 * }
 * if(RETCODE_OK == retVal )
 * {
 * retVal = BSP_ExtensionPort_EnableGpio(BSP_EXTENSIONPORT_GPIO_PA1);
 * }
 *  * if(RETCODE_OK == retVal )
 * {
 * // Take further action
 * }
 * @endcode
 */
Retcode_T BSP_ExtensionPort_SetGpioConfig(BSP_ExtensionPort_PinId_T pinID, BSP_ExtensionPortGpioConfig_T configItem, uint32_t value, void * pvalue);

/**
 * @brief Enables the control over the Gpio pin connected via extension port
 * @details This function once called will put the  pin mentioned by the PinID into operational state.
 *
 * @param [in] pinID :  Identity of the pin, which needs to be Enabled
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use.
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED if user tries to enable without connecting it before using BSP_ExtensionPort_ConnectGpio()
 *
 * @note  Prerequisite: It is necessary to call BSP_ExtensionPort_SetGpioConfig() function before calling BSP_ExtensionPort_EnableGpio() for proper operation
 *         as per the user defined settings. Failing which operations on GPIO may result in unexpected behavior.
 *
 * @note   Please note here that while calling BSP_ExtensionPort_EnableGpio(),
 *         the interrupt registered using BSP_ExtensionPort_RegisterGpioInt() will come into effect.
 */
Retcode_T BSP_ExtensionPort_EnableGpio(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Disables the control over the Gpio pin connected via extension port
 * @details This function once called will disable control over the
 * pin mentioned by the PinID will in disable state
 *
 * @param [in] pinID :  Identity of the pin, which needs to be disabled
 *
 * @note : In this the read / write for this disabled gpio pin should not to be initiated from the application.
 *
 * @note   Please note here that while calling BSP_ExtensionPort_DisableGpio(),
 *         the interrupt registered using BSP_ExtensionPort_RegisterGpioInt() will be disabled and unregistered.
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use hence not disabled.
 * @retval RETCODE_INVALID_PARAM if input parameter is out of limit
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED if user tries to disconnect gpio pin without connecting it before using BSP_ExtensionPort_ConnectGpio()
 */
Retcode_T BSP_ExtensionPort_DisableGpio(BSP_ExtensionPort_PinId_T pinID);

/**
 * @brief Registers the Gpio pin to generate interrupt
 * @details This function once called will configure and register interrupt for the  pin mentioned by the PinID
 *
 * @param [in] pinID :  Identity of the pin, which needs to be registered for gpio interrupt
 * @param [in] interruptMode :  Interrupt trigger mode for gpio interrupt
 * @param [in] gpioAppCallBack : application callback which needs to be called in case of events
 *
 *
 * @retval RETCODE_OK in the case of success
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_IN_USE if already this extension port pin is already in use.
 * @retval RETCODE_INVALID_PARAM if either one of the input parameters is out of limit
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_INT_BUSY  if the interrupt registration not happened,
 *         since other modules are using this pin as interrupt. If user is still read the pin using BSP_ExtensionPort_ReadGpio() from his application
 *
 * @retval RETCODE_BSP_EXTENSIONPORT_INT_REQ_ON_OUTPUT_PIN if user is trying to configure interrupt output pin
 * @retval RETCODE_BSP_EXTENSIONPORT_GPIO_NOT_CONNECTED if user tries to disconnect gpio pin without connecting it before using BSP_ExtensionPort_ConnectGpio()
 *
 * @note   If the user needs to register application callback using BSP_ExtensionPort_RegisterGpioInt(),before enabling the gpio pin using the
 *         BSP_ExtensionPort_EnableGpio(),Failing to do this will not generate event,
 *         even when there is actual trigger on the this input pin registered using this api .
 */
Retcode_T BSP_ExtensionPort_RegisterGpioInt(BSP_ExtensionPort_PinId_T pinID, BSP_ExtensionPort_GpioIntMode_T interruptMode, BSP_ExtensionPort_GpioCallback_T gpioAppCallBack);

/**
 * @brief Enables the power control for the extension board such that the extension bus gets power supply, also put
 *        all the GPIO pins of the extension port pins into disabled state to save power
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 *
 * Example Usage
 * @code
 * #include "BSP_ExtensionPort.h"
 * Retcode_T retVal = RETCODE_OK;
 * retVal = BSP_ExtensionPort_Connect();
 * if(RETCODE_OK == retVal )
 * {
 *  // Take further action
 * }
 *
 * @endcode
 */
Retcode_T BSP_ExtensionPort_Connect(void);

/**
 * @brief Disable the power control for the extension board such that the extension power supply got disconnected,also puts the pin to disable state
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_ExtensionPort_Disconnect(void);

#endif /* BSP_EXTENSIONPORT_H_ */
#endif /* BCDS_FEATURE_BSP_EXTENSIONPORT */
/**@}*/

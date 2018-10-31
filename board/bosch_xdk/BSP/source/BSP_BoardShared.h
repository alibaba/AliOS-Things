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
 *
 * @brief Shared Variables for BSP_XDK110 implementation
 *
 * @details This file contains global variables declaration.
 *
 * This file provides the definition of the shared resources in the board
 * and the means to control those resources from within the board support
 * package. It also contains the definition for the handles required by the
 * efm32 library.
 *
 * @file
 */

#ifndef BoardShared_H
#define BoardShared_H

#include "BCDS_BSP_Board.h"
#include "BCDS_USB.h"

/*
 * Enumeration used to control the locking/unlocking of GPIO functions
 */
enum BSPLock_E
{
    BSP_LOCKED = 0,
    BSP_UNLOCKED,
};

typedef volatile enum BSPLock_E BSPLock_T;
/*
 * Enumeration representing the peripherals connected to the board
 */
enum Peripherals_E
{
    MCU,
    BT_EM9301,
    WIFI_CC3100MOD,
    SENSOR_BMA280,
    MIC_AKU340,
    SENSOR_BMG160,
    SENSOR_BME280,
    SENSOR_BMM150,
    SENSOR_BMI150,
    SENSOR_MAX44009,
    SENSOR_BMI160,
    SD_CARD,
    LED_ALL,
    LED_ORANGE,
    LED_RED,
    LED_YELLOW,
    KEY1,
    KEY2,
    EXTENSION_BOARD,
};
typedef enum Peripherals_E Peripherals_T;

/*
 * Structure representing the nodes connected to the power supply rail 2v5
 * It allows to know if it is used or not and which peripheral is using it.
 */
struct PowerSupply2V5Register_S
{
    uint32_t MCU :1;
    uint32_t BT_EM9301 :1;
    uint32_t WiFi_CC3100MOD :1;
    uint32_t Sensor_BMA280 :1;
    uint32_t Sensor_BMG160 :1;
    uint32_t Sensor_BME280 :1;
    uint32_t Sensor_BMI150 :1;
    uint32_t Sensor_MAX44009 :1;
    uint32_t Sensor_BMI160 :1;
    uint32_t Mic_AKU340 :1;
    uint32_t LED_R :1;
    uint32_t LED_O :1;
    uint32_t LED_Y :1;
    uint32_t ExtensionBoard :1;
    uint32_t SDCard :1;
};

/*
 * Union used to control the power supply rail 2v5. It allows to know if it a
 * peripheral is using it.
 */
union PowerSupply2V5Register_U
{
    struct PowerSupply2V5Register_S Peripheral;
    uint32_t Register;
};
typedef union PowerSupply2V5Register_U PowerSupply2V5Register_T;

/*
 * Structure representing the nodes connected to the power supply rail 3v3
 * It allows to know if it is used or not and which peripheral is using it.
 */
struct PowerSupply3V3Register_S
{
    uint32_t MCU :1;
    uint32_t SDCard :1;
    uint32_t ExtensionBoard :1;
};

/*
 * Union used to control the power supply rail 3v3. It allows to know if it a
 * peripheral is using it.
 */
union PowerSupply3V3Register_U
{
    struct PowerSupply3V3Register_S Peripheral;
    uint32_t Register;
};
typedef union PowerSupply3V3Register_U PowerSupply3V3Register_T;

/*
 * Structure representing  peripherals using I2C1 bus. It allows to know
 * if it is used or not and which peripheral is using it.
 */
struct I2C0Register_S
{
    uint32_t Sensor_BMA280 :1;
    uint32_t Sensor_BMG160 :1;
    uint32_t Sensor_BME280 :1;
    uint32_t Sensor_BMM150 :1;
    uint32_t Sensor_MAX44009 :1;
    uint32_t Sensor_BMI160 :1;
};

/*
 * Union used to control the I2C1 bus. It allows to know if a
 * peripheral is using it.
 */
union I2C0Register_U
{
    struct I2C0Register_S Peripheral;
    uint32_t Register;
};

typedef union I2C0Register_U I2C0Register_T;

/*
 * Structure representing  peripherals using  odd IO external interrupt lines
 * It allows to know if it is used or not and which peripheral is using it.
 */
struct EXTILineOdd_S
{

};
/*
 * Union used to control the odd interrupt GPIO lines of the EFM32GG390 MCU.
 */
union EXTILineOdd_U
{
};

typedef union EXTILineOdd_U EXTILineOdd_T;

/*
 * Structure representing  peripherals using even IO external interrupt lines
 * It allows to know if it is used or not and which peripheral is using it.
 */
struct EXTILineEven_S
{
};
/*
 * Union used to control the even interrupt GPIO lines of the EFM32GG390 MCU.
 */
union EXTILineEven_U
{
};

typedef union EXTILineEven_U EXTILineEven_T;

/*
 * Structure representing the IO ports present on the MCU.
 */
struct GPIOPorts_S
{
    uint32_t GPIOA :1;
    uint32_t GPIOB :1;
    uint32_t GPIOC :1;
    uint32_t GPIOD :1;
    uint32_t GPIOE :1;
    uint32_t GPIOF :1;
};

/*
 * Union used to control the IO ports. It allows to know if it a peripheral is
 * using it.
 */
union GPIOPorts_U
{
    struct GPIOPorts_S Port;
    uint32_t Register;
};

typedef union GPIOPorts_U GPIOPorts_T;

/**
 * @brief
 *  GPIO interrupt callback function pointer.
 * @details
 *   Parameters:
 *   @li pin - The pin index the callback function is invoked for.
 */
typedef void (*GPIOINT_IrqCallbackPtr_t)(void);

extern BSPLock_T ADCLock;
extern I2C0Register_T BSP_I2C0;
extern OStoBSPDelayFunc_T OStoBSPdelayFunc;

/* USB Handle For the Test Interface module                            */
extern USB_lineCoding_t __attribute__((aligned(4))) USB_lineCoding_mds;

/**
 * @brief
 *   Registers user callback for given pin number.
 *
 * @details
 *   Use this function to register a callback which shall be called upon
 *   interrupt generated from given pin number (port is irrelevant). Interrupt
 *   itself must be configured externally. Function overwrites previously
 *   registered callback.
 *
 * @param[in] pin
 *   Pin number for the callback.
 * @param[in] callbackPtr
 *   A pointer to callback function.
 */
void Board_GPIOInterruptRegisterCallback(uint8_t pin, GPIOINT_IrqCallbackPtr_t callbackPtr);

/**
 * @brief
 *   Unregisters user callback for given pin number.
 *
 * @details
 *   Use this function to unregister a callback.
 *
 * @param[in] pin
 *   Pin number for the callback.
 *
 */
static inline void Board_GPIOInterruptUnregisterCallback(uint8_t pin)
{
    Board_GPIOInterruptRegisterCallback(pin, 0);
}

/**
 * @brief Returns the callback pointer
 * @details This function is used to query the callback function registered
 * for the requested pin.
 * @param pin The pin number.
 * @return The callback pointer.
 */
GPIOINT_IrqCallbackPtr_t Board_GPIOInterruptGetCallback(uint8_t pin);

/**
 * @brief Enables the 3V3 power rail
 *
 * @details This function holds a register of the connected peripherals to the
 * power supply and if a peripheral requests enabling it, the MCU output
 * enabling the Power rail is Set  its and the corresponding peripheral bit
 * field is Set
 *
 * @param Peripheral : enumerated value representing the peripheral requesting
 * the Enabling of the power supply
 *
 * @return RETCODE_OK in case of success error code otherwise.
 */
void Board_EnablePowerSupply3V3(Peripherals_T Peripheral);

/**
 * @brief Disables the 3V3 power rail
 *
 * @details This function holds a register of the connected peripherals to the
 * power supply and if a peripheral requests disabling it, the corresponding
 * bit field is Reset and the register  and if this register equals zero then
 * the power rail is safe to be disabled.
 *
 * @param Peripheral : enumerated value representing the peripheral requesting
 * the Disabling of the power supply
 *
 * @return RETCODE_OK in case of success error code otherwise.
 */
void Board_DisablePowerSupply3V3(Peripherals_T Peripheral);

/**
 * @brief Performs operations prior to SysTick count increment.
 * @details This function is initialized during BSP_Board_OSTickInitialize()
 * and makes it possible for the application to perform some operations before
 * the SysTick counter is incremented
 * (e.g. context switching in OS based applications)
 *  * This function is called in SysTick_Handler() function in IrqHandler.c file
 */

/**
 * @brief Wakes up the 2V5 power rail
 *
 * @details This function holds a register of the connected peripherals to the
 * power supply and if a peripheral requests enabling it, the MCU output
 * enabling the Power rail is Set  its and the corresponding peripheral bit
 * field is Set
 *
 * @param Peripheral : enumerated value representing the peripheral requesting
 * the Enabling of the power supply
 */
void Board_WakeupPowerSupply2V5(Peripherals_T Peripheral);

/**
 * @brief Puts the 2V5 power rail on low power mode(snooze).
 *
 * @details This function holds a register of the connected peripherals to the
 * power supply and if a peripheral requests disabling it, the corresponding
 * bit field is Reset and the register  and if this register equals zero then
 * the power rail is safe to be scaled down.
 *
 * @param Peripheral : enumerated value representing the peripheral requesting
 * the Snoozing of the power supply
 *
 * @return RETCODE_OK in case of success error code otherwise.
 */
void Board_SnoozePowerSupply2V5(Peripherals_T Peripheral);

/**
 * @brief Performs operations prior to SysTick count increment.
 * @details This function is initialized during BSP_Board_OSTickInitialize()
 * and makes it possible for the application to perform some operations before
 * the SysTick counter is incremented
 * (e.g. context switching in OS based applications)
 *  * This function is called in SysTick_Handler() function in IrqHandler.c file
 */

extern BSP_Systick_Callback_T SysTickOsPreProcessing;

/**
 * @brief Performs operations Post to SysTick count increment.
 * @details This function is initialized during BSP_Board_OSTickInitialize()
 * and makes it possible for the application to perform some operations after
 * the SysTick counter has is incremented
 * (e.g. context switching in OS based applications)
 * This function is called in SysTick_Handler() function in IrqHandler.c file
 */
extern BSP_Systick_Callback_T SysTickOsPostProcessing;

/**
 * @brief
 *   Function calls users callback for registered pin interrupts.
 *
 * @details
 *   This function is called when GPIO interrupts are handled by the dispatcher.
 *   Function gets even or odd interrupt flags and calls user callback
 *   registered for that pin. Function iterates on flags starting from MSB.
 *
 * @param iflags
 *  Interrupt flags which shall be handled by the dispatcher.
 *
 */
void Board_GPIOInterruptDispatch(uint32_t iflags);

/**
 * @brief This function is called to increment a global variable "BSP_Tick"
 *        used as application time base.
 * @note In the default implementation, this variable is incremented each 1ms
 *       in SysTick ISR.
 * @retval None
 */
void Board_IncTick(void);

/**
 * @brief Provide a tick value in millisecond.
 * @retval tick value
 */
uint32_t Board_GetTick(void);

/**
 * @brief Provide a delay in millisecond.
 * @param delay delay time in ms;
 */
void Board_Delay(uint32_t delay);

#endif /* BoardShared_H */



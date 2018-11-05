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
 * @defgroup BSP_BOARD_TYPE  BSP_BoardType
 * @{
 *
 * @brief Board support package of Bosch XDK (Cross Development Platform) project .
 *
 * @details This file is aimed to define the on configuration items
 * (e.g function arguments) necessary for the execution of the BSP functions.
 *
 * @file
 */

#ifndef BSP_BoardType_H
#define BSP_BoardType_H


/**
 * @ingroup BSP_BOARD_TYPE
 * @defgroup BOARD_TYPE_LED BSP_LED enums
 * @{
 * @brief List of Enums used for the BSP LED module
 *
 */
/**
 * Enumeration representing on board LEDs.
 */
enum BSP_LEDEnableCtrl_E
{
    BSP_XDK_LED_R = 1, /**< Red LED	*/
    BSP_XDK_LED_O = 2, /**< Orange LED */
    BSP_XDK_LED_Y = 3, /**< Yellow LED */
	BSP_XDK_LED_ONBOARD_MAX, /**< Boundary check macro for type BSP_LEDEnableCtrl_E */
};

/**
 * Enumeration of the implemented LED commands
 */
enum BSP_LED_Command_E
{
    BSP_LED_COMMAND_OFF = 1, /**< LED OFF command */
    BSP_LED_COMMAND_ON = 2, /**< LED ON command */
    BSP_LED_COMMAND_TOGGLE = 3 /**< LED TOGGLE command */
};

/**@}*/


/**
 * @ingroup BSP_BOARD_TYPE
 * @defgroup BOARD_TYPE_SD_CARD BSP_SD_CARD enums
 * @{
 * @brief List of Enums used for the BSP SD card module
 *
 */
/**
 * Enumeration for SD Card detection function.
 */
enum BSP_SDCardDetection_E
{
    BSP_XDK_SDCARD_DETECTION_ERROR = 0, /**< Value representing, error in detecting SD card */
    BSP_XDK_SDCARD_INSERTED = 1, /**< Value representing, SD card is inserted in XDK */
    BSP_XDK_SDCARD_EJECTED = 2 /**< Value representing, SD card is ejected out of XDK */
};
/**@}*/


/**
 * @ingroup BSP_BOARD_TYPE
 * @defgroup BOARD_TYPE_BUTTON BSP_BUTTON enums
 * @{
 * @brief List of Enums used for the BUTTON module
 *
 */
/**
 * Enumeration used to report the press nature either short press or long press
 *  - A button press is evaluated as short if it has been steady for minimum
 * specified duration, the event is reported when the button is released
 *
 *  - A Button Press is evaluated as long if it has lasted for longer than a
 * specified time duration e.g. 5 seconds, the event is reported during the
 * button press.
 */
enum BSP_ButtonPress_E
{
    BSP_XDK_BUTTON_PRESS = 1, /**< Button has been pressed and debounced */
    BSP_XDK_BUTTON_RELEASE = 2, /**< Button has been released */
};

typedef enum BSP_ButtonPress_E BSP_ButtonPress_T;

/**
 * Enumeration representing on board buttons.
 */
enum BSP_Button_E
{
    BSP_XDK_BUTTON_1 = 1, /**< User button 1 */
    BSP_XDK_BUTTON_2 = 2, /**< user button 2 */
	BSP_XDK_BUTTON_MAX = 3, /**< Boundary check macro for type BSP_Button_E */
};



/**
 * Enumeration used to report the button logic state (pressed or released).
 */
enum BSP_ButtonStatus_E
{
    BSP_XDK_BUTTON_ERROR = 0, /**< invalid parameter passed or button not enabled*/
    BSP_XDK_BUTTON_PRESSED = 1, /**< Button is pressed */
    BSP_XDK_BUTTON_RELEASED = 2, /**< Button is released */
};

/**
 * type definition for the button status.
 */
typedef enum BSP_ButtonStatus_E BSP_ButtonStatus_T;

/**@}*/

/**
 * @ingroup BSP_BOARD_TYPE
 * @defgroup BOARD_TYPE_SENSOR_NODE BSP_SENSOR_NODE enums
 * @{
 * @brief List of Enums used for the SENSOR_NODE module
 *
 */
/**
 * Enumeration used to report the interrupt that has occurred.
 * In addition to the communication interface interrupts the sensor node could
 * support peripherals that have IO interrupts e.g. in case of threshold the
 * interrupt could be used to wakeup the MCU the BSP informs the application
 * that the given peripheral has generated the interrupt indexed by
 * INTERRUPT_INDEX_x.
 */
enum BSP_InterruptIndex_E
{
    BSP_INTERRUPT_INDEX_1 = 1,/**< Refers to Interrupt Pin 1 of the sensor , if supported by the sensor*/
    BSP_INTERRUPT_INDEX_2 = 2,/**< Refers to Interrupt Pin 2 of the sensor, if supported by the sensor */
    BSP_INTERRUPT_INDEX_MAX /**< Boundary check macro for type BSP_InterruptIndex_E */
};
/**
 * Enumeration used to report the edge that resulted in an interrupt
 */
enum BSP_InterruptEdge_E
{
    BSP_FALLING_EDGE,/**< BSP_FALLING_EDGE */
    BSP_RISING_EDGE  /**< BSP_RISING_EDGE */
};

/**
 * Enumeration representing on board sensors. Those are the sensors connected
 * to the I2C0 bus of the MCU
 */
enum BSP_Sensor_E
{ 
    BSP_XDK_SENSOR_BMA280 = 1, /**< Accelerometer Sensor */
    BSP_XDK_SENSOR_BMG160 = 2, /**< Gyroscope sensor */
    BSP_XDK_SENSOR_BME280 = 3, /**< Barometric Pressure and Humidity Sensor */
    BSP_XDK_SENSOR_BMM150 = 4, /**< Geomagnetic sensor */
    BSP_XDK_SENSOR_MAX44009 = 5, /**< Ambient light sensor */
    BSP_XDK_SENSOR_BMI160 = 6 /**< Accelerometer and Gyroscope */
};
/**@}*/

/**
 * @ingroup BSP_BOARD_TYPE
 * @defgroup BOARD_TYPE_PWR_DETECT BSP_POWER_RELATED enums
 * @{
 * @brief List of Enums used for the power detection module
 *
 */
enum BSP_PowerSupply_E
{
    POWER_SUPPLY_UNKNOWN = 0,
    POWER_SUPPLY_USB = 1,
    POWER_SUPPLY_BATTERY = 2
};

enum BSP_PowerRailControl_E
{
    BSP_XDK_RAIL_3V3_DISABLE = 1, /**< Disable on board 3V3 power rail */
    BSP_XDK_RAIL_2V5_SNOOZE = 2, /**< Snooze on board 2V5 power rail */
    BSP_XDK_RAIL_3V3_EXT_DISABLE = 3 /**< disable 3V3 extension board power rail (e.g. cellular/GPS extension module) */
};

/**
 * Enumeration representing the logic states of an input
 */
enum BSP_IOState_E
{
    BSP_XDK_IO_STATE_ERROR = 0, /**< BSP_INPUT_STATE_ERROR */
    BSP_XDK_IO_STATE_LOW = 1, /**< BSP_INPUT_STATE_LOW */
    BSP_XDK_IO_STATE_HIGH = 2, /**< BSP_INPUT_STATE_HIGH */
};

/**
 * Enumeration representing the configurable power modes
 */
enum BSP_BoardPowerMode_E
{
    BSP_XDK_MODE_EM_2 = 2, /**< MCU Energy Mode 2 mode */
};

/**
 * Enumeration representing the possible wakeup events.
 */
enum BSP_WakeupEvent_E
{
    BSP_XDK_WAKE_EVENT_NONE = 1,
};

/**
 * type definition for the wakeup event.
 */
typedef enum BSP_WakeupEvent_E BSP_WakeupEvent_T;

/**
 * type definition for the power mode parameters.
 */
typedef enum BSP_BoardPowerMode_E BSP_BoardPowerMode_T;
/**
 * type definition for the power rail control parameters.
 */
typedef enum BSP_PowerRailControl_E BSP_BoardPowerRailCtrl_T;

/**
 * type definition for the power supply values.
 */
typedef enum BSP_PowerSupply_E BSP_PowerSupply_T;

/**
 * type definition for the input state values.
 */
typedef enum BSP_IOState_E BSP_IOState_T;

/**@}*/


/**
 * @ingroup BSP_BOARD_TYPE
 * @defgroup BOARD_TYPE_CHARGER BSP_BATT_CHARGER enums
 * @{
 * @brief List of Enums used for the battering monitoring module
 *
 */

/**
 * Enumeration representing the analog signals that are to be measured for the
 * battery charge monitoring.
 */
enum BSP_AnalogSignal_E
{
    BSP_XDK_VOUT = 1, /**< Battery voltage */ /**< BSP_XDK_VBAT */
};

/**
 * Enumeration representing the possible charging levels in the current HW
 * configuration
 */
enum BSP_ChargingSpeed_E
{
    BSP_XDK_CHARGING_SPEED_1 = 2, /**< BSP_XDK_CHARGING_SPEED_1: charging current set to 100mA */
    BSP_XDK_CHARGING_SPEED_2 = 3, /**< BSP_XDK_CHARGING_SPEED_2: charging current set to 500mA */
};

/**
 * Enumeration used to report the current charge state
 */
enum BSP_ChargeState_E
{
    BSP_XDK_CHARGE_STATUS_UNDEFINED = 0, /**< Charge status could not been defined */
    BSP_XDK_CHARGE_STATUS_NO_BATTERY, /**< Battery could not be detected, */
    BSP_XDK_CHARGE_STATUS_ON, /**< Battery is being charged */
    BSP_XDK_CHARGE_STATUS_CRITICAL, /**< Battery voltage is critical */
    BSP_XDK_CHARGE_STATUS_LOW, /**< Battery voltage is low */
    BSP_XDK_CHARGE_STATUS_NORMAL, /**< Battery voltage is normal */
    BSP_XDK_CHARGE_STATUS_FULL, /**< Battery voltage is full */
    BSP_XDK_CHARGE_STATUS_ERROR, /**< if charger has not been enabled by calling enable function with correct parameters */
};

enum BSP_ChargerMeasurement_E
{
    BSP_XDK_EXT_SENSE_VBAT_ADC = 55
};

/**
 * type definition for the charge state
 */
typedef enum BSP_ChargeState_E BSP_ChargeState_T;

/**@}*/



/**
 * Enumeration representing the logic states of an input
 */

enum BSP_IOPin_E
{
    BSP_XDK_IO_EXT_CHG_N,
    BSP_XDK_IO_EXT_PGOOD_N
};

enum BSP_BoardInitializeCtrl_E
{
    BSP_XDK_INIT_DEFAULT,
};

/**
 * Enumeration representing on Extension Bus Support Pins on XDK
 */
enum BSP_GPIO_ID_E
{
   BSP_GPIO_EXT_TEMPERATURE_IN_A = 0, /*< External Temperature Sensor interface >*/
   BSP_GPIO_EXT_TEMPERATURE_IN_B, /*< External Temperature Sensor interface >*/
   BSP_GPIO_EXT_TEMPERATURE_OUT_A, /*< External Temperature Sensor interface >*/
   BSP_GPIO_EXT_TEMPERATURE_OUT_B, /*< External Temperature Sensor interface >*/
   BSP_GPIO_EMPTY, /* Out of range */
};

#endif /* BSP_BoardType_H */
/**@}*/


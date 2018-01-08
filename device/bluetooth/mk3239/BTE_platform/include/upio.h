/*****************************************************************************
**
**  Name    upio.h
**
**  Description
**      Definitions for UPIO driver
**
*****************************************************************************/
#ifndef UPIO_H
#define UPIO_H

#define UDRV_API

/* Enumeration of UPIO features                                         */
/* Not all features enumerated here are supported by the hardware.      */
/* Use UPIO_Feature() to determine support of a particular feature.     */
enum {
    /* LEDs */
    UPIO_FEAT_LED1,
    UPIO_FEAT_LED2,
    UPIO_FEAT_LED3,
    UPIO_FEAT_LED4,
    UPIO_FEAT_LED5,
    UPIO_FEAT_LED6,
    UPIO_FEAT_LED7,
    UPIO_FEAT_LED8,

    /* Switches */
    UPIO_FEAT_SWITCH1,
    UPIO_FEAT_SWITCH2,
    UPIO_FEAT_SWITCH3,
    UPIO_FEAT_SWITCH4,
    UPIO_FEAT_SWITCH5,
    UPIO_FEAT_SWITCH6,
    UPIO_FEAT_SWITCH7,
    UPIO_FEAT_SWITCH8,
    UPIO_FEAT_SWITCH9,
    UPIO_FEAT_SWITCH10,
    UPIO_FEAT_SWITCH11,
    UPIO_FEAT_SWITCH12,
    UPIO_FEAT_SWITCH13,
    UPIO_FEAT_SWITCH14,
    UPIO_FEAT_SWITCH15,
    UPIO_FEAT_SWITCH16,

    /* Jumpers */
    UPIO_FEAT_JUMPER1,
    UPIO_FEAT_JUMPER2,
    UPIO_FEAT_JUMPER3,
    UPIO_FEAT_JUMPER4,
    UPIO_FEAT_JUMPER5,
    UPIO_FEAT_JUMPER6,
    UPIO_FEAT_JUMPER7,
    UPIO_FEAT_JUMPER8,

    /* Push buttons */
    UPIO_FEAT_BUTTON1,
    UPIO_FEAT_BUTTON2,
    UPIO_FEAT_BUTTON3,
    UPIO_FEAT_BUTTON4,
    UPIO_FEAT_BUTTON5,
    UPIO_FEAT_BUTTON6,
    UPIO_FEAT_BUTTON7,
    UPIO_FEAT_BUTTON8,

    /* General purpose */
    UPIO_FEAT_GENERAL1,
    UPIO_FEAT_GENERAL2,
    UPIO_FEAT_GENERAL3,
    UPIO_FEAT_GENERAL4,
    UPIO_FEAT_GENERAL5,
    UPIO_FEAT_GENERAL6,
    UPIO_FEAT_GENERAL7,
    UPIO_FEAT_GENERAL8,
    UPIO_FEAT_GENERAL9,
    UPIO_FEAT_GENERAL10,
    UPIO_FEAT_GENERAL11,
    UPIO_FEAT_GENERAL12,
    UPIO_FEAT_GENERAL13,
    UPIO_FEAT_GENERAL14,
    UPIO_FEAT_GENERAL15,
    UPIO_FEAT_GENERAL16,
    UPIO_FEAT_GENERAL17,
    UPIO_FEAT_GENERAL18,
    UPIO_FEAT_GENERAL19,
    UPIO_FEAT_GENERAL20,
    UPIO_FEAT_GENERAL21,
    UPIO_FEAT_GENERAL22,
    UPIO_FEAT_GENERAL23,
    UPIO_FEAT_GENERAL24,
    UPIO_FEAT_GENERAL25,
    UPIO_FEAT_GENERAL26,
    UPIO_FEAT_GENERAL27,
    UPIO_FEAT_GENERAL28,
    UPIO_FEAT_GENERAL29,
    UPIO_FEAT_GENERAL30,
    UPIO_FEAT_GENERAL31,
    UPIO_FEAT_GENERAL32,

    UPIO_FEAT_IN_HIGH,          /* Support for input with interrupt on high signal level. */
    UPIO_FEAT_IN_LOW,           /* Support for input with interrupt on low signal level. */
    UPIO_FEAT_IN_RISE,          /* Support for input with interrupt on rising edge. */
    UPIO_FEAT_IN_FALL           /* Support for input with interrupt on falling. */

};
typedef UINT8 tUPIO_FEATURE;


/* Enumeration of UPIO configurations */
enum {
    UPIO_OUT,
    UPIO_IN,
    UPIO_IN_EDGE,
    UPIO_IN_LEVEL,
    UPIO_NONE
};
typedef UINT8 tUPIO_CONFIG;


/* Enumeration of UPIO types */
enum {
    UPIO_LED,                   /* LED */
    UPIO_SWITCH,                /* Switch */
    UPIO_JUMPER,                /* Jumper */
    UPIO_BUTTON,                /* Push-button switch */
    UPIO_GENERAL,               /* General purpose I/O */

    UPIO_NUMBER_OF_TYPES
};
typedef UINT8 tUPIO_TYPE;


/* Enumeration of UPIO states */
enum {
    UPIO_OFF,
    UPIO_ON,
    UPIO_TOGGLE
};
typedef UINT8 tUPIO_STATE;


enum {
    UPIO_SW_BANK2,
    UPIO_SW_BANK3
};
typedef UINT8 tUPIO_SW_BANK;

/* Jumper masks */
#define UPIO_JUMPER1    0x00000001
#define UPIO_JUMPER2    0x00000002
#define UPIO_JUMPER3    0x00000004
#define UPIO_JUMPER4    0x00000008
#define UPIO_JUMPER5    0x00000010
#define UPIO_JUMPER6    0x00000020
#define UPIO_JUMPER7    0x00000040
#define UPIO_JUMPER8    0x00000080
#define UPIO_JUMPER9    0x00000100
#define UPIO_JUMPER10   0x00000200
#define UPIO_JUMPER11   0x00000400
#define UPIO_JUMPER12   0x00000800

/* General purpose i/o masks */
#define UPIO_GENERAL1   0x00000001
#define UPIO_GENERAL2   0x00000002
#define UPIO_GENERAL3   0x00000004
#define UPIO_GENERAL4   0x00000008
#define UPIO_GENERAL5   0x00000010
#define UPIO_GENERAL6   0x00000020
#define UPIO_GENERAL7   0x00000040
#define UPIO_GENERAL8   0x00000080
#define UPIO_GENERAL9   0x00000100
#define UPIO_GENERAL10  0x00000200
#define UPIO_GENERAL11  0x00000400
#define UPIO_GENERAL12  0x00000800
#define UPIO_GENERAL13  0x00001000
#define UPIO_GENERAL14  0x00002000
#define UPIO_GENERAL15  0x00004000
#define UPIO_GENERAL16  0x00008000
#define UPIO_GENERAL17  0x00010000
#define UPIO_GENERAL18  0x00020000
#define UPIO_GENERAL19  0x00040000
#define UPIO_GENERAL20  0x00080000
#define UPIO_GENERAL21  0x00100000
#define UPIO_GENERAL22  0x00200000
#define UPIO_GENERAL23  0x00400000
#define UPIO_GENERAL24  0x00800000
#define UPIO_GENERAL25  0x01000000
#define UPIO_GENERAL26  0x02000000
#define UPIO_GENERAL27  0x04000000
#define UPIO_GENERAL28  0x08000000
#define UPIO_GENERAL29  0x10000000
#define UPIO_GENERAL30  0x20000000
#define UPIO_GENERAL31  0x40000000
#define UPIO_GENERAL32  0x80000000

typedef UINT32 tUPIO;

/* LED masks */
#define UPIO_LED1       0x00000001
#define UPIO_LED2       0x00000002
#define UPIO_LED3       0x00000004
#define UPIO_LED4       0x00000008
#define UPIO_LED5       0x00000010
#define UPIO_LED6       0x00000020
#define UPIO_LED7       0x00000040
#define UPIO_LED8       0x00000080

#define UPIO_LED_ALL    (UPIO_LED1 | UPIO_LED2 | UPIO_LED3 | UPIO_LED4 | \
                        UPIO_LED5 | UPIO_LED6 | UPIO_LED7 | UPIO_LED8)


/* Switch masks */
#define UPIO_SWITCH1    0x00000001
#define UPIO_SWITCH2    0x00000002
#define UPIO_SWITCH3    0x00000004
#define UPIO_SWITCH4    0x00000008
#define UPIO_SWITCH5    0x00000010
#define UPIO_SWITCH6    0x00000020
#define UPIO_SWITCH7    0x00000040
#define UPIO_SWITCH8    0x00000080
#define UPIO_SWITCH9    0x00000100
#define UPIO_SWITCH10   0x00000200
#define UPIO_SWITCH11   0x00000400
#define UPIO_SWITCH12   0x00000800
#define UPIO_SWITCH13   0x00001000
#define UPIO_SWITCH14   0x00002000
#define UPIO_SWITCH15   0x00004000
#define UPIO_SWITCH16   0x00008000

/* Push button masks */
#define UPIO_BUTTON1    0x00000001
#define UPIO_BUTTON2    0x00000002
#define UPIO_BUTTON3    0x00000004
#define UPIO_BUTTON4    0x00000008
#define UPIO_BUTTON5    0x00000010
#define UPIO_BUTTON6    0x00000020
#define UPIO_BUTTON7    0x00000040
#define UPIO_BUTTON8    0x00000080

typedef void (tUPIO_CBACK)(tUPIO_TYPE type, tUPIO pio, tUPIO_STATE state);

#ifdef __cplusplus
extern "C" {
#endif

/* API functions for UPIO driver */

/*****************************************************************************
**
** Function         UPIO_Init
**
** Description
**      Initialize the GPIO service.
**      This function is typically called once upon system startup.
**
** Returns          nothing
**
*****************************************************************************/
UDRV_API void UPIO_Init(void *p_cfg);


/*****************************************************************************
**
** Function         UPIO_Set
**
** Description
**      This function sets one or more GPIO devices to the given state.
**      Multiple GPIOs of the same type can be masked together to set more
**      than one GPIO. This function can only be used on types UPIO_LED and
**      UPIO_GENERAL.
**
** Input Parameters:
**      type    The type of device.
**      pio     Indicates the particular GPIOs.
**      state   The desired state.
**
** Output Parameter:
**      None.
**
** Returns:
**      None.
**
*****************************************************************************/
UDRV_API void UPIO_Set(tUPIO_TYPE type, tUPIO pio, tUPIO_STATE state);


/*****************************************************************************
**
** Function         UPIO_Read
**
** Description
**      Read the state of a GPIO. This function can be used for any type of
**      device. Parameter pio can only indicate a single GPIO; multiple GPIOs
**      cannot be masked together.
**
** Input Parameters:
**      Type:   The type of device.
**      pio:    Indicates the particular GUPIO.
**
** Output Parameter:
**      None.
**
** Returns:
**      State of GPIO (UPIO_ON or UPIO_OFF).
**
*****************************************************************************/
UDRV_API tUPIO_STATE UPIO_Read(tUPIO_TYPE type, tUPIO pio);


/*****************************************************************************
**
** Function         UPIO_Config
**
** Description      - Configure GPIOs of type UPIO_GENERAL as inputs or outputs
**                  - Configure GPIOs to be polled or interrupt driven
**
**                  Currently only support polled GPIOs.
**
** Input Parameters:
**      type    The type of device.
**      pio     Indicates the particular GPIOs.
**      config
**      cback
**
** Output Parameter:
**      None.
**
** Returns:
**      None.
**
*****************************************************************************/
UDRV_API void UPIO_Config(tUPIO_TYPE type, tUPIO pio, tUPIO_CONFIG config, tUPIO_CBACK *cback);


/*****************************************************************************
**
** Function         UPIO_Feature
**
** Description
**      Checks whether a feature of the pio API is supported
**
** Input Parameter:
**      feature     The feature to check
**
** Output Parameter:
**      None.
**
** Returns:
**      TRUE if feature is supported, FALSE if it is not.
**
*****************************************************************************/
UDRV_API BOOLEAN UPIO_Feature(tUPIO_FEATURE feature);

#ifdef __cplusplus
}
#endif

#endif  /* ifdef UPIO_H */

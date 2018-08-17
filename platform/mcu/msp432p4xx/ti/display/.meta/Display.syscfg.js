/*
 *  ======== Display ========
 *  Display configuration support
 */
"use strict";
/* global exports, system */

let Common   = system.getScript("/ti/drivers/Common.js");
let UART     = system.getScript("/ti/drivers/UART");
let logError = Common.logError;

/* Intro splash on GUI */
let longDescription = "The Display middleware-driver in TI-RTOS is "
    + "designed to abstract operations and considerations specific to "
    + "given output method.";

/* The configurables common to Display across all devices */
let config = [
    {
        name        : "displayType",
        displayName : "Display Type",
        default     : "uart",
        onChange   : onDisplayTypeChange,
        options : [
            {name: "uart", displayName: "UART"},
            {name: "lcd", displayName: "LCD"},
            {name : "host", displayName: "Host"}
        ]
    },
    {
        name        : "lcdSize",
        displayName : "LCD Display Size",
        description : "Size of the LCD in pixels.",
        default     : 96,
        hidden      : true
    },
    {
        name        : "uartBufferSize",
        displayName : "UART Display Buffer Size",
        description : "UART display buffer size in bytes",
        default     : 1024
    },
    {
        name        : "useANSI",
        displayName : "Enable ANSI cursor support",
        default     : false
    },
    {
        name        : "maxPrintLen",
        displayName : "Maximum print length",
        default     : 256,
        hidden      : true
    },
    {
        name        : "mutexTimeout",
        displayName : "Mutex Timeout",
        default     : 'neverTimeout',
        onChange    : onChangeMutexTimeout,
        options     : [
            { name : 'neverTimeout' },
            { name : 'neverWait'    },
            { name : 'custom', displayName: 'Custom number of RTOS Clocks' }
        ]
    },
    {
        name        : "mutexTimeoutValue",
        displayName : "Mutex Timeout Custom Numeric Value",
        hidden      : true,
        default     : 1
    },
    {
        name        : "baudRate",
        displayName : "Baud Rate Required",
        default     : 115200,
        options     : [
            { name:   1200 },
            { name:   2400 },
            { name:   4800 },
            { name:   9600 },
            { name:  19200 },
            { name:  38400 },
            { name:  57600 },
            { name: 115200 },
            { name: 230400 },
            { name: 460800 },
            { name: 921600 }
        ]
    }
];

/*
 * ======== moduleInstances ========
 * Return an array of driver module instances owned by this module.
 *
 * The module instances returned from this function will be exclusively
 * owned by this Display module.
 *
 * returns - array of owned module instances
 */
function moduleInstances(inst)
{
    if (inst.displayType == "uart") {
        return ([
            {
                name       : "UART",
                moduleName : "/ti/drivers/UART",
                hardware   : inst.$hardware
            }
        ]);
    }

    if (inst.displayType == "lcd") {
        return ([
            {
                name       : "GPIO_LCD_ENABLE",
                moduleName : "/ti/drivers/GPIO",
                hardware   : inst.$hardware ? inst.$hardware.subComponents.LCD_ENABLE : null
            },
            {
                name       : "GPIO_LCD_POWER",
                moduleName : "/ti/drivers/GPIO",
                hardware   : inst.$hardware ? inst.$hardware.subComponents.LCD_POWER : null
            },
            {
                name       : "GPIO_LCD_CS",
                moduleName : "/ti/drivers/GPIO",
                hardware   : inst.$hardware ? inst.$hardware.subComponents.LCD_CS : null
            }
        ]);
    }

    return ([]);
}

/*
 * ======== sharedModuleInstances ========
 * Return an array of driver module instances shared with this module.
 *
 * returns - array of shared driver instances
 */
function sharedModuleInstances(inst)
{
    if (inst.displayType == "lcd") {
        return ([
            {
                name       : "SPI",
                moduleName : "/ti/drivers/SPI",
                hardware   : inst.$hardware ? inst.$hardware.subComponents.LCD_SPI : null
            }
        ]);
    }

    return ([]);
}

/*
 * ======== validate ========
 * Validate the display configurables
 *
 * param inst       - Display instance
 * param validation - Validation object
 *
 */
function validate(inst, validation)
{
    if (inst.mutexTimeoutValue <= 0) {
        logError(validation, inst, 'mutexTimeoutValue', 
                 'Must be a positive integer.');
    }

    if (inst.uartBufferSize < 32) {
        logError(validation, inst, 'uartBufferSize', 
                 'Must be greater than 32 bytes.');
    }

    if (inst.maxPrintLen <= 0) {
        logError(validation, inst, 'maxPrintLen', 
                 'Must be a positive integer.');
    }

    if (inst.lcdSize <= 0) {
        logError(validation, inst, 'lcdSize', 'Must be a positive integer.');
    }

    /* Validate the display baud rate against the uart baud rate list */
    if (inst.displayType == "uart" && inst.UART.hasOwnProperty('baudRates')) {
        if (! inst.UART.baudRates.includes(inst.baudRate)) {
            logError(validation, inst, 'baudRate', 
            "Value not configured as a possibility in UART's baudrates");
        }
    }
}

/*
 * ======== onChangeMutexTimeout ========
 * Callback function invoked when the mutexTimeout is Changed
 *
 * param inst  - Display instance
 * param ui    - A handle to the UI state
 *
 */
function onChangeMutexTimeout(inst, ui)
{
    if (inst.mutexTimeout == 'custom') {
        ui.mutexTimeoutValue.hidden = false;
    }
    else {
        ui.mutexTimeoutValue.hidden = true;
    }
}

/*
 *  ======== onDisplayTypeChange ========
 *  Show/hide appropriate config options for each type of display
 */
function onDisplayTypeChange(inst, ui)
{
    if (inst.displayType == "lcd") {
        ui.useANSI.hidden = true;
        ui.maxPrintLen.hidden = true;
        ui.uartBufferSize.hidden = true;
        ui.baudRate.hidden = true;
        ui.lcdSize.hidden = false;
        ui.mutexTimeout.hidden = true;
        ui.mutexTimeoutValue.hidden = true;
    }
    else if (inst.displayType == "host") {
        ui.useANSI.hidden = true;
        ui.maxPrintLen.hidden = false;
        ui.uartBufferSize.hidden = true;
        ui.baudRate.hidden = true;
        ui.lcdSize.hidden = true;
        ui.mutexTimeout.hidden = true;
        ui.mutexTimeoutValue.hidden = true;
    }
    else {
        ui.useANSI.hidden = false;
        ui.maxPrintLen.hidden = true;
        ui.uartBufferSize.hidden = false;
        ui.baudRate.hidden = false;
        ui.lcdSize.hidden = true;
        ui.mutexTimeout.hidden = false;
        onChangeMutexTimeout(inst, ui);
    }
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with PWM
 *
 *  param component - hardware object describing signals and
 *                     resources they're attached to
 *  returns matching pinRequirement object if PWM is supported.
 */
function filterHardware(component)
{
    let ret = false;

    if (component.name) {
        if (component.name.match("MSP430BOOST_SHARP96")) {
            return (true);
        }
    }

    ret = UART.filterHardware(component);

    return (ret);
}

/*
 * ======== onHardwareChanged ========
 * Callback function invoked when the $hardware configurable is modified.
 *
 * param inst  - Display instance
 * param ui    - A handle to the UI state
 */
function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {
        if (inst.$hardware.name) {
            if (inst.$hardware.name.match("MSP430BOOST_SHARP96")) {
                ui.displayType.readOnly = false;
                inst.displayType = "lcd";
                onDisplayTypeChange(inst, ui);
                inst.lcdSize = 96;
                ui.displayType.readOnly = true;

                return;
            }
        }

        if (Common.typeMatches(inst.$hardware.type, ["UART"])) {
            ui.displayType.readOnly = false;
            inst.displayType = "uart";
            onDisplayTypeChange(inst, ui);
            ui.displayType.readOnly = true;

            return;
        }
    }
    else {
        ui.displayType.readOnly = false;
        inst.displayType = "uart";
        onDisplayTypeChange(inst, ui);

        return;
    }
}

/* The base object that will be exported for Display */
let base = {
    displayName           : "Display",
    description           : "Display Driver",
    longDescription       : longDescription,
    config                : config,
    defaultInstanceName   : "Board_Display",
    validate              : validate,
    maxInstances          : 3,
    filterHardware        : filterHardware,
    onHardwareChanged     : onHardwareChanged,
    moduleInstances       : moduleInstances,
    sharedModuleInstances : sharedModuleInstances,
    templates             : {
        boardc: "/ti/display/Display.Board.c.xdt"
    }
};

exports = base;

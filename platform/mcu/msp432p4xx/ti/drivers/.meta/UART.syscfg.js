/*!
 *  UART configuration support
 */
"use strict";
/*global exports, system*/

let Common   = system.getScript("/ti/drivers/Common.js");
let family   = Common.device2Family(system.deviceData, "UART");
let logError = Common.logError;

/* Intro splash on GUI */
let longDescription = "A UART is used to translate data between " +
    "the chip and a serial port. The UART driver simplifies reading " +
    "and writing to any of the UART peripherals on the board, with " +
    "multiple modes of operation and performance. These include " +
    "blocking, non-blocking, and polling, as well as text/binary " +
    "mode, echo and return characters.";

/*!
 * Common UART configurables across all devices.
 */
let config = [
    {
        name        : "dataDirection",
        displayName : "Data Send and/or Receive",
        default     : 'SendReceive',
        options     : [
            { name:   'SendReceive', displayName: "Send and Receive" },
            { name:   'SendOnly',    displayName: "Send Only"        },
            { name:   'ReceiveOnly', displayName: "Receive Only"     }
        ]
    },

    {
        name        : "errorFxn",
        displayName : "Error callback function",
        description : 'Optional application defined error function to be '
                      + 'called on receive errors.',
        default     : ''
    },

    /* baudRates is only needed by MSP432P but exists for all devs to enable 
     * more portable syscfg scripts.
     */
    {
        name        : "baudRates",
        displayName : "Baud Rates Required",
        description : 'The set of baud rates that are used by this instance. '
                      + 'On some devices, these rates are used to pre-compute '
                      + 'a table of clock dividers necessary for each rate.',
        hidden      : true,
        default     : [115200],
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

/*!
 * Validate this module's configuration
 *
 * @param inst       - UART instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation)
{
    let errorFxn = inst.errorFxn;

    Common.validateNames(inst, validation);

    if ((errorFxn !== '') && !Common.isCName(inst.errorFxn)) {
        logError(validation, inst, 'errorFxn', "Not a valid C Indentifier.");
    }
}

/* Common UART instance components across all devices. */
let base = {
    displayName   : "UART",
    description   : "Universal Asynchronous Receiver Transmitter (UART) Driver",
    modules       : Common.autoForcePowerModule,
    config        : config,
    validate      : validate,
    longDescription: longDescription,
    defaultInstanceName: "Board_UART"
};

/*
 * Compose a device specific UART from this common UART definition and
 * export the device specific UART
 */
let devUART = system.getScript("/ti/drivers/uart/UART" + family);
exports     = devUART.extend(base);

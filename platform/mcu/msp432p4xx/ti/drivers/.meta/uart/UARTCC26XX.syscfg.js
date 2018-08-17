/*!
 *  ======== CC26XX UART configuration ========
 *
 *  Device-specific UART configuration support
 */
"use strict";
/* global exports, system */

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "UART peripheral interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "UART interrupt Swi handler priority";

let logError = Common.logError;
let logInfo  = Common.logInfo;

let devId = system.deviceData.deviceId;
let numUarts = (devId.match(/CC26.2/) || devId.match(/CC13.2/)) ? 2 : 1;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base UART configuration
 */
let devSpecific = {
    config: [
        {
            name        : "flowControl",
            displayName : "Hardware Flow Control",
            default     : false,
            description : "Enable hardware flow control.",
        },
        {
            name        : "ringBufSize",
            displayName : "Ring Buffer Elements",
            default     : 32
        },
        intPriority,
        swiPriority,

        /* RX and TX Fifo level thresholds */
        {
            name        : "txIntFifoThr",
            displayName : "TX interrupt FIFO threshold",
            description : "Select the FIFO level for the TX interrupt",
            default     : 'fifo_threshold_1_8',
            options     : [
                { name:   'fifo_threshold_1_8',    displayName: "1/8" },
                { name:   'fifo_threshold_2_8',    displayName: "2/8" },
                { name:   'fifo_threshold_4_8',    displayName: "4/8" },
                { name:   'fifo_threshold_6_8',    displayName: "6/8" },
                { name:   'fifo_threshold_7_8',    displayName: "7/8" }
            ]
        },
        {
            name        : "rxIntFifoThr",
            displayName : "RX interrupt FIFO threshold",
            description : "Select the FIFO level for the RX interrupt",
            default     : 'fifo_threshold_4_8',
            options     : [
                { name:   'fifo_threshold_1_8',    displayName: "1/8" },
                { name:   'fifo_threshold_2_8',    displayName: "2/8" },
                { name:   'fifo_threshold_4_8',    displayName: "4/8" },
                { name:   'fifo_threshold_6_8',    displayName: "6/8" },
                { name:   'fifo_threshold_7_8',    displayName: "7/8" }
            ]
        }
    ],

    /* override generic pin requirements */
    pinmuxRequirements    : pinmuxRequirements,

    /* PIN instances */
    moduleInstances: moduleInstances,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/uart/UARTCC26XX.Board.c.xdt",
        boardh : "/ti/drivers/uart/UART.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate              : validate,
    filterHardware        : filterHardware,
    maxInstances          : numUarts
};

/*!
 *  ======== pinmuxRequirements ========
 *  Control RX, TX pin usage by the user specified dataDirection.
 *  Add CTS, RTS Pins if use specified flowControl is selected.
 *
 *  @param inst      - UART instance
 *
 *  @returns pinmuxRequirements[] - array of pinmuxRequirements for this module
 */
function pinmuxRequirements(inst)
{
    let cts = {
        name           : "ctsPin",    /* config script name */
        displayName    : "CTS",       /* GUI name */
        interfaceNames : ["CTS"]      /* pinmux tool name */
    };

    let rts = {
        name           : "rtsPin",
        displayName    : "RTS",
        interfaceNames : ["RTS"]
    };

    let tx = {
        name              : "txPin",  /* config script name */
        displayName       : "TX",     /* GUI name */
        interfaceNames    : ["TXD"]   /* pinmux tool name */
    };

    let rx = {
        name              : "rxPin",
        displayName       : "RX",
        interfaceNames    : ["RXD"]
    };

    let resources = [];

    if (inst.dataDirection != 'ReceiveOnly') {
        resources.push(tx);
    }

    if (inst.dataDirection != 'SendOnly') {
        resources.push(rx);
    }

    if (inst.flowControl === true) {
        resources.push(cts);
        resources.push(rts);
    }

    let uart = {
        name          : "uart",
        interfaceName : "UART",
        displayName   : "UART",
        resources     : resources,
        signalTypes   : {
            txPin     : ['UART_TXD'],
            rxPin     : ['UART_RXD'],
            ctsPin    : ['UART_CTS'],
            rtsPin    : ['UART_RTS']
        }
    };

    return [uart];
}

/*!
 *  ======== filterHardware ========
 *  Control RX, TX Pin usage by the user specified dataDirection.
 *
 *  @param component - a hardware componenet
 *
 *  @returns a mapping of component names to pin names
 *  @returns null if the component does not match this module
 */
function filterHardware(component)
{
    return (Common.typeMatches(component.type, ["UART"]));
}

/*!
 *  ======== moduleInstances ========
 *  returns PIN instances
 */
function moduleInstances(inst)
{
    let pinInstances = new Array();

    if (inst.dataDirection != "ReceiveOnly") {
        pinInstances.push(
            {
                name: "txPinInstance",
                displayName: "UART Tx PIN Configuration While Pin is Not In Use",
                moduleName: "/ti/drivers/PIN",
                collapsed: true,
                args: { parentMod: "/ti/drivers/UART",
                        parentInstanceName: inst.$name + " : TX",
                        parentInterfaceName: "uart",
                        parentSignalName: "txPin",
                        mode: "Output",
                        outputState: "Low",
                        pull: "None" }
            }
        );
    }

    if (inst.dataDirection != "SendOnly") {
        pinInstances.push(
            {
                name: "rxPinInstance",
                displayName: "UART Rx PIN Configuration While Pin is Not In Use",
                moduleName: "/ti/drivers/PIN",
                collapsed: true,
                args: { parentMod: "/ti/drivers/UART",
                        parentInstanceName: inst.$name + " : RX",
                        parentInterfaceName: "uart",
                        parentSignalName: "rxPin",
                        mode: "Input",
                        pull: "Pull Down" }
            }
        );
    }

    return (pinInstances);
}

/*!
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  @param inst       - UART instance to be validated
 *  @param validation - Issue reporting object
 */
function validate(inst, validation)
{
    if (inst.ringBufSize < 0) {
        logError(validation, inst, "ringBufSize", "value must be positive");
    }

    /* don't allow an unreasonably large ring buffer size */
    if (inst.ringBufSize > 1024) {
        logInfo(validation, inst, "ringBufSize",
            "consider reducing size for space optimization");
    }

    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*!
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic UART module to
 *  allow us to augment/override as needed for the CC262X
 *
 *  @param base - the exports from the base object
 *
 *  @returns - a composite object of the base + device specific modules
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* concatenate device-specific configs */
    devSpecific.config = base.config.concat(devSpecific.config);

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/*!
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base UART module */
    extend: extend
};

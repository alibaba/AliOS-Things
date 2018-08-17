/*!
 *  ======== CC32XX UART configuration ========
 *
 *  Device-specific UART configuration support
 */
"use strict";
/* global exports, system */

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let Power    = system.getScript("/ti/drivers/Power");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "UART peripheral interrupt priority";

let logError = Common.logError;
let logInfo  = Common.logInfo;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base UART configuration
 */
let devSpecific = {
    config: [
        {
            name        : "useDMA",
            displayName : "Use uDMA",
            onChange    : onChangeUseDMA,
            default     : false
        },
        {
            name        : "flowControl",
            displayName : "Hardware Flow Control",
            default     : false,
            description : "Enable hardware flow control."
        },
        {
            name        : "ringBufSize",
            displayName : "Ring Buffer Elements",
            default     : 32
        },
        intPriority
    ],

    /* override generic pin requirements */
    pinmuxRequirements    : pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/uart/UARTCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/uart/UART.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate              : validate,
    modules               : modules,
    filterHardware        : filterHardware,
    maxInstances          : 2
};

/*!
 *  ======== modules ========
 *  Return array of other modules required by a UARTCC32XX instance
 */
function modules(inst)
{
    if (inst.useDMA) {
        return (Common.autoForcePowerAndDMAModules());
    }
    else {
        return (Common.autoForcePowerModule());
    }
}

/*!
 *  ======== onChangeUseDMA ========
 *  onChange callback function for the useDMA config
 *
 *  @param inst  - Power instance containing the config that changed
 *  @param ui    - The User Interface object
 *
 */
function onChangeUseDMA(inst, ui)
{
    if (inst.useDMA) {
        ui.ringBufSize.hidden = true;
    }
    else {
        ui.ringBufSize.hidden = false;
    }
}

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
        interfaceNames    : ["TX"]    /* pinmux tool name */
    };

    let rx = {
        name              : "rxPin",
        displayName       : "RX",
        interfaceNames    : ["RX"]
    };

    let dma_tx = {
        name           : "txDmaChannel",
        displayName    : "TX DMA Channel",
        interfaceNames : ["DMA_CHTX"]
    };

    let dma_rx = {
        name           : "rxDmaChannel",
        displayName    : "RX DMA Channel",
        interfaceNames : ["DMA_CHRX"]
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

    if (inst.useDMA === true) {
        if (inst.dataDirection != 'ReceiveOnly') {
            resources.push(dma_tx);
        }
        if (inst.dataDirection != 'SendOnly') {
            resources.push(dma_rx);
        }
    }

    let uart = {
        name          : "uart",
        interfaceName : "UART",
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
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  @param inst       - UART instance to be validated
 *  @param validation - Issue reporting object
 */
function validate(inst, validation)
{
    if (!inst.useDMA) {

        if (inst.ringBufSize === 0) {
            logError(validation, inst, "ringBufSize", "Size must be > 0, if " +
                                       "uDMA is not used for UARTs.");
        }

        if (inst.ringBufSize < 0) {
            logError(validation, inst, "ringBufSize", "Size must be positive.");
        }

        /* don't allow an unreasonably large ring buffer size */
        if (inst.ringBufSize > 1024) {
            logInfo(validation, inst, "ringBufSize",
              "Consider reducing size for space optimization");
        }
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

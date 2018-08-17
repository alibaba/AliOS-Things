/*!
 *  ======== MSP432P UART configuration ========
 *
 *  Device-specific UART configuration support
 */
"use strict";
/* global exports, system */

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let Power  = system.getScript("/ti/drivers/Power");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "UART peripheral interrupt priority";

let logError = Common.logError;
let logInfo  = Common.logInfo;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base UART configuration
 */
let devSpecific = {
    config:
    [
        /*
         * This field is inter-related with the baudRates field.  Ideally
         * the user specifies the baudRates and leaves this field as 'Any',
         * which allows the config tool to solve for clock source.  However,
         * for expert users, we allow clockSource to be locked which will
         * 1) disable auto solving based on baudRates, and potentially
         * 2) prevent the configuration from migrating to an alternate
         *    board/device which does not support the locked clockSource
         *    value.
         */
        {
            name        : "clockSource",
            displayName : "Clock Source",
            default     : "Any",
            description : "If 'Any' is selected, an appropriate clock source " +
                          "will be auto selected. For maximum portability to " +
                          "other devices, it is recommended that this option " +
                          "be left in it's default state of 'Any'.",

            /* These are ordered by preference.  If multiple sources can
             * satisfy the baud rate, then the first one in the list
             * that can satisfy the baud rate will be selected.
             */
            options:
            [
                { name: "Any" },
                { name: "ACLK" },
                { name: "SMCLK" }
                // { name : "HSMCLK" }, // Not Yet Supported by Driverlib
                // { name : "MCLK"   }, // Not Yet Supported by Driverlib
            ]
        },
        {
            name        : "ringBufSize",
            displayName : "Ring Buffer Elements",
            default     : 32
        },
        {
            name        : "bitOrder",
            displayName : "Bit Order",
            default     : "LSB_FIRST",
            description : "Almost always LSB First",
            options     :
            [
                {
                    name        : "MSB_FIRST",
                    displayName : "MSB First"
                },
                {
                    name        : "LSB_FIRST",
                    displayName : "LSB First"
                }
            ]
        },
        intPriority
    ],

    /* override generic pin requirements */
    pinmuxRequirements    : pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/uart/UARTMSP432.Board.c.xdt",
        boardh : "/ti/drivers/uart/UART.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate              : validate,
    filterHardware        : filterHardware,
    maxInstances          : 4,
    autoAssignClockSource : autoAssignClockSource,
    genBaudRateTable      : genBaudRateTable
};

/*!
 *  ======== autoAssignClockSource ========
 *  Assign a clock source for a UART.
 *
 *  Assign the clock source based on a required baud rate and
 *  known frequencies of the clock sources from the Power module.
 *
 *  @param baudRates         - The Baud Rate for the UART
 *
 *  @returns clockSourceName - The name of the most appropriate clock source.
 */
function autoAssignClockSource(baudRates)
{
    /* Find the clock source options from the module */
    let clockSources;
    for (let idx = 0; idx < devSpecific.config.length; idx++) {
        if (devSpecific.config[idx].name === 'clockSource') {
            clockSources = devSpecific.config[idx].options;
        }
    }

    if (clockSources === undefined) {
        throw new Error('Uart Metacode is not in sync with Uart Metadata. ');
    }

    let choices = [];

    /* Walk through the clock source options for this uart.
     * This walks through the module exports, not the instance
     */
    for (let i = 0; i < clockSources.length; i++) {
        let optionName = clockSources[i].name;
        if (optionName === 'Any') {
            continue;
        }

        let frequencies = Power.getClockFrequencies(optionName);
        if (frequencies.length === 0) {
            throw new Error(optionName +
                ' cannot be found in Performance_levels tables. ');
        }

        let numGoodFreqs = 0;

        for (let f = 0; f < frequencies.length; f++) {
            for (let bidx = 0; bidx < baudRates.length; bidx++) {
                if (frequencies[f] >= baudRates[bidx]) {
                    numGoodFreqs++;
                }
            }
        }

        /* If we have a clock where all freqs cover all baudRates, then done */
        if (numGoodFreqs == (frequencies.length * baudRates.length)) {
            return optionName;
        }

        /* else save the coverage factor so that a later choice can be made */
        choices.push({
            clock: optionName,
            quality: (numGoodFreqs / (frequencies.length * baudRates.length))
        });
    }

    let bestIdx = 0;
    for (let c = 1; c < choices.length; c++) {
        if (choices[c].quality > choices[bestIdx].quality) {
            bestIdx = c;
        }
    }

    return choices[bestIdx].clock;
}

/*!
 *  ======== genBaudRateTable ========
 *  Generate a baud rate table for a UART instance
 *
 *  Given a baud rate and a clock source, generate baud rate table entries for
 *  the cross product of baud rates x clock source frequencies. Currently the
 *  only a single baud rate is supported.
 *
 *  @param baudRates       - The Baud Rates for the UART
 *  @param clockSource     - The name of the clock source.
 *
 *  @returns baudRateTable - The baud rate table entries for this UART instance.
 */
function genBaudRateTable(baudRates, clockSource)
{
    let frequencies = Power.getClockFrequencies(clockSource);
    if (frequencies.length === 0) {
        throw new Error(clockSource +
            ' cannot be found in Performance_Levels tables. ');
    }

    let baudRateTable = [];

    for (let b = 0; b < baudRates.length; b++) {
        for (let j = 0; j < frequencies.length; j++) {
            let res = eusci_calcBaudDividers(frequencies[j], baudRates[b]);
            res.baud = baudRates[b];
            res.freq = frequencies[j];
            baudRateTable.push(res);
        }
    }

    return baudRateTable;
}

/*!
 *  ======== bitPosition ========
 *  Determine if a bit in an integer is set
 *
 *  @param value      - The integer to check
 *  @param position   - The position of the bit to check.
 *
 *  @returns boolean  - true, if the bit in the position is set,
 *                     false, otherwise.
 */
function bitPosition(value, position)
{
    if ((value & (1 << position)) !== 0) {
        return (1);
    }
    return (0);
}

/*!
 *  ======== eusci_calcBaudDividers ========
 *  Computes the eUSCI_UART register settings for a given clock and baud rate
 *
 *  This function returns a JavaScript object containing the fields:
 *      UCOS16:      the oversampling bit (0 or 1)
 *      UCBRx:       the Baud Rate Control Word
 *      UCFx:        the First modulation stage select (UCBRFx)
 *      UCSx:        the Second modulation stage select (UCBRSx)
 *      maxAbsError: the maximum TX error for the register setting above
 *
 *  The first four field names match the names used in Table 18-5,
 *  "Recommended Settings for Typical Crystals and Baudrates", of the
 *  MSP430FR57xx Family User's Guide (SLAU272A).
 *
 *  @param clockRate      - The input clock frequency
 *  @param baudRate       - The desired output baud rate
 *
 * @returns baudRateTableEntry - A baud rate table entriy
 */
function eusci_calcBaudDividers(clockRate, baudRate)
{
    let result      = { UCOS16: 0, UCBRx: 0, UCFx: 0, UCSx: 0, maxAbsError: 0 };
    let N           = Math.floor(clockRate / baudRate);
    let baudPeriod  = 1 / baudRate;
    let clockPeriod = 1 / clockRate;
    let minAbsError = 100000;

    for (let jj = 0; jj <= 255; jj++) {
        let maxAbsErrorInByte = 0;
        let count = 0;

        for (let ii = 0; ii <= 10; ii++) {
            count += N + bitPosition(jj, 7 - (ii % 8));
            let error = (ii + 1) * baudPeriod - count * clockPeriod;

            error = Math.abs(error);
            if (error > maxAbsErrorInByte) {
                maxAbsErrorInByte = error;
            }
        }

        if (maxAbsErrorInByte < minAbsError) {
            minAbsError = maxAbsErrorInByte;
            result.UCSx = jj;
        }
    }

    if (N < 20) {
        result.UCOS16 = 0;
        result.UCBRx  = N;
        result.UCFx   = 0;
    }
    else {
        result.UCOS16 = 1;
        result.UCBRx  = Math.floor(N / 16);
        result.UCFx   = N - (result.UCBRx * 16);
    }

    result.maxAbsError = minAbsError * baudRate * 100;

    return (result);
}

/*!
 *  ======== pinmuxRequirements ========
 *  Control RX, TX pin usage by the user specified dataDirection.
 *
 *  @param inst      - UART instance
 *
 *  @returns pinmuxRequirements[] - array of pinmuxRequirements for this module
 */
function pinmuxRequirements(inst)
{

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

    let uart = {
        name          : "uart",
        interfaceName : "UART",
        resources     : resources,
        signalTypes   : {
            txPin     : ['UART_TXD'],
            rxPin     : ['UART_RXD']
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
    let bitOrder    = inst.bitOrder;
    let baudRates   = inst.baudRates;
    let ringBufSize = inst.ringBufSize;
    let clockName   = inst.clockSource;
    let message;

    if (clockName == 'Any') {
        clockName = autoAssignClockSource(baudRates);
    }

    validateBaudRates(inst, validation, clockName);

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
 *  ======== validateBaudRates ========
 *  Validate the clockName vs. the selected baud rates.
 *
 *  @param inst       - UART instance to be validated
 *  @param validation - Issue reporting object
 *  @param clockName  - The selected clock source name
 */
function validateBaudRates(inst, validation, clockName)
{
    let frequencies = Power.getClockFrequencies(clockName);
    let numFreqs    = frequencies.length;
    let baudRates   = inst.baudRates;
    let message     = '';
    let numGoodFreqs = 0;

    for (let f = 0; f < frequencies.length; f++) {
        for (let b = 0; b < baudRates.length; b++) {
            if (frequencies[f] >= baudRates[b]) {
                numGoodFreqs++;
            }
        }
    }

    /* If clock has all freqs covering all baudRates, then no problems */
    if (numGoodFreqs == (frequencies.length * baudRates.length)) {
        return; // no error or warning
    }

    /* if the clock source cannot support the baudRates at all */
    if (numGoodFreqs == 0) {
        message = "Clock source " + clockName +
            " cannot support any of the specified baud rates";
        logError(validation, inst, ["baudRates","clockSource"], message);
    }

    /* if the clock source can support some baudRates at only some
     * performance levels, but not all of them
     */
    else {
        message = "Clock source " + clockName +
            " can only support some baud rates at some performance levels.";
        logInfo(validation, inst, ["baudRates","clockSource"], message);
    }
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic UART module to
 *  allow us to augment/override as needed for the MSP432
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

    /* override baudRates table */
    for (let i = 0; i < devSpecific.config.length; i++) {
        let cfg = devSpecific.config[i];
        if (cfg.name == "baudRates") {
            cfg.hidden = false;
            break;
        }
    }

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

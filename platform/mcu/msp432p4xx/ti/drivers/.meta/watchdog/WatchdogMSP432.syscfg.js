"use strict";
/* global exports, system */

let Power  = system.getScript("/ti/drivers/Power");
let Common = system.getScript("/ti/drivers/Common.js");
let $super;

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Interrupt Priority";

/* The device specific exports for the watchdog module */
let devSpecific =
{

    config : [
        intPriority
    ],

    validate     : validate,
    maxInstances : 1,
    templates    :
    {
        boardc : "/ti/drivers/watchdog/WatchdogMSP432.Board.c.xdt",
        boardh : "/ti/drivers/watchdog/Watchdog.Board.h.xdt"
    }
};

 /**
 * Validate this module's configuration
 *
 * @param inst       - UART instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation)
{
   $super.validate(inst, validation); // Defer to the base validation
}

 /**
 * Add enumeration values for the period configurable.
 *
 * The common period configurable is not an enumerable, but the MSP432
 * watchdog is constrained to a set of values.  This routine will constrain
 * the period configurable defined in the common watchdog module, with MSP432
 * specific constraints.
 *
 * @param config - The configurables list for watchdog
 *
 */
function addPeriodChoices(config)
{

    let divide_ratios = [31, 27, 23, 19, 15, 13, 9, 6];
    let frequencies   = Power.getClockFrequencies('ACLK');

    /* We restrict the possible period values to just the one
    * supported by ACLK clock source. This eliminates dependency
    * on performance level.
    */
    if (frequencies.length !== 1) {
        throw new Error('Watchdog clock (ACLK) cannot vary with perf levels. ');
    }

    /* find the period configurable in config */
    let period;
    for (let idx = 0; idx < config.length; idx++) {
        if (config[idx].name === 'period') {
            period = config[idx];
        }
    }

    if (period === undefined) {
        throw new Error('Watchdog Metacode is not in sync with Metadata. ');
    }

    let freq = frequencies[0];

    period.options = [];
    for (let idx=0; idx<divide_ratios.length; idx++) {
        let periodMs        = (Math.pow(2,divide_ratios[idx]) / freq) * 1000;
        period.options[idx] = { name: periodMs };
    }

    return config;
}

/**
 * The base object is passed to this function and allows the device
 * specific object to incorporate properties from the base object.
 *
 * @param base - the exports from the base object
 *
 * @returns - a composite object of the base + device specific modules
 */
function extend (base)
{
    $super             = base;

    devSpecific.config = devSpecific.config.concat(base.config);
    devSpecific.config = addPeriodChoices(devSpecific.config);

    return Object.assign({}, base, devSpecific);
}

exports = { extend : extend };

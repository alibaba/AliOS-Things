"use strict";
/* global exports, system */

let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let $super;

/* The device specific exports for the watchdog module */
let devSpecific =
{
    validate : validate,
    maxInstances : 1,
    templates:
    {
        boardc : "/ti/drivers/watchdog/WatchdogCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/watchdog/Watchdog.Board.h.xdt"
    }
};

/**
 * Validate this module's configuration
 *
 * @param inst       - WDT instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation)
{
   let maxPeriod = 53687;
   let period    = parseInt(inst.period);
   let message;

   $super.validate(inst, validation); // Defer to the base validation

   if (period > maxPeriod) {
     message = 'Period value: ' + period +
               ' exceeds maximum allowed period: ' + maxPeriod;
     logError(validation, inst, "period", message);
   }
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
    devSpecific.config = base.config.concat(Common.newIntPri());
    return Object.assign({}, base, devSpecific);
}

exports = { extend : extend };

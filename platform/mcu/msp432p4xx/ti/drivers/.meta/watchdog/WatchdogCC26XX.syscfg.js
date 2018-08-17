"use strict";
/* global exports, system */

let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let $super;

/* The device specific exports for the watchdog module */
let devSpecific =
{
    maxInstances : 1,

    validate : validate,
    templates:
    {
        boardc : "/ti/drivers/watchdog/WatchdogCC26XX.Board.c.xdt",
        boardh : "/ti/drivers/watchdog/Watchdog.Board.h.xdt"
    },

    pinmuxRequirements : pinmuxRequirements
};


/**
 * Return an array of hardware requirements for this module.
 * This overwrite the toplevel Watchdog.syscfg.js
 * pinmuxrequirements.
 *
 * @param inst - WDT instance to be validated
 * @returns - array of hardware requirements
 */
function pinmuxRequirements(inst)
{
    return ([
        {
            name          : "watchdog",
            interfaceName : "WDT",
            resources     : []
        }
    ]);
}

/**
 * Validate this module's configuration
 *
 * @param inst       - WDT instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation)
{
   let maxPeriod = 2863300;
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

    return (Object.assign({}, base, devSpecific));
}

exports = { extend : extend };

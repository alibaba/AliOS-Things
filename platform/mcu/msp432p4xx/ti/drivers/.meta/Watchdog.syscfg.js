"use strict";
/*global exports, system*/

let Common   = system.getScript("/ti/drivers/Common.js");
let family   = Common.device2Family(system.deviceData, "Watchdog");
let logError = Common.logError;

/* Intro splash on GUI */
let longDescription = "A watchdog timer can be used to generate " +
    "a reset signal if a system has become unresponsive. The " +
    "Watchdog driver simplifies configuring and starting the " +
    "watchdog peripherals. The watchdog peripheral can be configured " +
    "with resets either on or off and a user-specified timeout period.";

/* Array of Watchdog configurables that are common across device families */
let config = [
    {
        name        : "period",
        displayName : "Period (ms)",
        default     : 1000
    }
];

/**
 * Return an array of hardware requirements for this module.
 *
 * @param inst - Watchdog instance to be validated
 * @returns - array of hardware requirements
 */
function pinmuxRequirements(inst)
{
    return ([
        {
            name          : "watchdog",
            displayName   : "Watchdog",
            interfaceName : "WATCHDOG",
            resources          : []
        }
    ]);
}

/**
 * Validate this module's configuration
 *
 * @param inst       - Watchdog instance to be validated
 * @param validation - Issue reporting object
 */
function validate(inst, validation)
{
    let period = parseInt(inst.period);
    let message;

    if (period < 0) {
        message = 'Period value: ' + period + ' can not be negative';
        logError(validation, inst, "period", message);
    }

    Common.validateNames(inst, validation);
}

/* Define the common/portable base Watchdog */
let base = {
    displayName         : "Watchdog",
    description         : "Watchdog Timer Driver",
    defaultInstanceName : "Board_WATCHDOG",
    longDescription     : longDescription,
    modules             : Common.autoForcePowerModule,
    config              : config,
    pinmuxRequirements  : pinmuxRequirements,
    validate            : validate
};

/*
 * Compose a device specific Watchdog from this base Watchdog definition and
 * export the device specific Watchdog
 */
let deviceWatchdog = system.getScript("/ti/drivers/watchdog/Watchdog" + family);
exports = deviceWatchdog.extend(base);

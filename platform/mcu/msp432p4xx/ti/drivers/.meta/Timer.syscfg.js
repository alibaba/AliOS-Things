"use strict";
/*global exports, system*/

let Common   = system.getScript("/ti/drivers/Common.js");
let family   = Common.device2Family(system.deviceData, "Timer");
let logError = Common.logError;

/* Intro splash on GUI */
let longDescription = "The timer driver serves as the main interface " +
    "for a typical RTOS application. Its purpose is to redirect the " +
    "timer APIs to device specific implementations which are specified " +
    "using a pointer to a Timer_FxnTable. The device specific " +
    "implementations are responsible for creating all the RTOS " +
    "specific primitives to allow for thead-safe operation.";


let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Timer Interrupt Priority";

let timerConfig = [
    {
        name        : "timerType",
        displayName : "Timer Type",
        default     : "16 Bits",
        options     : [
            { name: "16 Bits" },
            { name: "32 Bits" }
        ]
    },

    intPriority
];

/* define common/portable exports */
let base = {
    displayName         : "Timer",
    description         : "Timer Driver",
    longDescription     : longDescription,
    defaultInstanceName : "Board_TIMER",
    config              : timerConfig,
    validate            : validate
};

/**
 * Validate this module's configuration
 *
 * @param inst       - Timer instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/* Compose a device specific Timer from this common Timer definition and
 * export the device specific Timer
 */
let devTimer = system.getScript("/ti/drivers/timer/Timer" + family);
exports      = devTimer.extend(base);

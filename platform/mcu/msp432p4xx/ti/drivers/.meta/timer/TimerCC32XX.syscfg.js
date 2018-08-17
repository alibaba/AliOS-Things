/*!
 *  CC32XX Timer module
 *
 *  Device-specific Timer configuration support
 */

"use strict";
/*global exports, system*/

let Common = system.getScript("/ti/drivers/Common.js");
let $super;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Timer configuration
 */
let devSpecific = {

    config: [
    ],

    templates: {
        boardc : "/ti/drivers/timer/TimerCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/timer/Timer.Board.h.xdt"
    },

    maxInstances       : 8,
    modules            : Common.autoForcePowerModule,
    pinmuxRequirements : pinmuxRequirements,
    validate           : validate
};

/**
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let timer = {
        name          : "timer",
        interfaceName : "Timer",
        resources          : [] // no resources required for Timer
    };

    if (inst.timerType == "16 Bits") {
        timer.canShareWith = "16 Bit Timer";
        timer.maxShareCount = 2;
    }

    return ([timer]);
}

/**
 *  ======== validate ========
 *  Perform CC32XX-specific validation
 *
 *  @param inst       - TimerCC32XX instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
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
function extend(base)
{
    $super = base;
    devSpecific.config = devSpecific.config.concat(base.config);
    return Object.assign({}, base, devSpecific);
}

/* Export device-specific extensions to base exports */
exports = { extend : extend };

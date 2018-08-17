"use strict";
/*global exports, system*/

let Common = system.getScript("/ti/drivers/Common.js");
let $super;

/* MSP432 specific Timer configurables. */
let config = [
    {
        name        : "timerType",
        displayName : "Timer Type",
        onChange    : onChangeTimerType,
        default     : "16 Bits",
        options     : [
                         { name : "32 Bits", displayName: 'Timer32' },
                         { name: "16 Bits", displayName: 'Timer_A' }
                      ]
    },
    /* clockSource is only valid for Timer_A.  It is hidden by default and
     * is exposed when timer type is changed to Timer_A
     */
    {
        name        : "clockSource",
        displayName : "Clock Source",
        default     : "ACLK",
        hidden      : true,
        options     : [
            { name: "ACLK" },
            { name: "SMCLK" },
            { name: "EXTERNAL TXCLK" },
            { name: "INVERTED EXTERNAL TXCLK" }
        ]
    }
];

/**
 * onChange callback function for the timerType config
 *
 * @param inst  - Instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangeTimerType(inst, ui)
{
    if (inst.timerType === "32 Bits") {
        ui.clockSource.hidden = true;
    } else {
        ui.clockSource.hidden = false;
    }
}

/*
 * Validate this module's configuration
 *
 * @param inst       - Instance to be validated
 * @param validation - Issue reporting object
 */
function validate(inst, validation)
{
    $super.validate(inst, validation);
}

/*
 * ======== timerFilter ========
 *  Filter out Timer_A0 for now.  It may be used by BIOS.
 *  TODO: Figure out a way to add comments to timers displayed in
 *  drop-down menu of pinmux tool.  Would also be nice to re-order
 *  them so Timer_A0 is last in the list.  See PMUX-1092
 */
function timerFilter(peripheral)
{
    return (!(peripheral.name.match(/A0/)));
}

/*
 * pinRequirements:
 *
 * Return array of interface requirements
 *
 * @param inst - instance
 *
 * @returns pinmuxRequirements[] - array of pinmuxRequirements for this module
 */
function pinmuxRequirements(inst)
{
    let timer = {
        name          : "timer",
        interfaceName : "TIMER",
        resources     : [] // no resources required for Timer
    };

    if (inst.timerType === "32 Bits") {
        timer.name          += "32";
        timer.interfaceName += "32";
    }
    else {
        timer.name          += "_A";
        timer.interfaceName += "_A";
        timer.filter        = timerFilter;
    }

    return ([timer]);
}

let devSpecific = {
    config            : config,
    validate          : validate,
    pinmuxRequirements: pinmuxRequirements,
    maxInstances      : 6,
    modules           : Common.autoForcePowerModule,
    templates         : {
                          boardc: "/ti/drivers/timer/TimerMSP432.Board.c.xdt",
                          boardh: "/ti/drivers/timer/Timer.Board.h.xdt"
                        }
};

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

    /* Determine the timerType config Index */
    let index = base.config.findIndex(x => x.name === "timerType");

    /* Delete the top level timerType config */
    base.config.splice(index, 1);

    devSpecific.config = devSpecific.config.concat(base.config);

    return (Object.assign({}, base, devSpecific));
}

/* Export device-specific extensions to base exports */
exports = { extend : extend };

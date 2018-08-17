/*
 *  ======== PWMTimerCC32XX.js ========
 *  CC32XX PWM module
 *
 *  Device-specific PWM configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super = {};  /* reset by extend() */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== pwmPins ========
 *  from which datasheet(?)  Can this list vary based on device/package?
 */
let pwmPins = ["1", "2", "17", "19", "21", "64"];

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base PWM configuration
 */
let devSpecific = {
    maxInstances: 6,

    config: [],

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    filterHardware: filterHardware,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/pwm/PWMTimerCC32XX.Board.c.xdt",
        boardh: "/ti/drivers/pwm/PWMTimer.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate,

    modules: Common.autoForcePowerModule
};

/**
 *  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  @param inst    - a fully configured instance
 *
 *  @returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let timer = {
        name: "timer",
        displayName: "Timer",
        interfaceName: "Timer",
        canShareWith: "PWM",
        resources: [
            {
                name: "pwmPin",
                displayName: "PWM Output",
                interfaceNames: [
                    "GT_PWM00", "GT_PWM01"
                ]
            }
        ],
        signalTypes: { pwmPin: ["PWM"] }
    };

    return ([timer]);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with PWM
 *
 *  param component - hardware object describing signals and
 *                    resources they're attached to
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["PWM"])) {
            let devicePin = component.signals[sig].devicePin;
            if (isValidPwmPin(devicePin.name)) {
                return (true);
            }
        }
    }

    return (false);
}

/*
 *  ======== isValidPwmPin ========
 *  Verify pin can support PWM
 */
function isValidPwmPin(devicePin)
{
    for (let i = 0; i < pwmPins.length; i++) {
        if (devicePin == pwmPins[i]) {
            return (true);
        }
    }
    return (false);
}

/**
 *  ======== validate ========
 *  Perform CC32XX-specific validation
 *
 *  @param inst       - PWMCC32XX instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/**
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic PWM module to
 *  allow us to augment/override as needed for the CC32XX
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

/**
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base PWM module */
    extend: extend
};

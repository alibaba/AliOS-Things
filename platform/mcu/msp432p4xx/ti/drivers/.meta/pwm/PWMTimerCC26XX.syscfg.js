/*
 *  ======== PWMTimerCC26XX.js ========
 *  CC26XX PWM module
 *
 *  Device-specific PWM configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super = {};

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base PWM configuration
 */
let devSpecific = {
    config: [],

    /* referenced GPTimer module instances */
    moduleInstances: moduleInstances,

    filterHardware: filterHardware,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/pwm/PWMTimerCC26XX.Board.c.xdt",
        boardh: "/ti/drivers/pwm/PWMTimer.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate,

    modules: Common.autoForcePowerModule
};

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with PWM
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinmuxRequirement object if PWM is supported.
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["PWM"])) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== moduleInstances ========
 *  returns a shared GPTimer instance
 */
function moduleInstances(inst)
{
    let timer = new Array();

    timer.push(
        {
            name: "timerObject",
            displayName: "GP Timer Instance",
            moduleName: "/ti/drivers/Timer",
            hidden: false,
            collapsed: false,
            hardware: inst.$hardware ? inst.$hardware : null,
            args: {
                timerType: "16 Bits",
                pinType: "PWM"
            }
        }
    );

    return (timer);
}

/**
 *  ======== validate ========
 *  Perform CC26XX-specific validation
 *
 *  @param inst       - PWMCC26XX instance to be validated
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
 *  allow us to augment/override as needed for the CC26XX
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

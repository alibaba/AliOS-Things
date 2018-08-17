/*
 *  ======== PWMTimerMSP432E4.js ========
 *  MSP432E4 PWM module
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
    maxInstances: 8,

    config: [
        {
            name: "pwmGenMode",
            displayName: "PWM Generation Mode",
            description: "A PWM generator can count in two different modes: " +
            "count down mode or count up/down mode.  In count down mode, it " +
            "counts from a value down to zero, and then resets to the preset " +
            "value, producing left-aligned PWM signals (that is, the rising " +
            "edge of the two PWM signals produced by the generator occur at " +
            "the same time).  In count up/down mode, it counts up from zero " +
            "to the preset value, counts back down to zero, and then repeats " +
            "the process, producing center-aligned PWM signals (that is, the " +
            "middle of the high/low period of the PWM signals produced by the " +
            "generator occurs at the same time).",
            default: "Down",
            options: [
                { name: "Down" },
                { name: "Up Down" }
            ]
        },
        {
            name: "syncMode",
            displayName: "Synchronization Mode",
            description: "When the PWM generator parameters (period and pulse" +
            " width) are modified, their effect on the output PWM signals " +
            "can be delayed.  In synchronous mode, the parameter updates are " +
            "not applied until a synchronization event occurs. This mode " +
            "allows multiple parameters to be modified and take effect " +
            "simultaneously, instead of one at a time. Additionally, " +
            "parameters to multiple PWM generators in synchronous mode can " +
            "be updated simultaneously, allowing them to be treated as if " +
            "they were a unified generator. In non-synchronous mode, the " +
            "parameter updates are not delayed until a synchronization event. " +
            "In either mode, the parameter updates only occur when the counter" +
            " is at zero to help prevent oddly formed PWM signals during the " +
            "update (that is, a PWM pulse that is too short or too long).",
            default: "Immediate Updates",
            options: [
                { name: "Immediate Updates" },
                { name: "Local Synchronous Updates" },
                { name: "Global Synchronous Updates" }
            ]
        },
        {
            name: "debugMode",
            displayName: "Debug Mode",
            description: "The PWM generator can either pause or continue " +
            "running when the processor is stopped via the debugger. If " +
            "configured to pause, it continues to count until it reaches " +
            "zero, at which point it pauses until the processor is " +
            "restarted. If configured to continue running, it keeps " +
            "counting as if nothing had happened.",
            default: "Debug Run",
            options: [
                { name: "Debug Run" },
                { name: "Debug Stop" }
            ]
        }
    ],

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    filterHardware: filterHardware,

    templates: {
        boardc: "/ti/drivers/pwm/PWMMSP432E4.Board.c.xdt",
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
 *  @param inst    - a fully configured ADC instance
 *
 *  @returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let pwm = {
        name: "pwm",
        displayName: "PWM",
        interfaceName: "PWM",
        canShareWith: "PWM",
        resources: [
            {
                name: "pwmPin",
                displayName: "PWM Output Pin",
                interfaceNames: [
                    "PWM0", "PWM1", "PWM2", "PWM3", "PWM4", "PWM5",
                    "PWM6", "PWM7"
                ]
            }
        ],
        signalTypes: { pwmPin: ["PWM"] }
    };

    return ([pwm]);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with PWM
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinRequirement object if PWM is supported.
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        let type = component.signals[sig].type;

        if (Common.typeMatches(type, ["PWM"])) {
            let devicePin = component.signals[sig].devicePin;

            if (isValidPwmPin(devicePin.mux.muxSetting)) {
                return (true);
            }
        }
    }

    return (false);
}

/*
 *  ======== isValidPwmPin ========
 *  Verify pin can support PWM.
 */
function isValidPwmPin(muxSettings)
{
    for (let i in muxSettings) {
        if (muxSettings[i].peripheralPin.name.match("PWM")) {
            return (true);
        }
    }

    return (false);
}

/**
 *  ======== validate ========
 *  Perform MSP432E4-specific validation
 *
 *  @param inst       - PWMMSP432E4 instance to be validated
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
 *  allow us to augment/override as needed for the MSP432E4
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

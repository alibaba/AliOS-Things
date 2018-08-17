/*!
 *  ======== PWMTimerMSP432.js ========
 *  MSP432 PWM module
 *
 *  Device-specific PWM configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super = {};

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* GPIO resources that support PWM */
let pwmPins =
    "P2.0 P2.1 P2.2 P2.3" +
    "P2.4 P2.5 P2.6 P2.7" +
    "P3.0 P3.1 P3.2 P3.3" +
    "P3.4 P3.5 P3.6 P3.7" +
    "P5.6 P5.7 P6.6 P6.7" +
    "P7.0 P7.1 P7.2 P7.3" +
    "P7.4 P7.5 P7.6 P7.7" +
    "P8.2 P9.2 P9.3 P10.5";

/*!
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base PWM configuration
 */
let devSpecific = {
    config: [
        {
            name: "clockSource",
            displayName: "Clock Source",
            default: "SMCLK",
            options: [
                { name: "ACLK" },
                { name: "SMCLK" }
            ]
        }
    ],

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    filterHardware: filterHardware,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/pwm/PWMTimerMSP432.Board.c.xdt",
        boardh: "/ti/drivers/pwm/PWMTimer.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate,

    modules: Common.autoForcePowerModule
};

/*!
 *  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  @param inst    - a fully configured ADC instance
 *
 *  @returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let timer = {
        name: "timer",
        displayName: "Timer",
        interfaceName: "TIMER_A",
        canShareWith: "PWM",
        /*
         *  Filter out Timer_A0 for now.  It may be used by BIOS.
         *  TODO: Figure out a way to add comments to timers displayed in
         *  drop-down menu of pinmux tool.  Would also be nice to re-order
         *  them so Timer_A0 is last in the list.  See PMUX-1092
         */
        filter: function (peripheral) {
            return (!(peripheral.name.match(/A0/)));
        },
        resources: [
            {
                name: "pwmPin",
                displayName: "PWM Output",
                interfaceNames: [
                    "Out1", "Out2", "Out3", "Out4"
                ]
            }
        ],
        signalTypes: { pwmPin: ["PWM"] }
    };

    return ([timer]);
}

/*!
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
            let devicePin = component.signals[sig].devicePin;
            if (isValidPwmPin(devicePin.designSignalName)) {
                return (true);
            }
        }
    }

    return (false);
}

/*!
 *  ======== isValidPwmPin ========
 *  Verify pin can support PWM
 */
function isValidPwmPin(designSignalName)
{
    let portName = designSignalName.match(/^P[0-9]+\.[0-9]+/)[0]; /* PX.Y */
    let match = pwmPins.match(portName);
    return (match != null);
}

/*!
 *  ======== validate ========
 *  Perform MSP432-specific validation
 *
 *  @param inst       - PWMMSP432 instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*!
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic PWM module to
 *  allow us to augment/override as needed for the MSP432
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

/*!
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base PWM module */
    extend: extend
};

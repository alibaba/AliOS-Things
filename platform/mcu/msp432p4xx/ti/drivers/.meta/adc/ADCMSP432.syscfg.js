/*
 *  MSP432 ADC module
 *
 *  Device-specific ADC configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base ADC configuration
 */
let devSpecific = {
    maxInstances: 24,

    config: [
        {
            name: "refVoltage",
            displayName: "Reference Voltage",
            default: "VDD",
            options: [
                { name: "VDD" },
                { name: "1.2V" },
                { name: "1.45V" },
                { name: "2.5V" },
                { name: "External" },
                { name: "External Buffered" }
            ]
        },
        {
            name: "resolution",
            displayName: "Resolution",
            default: "14 Bits",
            options: [
                { name: "8 Bits" },
                { name: "10 Bits" },
                { name: "12 Bits" },
                { name: "14 Bits" }
            ]
        }
    ],

    /* override generic pin requirements with dev-specific pin reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/adc/ADCMSP432.Board.c.xdt",
        boardh: "/ti/drivers/adc/ADC.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate
};

/*
 *  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  param inst    - a fully configured ADC instance
 *
 *  returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let adc = {
        name: "adc",
        hidden: true,
        displayName: "ADC14",
        interfaceName: "ADC14",
        canShareWith: "ADC",
        resources: [
            {
                name: "adcPin",
                hidden: false,
                displayName: "ADC Input Pin",
                interfaceNames: [
                    "A0", "A1", "A2", "A3",
                    "A4", "A5", "A6", "A7",
                    "A8", "A9", "A10", "A11",
                    "A12", "A13", "A14", "A15",
                    "A16", "A17", "A18", "A19",
                    "A20", "A21", "A22", "A23"
                ]
            }
        ],
        signalTypes: { adcPin: ["AIN"] }
    };

    return ([adc]);
}

/*
 *  ======== validate ========
 *  Perform MSP432-specific validation
 *
 *  param inst       - ADCMSP432 instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic ADC module to
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

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ADC module */
    extend: extend
};

/*!
 *  CC322X ADC module
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
    maxInstances: 4,

    config: [
        {
            name: "resolution",
            displayName: "Resolution",
            hidden: true,
            default: "12 Bits",
            options: [
                { name: "8 Bits" },
                { name: "10 Bits" },
                { name: "12 Bits" }
            ]
        }
    ],

    /* override generic pin requirements with device-specific pin reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/adc/ADCCC32XX.Board.c.xdt",
        boardh: "/ti/drivers/adc/ADC.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate
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
    let adc = {
        name: "adc",
        hidden: true,
        displayName: "ADC",
        interfaceName: "ADC",
        canShareWith: "ADC",
        resources: [
            {
                name: "adcPin",
                hidden: false,
                displayName: "ADC Input Pin",
                interfaceNames: [
                    "CH0", "CH1", "CH2", "CH3"
                ]
            }
        ],
        signalTypes: { adcPin: ["AIN"] }
    };

    return ([adc]);
}

/**
 *  ======== validate ========
 *  Perform CC322X-specific validation
 *
 *  @param inst       - ADCCC322X instance to be validated
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
 *  This function is invoked by the generic ADC module to
 *  allow us to augment/override as needed for the CC322X
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
    /* required function, called by base ADC module */
    extend: extend
};

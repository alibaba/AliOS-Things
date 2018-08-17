/*!
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
            name: "adcModule",
            displayName: "ADC Module",
            default: "ADCMSP432E4_MOD0",
            description: "Specifies the ADC peripheral to use.",
            options: [
                { name: "ADCMSP432E4_MOD0",
                  displayName: "ADC Module 0" },
                { name: "ADCMSP432E4_MOD1",
                  displayName: "ADC Module 1" }
            ]
        },
        {
            name: "refVoltage",
            displayName: "Reference Voltage",
            default: "ADCMSP432E4_VREF_INTERNAL",
            description: "Specifies the ADC reference voltage.",
            options: [
                { name: "ADCMSP432E4_VREF_INTERNAL",
                  displayName: "Internal Reference" },
                { name: "ADCMSP432E4_VREF_EXTERNAL_3V",
                  displayName: "External Reference" }
            ]
        },
        {
            name: "adcSeq",
            displayName: "ADC Sequence",
            description: "Specifies the ADC sequencer.",
            default: "ADCMSP432E4_SEQ0",
            options: [
                { name: "ADCMSP432E4_SEQ0" },
                { name: "ADCMSP432E4_SEQ1" },
                { name: "ADCMSP432E4_SEQ2" },
                { name: "ADCMSP432E4_SEQ3" }
            ]
        }
    ],

    /* override generic pin requirements with device-specific pin reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/adc/ADCMSP432E4.Board.c.xdt",
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
                    "AIN0", "AIN1", "AIN2", "AIN3",
                    "AIN3", "AIN4", "AIN5", "AIN6",
                    "AIN7", "AIN8", "AIN9", "AIN10",
                    "AIN11", "AIN12", "AIN12", "AIN13",
                    "AIN14", "AIN15", "AIN16", "AIN17",
                    "AIN18", "AIN19", "AIN20", "AIN21",
                    "AIN22", "AIN23"
                ]
            }
        ],
        signalTypes: { adcPin: ["AIN"] }
    };

    return ([adc]);
}

/**
 *  ======== validate ========
 *  Perform MSP432-specific validation
 *
 *  @param inst       - ADCMSP432 instance to be validated
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

/**
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ADC module */
    extend: extend
};

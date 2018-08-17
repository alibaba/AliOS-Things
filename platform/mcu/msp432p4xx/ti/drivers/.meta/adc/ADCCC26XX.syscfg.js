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
    maxInstances: 8,

    config: [
        {
            name: "refSource",
            displayName: "Reference Source",
            default: "ADCCC26XX_FIXED_REFERENCE",
            description: "Specifies the ADC reference source.",
            options: [
                { name: "ADCCC26XX_FIXED_REFERENCE",
                  displayName: "Fixed" },
                { name: "ADCCC26XX_VDDS_REFERENCE",
                  displayName: "VDDS" }
            ]
        },
        {
            name: "samplingDuration",
            displayName: "Sampling Duration",
            description: "Specifies the ADC's sampling duration.",
            default: "ADCCC26XX_SAMPLING_DURATION_2P7_US",
            options: [
                { name: "ADCCC26XX_SAMPLING_DURATION_2P7_US",
                  displayName: "2.7 microseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_5P3_US",
                  displayName: "5.3 microseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_10P6_US",
                  displayName: "10.6 microseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_21P3_US",
                  displayName: "21.3 microseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_42P6_US",
                  displayName: "42.6 microseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_85P3_US",
                  displayName: "85.3 microseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_170_US",
                  displayName: "170 microseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_341_US",
                  displayName: "341 microseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_682_US",
                  displayName: "682 microseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_1P37_MS",
                  displayName: "1.37 milliseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_2P73_MS",
                  displayName: "2.73 milliseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_5P46_MS",
                  displayName: "5.46 milliseconds" },
                { name: "ADCCC26XX_SAMPLING_DURATION_10P9_MS",
                  displayName: "10.9 milliseconds" }
            ]
        },
        {
            name: "inputScaling",
            displayName: "Enable Input Scaling",
            default: true,
            description: "Enables or Disables Input Scaling"
        },
        {
            name: "triggerSource",
            displayName: "Trigger Source",
            default: "ADCCC26XX_TRIGGER_MANUAL",
            readOnly: true,
            hidden: true,
            options: [
                { name: "ADCCC26XX_TRIGGER_MANUAL" }
            ]
        },
        {
            name: "returnAdjustedVal",
            displayName: "Return Adjusted Value",
            default: false
        }
    ],

    /* override generic pin requirements with device-specific pin reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* PIN instances */
    moduleInstances: moduleInstances,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/adc/ADCCC26XX.Board.c.xdt",
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
        displayName: "ADC Pin",
        interfaceName: "ADC",
        canShareWith: "ADC",
        resources: [
            {
                name: "adcPin",
                hidden: false,
                displayName: "ADC Input Pin",
                interfaceNames: [
                    "PIN0", "PIN1", "PIN2", "PIN3",
                    "PIN4", "PIN5", "PIN6", "PIN7"
                ]
            }
        ],
        signalTypes: { adcPin: ["AIN"] }
    };

    return ([adc]);
}

/*!
 *  ======== moduleInstances ========
 *  returns PIN instances
 */
function moduleInstances(inst)
{
    let pinInstances = new Array();

    pinInstances.push(
        {
            name: "adcPinInstance",
            displayName: "ADC Input PIN Configuration While Pin is Not In Use",
            moduleName: "/ti/drivers/PIN",
            collapsed: true,
            args: { parentMod: "/ti/drivers/ADC",
                    parentInstanceName: inst.$name + " : ADC Input",
                    parentInterfaceName: "adc",
                    parentSignalName: "adcPin",
                    mode: "Input",
                    pull: "None" }
        }
    );

    return (pinInstances);
}

/*!
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

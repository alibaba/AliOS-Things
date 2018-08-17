/*!
 *  MSP432 ADCBuf module
 *
 *  Device-specific ADCBuf configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base ADCBuf configuration
 */
let devSpecific = {
    maxInstances: 1,

    config: [
        {
            name: "numChans",
            displayName: "Number of ADCBuf Channels",
            description: "Maximum of 24 channels",
            default: 1,
            options: [
                { name: 1 },  { name: 2 },  { name: 3 },  { name: 4 },
                { name: 5 },  { name: 6 },  { name: 7 },  { name: 8 },
                { name: 9 },  { name: 10 }, { name: 11 }, { name: 12 },
                { name: 13 }, { name: 14 }, { name: 15 }, { name: 16 },
                { name: 17 }, { name: 18 }, { name: 19 }, { name: 20 },
                { name: 21 }, { name: 22 }, { name: 23 }, { name: 24 }
            ]
        },
        {
            name: "adcTriggerSource",
            displayName: "Trigger Source",
            default: "Timer",
            options: [
                { name: "Timer" },
                { name: "Auto" }
            ]
        },
        {
            name: "timerDutyCycle",
            displayName: "Timer Duty Cycle (1-99%)",
            default: 50
        },
        {
            name: "clockSource",
            displayName: "Clock Source",
            default: "ADC",
            options: [
                { name: "ADC" },
                { name: "SYSOSC" },
                { name: "ACLK" },
                { name: "MCLK" },
                { name: "SMCLK" },
                { name: "HSMCLK" }
            ]
        },
        {
            name: "useDMA",
            displayName: "Enable DMA use for the ADC",
            default: true
        }
    ],

    moduleInstances: moduleInstances,

    modules: modules,

    /* override generic pin requirements with device-specific pin reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/adcbuf/ADCBufMSP432.Board.c.xdt",
        boardh: "/ti/drivers/adcbuf/ADCBuf.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate,

    filterHardware: filterHardware
};

/*
 *  ======== triggerFilter ========
 *  must provide a filter to offer only the default mappings
 *  for these trigger sources:
 *  Timer_A0 CC1, Timer_A0 CC2, Timer_A1 CC1, Timer_A1 CC2
 *  Timer_A2 CC1, Timer_A2 CC2, Timer_A3 CC1
 *
 *  No pinMappable solutions are currently allowed by the driver.
*/
function triggerFilter (devicePin, peripheralPin)
{
    /* filter out pinMappable options */
    if (!devicePin.designSignalName.match(peripheralPin.name)) {
        return (false);
    }

    /* filter out Timer_A3.CCI2A */
    if (peripheralPin.name == "Timer_A3.CCI2A") {
        return (false);
    }

    return (true);
}

/**
 *  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  @param inst    - a fully configured ADCBuf instance
 *
 *  @returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{

    let reqs = [];

    let timer = {
        name: "timer",
        displayName: "Trigger Source Timer",
        interfaceName: "TIMER_A",
        resources: []
    };

    let adc = {
        name: "adc",
        displayName: "ADC14",
        hidden: true,
        interfaceName: "ADC14",
        canShareWith: "ADCBuf",
        resources: []
    };

    let dma = [
        {
            name: "dmaIntNum",
            displayName: "DMA Interrupt Number",
            interfaceNames: [ "DMA_INT" ]
        },
        {
            name: "dmaChannel",
            displayName: "DMA Channel",
            hidden: true,
            readOnly: true,
            interfaceNames: [ "DMA_CH" ]
        }
        ];

    if (inst.adcTriggerSource == "Timer") {
        reqs.push(timer);
    }

    if (inst.useDMA) {
        adc.resources = dma;
    }

    /* Always return an ADC14 peripheral */
    reqs.push(adc);

    return (reqs);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with ADC
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinRequirement object if ADC is supported.
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        if (component.signals[sig].type == "AIN") {
            return (true);
        }
    }
    return (false);
}
/*
 *  ======== modules ========
 */
function modules(inst)
{
    return (Common.autoForceDMAModule(inst));
}

/*
 *  ======== moduleInstances ========
 *  returns an array of ADCBufChanMSP432 instances.
 */
function moduleInstances(inst)
{
    let result = [];

    for (let i = 0; i < inst.numChans; i++) {
        result.push({
            name: "chan" + i,
            displayName: "ADCBuf Channel " + i + " ",
            moduleName: "/ti/drivers/adcbuf/ADCBufChanMSP432",
            hardware: inst.$hardware,
            hidden: false
        });
    }

    return (result);
}

/**
 *  ======== validate ========
 *  Perform MSP432-specific validation
 *
 *  @param inst       - ADCBufMSP432 instance to be validated
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
 *  This function is invoked by the generic ADCBuf module to
 *  allow us to augment/override as needed for the MSP432
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* concatenate device-specific configs */
    devSpecific.config = base.config.concat(
        Common.newIntPri()).concat(devSpecific.config);

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/**
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ADCBuf module */
    extend: extend
};

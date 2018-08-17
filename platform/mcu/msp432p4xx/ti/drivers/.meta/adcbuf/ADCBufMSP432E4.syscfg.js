/*
 *  ADCBufMSP432E4.syscfg.js
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Interrupt Priority";

/* There are 4 hardware sequencers per ADC peripheral */
const SEQUENCER_COUNT = 4;

/* Each sequencer uses a unique DMA channel per ADC peripheral */
var SEQUENCER_DMA = ["DMA_SS0", "DMA_SS1", "DMA_SS2", "DMA_SS3"];

/* Sequencer 0-3 support 8, 4, 4, and 1 channel(s) respectively */
var MAX_SEQ_CHANS = [8, 4, 4, 1];

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base ADCBuf configuration
 */
let devSpecific = {
    maxInstances: 1,

    config: [
        {
            name: "sequencers",
            displayName: "Select Sequencers",
            description: "Maximum of 4 sequencer. Each sequencer can support 8"
                + ", 4, 4, and 1 channels respectively",
            default: [0],
            options: [
                {name: 0, displayName: "Sequencer 0"},
                {name: 1, displayName: "Sequencer 1"},
                {name: 2, displayName: "Sequencer 2"},
                {name: 3, displayName: "Sequencer 3"}
            ]
        },
        {
            name: "modulePhase",
            displayName: "Module Phase",
            default: "ADCBufMSP432E4_Phase_Delay_0",
            options : [
                {name: "ADCBufMSP432E4_Phase_Delay_0"},
                {name: "ADCBufMSP432E4_Phase_Delay_22_5"},
                {name: "ADCBufMSP432E4_Phase_Delay_45"},
                {name: "ADCBufMSP432E4_Phase_Delay_67_5"},
                {name: "ADCBufMSP432E4_Phase_Delay_90"},
                {name: "ADCBufMSP432E4_Phase_Delay_112_5"},
                {name: "ADCBufMSP432E4_Phase_Delay_135"},
                {name: "ADCBufMSP432E4_Phase_Delay_157_5"},
                {name: "ADCBufMSP432E4_Phase_Delay_180"},
                {name: "ADCBufMSP432E4_Phase_Delay_202_5"},
                {name: "ADCBufMSP432E4_Phase_Delay_225"},
                {name: "ADCBufMSP432E4_Phase_Delay_247_5"},
                {name: "ADCBufMSP432E4_Phase_Delay_270"},
                {name: "ADCBufMSP432E4_Phase_Delay_292_5"},
                {name: "ADCBufMSP432E4_Phase_Delay_315"},
                {name: "ADCBufMSP432E4_Phase_Delay_337_5"}
            ]
        },
        {
            name: "refSource",
            displayName: "Reference Source",
            default: "ADCBufMSP432E4_VREF_INTERNAL",
            options : [
                {name: "ADCBufMSP432E4_VREF_INTERNAL"},
                {name: "ADCBufMSP432E4_VREF_EXTERNAL_3V"}
            ]
        },
        {
            name: "useDMA",
            displayName: "Enable DMA use for the ADC",
            default: true
        },
        {
            name: "useTimer",
            displayName: "Enable Timer use for Sequencers",
            default: true
        },

        intPriority

    ],

    moduleInstances: moduleInstances,

    modules: modules,

    /* override generic pin requirements with device-specific pin reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/adcbuf/ADCBufMSP432E4.Board.c.xdt",
        boardh: "/ti/drivers/adcbuf/ADCBufMSP432E4.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate,

    filterHardware: filterHardware
};


/*
 *  ======== pinmuxRequirements ========
 */
function pinmuxRequirements(inst)
{
    let requirements = [];

    let timer = {
        name: "timer",
        displayName: "Trigger Source Timer",
        interfaceName: "Timer",
        resources: []
    };

    let adc = {
        name: "adc",
        displayName: "ADC",
        interfaceName: "ADC",
        canShareWith: inst.$name,
        resources: []
    };

    if (inst.useDMA === true) {
        /* Determine DMA Channels needed */
        for (let i = 0; i < SEQUENCER_COUNT; i++) {
            if (inst["sequence" +i]) {
                adc.resources.push({
                    name: "dma_ss" + i,
                    displayName: "uDMA Channel",
                    interfaceNames: [SEQUENCER_DMA[i]],
                    hidden: true,
                    readOnly: true
                });
            }
        }
    }

    if (inst.useTimer === true) {
        requirements.push(timer);
    }

    requirements.push(adc);

    return (requirements);
}

/*
 *  ========= filterHardware ========
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
 *  returns the DMA module if this instance uses DMA.
 */
function modules(inst)
{
    if (inst.useDMA === true) {
        return (Common.autoForceDMAModule(inst));
    }
    else {
        return ([]);
    }
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    let result = [];
    let triggerSource = "ADCBufMSP432E4_TIMER_TRIGGER";
    if (inst.useTimer === false) {
        triggerSource = "ADCBufMSP432E4_SOFTWARE_AUTOMATIC_TRIGGER";
    }

    /* Create a Sequence instance for each of the unique sequencers. */
    for (let i in inst.sequencers) {
        let sequencer = inst.sequencers[i];
        result.push({
            name: "sequence" + sequencer,
            displayName: "Sequencer " + sequencer + " ",
            moduleName: "/ti/drivers/adcbuf/ADCBufSeqMSP432E4",
            args: {
                maxChans: MAX_SEQ_CHANS[sequencer],
                number: parseInt(sequencer),
                triggerSource: triggerSource
            }
        });
    }

    return (result);
}

/*
 *  ======== validate ========
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*
 *  ======== extend ========
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
    /* required function, called by base ADCBuf module */
    extend: extend
};

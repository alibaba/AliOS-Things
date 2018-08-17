/*
 *  ADCBufSeqMSP432E4.syscfg.js
 */
"use strict";

let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

/*
 *  ======== exports ========
 */
exports = {
    config: [
        {
            name: "priority",
            displayName: "Sequence Priority",
            descriptions: "Specify sequencer priority.",
            default: "ADCBufMSP432E4_Priority_0",
            options: [
                { name: "ADCBufMSP432E4_Priority_0" },
                { name: "ADCBufMSP432E4_Priority_1" },
                { name: "ADCBufMSP432E4_Priority_2" },
                { name: "ADCBufMSP432E4_Priority_3" }
            ]
        },
        {
            name: "triggerSource",
            displayName: "Sequence Trigger Source",
            default: "ADCBufMSP432E4_TIMER_TRIGGER",
            options: [
                { name: "ADCBufMSP432E4_TIMER_TRIGGER" },
                { name: "ADCBufMSP432E4_SOFTWARE_AUTOMATIC_TRIGGER" }
            ]
        },
        {
            name: "channels",
            default: 1,
            onChange: channelsChange
        },
        {
            name: "maxChans",
            default: -1,
            hidden: true
        },
        {
            name: "number",
            default: -1,
            hidden: true
        }
    ],

    moduleInstances : moduleInstances,

    validate: validate
};

/*
 *  ======== validate ========
 */
function validate(inst, validation)
{
    let parent = inst.$ownedBy;
    if (parent) {
        if (parent.useTimer === false) {
            if (inst.triggerSource === "ADCBufMSP432E4_TIMER_TRIGGER") {
                logError(validation, inst, "triggerSource", "Timer is not enabled"
                    + " for sequencers.");
            }
        }
    }
}

/*
 *  ======== channelsChange ========
 */
function channelsChange(inst, ui)
{
    if (inst.maxChans != -1) {
        if (inst.channels > inst.maxChans) {
            inst.channels = inst.maxChans;
        }
    }
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    let result = [];
    if (inst.maxChans != -1) {
        let parent = inst.$ownedBy;
        for (let i = 0; i < inst.channels; i++) {
            result.push({
                name: "chan" + i,
                displayName: "Sequencer " + inst.number + " Channel " + i + " ",
                moduleName: "/ti/drivers/adcbuf/ADCBufChanMSP432E4",
                hardware: inst.$hardware,
                hidden: false
            });
        }
    }
    return (result);
}
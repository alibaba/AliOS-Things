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

let adcChanNames = [
    "A0", "A1", "A2", "A3",
    "A4", "A5", "A6", "A7",
    "A8", "A9", "A10", "A11",
    "A12", "A13", "A14", "A15",
    "A16", "A17", "A18", "A19",
    "A20", "A21", "A22", "A23"
];

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
    let adcBufChan = {
        name: "adc",
        displayName: "ADC14",
        hidden: true,
        interfaceName: "ADC14",
        canShareWith: "ADCBuf",
        resources: []
    };

    if (inst.inputSource == "External Pin") {
        adcBufChan.resources.push({
            name: "adcPin",
            displayName: "ADCBuf Channel",
            interfaceNames: adcChanNames
        });
        adcBufChan.signalTypes = { adcPin: ["AIN"] };
    }

    /* add the differential input pin if required */
    if (inst.inputMode == "Differential") {
        adcBufChan.resources.push({
            name: "adcDiffPin",
            displayName: "ADCBuf Differential Input",
            interfaceNames: adcChanNames
        });
    }

    return ([adcBufChan]);
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

function onRefVoltageChange(inst, ui)
{
    let vRef = "";
    let vSrc = "";

    ui.refVoltage.readOnly = false;

    switch (inst.refSource) {
        case "VDD":
            inst.refVoltage = 3300000;
            break;
        case "1.2V":
            inst.refVoltage = 1200000;
            break;
        case "1.45V":
            inst.refVoltage = 1450000;
            break;
        case "2.5V":
            inst.refVoltage = 2500000;
            break;
        case "External":
            ui.refVoltage.readOnly = false;
            break;
        case "External Buffered":
            ui.refVoltage.readOnly = false;
            break;
    }
}

function onInputSourceChange(inst, ui)
{
    if (inst.inputSource != "External Pin") {
        ui.inputMode.hidden = true;
    }
    else {
        ui.inputMode.hidden = false;
    }
}

/**
 *  ======== exports ========
 */
exports = {
    config: [
        {
            name: "refSource",
            displayName: "Reference Source",
            default: "VDD",
            options: [
                { name: "VDD" },
                { name: "1.2V" },
                { name: "1.45V" },
                { name: "2.5V" },
                { name: "External" },
                { name: "External Buffered" }
            ],
            onChange: onRefVoltageChange
        },
        {
            name: "refVoltage",
            displayName: "Reference Voltage",
            description: "in micovolts",
            hidden: false,
            readOnly: false,
            default: 3300000
        },
        {
            name: "inputSource",
            displayName: "Input Source",
            default: "External Pin",
            options: [
                { name: "External Pin" },
                { name: "Internal Temperature" },
                { name: "Internal Battery" }
            ],
            onChange: onInputSourceChange
        },
        {
            name: "inputMode",
            displayName: "Input Mode",
            hidden: false,
            default: "Non Differential",
            options: [
                { name: "Non Differential" },
                { name: "Differential" }
            ]
        }
    ],

    /* override generic pin requirements with device-specific pin reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    filterHardware: filterHardware
};

/*
 *  ADCBufChanMSP432E4.syscfg.js
 */
"use strict";
/*global exports, system*/

let adcChanNames = [
    "AIN0", "AIN1", "AIN2", "AIN3",
    "AIN3", "AIN4", "AIN5", "AIN6",
    "AIN7", "AIN8", "AIN9", "AIN10",
    "AIN11", "AIN12", "AIN12", "AIN13",
    "AIN14", "AIN15", "AIN16", "AIN17",
    "AIN18", "AIN19", "AIN20", "AIN21",
    "AIN22", "AIN23"
];

/*
 *  ======== pinmuxRequirements ========
 */
function pinmuxRequirements(inst)
{
    let adcBufChan = {
        name: "adc",
        displayName: "ADC",
        hidden: true,
        interfaceName: "ADC",
        canShareWith: inst.$ownedBy.$ownedBy.$name,
        resources: [
        ]
    };

    if (inst.inputSource == "External Pin") {
        adcBufChan.resources.push({
            name: "adcPin",
            displayName: "ADC Channel",
            interfaceNames: adcChanNames
        });
        adcBufChan.signalTypes = { adcPin: ["AIN"] };
    }

    /* add the differential input pin if required */
    if (inst.inputMode == "Differential") {
        adcBufChan.resources.push({
            name: "adcDiffPin",
            displayName: "ADC Differential Input",
            interfaceNames: adcChanNames
        });
    }

    return ([adcBufChan]);
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
 *  ========= onInputSourceChange ========
 */
function onInputSourceChange(inst, ui)
{
    if (inst.inputSource === "External Pin") {
        ui.inputMode.hidden = false;
    }
    else {
        ui.inputMode.hidden = true;
        inst.inputMode = "Non Differential";
    }
}

/*
 *  ======== exports ========
 */
exports = {
    config: [
        {
            name: "inputSource",
            displayName: "Input Source",
            default: "External Pin",
            options: [
                { name: "External Pin" },
                { name: "Internal Temperature" }
            ],
            onChange : onInputSourceChange
        },
        {
            name: "inputMode",
            displayName: "Input Mode",
            default: "Non Differential",
            options: [
                { name: "Non Differential" },
                { name: "Differential" }
            ]
        },
        {
            name: "refVoltage",
            displayName: "Reference Voltage",
            description: "in microvolts",
            default: 3300000
        }
    ],

    /* override generic pin requirements with device-specific pin reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    filterHardware: filterHardware
};

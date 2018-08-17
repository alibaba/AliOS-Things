/*!
 *  CC32XX Timer module
 *
 *  Device-specific Timer configuration support
 */

"use strict";
/*global exports, system*/

let Common = system.getScript("/ti/drivers/Common.js");
let $super;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Timer configuration
 */
let devSpecific = {

    config: [
        {
            name        : "pinType",
            displayName : "Use Case",
            hidden      : true,
            onChange    : onPinTypeChange,
            default     : "None",
            options     : [
                { name: "None" },
                { name: "ADCBuf" },
                { name: "PWM" },
                { name: "Capture" }
            ]
        }
    ],

    templates: {
        boardc : "/ti/drivers/timer/TimerCC26XX.Board.c.xdt",
        boardh : "/ti/drivers/timer/Timer.Board.h.xdt"
    },

    maxInstances       : 8,

    /* override generic pin requirements */
    pinmuxRequirements : pinmuxRequirements,

    /* PIN instances */
    moduleInstances    : moduleInstances,

    filterHardware     : filterHardware,
    onHardwareChanged  : onHardwareChanged,
    validate           : validate
};

/**
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let timer = {
        name          : "timer",
        displayName   : "Timer",
        interfaceName : "GPTM"
    };

    switch (inst.pinType) {
        case "None": {
            if (inst.timerType == "16 Bits") {
                timer.canShareWith = "16 Bit Timer";
                timer.maxShareCount = 2;
                timer.resources = []; // no resources required for Timer
            }
            break;
        }
        case "ADCBuf": {
            timer.canShareWith = "ADCBuf";
            timer.maxShareCount = 2;
            timer.resources = []; // no resources required for Timer
            break;
        }
        case "PWM" : {
            timer.canShareWith = "PWM";
            timer.maxShareCount = 2;
            timer.resources = [{
                name: "pwmPin",
                displayName: "PWM Output",
                interfaceNames: [
                    "TIMER_A_PWM", "TIMER_B_PWM"
                ]
            }];
            timer.signalTypes = { pwmPin: ["PWM"] };
            inst.intPriority.hidden = true;
            break;
        }
        case "Capture": {
            timer.canShareWith = "Capture";
            timer.maxShareCount = 2;
            timer.resources = [{
                name: "capturePin",
                displayName: "Capture Input",
                interfaceNames: [
                    "TIMER_A_CCP", "TIMER_B_CCP"
                ]
            }];
            timer.signalTypes = { capturePin: ["DIN"] };
            inst.intPriority.hidden = true;
            break;
        }
    }

    return ([timer]);
}

/*!
 *  ======== moduleInstances ========
 *  returns PIN instance
 */
function moduleInstances(inst)
{
    let pinInstances = new Array();

    switch (inst.pinType) {
        case "None":
        case "ADCBuf":
            break;
        case "PWM": {
            pinInstances.push(
                {
                    name: "pwmPinInstance",
                    displayName: "PWM Output PIN Configuration While Pin is Not In Use",
                    moduleName: "/ti/drivers/PIN",
                    collapsed: true,
                    args: { parentMod: "/ti/drivers/Timer",
                            parentInstanceName: inst.$name + " : PWM Output",
                            parentInterfaceName: "timer",
                            parentSignalName: "pwmPin",
                            mode: "Output",
                            outputState: "Low",
                            pull: "None" }
                }
            );
            break;
        }
    }

    return (pinInstances);
}

function onPinTypeChange(inst, ui)
{
    switch (inst.pinType) {
        case "None":
            ui.intPriority.hidden = false;
            ui.timerType.readOnly = false;
            ui.pinType.hidden = true;
            ui.pinType.readOnly = false;
            break;
        case "PWM":
        case "Capture":
            ui.intPriority.hidden = true;
            ui.timerType.readOnly = true;
            ui.pinType.hidden = false;
            ui.pinType.readOnly = true;
            break;
    }
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with pinType.
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinmuxRequirement object if pinType is supported.
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["PWM", "Capture"])) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== onHardwareChanged ========
 *  Handle new hardware selection
 */
function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {
        let key = Object.keys(inst.$hardware.signals)[0];
        let signal = inst.$hardware.signals[key];
        let type = signal.type;

        if (Common.typeMatches(type, ["PWM"])) {
            inst.pinType = "PWM";
            ui.pinType.readOnly = true;
            onPinTypeChange(inst, ui);
        }
        if (Common.typeMatches(type, ["Capture"])) {
            inst.pinType = "Capture";
            ui.pinType.readOnly = true;
            onPinTypeChange(inst, ui);
        }
    }
}

/**
 *  ======== validate ========
 *  Perform CC26XX-specific validation
 *
 *  @param inst       - TimerCC26XX instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/**
 * The base object is passed to this function and allows the device
 * specific object to incorporate properties from the base object.
 *
 * @param base - the exports from the base object
 *
 * @returns - a composite object of the base + device specific modules
 */
function extend(base)
{
    $super = base;
    devSpecific.config = devSpecific.config.concat(base.config);
    return Object.assign({}, base, devSpecific);
}

/* Export device-specific extensions to base exports */
exports = { extend : extend };

/*!
 *  PIN configuration support
 */
"use strict";
/*global exports, system*/

/* issue: what's difference between system.getScript(), system.modules[] ...?
 *     system.getScript()      - == xdc.loadCapsule()
 *     system.modules[)        - == xdc.useModule(],
 *     scripting.modules.addInstance() - create instance of module:
 */

/* get Common /ti/drivers utility functions */
let Common     = system.getScript("/ti/drivers/Common.js");
let logError   = Common.logError;
let logWarning = Common.logWarning;

/* compute /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "PIN");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "PIN interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "PIN Swi handler priority";

/* Intro splash on GUI */
let longDescription =
   "  The PIN driver allows clients (applications or other drivers) to allocate"
 + "  and control the I/O pins on the device. The pins can either be software-"
 + "  controlled general-purpose I/O (GPIO) or connected to hardware peripherals."
 + "  Furthermore, the PIN driver allows clients to configure interrupt"
 + "  functionality on the pins to receive callbacks (and potentially wake up from"
 + "  the standby or idle power modes) on configurable signal edges."
 + "\n"
 + "  Most other drivers rely on functionality in the PIN driver."
 + "\n"
 + "  The PIN module provides the following functionality:"
 + "  - Initialize I/O pins upon boot to a default configuration (possibly"
 + "   user-generated)";

/* generic configuration parameters for PIN instances */
let pinConfig = [
    {
        name: "mode",
        displayName: "Mode",
        default: "Input",
        options: [
            { name: "Input" },
            { name: "Output" }
        ],
        onChange: updateConfigs
    },
    {
        name: "invert",
        displayName: "Invert Input/Output",
        default: false,
        onChange: updateConfigs
    },
    /* Output Configs */
    {
        name: "outputState",
        displayName: "Initial Output State",
        hidden: true,
        default: "Low",
        options: [
            { name: "High" },
            { name: "Low" }
        ]
    },
    {
        name: "outputType",
        displayName: "Output Type",
        hidden: true,
        default: "Push/Pull",
        options: [
            { name: "Push/Pull" },
            { name: "Open Drain" },
            { name: "Open Source" }
        ],
        onChange: updateConfigs
    },
    {
        name: "outputStrength",
        displayName: "Output Strength",
        hidden: true,
        default: "Medium",
        options: [
            { name: "Maximum",
              displayName: "Maximum (8mA)" },
            { name: "Medium",
              displayName: "Medium  (4mA)" },
            { name: "Minimum",
              displayName: "Minimum (2mA)" }
        ],
        onChange: updateConfigs
    },
    {
        name: "outputSlew",
        displayName: "Enable Output Slew",
        hidden: true,
        default: false,
        onChange: updateConfigs
    },
    /* Input Configs */
    {
        name: "hysteresis",
        displayName: "Enable Input Hysteresis",
        default: false,
        onChange: updateConfigs
    },
    {
        name: "pull",
        displayName: "Pull up/down",
        hidden: false,
        default: "None",
        options: [
            { name: "None" },
            { name: "Pull Up" },
            { name: "Pull Down" }
        ],
        onChange: updateConfigs
    },
    {
        name: "irq",
        displayName: "Interrupt",
        hidden: false,
        default: "Disabled",
        options: [
            { name: "Disabled" },
            { name: "Falling Edge" },
            { name: "Rising Edge" },
            { name: "Both Edges" }
        ]
    }
];

/*
 *  ======== updateConfigs ========
 *  Adjust UI properties of configs based on current config settings
 *
 *  issue: must modify isr config to prevent false readonly display of
 *         interrupt trigger
 */
function updateConfigs(inst, ui)
{
    switch (inst.mode) {
        case "Output": {
            ui.pull.hidden = true;
            ui.outputType.hidden = false;
            ui.outputState.hidden = false;
            ui.outputStrength.hidden = false;
            ui.hysteresis.hidden = true;
            ui.irq.hidden = true;
            break;
        }
        case "Input": {
            ui.pull.hidden = false;
            ui.outputType.hidden = true;
            ui.outputStrength.hidden = true;
            ui.outputState.hidden = true;
            ui.hysteresis.hidden = false;
            ui.irq.hidden = false;
            break;
        }
    }
}

/**
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  @param inst       - PIN instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if (inst.mode == "Output") {
        /* output PINs can't have interrupts enabled */
        if (inst.irq != "Disabled") {
            logError(validation, inst, "irq",
                "Output mode PIN resources can't have interrupts enabled");
        }
    }
}


/*
 *  ======== getAttrs ========
 *  Return a symbolic PIN bit mask corresponding to inst's configs
 */
function getAttrs(inst)
{
    let input = "PIN_INPUT_EN";
    let output = "PIN_GPIO_OUTPUT_EN";

    if (inst.mode == "Output") {
        if (inst.outputState == "High") {
            output += " | PIN_GPIO_HIGH";
        }
        else {
            output += " | PIN_GPIO_LOW";
        }
        switch (inst.outputType) {
            case "Push/Pull": {
                output += " | PIN_PUSHPULL";
                break;
            }
            case "Open Drain": {
                output += " | PIN_OPENDRAIN";
                break;
            }
            case "Open Source": {
                output += " | PIN_OPENSOURCE";
                break;
            }
        }
        switch (inst.outputStrength) {
            case "Maximum": {
                output += " | PIN_DRVSTR_MAX";
                break;
            }
            case "Medium": {
                output += " | PIN_DRVSTR_MED";
                break;
            }
            case "Minimim":
            default: {
                output += " | PIN_DRVSTR_MIN";
                break;
            }
        }
        return (output);
    }
    else {
        switch (inst.pull) {
            case "Pull Up": {
                input += " | PIN_PULLUP";
                break;
            }
            case "Pull Down": {
                input += " | PIN_PULLDOWN";
                break;
            }
            case "Push/Pull": {
                input += " | PIN_PUSHPULL";
                break;
            }
            case "None":
            default: {
                input += " | PIN_NOPULL";
                break;
            }
        }

        switch (inst.irq) {
            case "Disabled": {
                input += " | PIN_IRQ_DIS";
                break;
            }
            case "Falling Edge": {
                input += " | PIN_IRQ_NEGEDGE";
                break;
            }
            case "Rising Edge": {
                input += " | PIN_IRQ_POSEDGE";
                break;
            }
            case "Both Edges": {
                input += " | PIN_IRQ_BOTHEDGES";
                break;
            }
        }

        if (inst.hysteresis) {
            input += " | PIN_HYSTERESIS";
        }

        return (input);
    }
}

/*
 *  ======== base ========
 *  Define the base/common PIN property and method exports
 */
let base = {
    displayName: "PIN",
    defaultInstanceName: "Board_PIN",
    description: "General Purpose PIN Driver",
    longDescription: longDescription,
//    validate: validate,
    config: [],
    pinConfig: pinConfig,
    moduleStatic: {
        displayName: "PIN Global",
        config: [
            intPriority,
            swiPriority
        ],
        getAttrs: getAttrs
    }
};

/* get the family-specific PIN module */
let devicePIN = system.getScript("/ti/drivers/pin/PIN" + family);

/* extend our common exports to include the family-specific content */
exports = devicePIN.extend(base);

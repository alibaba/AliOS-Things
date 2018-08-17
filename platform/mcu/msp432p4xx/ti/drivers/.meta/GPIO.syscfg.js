/*!
 *  GPIO configuration support
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
let family = Common.device2Family(system.deviceData, "GPIO");

/* Common interrupt priority element added to GPIO static configs */
let intPriority = Common.newIntPri()[0];

/* Intro splash on GUI */
let longDescription = "The GPIO module allows you to manage General " +
    "Purpose I/O resources via simple and portable APIs. " +
    "GPIO pin behavior is usually configured statically, " +
    "but can also be configured or reconfigured at runtime.";

/* generic configuration parameters for GPIO instances */
let gpioConfig = [
    {
        name: "mode",
        displayName: "Mode",
        default: "Input",
        options: [
            { name: "Input" },
            { name: "Output" },
            { name: "Dynamic",
              description: "Mode determined at runtime." },
            { name: "Empty",
              description: "Used to force a GPIO indexing scheme (ie Energia)." }
        ],
        onChange: updateConfigs
    },
    {
        name: "outputType",
        displayName: "Output Type",
        hidden: true,
        default: "Standard",
        options: [
            { name: "Standard" },
            { name: "Open Drain" }
        ]
    },
    {
        name: "outputStrength",
        displayName: "Output Strength",
        hidden: true,
        default: "Medium",
        options: [
            { name: "High" },
            { name: "Medium" },
            { name: "Low" }
        ]
    },
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
        name: "pull",
        displayName: "Pull up/down",
        hidden: false,
        default: "None",
        options: [
            { name: "None" },
            { name: "Pull Up" },
            { name: "Pull Down" }
        ]
    },
    {
        name: "interrupt",
        displayName: "Interrupt Trigger",
        hidden: false,
        default: "None",
        options: [
            { name: "None" },
            { name: "High" },
            { name: "Low" },
            { name: "Falling Edge" },
            { name: "Rising Edge" },
            { name: "Both Edges" }
        ]
    },
    {
        name: "callback",
        hidden: false,
        displayName: "Callback function",
        default: "",
        onChange: updateConfigs
    },
    {
        name: "comment",
        displayName: "Comment",
        hidden: true,
        default: ""
    }
];

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    if (inst.mode == "Empty") {
        return ([]);
    }

    /*
     * name is the name of a property that will be added to inst;
     * this property's value is an object returned by the
     * pinmux solver
     */
    let gpio = {
        name: "gpioPin",
        displayName: "GPIO Pin",
        interfaceName: "GPIO",    /* pmux interface name (not module name!!) */
        signalTypes: ["DIN", "DOUT"]
    };

    return ([gpio]);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with GPIO
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinRequirement object if GPIO is supported.
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["DIN", "DOUT"])) {
            return (true);
        }
    }

    return (false);
}

/**
 *  ======== sort ========
 *  Reorder GPIO instances to minimize space required for callbacks
 *
 *  @param instances - GPIO.$instances array
 *  @returns array of instances such that all GPIOs with interrupts
 *   appear ahead of GPIOs that can't trigger an interrupt.
 */
function sort(instances, addCallback)
{
    if (instances.length == 0 
        || instances[0].$module.$static.enableSort == false) {
        return (instances);
    }

    let callbackInstances = [];
    let otherInstances = [];

    for (let i = 0; i < instances.length; i++) {
        let inst = instances[i];
        if (addCallback[inst.$name] == true) {
            callbackInstances[callbackInstances.length] = inst;
        } else {
            otherInstances[otherInstances.length] = inst;
        }
    }

    return (callbackInstances.concat(otherInstances));
}

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
        case "Output":
            {
                ui.callback.hidden = true;
                ui.outputType.hidden = false;
                ui.outputState.hidden = false;
                if (inst.outputType == "Standard") {
                    ui.pull.hidden = true;
                    ui.outputStrength.hidden = false;
                } else {
                    ui.pull.hidden = false;
                    ui.outputStrength.hidden = true;
                }
                ui.pull.hidden = false;
                ui.interrupt.hidden = true;
                break;
            }
        case "Input":
            {
                ui.outputType.hidden = true;
                ui.outputStrength.hidden = true;
                ui.outputState.hidden = true;
                ui.pull.hidden = false;
                ui.interrupt.hidden = false;
                ui.callback.hidden = false;
                break;
            }
        case "Dynamic":
            {
                ui.outputType.hidden = true;
                ui.outputStrength.hidden = true;
                ui.outputState.hidden = true;
                ui.pull.hidden = true;
                ui.callback.hidden = false;
                ui.interrupt.hidden = true;
                break;
            }
        case "Empty":
            {
                ui.callback.hidden = true;
                ui.outputStrength.hidden = true;
                ui.outputState.hidden = true;
                ui.pull.hidden = true;
                ui.interrupt.hidden = true;
            }
    }
}

/**
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  @param inst       - GPIO instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if (inst.mode == "Output") {
        /* output GPIOs don't trigger interrupts */
        if (inst.interrupt != "None") {
            logError(validation, inst, "interrupt",
                "Output mode GPIO resources can't trigger interrupts");
        }

        /* specified callback is never called for output only GPIOs */
        if (inst.callback.length > 0) {
            let message = "callback '" + inst.callback + "' set, but won't be triggered by this output-only GPIO";

            logError(validation, inst, "callback", message);
        }
    }

    /* callback must be valid C identifier */
    if (!Common.isCName(inst.callback)) {
        let message = "'" + inst.callback + "' is not a valid a C identifier";
        logError(validation, inst, "callback", message);
    }

    /* TODO: if not defined should we plug-in NOP and remark? */
    if ((inst.interrupt != "None") && (inst.callback == "") && (inst.mode != "Dynamic")) {
        logWarning(validation, inst, "callback", "Callback must not be NULL or the 'mode' must be set to 'Dynamic' (not " + inst.mode + ")");
    }
    
    if (inst.callback != "NULL" && (inst.callback.toLowerCase() == "null")) {
        logWarning(validation, inst, "callback", "Did you mean \"NULL\"?");
    }
}

/*
 *  ======== getPinNum ========
 * Translate GPIO name ("Px.y") into corresponding device pin number.
 */
function getPinNum(gpioName)
{
    for (let x in system.deviceData.devicePins) {
        if (system.deviceData.devicePins[x].description.match(gpioName)) {
            return (String(system.deviceData.devicePins[x].ball));
        }
    }

    return ("");
}

/*
 *  ======== getFullPinName ========
 * Translate GPIO name ("Px.y") into corresponding description.
 */
function getFullPinName(gpioName)
{
    for (let x in system.deviceData.devicePins) {
        if (system.deviceData.devicePins[x].description.match(gpioName)) {
            return (String(system.deviceData.devicePins[x].description));
        }
    }

    return ("");
}

/*
 *  ======== getPinName ========
 * Translate device pin number into GPIO name ("Px.y")
 */
function getPinName(pinNum)
{
    if (isNaN(pinNum)) {
        throw Error("'" + pinNum + "' is not a pin number");
    }
    for (let x in system.deviceData.devicePins) {
        if (system.deviceData.devicePins[x].ball == pinNum) {
            let desc = String(system.deviceData.devicePins[x].description);
            return (desc.substr(0, desc.indexOf("G")-1));
        }
    }

    return ("");
}

/*
 *  ======== onHardwareChanged ========
 *  Handle new hardware selection
 *  issue: when is this onHardwareChanged called????
 */
function onHardwareChanged(inst, ui)
{
    let compatible = false;

    if (inst.$hardware) {
        let key = Object.keys(inst.$hardware.signals)[0];
        let signal = inst.$hardware.signals[key];
        let type = signal.type;

        if (Common.typeMatches(type, ["DOUT"])) {
            if (Common.typeMatches(type, ["DIN"])) {
                inst.mode = "Dynamic";
            }
            else {
                inst.mode = "Output";
                Common.setDefaults(inst, signal, "DOUT");
            }
            compatible = true;
        }
        else if (Common.typeMatches(type, ["DIN"])) {
            inst.mode = "Input";
            Common.setDefaults(inst, signal, "DIN");
            compatible = true;
        }
    }

    if (compatible) {
        updateConfigs(inst, ui);
    }
}

/*
 *  ======== getAttrs ========
 *  Return a symbolic GPIO bit mask corresponding to inst's configs
 */
function getAttrs(inst)
{
    if (inst.mode == "Dynamic" || inst.mode == "Empty") {
        return ("GPIO_DO_NOT_CONFIG");
    }

    if (inst.mode == "Output") {
        let output = "";
        if (inst.outputType == "Standard") {
            output += "GPIO_CFG_OUT_STD";
            switch (inst.outputStrength) {
                case "High": {
                    output += " | GPIO_CFG_OUT_STR_HIGH";
                    break;
                }
                case "Medium": {
                    output += " | GPIO_CFG_OUT_STR_MED";
                    break;
                }
                case "Low":
                default: {
                    output += " | GPIO_CFG_OUT_STR_LOW";
                    break;
                }
            }
        }
        else {
            switch (inst.pull) {
                case "Pull Up": {
                    output = "GPIO_CFG_OUT_OD_PU";
                    break;
                }
                case "Pull Down": {
                    output = "GPIO_CFG_OUT_OD_PD";
                    break;
                }
                case "None":
                default: {
                    output = "GPIO_CFG_OUT_OD_NOPULL";
                    break;
                }
            }
        }
        if (inst.outputState == "High") {
            output += " | GPIO_CFG_OUT_HIGH";
        }
        else {
            output += " | GPIO_CFG_OUT_LOW";
        }
        return (output);
    }
    else {
        let input = "";
        switch (inst.pull) {
            case "Pull Up": {
                input = "GPIO_CFG_IN_PU";
                break;
            }
            case "Pull Down": {
                input = "GPIO_CFG_IN_PD";
                break;
            }
            case "None":
            default: {
                input = "GPIO_CFG_IN_NOPULL";
                break;
            }
        }

        if (inst.interrupt != "None") {
            switch (inst.interrupt) {
                case "Falling Edge": {
                    input += " | GPIO_CFG_IN_INT_FALLING";
                    break;
                }
                case "Rising Edge": {
                    input += " | GPIO_CFG_IN_INT_RISING";
                    break;
                }
                case "High": {
                    input += " | GPIO_CFG_IN_INT_HIGH";
                    break;
                }
                case "Low": {
                    input += " | GPIO_CFG_IN_INT_LOW";
                    break;
                }
                case "Both Edges": {
                    input += " | GPIO_CFG_IN_INT_BOTH_EDGES";
                    break;
                }
            }
        }
        else {
            input += " | GPIO_CFG_IN_INT_NONE";
        }
        return (input);
    }
}

/*
 *  ======== collectCallbacks ========
 *
 */
function collectCallbacks(instances, externs, addCallback)
{
    let callbacks = {};
    let callbackCount = 0;
    let sortDisabled = system.getScript("/ti/drivers/GPIO").$static.enableSort == false;

    for (let i = 0; i < instances.length; i++) {
        let inst = instances[i];

        if (sortDisabled ||
            (inst.callback != "") ||
            (inst.interrupt != "None") ||
            (inst.mode == "Dynamic") ||
            (inst.mode == "Empty")) {
                addCallback[inst.$name] = true;
                callbackCount++;
        }
        else {
            addCallback[inst.$name] = false;
        }

        if (inst.callback != "" && inst.callback != "NULL") {
            callbacks[inst.callback] = 1;
        }
    }

    for (let name in callbacks) {
        externs.push("extern void " + name + "(uint_least8_t index);");
    }

    return (callbackCount);
}

/*
 *  ======== addComment ========
 *
 *  %p = Px.y extracted from $solution.peripheralPinName
 *  %P = entire pin name from $solution.devicePinName
 *  %i = GPIO config array index
 *  %c = C Name
 *  %n = $name
 *  %l = comment text is inserted a line before the config line.
 *  %tnum = comment is placed at character position - num.
 */
function addComment(line, inst, index, pin)
{
    let padding = Array(80).join(' ');
    let comment = inst.comment;
    if (inst.comment == "") {
        if (inst.$hardware && inst.$hardware.displayName) {
            comment = "    /* " + inst.$name + " : " + inst.$hardware.displayName + " */\n";
        }
        else {
            comment = "    /* " + inst.$name + " */\n";
        }
        return (comment + line);
    }
    if (comment.match("%t")) {
        let tab = comment.substring(comment.indexOf("%t") + 2);
        let ts = parseInt(tab);
        comment = comment.replace("%t"+ts, "");
        comment = padding.substring(0, ts - line.length) + comment;
    }
    if (comment.match("%c")) {
        comment = comment.replace("%c",  inst.$name);
    }
    if (comment.match("%n")) {
        comment = comment.replace("%n",  inst.$name);
    }
    if (comment.match("%i")) {
        comment = comment.replace("%i", index);
    }
    if (comment.match("%p")) {
        if (inst.mode == "Empty") {
            comment = comment.replace("%p", "Empty Pin");
        }
        else {
            let pName = pin.$solution.peripheralPinName;
            comment = comment.replace("%p", pName);
        }
    }
    if (comment.match("%P")) {
        if (inst.mode == "Empty") {
            comment = comment.replace("%P", "Empty Pin");
        }
        else {
            comment = comment.replace("%P",  pin.$solution.devicePinName);
        }
    }
    if (comment.match("%l")) {
        comment = comment.replace("%l", "");
        line = comment + "\n" + line;
        return (line);
    }
    return (line + comment);
}

/*
 *  ======== base ========
 *  Define the base/common GPIO property and method exports
 */
let base = {
    displayName: "GPIO",
    defaultInstanceName: "Board_GPIO",
    description: "General Purpose I/O Driver",
    longDescription: longDescription,
    pinmuxRequirements: pinmuxRequirements,
    validate: validate,

    config: gpioConfig,

    moduleStatic: {
        displayName: "GPIO Global",
        config: [
            intPriority,
            {
                name: "enableSort",
                displayName: "Optimize Callback Table Size",
                description: "When enabled, care must be taken at runtime to not enable interrupts on GPIOs without callbacks",
                hidden: false,
                default: false
            }
        ],
        getPinNum: getPinNum,
        getPinName: getPinName,
        getFullPinName: getFullPinName,
        getAttrs: getAttrs,
        collectCallbacks: collectCallbacks,
        addComment: addComment
    },

    /* common sort for GPIO tables to minimize GPIO ISR table size */
    sort: sort,
    filterHardware: filterHardware,
    onHardwareChanged: onHardwareChanged
};

/* get the family-specific GPIO module */
let deviceGPIO = system.getScript("/ti/drivers/gpio/GPIO" + family);

/* extend our common exports to include the family-specific content */
exports = deviceGPIO.extend(base);

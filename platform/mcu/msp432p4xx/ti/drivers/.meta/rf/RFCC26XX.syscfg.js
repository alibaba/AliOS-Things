/*
 *  CC26XX RF module
 *
 *  Device-specific RF configuration support
 */
"use strict";
/* global exports */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* $super is used to call generic module's methods */
let $super;

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "RF Hwi priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "RF Swi priority";

let globalCallbackLongDesc = "The RF driver provides an interface "
    + "to register a global, client independent callback.  This callback "
    + "is typically used to control board related configurations, such "
    + "as antenna switches.  If the board has antenna switches, a "
    + "default callback (rfDriverCallback) is provided.  By setting "
    + "globalCallback to NULL, the default callback, if it exists, will "
    + "be registered.";

let initFxnLongDescription = "In some cases, the board may require a "
    + "function to be called from Board_init() to initialize antenna pins."
    + "If needed, set this function to the name of a user provided "
    + "initialization function.  If this function is NULL, and the "
    + "board has a hardware RF component, a default function "
    + "(initAntennaSwitch()) will be called.  The default function "
    + "may be empty, depending on the board.";


/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base PIN1 configuration
 */
let devSpecific = {

    moduleStatic: {
        displayName: "RF Global",
        config: [
            intPriority,
            swiPriority,
            {
                name: "xoscNeeded",
                displayName: "Keep XOSC Dependency While in Standby",
                default: true
            },
            {
                name: "globalCallback",
                displayName: "Global Callback Function",
                longDescription: globalCallbackLongDesc,
                default: "NULL"
            },
            {
                name: "globalEventMask",
                displayName: "Global Event Mask",
                minSelections: 0,
                default: [],
                options: [
                    { name: "RF_GlobalEventRadioSetup" },
                    { name: "RF_GlobalEventRadioPowerDown" }
                ]
            }
        ],

        /* Aarray to hold RF antenna pins, if any */
        devicePins : []
    },

    /*
     *  Work-around for PMUX-1143: singleton modules don't support
     *  filterHardware and pinmuxRequirements.
     */
    config: [
        {
            name: "dummy",
            displayName: "dummy",
            default: "",
            hidden: true
        }
    ],

    /* Work-around for PMUX-1143 */
    maxInstances : 1,

    /* bring in Power module */
    modules: Common.autoForcePowerModule,

    moduleInstances: moduleInstances,
    filterHardware: filterHardware,
    onHardwareChanged: onHardwareChanged,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/rf/RFCC26XX.Board.c.xdt",
        boardh : "/ti/drivers/rf/RFCC26XX.Board.h.xdt"
    }
};

/*
 *  ======== onHardwareChange ========
 */
function onHardwareChanged(inst, ui)
{
    let RF = system.getScript("/ti/drivers/RF");

    if (inst.$hardware) {
        let component = inst.$hardware;

        for (let sig in component.signals) {
            /* Build up array of pins */
            let sigObj = component.signals[sig];
            /* Use the IOID name of the pin, e.g., "IOID_30"  */
            let io = sigObj.devicePin.description.replace("DIO", "IOID");
            let pin = {name: sigObj.name, ioid: io};

            /* Save the pins for generating defines in Board.h */
            RF.moduleStatic.devicePins.push(pin);
        }

        inst.$module.$static.globalCallback = "rfDriverCallback";
        inst.$module.$static.globalEventMask = [
                "RF_GlobalEventRadioSetup",
                "RF_GlobalEventRadioPowerDown"];
    }
    else {
        RF.moduleStatic.devicePins = [];
        inst.$module.$static.globalCallback = "NULL";
        inst.$module.$static.globalEventMask = [];
    }
}

/*
 *  ======== filterHardware ========
 *  component - a hardware componenet
 *
 *  Use this function to get the pins used by the RF antenna.
 */
function filterHardware(component)
{
    /*
     *  Check for an "RF" component in the board file.  If there is an
     *  RF component, collect the pins so we can pass them to the PIN
     *  module.
     */
    if (Common.typeMatches(component.type, ["RF"])) {
        let RF = system.getScript("/ti/drivers/RF");
        return (true);
    }
    return (false);
}

/*
 *  ======== moduleInstances ========
 *  returns a PIN instance
 */
function moduleInstances(inst)
{
    let pinInstance = new Array();

    let RF = system.getScript("/ti/drivers/RF");

    if (inst.$hardware) {
        let component = inst.$hardware;
        let i = 0;

        for (let sig in component.signals) {
            let sigObj = component.signals[sig];
            /* Use the IOID name of the pin, e.g., "IOID_30"  */
            let io = sigObj.devicePin.description.replace("DIO", "IOID");

            pinInstance.push({
                    name: "pinInstance" + i,
                    displayName: "PIN Configuration While Pin is Not In Use",
                    moduleName: "/ti/drivers/PIN",
                    collapsed: true,
                    args: {
                        $name: "Board_RF_" + sigObj.name,
                        ioid: io,
                        mode: "Output",
                        outputStrength: "Maximum",
                        outputState: "Low",
                        outputType: "Push/Pull"
                    }
            });
            i += 1;
        }
    }

    return (pinInstance);
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic PIN module to
 *  allow us to augment/override as needed for the CC26XX
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by generic PIN1 module */
    extend: extend
};

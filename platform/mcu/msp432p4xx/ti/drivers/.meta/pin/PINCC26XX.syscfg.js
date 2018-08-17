/*
 *  CC26XX PIN module
 *
 *  Device-specific PIN configuration support
 */
"use strict";
/*global exports*/

/* get Common /ti/drivers utility functions */
let Common     = system.getScript("/ti/drivers/Common.js");

/* $super is used to call generic module's methods */
let $super;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base PIN configuration
 */
let devSpecific = {
    config: [
        {
            name: "signalName",
            displayName: "Hardware Signal Name",
            hidden: true,
            default: ""
        },
        {
            name: "parentMod",
            displayName: "Parent Module",
            hidden: true,
            default: ""
        },
        {
            name: "parentInstanceName",
            displayName: "Parent Instance Name",
            hidden: true,
            readOnly: true,
            default: ""
        },
        {
            name: "parentInstanceId",
            displayName: "Parent Instance Index",
            hidden: true,
            default: 0
        },
        {
            name: "parentInterfaceName",
            displayName: "Parent Interface Name",
            hidden: true,
            default: ""
        },
        {
            name: "parentSignalName",
            displayName: "Parent Signal Name",
            hidden: true,
            onChange: updateConfigs,
            default: ""
        },
        {
            name: "ioid",
            displayName: "IOID",
            hidden: true,
            onChange: updateConfigs,
            default: ""
        }
    ],

    /* override generic requirements with device-specific reqs */
    pinmuxRequirements: pinmuxRequirements,

    filterHardware: filterHardware,

    /* bring in Power module */
    modules: Common.autoForcePowerModule,

    /* helper functions */
    getIOID: getIOID,
    getDIO: getDIO,

    /* override device-specific templates */
    templates:
    {
        boardc : "/ti/drivers/pin/PINCC26XX.Board.c.xdt",
        board_initc : "/ti/drivers/pin/PINCC26XX.Board_init.c.xdt",
        boardh : "/ti/drivers/pin/PIN.Board.h.xdt"
    },

    /* ensure PIN is initialized before other modules */
    initPriority: 0, /* low numbers are called earlier, undefined last */

    /* override generic validation with ours */
    validate: validate
};

/*
 *  ======== updateConfigs ========
 *  Adjust UI properties of configs based on current config settings
 */
function updateConfigs(inst, ui)
{
    ui.$name.hidden = false;
    if (inst.ioid != "") {
        ui.ioid.hidden = false;
    }
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - PIN instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*
 *  ======== getIOID ========
 *  Returns the IOID for this PIN instance
 */
function getIOID(inst)
{
    if (inst.ioid != "") return (inst.ioid);

    if ((inst.parentMod == "") || (inst.$hardware)) {
        return ("IOID_" + inst.pinPin.$solution.devicePinName.substring(4));
    }
    else {
        let parentInterface;
        let mod = system.getScript(inst.parentMod);
        let parentInstance = inst.$ownedBy;
        if (inst.parentInterfaceName) {
            parentInterface = parentInstance[inst.parentInterfaceName];
        }
        else {
            parentInterface = parentInstance;  /* GPIO has no interface */
        }
        if (parentInterface[inst.parentSignalName].$solution) {
            return ("IOID_" + parentInterface[inst.parentSignalName].$solution.devicePinName.substring(4));
        }
        else {
            return ("");
        }
    }
}

/*
 *  ======== getDIO ========
 *  Returns the DIO for this PIN instance
 */
function getDIO(inst)
{
     return (getIOID(inst).replace("IOID_","DIO"));
}

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    if ((inst.parentMod != "") || (inst.ioid != "")) {
        return ([]);
    }

    /*
     * name is the name of a property that will be added to inst;
     * this property's value is an object returned by the
     * pinmux solver
     */
    let pin = {
        name: "pinPin",
        displayName: "Pin",
        interfaceName: "GPIO",    /* pmux interface name (not module name!!) */
        signalTypes: ["DIN", "DOUT"]
    };

    return ([pin]);
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

    /* concatenate device-specific configs */
    devSpecific.config = base.config.concat(base.pinConfig).concat(devSpecific.config);

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by generic PIN module */
    extend: extend
};
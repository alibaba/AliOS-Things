/*!
 *  CC26XX GPIO module
 *
 *  Device-specific GPIO configuration support
 */
"use strict";
/*global exports*/

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base GPIO configuration
 */
let devSpecific = {
    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* PIN instances */
    moduleInstances: moduleInstances,

    /* override device-specific templates */
    templates:
    {
        boardc : "/ti/drivers/gpio/GPIOCC26XX.Board.c.xdt",
        boardh : "/ti/drivers/gpio/GPIO.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate
};

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 *  Called on instantiation and every config change.
 */
function pinmuxRequirements(inst)
{
    let result = $super.pinmuxRequirements ? $super.pinmuxRequirements(inst) : [];
    return (result);
}

/*
 *  ======== moduleInstances ========
 *  returns a PIN instance
 */
function moduleInstances(inst)
{
    if ((inst.mode == "Dynamic") || (inst.mode == "Empty")) return [];

    let pinInstance = new Array();

    pinInstance.push(
        {
            name: "pinInstance",
            displayName: "PIN Configuration While Pin is Not In Use",
            moduleName: "/ti/drivers/PIN",
            collapsed: true,
            args: { parentMod: "/ti/drivers/GPIO",
                    parentInstanceName: inst.$name,
                    parentSignalName: "gpioPin",
                    mode: inst.mode,
                    outputState: inst.outputState,
                    pull: inst.pull }
        }
    );

    return (pinInstance);
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
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*!
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic GPIO module to
 *  allow us to augment/override as needed for the CC26XX
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/**
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by generic GPIO module */
    extend: extend
};

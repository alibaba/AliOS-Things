/*!
 *  MSP432 GPIO module
 *
 *  Device-specific GPIO configuration support
 */
"use strict";
/*global exports*/

let Common = system.getScript("/ti/drivers/Common.js");

/* $super is used to call generic module's methods */
let $super;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base GPIO configuration
 */
let devSpecific = {
    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates:
    {
        boardc : "/ti/drivers/gpio/GPIOMSP432.Board.c.xdt",
        boardh : "/ti/drivers/gpio/GPIO.Board.h.xdt"
    },

    modules : Common.autoForcePowerModule,

    /* override generic validation with ours */
    validate: validate
};

/*
 *  ======== resourcesWithoutInterruptsFilter ========
 *  Only ports 1-6 support interrupts.
 */
function resourcesWithoutInterruptsFilter (devicePin, peripheralPin)
{
    let pName = peripheralPin.peripheralName;
    let pNum = pName.substring(1, pName.indexOf("."));

    /* only ports 1-6 support interrupts */
    if (pNum < 7) {
        return (true);
    }
    else {
        return (false);
    }
}

/*
 *  ======== noFilter ========
 *  All ports/resources are available if interrupts are not required.
 */
function noFilter (devicePin, peripheralPin)
{
    return (true);
}

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 *  Called on instantiation and every config change.
 */
function pinmuxRequirements(inst)
{
    let result = $super.pinmuxRequirements ? $super.pinmuxRequirements(inst) : [];

    /*
     * Install the appropriate GPIO filter
     * Unless the filter function changes, it is called only once
     * for every possible pin. The following logic forces a re-evaluation
     * of the set of valid resources whenever the state of 'interrupt'
     * changes.
     */
    if (result[0] != null) {
        result[0].filter = inst.interrupt != "None"
            ? resourcesWithoutInterruptsFilter : noFilter;
    }

    return (result);
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
 *  allow us to augment/override as needed for the MSP432
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

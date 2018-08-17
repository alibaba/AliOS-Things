/*!
 *  CC32XX GPIO module
 *
 *  Device-specific GPIO configuration support
 */
"use strict";
/*global exports*/

/* $super is used to call generic module's methods */
let $super;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base GPIO configuration
 */
let devSpecific = {
    maxInstances: 27,

    /* override device-specific templates */
    templates:
    {
        boardc : "/ti/drivers/gpio/GPIOCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/gpio/GPIO.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate,

    modules: modules
};

/**
 *  ======== modules ========
 *  Return all used modules
 *
 *  @param inst - ???
 *  @returns    - array of modules used in the implementation
 */
function modules(inst)
{
    let result = $super.modules ? $super.modules() : [];

    return result.concat([
        /* name = name of property injected int each instance (Blech!!) */
        {name : "Power", moduleName: "/ti/drivers/Power"}
    ]);
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
 *  allow us to augment/override as needed for the CC32XX
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

/*
 *  ======== CC32XX I2C ========
 *
 *  Device-specific I2C configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic I2C module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* Common interrupt priority element added to GPIO static configs */
let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Hardware Interrupt Priority";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base I2C configuration
 */
let devSpecific = {
    maxInstances: 1,

    config : [
        intPriority
    ],

    /* override with device-specific templates */
    templates: {
        boardc : "/ti/drivers/i2c/I2CCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/i2c/I2C.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate
};

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic I2C module to
 *  allow us to augment/override as needed for the CC32XX
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* concatenate device-specific configs */
    devSpecific.config = base.config.concat(devSpecific.config);

    return (Object.assign({}, base, devSpecific));
}

/*
 *  ======== validate ========
 *  Perform CC32XX-specific validation
 *
 *  @param inst       - I2CCC32XX instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }

    /* TODO: add device-specific validation here (if any) */
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base I2C module */
    extend: extend
};

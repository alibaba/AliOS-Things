/*
 *  ======== MSP432 I2CSlave ========
 *
 *  Device-specific I2CSlave configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base I2CSlave configuration
 */
let devSpecific = {
    config: [
        {
            name        : "slaveAddr",
            displayName : "Slave Address",
            default     : "0x20"
        }
    ],

    layout: [
        "intPriority", "slaveAddr"
    ],

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/i2cslave/I2CSlaveMSP432.Board.c.xdt",
        boardh : "/ti/drivers/i2cslave/I2CSlaveMSP432.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate
};

/**
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic I2CSlave module to
 *  allow us to augment/override as needed for the MSP432
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* "inherit" the base properties */
    for (let p in base) {
        if (devSpecific[p] == null) {
            devSpecific[p] = base[p];
        }
    }

    /* concatenate device-specific configs */
    devSpecific.config = base.config.concat(
        Common.newIntPri()).concat(devSpecific.config);

    return (devSpecific);
}

/*
 *  ======== validate ========
 *  Perform MSP432-specific validation
 *
 *  @param inst       - I2CSlaveMSP432 instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }

    /* TODO: add device-specific validation here (if any) */
}

/**
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base I2CSlave module */
    extend: extend
};

/*!
 *  CC32XX Capture module
 *
 *  Device-specific Capture configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Capture configuration
 */
let devSpecific = {
    config: [],

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/capture/CaptureCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/capture/Capture.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate
};

/**
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let timer = {
        name: "timer",
        displayName: "Timer",
        interfaceName: "Timer",
        resources: [
            {
                name: "capturePin",
                displayName: "Capture Input",
                interfaceNames: [
                    "GT_CCP00", "GT_CCP01"
                ]
            }
        ]
    };

    return ([timer]);
}

/**
 *  ======== validate ========
 *  Perform CC32XX-specific validation
 *
 *  @param inst       - CaptureMSP432 instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/**
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic Capture module to
 *  allow us to augment/override as needed for the MSP432
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* concatenate device-specific configs */
    devSpecific.config = base.config.concat(
        Common.newIntPri());

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/**
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base Capture module */
    extend: extend
};

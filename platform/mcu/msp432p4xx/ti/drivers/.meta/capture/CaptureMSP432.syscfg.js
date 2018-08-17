/*!
 *  MSP432 Capture module
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
    config: [
        {
            name        : "clockSource",
            displayName : "Clock Source",
            default     : "SMCLK",
            options     : [
                { name: "ACLK" },
                { name: "SMCLK" },
                { name: "EXTERNAL TXCLK" },
                { name: "INVERTED EXTERNAL TXCLK" }
            ]
        },
        {
            name        : "clockDivider",
            displayName : "Clock Divider",
            default     : "2",
            options     : [
                { name: "1" }, { name: "2" }, { name: "3" }, { name: "4" },
                { name: "5" }, { name: "6" }, { name: "7" }, { name: "8" },
                { name: "10" }, { name: "12" }, { name: "14" }, { name: "16" },
                { name: "20" }, { name: "24" }, { name: "28" }, { name: "32" },
                { name: "40" }, { name: "48" }, { name: "56" }, { name: "64" }
            ]
        }
    ],

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/capture/CaptureMSP432.Board.c.xdt",
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
        interfaceName: "TIMER_A",

        /*
         *  Filter out Timer_A0 for now.  It may be used by BIOS.
         *  TODO: Figure out a way to add comments to timers displayed in
         *  drop-down menu of pinmux tool.  Would also be nice to re-order
         *  them so Timer_A0 is last in the list.  See PMUX-1092
         */
        filter: function (peripheral) {
            return (!(peripheral.name.match(/A0/)));
        },

        resources: [
            {
                name: "capturePin",
                displayName: "Capture Input",
                interfaceNames: [
                    "CCI0A", "CCI1A", "CCI2A", "CCI3A", "CCI4A"
                ]
            }
        ]
    };

    return ([timer]);
}

/**
 *  ======== validate ========
 *  Perform MSP432-specific validation
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
        Common.newIntPri()).concat(devSpecific.config);

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

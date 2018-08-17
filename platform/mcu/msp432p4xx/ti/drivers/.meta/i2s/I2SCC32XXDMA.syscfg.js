/*!
 *  ======== CC32XX I2S Configuration ========
 */
"use strict";
/* global exports, system */

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* Common interrupt priority element added to I2S static configs */
let intPriority = Common.newIntPri()[0];
intPriority.displayName = "I2S peripheral interrupt priority";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base I2S configuration
 */
let devSpecific = {
    config:
    [
        intPriority
    ],

    maxInstances: 1,

    layout: [
        "intPriority", "clockSource"
    ],

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/i2s/I2SCC32XXDMA.Board.c.xdt",
        boardh : "/ti/drivers/i2s/I2SCC32XXDMA.Board.h.xdt"
    },

    /* bring in DMA and Power modules */
    modules: Common.autoForcePowerAndDMAModules,

    /* override hardware filter */
    filterHardware: filterHardware,

    /* override generic validation with ours */
    validate: validate
};

/*!
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let i2s = {
        /* name is the name of a property that will be added to inst;
         * this property's value is an object returned by the
         * pinmux solver
         */
        name: "i2sData",

        interfaceName: "McASP",

        /* request specific signals for I2S */
        resources: [
            {
                name: "clkPin",
                displayName: "ACLK",
                interfaceNames: ["McACLK"],
            },
            {
                name: "clkxPin",
                displayName: "ACLKX",
                interfaceNames: ["McACLKX"],
            },
            {
                name: "xr0Pin",
                displayName: "AXR0",
                interfaceNames: ["McAXR0"],
            },
            {
                name: "xr1Pin",
                displayName: "AXR1",
                interfaceNames: ["McAXR1"],
            },
            {
                name: "fsxPin",
                displayName: "AFSX",
                interfaceNames: ["McAFSX"],
            },
            {
                name: "dmaTxChannel",
                displayName: "I2S (TX) DMA CHANNEL",
                interfaceNames: ["DMA_TX"]
            },
            {
                name: "dmaRxChannel",
                displayName: "I2S (RX) DMA CHANNEL",
                interfaceNames: ["DMA_RX"]
            }
        ]
    };

    i2s.signalTypes = {
        clkPin:  ["I2S_ACLK"],
        clkxPin: ["I2S_ACLKX"],
        xr0Pin:  ["I2S_AXR0"],
        xr1Pin:  ["I2S_AXR1"],
        fsxPin:  ["I2S_AFSX"]
    };

    return ([i2s]);
}

/*!
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with I2S
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinmuxRequirement object if I2S is supported.
 */
function filterHardware(component)
{
    let aclk, aclkx, afsx, axr0, axr1;

    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["I2S_ACLK"])) aclk = sig;
        if (Common.typeMatches(type, ["I2S_ACLKX"])) aclkx = sig;
        if (Common.typeMatches(type, ["I2S_AFSX"])) afsx = sig;
        if (Common.typeMatches(type, ["I2S_AXR0"])) axr0 = sig;
        if (Common.typeMatches(type, ["I2S_AXR1"])) axr1 = sig;
    }

    if (aclk && aclkx && afsx && axr0 && axr1) {
        return ({
            i2sData: {
                McACLK: [aclk],
                McACLKX: [aclkx],
                McAFSX: [afsx],
                McAXR0: [axr0],
                McAXR1: [axr1]
            }
        });
    }

    return (null);
}

/*!
 *  ======== validate ========
 *  Perform CC32XX-specific validation
 *
 *  @param inst       - I2SCC32XX instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }

    /* TODO: add device-specific validation here (if any) */
}

/*!
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic I2S module to
 *  allow us to augment/override as needed for the CC32XX
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    return (Object.assign({}, base, devSpecific));
}

/*!
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base I2S module */
    extend: extend
};

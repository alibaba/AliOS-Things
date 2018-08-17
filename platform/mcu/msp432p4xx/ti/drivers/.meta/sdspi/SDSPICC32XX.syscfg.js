/*!
 *  CC32XX SPI module
 *
 *  Device-specific SPI configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base SPI configuration
 */
let devSpecific = {

    config: [],

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/sdspi/SDSPICC32XX.Board.c.xdt",
        boardh : "/ti/drivers/sdspi/SDSPI.Board.h.xdt"
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
    /* needs a 3 pin SPI interface */
    let spi = {
        /* name is the name of a property that will be added to inst;
         * this property's value is an object returned by the
         * pinmux solver
         */
        name: "spi",
        displayName: "SPI Peripheral",

        /* issue: "SPI" allocates STE _and_ a GPIO for chip-select */
        interfaceName: "SPI",

        /* request specific signals for SPI */
        resources: [
            {
                name: "clkPin",
                displayName: "SPI Clock",
                interfaceNames: ["CLK"]
            },
            {
                name: "somiPin",
                displayName: "SPI MISO",
                interfaceNames: ["DIN"]
            },
            {
                name: "simoPin",
                displayName: "SPI MOSI",
                interfaceNames: ["DOUT"]
            }
        ]
    };

    /* needs one GPIO pin for chip select */
    let gpio = {
        name: "gpioPin",
        displayName: "SPI Chip Select GPIO",
        interfaceName: "GPIO"
    };

    return ([spi, gpio]);
}

/**
 *  ======== validate ========
 *  Perform CC32XX-specific validation
 *
 *  @param inst       - SDSPICC32XX instance to be validated
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
 *  This function is invoked by the generic SPI module to
 *  allow us to augment/override as needed for the CC32XX
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* concatenate device-specific configs */
    devSpecific.config = base.config.concat(devSpecific.config);

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/**
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base SPI module */
    extend: extend
};

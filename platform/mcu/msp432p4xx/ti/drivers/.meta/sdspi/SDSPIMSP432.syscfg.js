/*!
 *  MSP432 SPI module
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
        boardc : "/ti/drivers/sdspi/SDSPIMSP432.Board.c.xdt",
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
                interfaceNames: ["SOMI"]
            },
            {
                name: "simoPin",
                displayName: "SPI MOSI",
                interfaceNames: ["SIMO"]
            }
        ]
    };

    spi.signalTypes = {
        clkPin:  ["SPI_CLK"],
        simoPin: ["SPI_SIMO"],
        somiPin: ["SPI_SOMI"]
    };

    /* needs one GPIO pin for chip select */
    let gpio = {
        name: "gpioPin",
        displayName: "SPI Chip Select GPIO",
        interfaceName: "GPIO",
        signalTypes: ["DOUT", "SPI_STE"]
    };

    return ([spi, gpio]);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with SPI
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinmuxRequirement object if SPI is supported.
 */
function filterHardware(component)
{
    let clk, simo, somi;

    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["SPI_CLK"])) clk = sig;
        if (Common.typeMatches(type, ["SPI_SIMO"])) simo = sig;
        if (Common.typeMatches(type, ["SPI_SOMI"])) somi = sig;
    }

    if (clk && simo && somi) {
        return (true);
    }

    return (false);
}

/**
 *  ======== validate ========
 *  Perform MSP432-specific validation
 *
 *  @param inst       - SPIMSP432 instance to be validated
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
 *  allow us to augment/override as needed for the MSP432
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

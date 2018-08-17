/*!
 *  MSP432 SPI module
 *
 *  Device-specific SPI configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super = {};

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let intPriority = Common.newIntPri()[0];
intPriority.name = "intPriority";
intPriority.displayName = "SPI peripheral hardware interrupt priority";

/*!
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base SPI configuration
 */
let devSpecific = {
    config: [
        intPriority
    ],

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/spi/SPIMSP432.Board.c.xdt",
        boardh: "/ti/drivers/spi/SPI.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate,

    /* pull in Power and DMA modules */
    modules: Common.autoForcePowerAndDMAModules
};

/*!
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let spi = {
        /* name is the name of a property that will be added to inst;
         * this property's value is an object returned by the
         * pinmux solver
         */
        name: "spi",
        displayName: "SPI Peripheral",

        /* issue: "SPI" allocates STE _and_ a GPIO for chip-select */
        interfaceName: "SPI",
        canShareWith: "SPI",

        /* request specific signals for SPI */
        resources: [
            {
                name: "clkPin",
                displayName: "SPI Clock",
                interfaceNames: ["CLK"]
            },
            {
                name: "dmaTxChannel",
                displayName: "SPI MOSI (TX) DMA CHANNEL",
                interfaceNames: ["DMA_TX"]
            },
            {
                name: "dmaRxChannel",
                displayName: "SPI MISO (RX) DMA CHANNEL",
                interfaceNames: ["DMA_RX"]
            },
            {
                name: "dmaIntNum",
                displayName: "SPI DMA INTERRUPT NUMBER",
                interfaceNames: ["DMA_INT"]
            }
        ]
    };

    /* add STE pin if one of the four pin modes is selected */
    if (inst.mode != "Three Pin") {
        spi.resources.push({
                    name: "csPin",
                    displayName: "SPI STE",
                    interfaceNames: ["STE"]
                });
    }

    if (inst.duplex == "full") {
        spi.resources.push({
            name: "simoPin",
            displayName: "SPI MOSI",
            interfaceNames: ["SIMO"]});
        spi.resources.push({
            name: "somiPin",
            displayName: "SPI MISO",
            interfaceNames: ["SOMI"]});
    } else if (inst.duplex == "txOnly") {
        spi.resources.push({
            name: "simoPin",
            displayName: "SPI MOSI",
            interfaceNames: ["SIMO"]});
    } else if (inst.duplex == "rxOnly") {
        spi.resources.push({
            name: "somiPin",
            displayName: "SPI MISO",
            interfaceNames: ["SOMI"]});
    }

    spi.signalTypes = {
        clkPin:  ["SPI_CLK"],
        simoPin: ["SPI_SIMO"],
        somiPin: ["SPI_SOMI"],
        csPin:  ["DOUT", "SPI_STE"]
    };

    return ([spi]);
}

/*!
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

/*!
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

    /* WARNING: if duplex config name changes this throws an exception */
    devSpecific.config.find(index => index.name === "duplex").hidden = false;

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/*!
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base SPI module */
    extend: extend
};

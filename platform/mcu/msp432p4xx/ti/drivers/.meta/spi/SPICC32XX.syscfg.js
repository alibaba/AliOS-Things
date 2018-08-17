/*!
 *  CC32XX SPI module
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

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base SPI configuration
 */
let devSpecific = {
    config: [
        {
            name: "tMode",
            displayName: "Turbo Mode",
            default: "Enabled",
            options: [
                { name: "Enabled" },
                { name: "Disabled" }
            ]
        },
        intPriority
    ],

    moduleStatic: { /* issue: this is needed to allow this module to be added without any instances */
        displayName: "SPI Module Configuration",

        /* bring in DMA and Power modules */
        modules: Common.autoForcePowerAndDMAModules
    },

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/spi/SPICC32XX.Board.c.xdt",
        boardh: "/ti/drivers/spi/SPICC32XX.Board.h.xdt"
    },

    /* bring in DMA and Power modules */
    modules: Common.autoForcePowerAndDMAModules,

    /* override hardware filter */
    filterHardware: filterHardware,

    /* override generic validation with ours */
    validate: validate
};

/*
 *  ======== lspiFilter ========
 *  The LSPI SPI interface is hard coded to SPI instance 0.
 *  Remove LSPI from the list of SPI choices
 */
function lspiFilter(iface, peripheralPin)
{
    /* At this moment, only SPI0 (GSPI) is available */
    if (iface.name == "SPI0") return (true);

    return (false);
}

/**
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

        filter: lspiFilter,

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
            }
        ]
    };

    /* add CS pin if one of the four pin modes is selected */
    if (inst.mode != "Three Pin") {
        spi.resources.push({
            name: "csPin",
            displayName: "SPI CS",
            interfaceNames: ["CS"]
        });
    }

    spi.signalTypes = {
        clkPin:  ["SPI_CLK"],
        simoPin: ["SPI_SIMO"],
        somiPin: ["SPI_SOMI"],
        csPin:  ["DOUT", "SPI_STE"]
    };

    return ([spi]);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with SPI
 *
 *  param component - hardware object describing signals and
 *                    resources they're attached to
 *
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component)
{
    let clk, simo, somi, ste;

    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["SPI_CLK"])) clk = sig;
        if (Common.typeMatches(type, ["SPI_SIMO"])) simo = sig;
        if (Common.typeMatches(type, ["SPI_SOMI"])) somi = sig;
        if (Common.typeMatches(type, ["SPI_STE"])) ste = sig;
    }

    if (clk && simo && somi) {
        if (ste) {
            return ({
                spi: {
                    clkPin: [clk],
                    simoPin: [simo],
                    somiPin: [somi],
                    csPin:  [ste]
                }
            });
        }
        else {
            return ({
                spi: {
                    clkPin: [clk],
                    simoPin: [simo],
                    somiPin: [somi]
                }
            });
        }
    }

    return (null);
}

/**
 *  ======== validate ========
 *  Perform CC32XX-specific validation
 *
 *  @param inst       - SPICC32XX instance to be validated
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

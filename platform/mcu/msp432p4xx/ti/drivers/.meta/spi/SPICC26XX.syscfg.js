/*
 *  ======== SPICC26XX ========
 *  CC26XX SPI module
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

let swiPriority = Common.newSwiPri();
swiPriority.name = "swiPriority";
swiPriority.displayName = "SPI peripheral software interrupt priority";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base SPI configuration
 */
let devSpecific = {

    maxInstances : 2,

    config: [
        intPriority,
        swiPriority
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
        boardc: "/ti/drivers/spi/SPICC26XX.Board.c.xdt",
        boardh: "/ti/drivers/spi/SPI.Board.h.xdt"
    },

    /* bring in DMA and Power modules */
    modules: Common.autoForcePowerAndDMAModules,

    /* PIN instances */
    moduleInstances: moduleInstances,

    /* override hardware filter */
    filterHardware: filterHardware,

    /* override generic validation with ours */
    validate: validate
};

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
        interfaceName: "SSI",

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
                interfaceNames: ["RX"]
            },
            {
                name: "simoPin",
                displayName: "SPI MOSI",
                interfaceNames: ["TX"]
            }
        ]
    };

    /* add CS pin if one of the four pin modes is selected */
    if (inst.mode != "Three Pin") {
        spi.resources.push({
                name: "csPin",
                displayName: "SPI CS",
                interfaceNames: ["FSS"]
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
        return (true);
    }

    return (false);
}

/*
 *  ======== moduleInstances ========
 *  returns PIN instances
 */
function moduleInstances(inst)
{
    let pinInstances = new Array();

    pinInstances.push(
        {
            name: "clkPinInstance",
            displayName: "SPI Clock PIN Configuration While Pin is Not In Use",
            moduleName: "/ti/drivers/PIN",
            collapsed: true,
            args: { parentMod: "/ti/drivers/SPI",
                    parentInstanceName: inst.$name + " : Clock",
                    parentInterfaceName: "spi",
                    parentSignalName: "clkPin",
                    mode: "Output",
                    outputState: "Low",
                    pull: "None" }
        }
    );

    pinInstances.push(
        {
            name: "somiPinInstance",
            displayName: "SPI MISO PIN Configuration While Pin is Not In Use",
            moduleName: "/ti/drivers/PIN",
            collapsed: true,
            args: { parentMod: "/ti/drivers/SPI",
                    parentInstanceName: inst.$name + " : MISO",
                    parentInterfaceName: "spi",
                    parentSignalName: "somiPin",
                    mode: "Input",
                    pull: "None" }
        }
    );

    pinInstances.push(
        {
            name: "simoPinInstance",
            displayName: "SPI MOSI PIN Configuration While Pin is Not In Use",
            moduleName: "/ti/drivers/PIN",
            collapsed: true,
            args: { parentMod: "/ti/drivers/SPI",
                    parentInstanceName: inst.$name + " : MOSI",
                    parentInterfaceName: "spi",
                    parentSignalName: "simoPin",
                    mode: "Output",
                    outputState: "Low",
                    pull: "None" }
        }
    );

    if (inst.mode != "Three Pin") {
        pinInstances.push(
            {
                name: "csPinInstance",
                displayName: "SPI CS PIN Configuration While Pin is Not In Use",
                moduleName: "/ti/drivers/PIN",
                collapsed: true,
                args: { parentMod: "/ti/drivers/SPI",
                        parentInstanceName: inst.$name + " : CS",
                        parentInterfaceName: "spi",
                        parentSignalName: "csPin",
                        mode: "Output",
                        outputState: inst.mode.match("high") ? "Low" : "High",
                        pull: "None" }
            }
        );
    }

    return (pinInstances);
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

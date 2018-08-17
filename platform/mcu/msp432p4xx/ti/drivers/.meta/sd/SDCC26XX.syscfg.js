/*
 *  Device independent SDSPI module
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let devSpecific = {

    sharedModuleInstances: sharedModuleInstances,

    moduleInstances: moduleInstances,

    validate: validate,

    filterHardware: filterHardware,

    templates: {
        boardc: "/ti/drivers/sd/SDCC26XX.Board.c.xdt",
        boardh: "/ti/drivers/sd/SD.Board.h.xdt"
    }
}

/**
 *  ======== validate ========
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== sharedModuleInstances ========
 */
function sharedModuleInstances(inst)
{
    return ([{
        name: "spiInstance",
        displayName: "SPI",
        moduleName: "/ti/drivers/SPI",
        hardware: inst.$hardware
    }]);
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    return ([{
        name: "spiCsGpioInstance",
        displayName: "SDSPI Chip Select",
        moduleName: "/ti/drivers/GPIO",
        hardware: inst.$hardware,
        args: {
            comment: "%l    /* SDSPI Chip Select */",
            mode: "Output",
            outputType: "Standard",
            outputState:"High"
        }
    }]);
}

/*!
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic PWM module to
 *  allow us to augment/override as needed for the MSP432
 */
function extend(base)
{
    base.config = [];

    return (Object.assign({}, base, devSpecific));
}

/*
 *  ========= filterHardware ========
 */
function filterHardware(component) {
    /* Let SPI identify the component */
    let SPI = system.getScript("/ti/drivers/SPI");
    return (SPI.filterHardware(component));
}

exports = {
    extend: extend
};
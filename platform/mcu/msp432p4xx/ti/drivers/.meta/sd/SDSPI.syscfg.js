/*!
 *  Device independent SDSPI module
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/**
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - SPI instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== getSharedModuleInstances ========
 *  returns a shareable SPI instance used for Chip Select
 */
function getSharedModuleInstances(inst)
{
    return ([{
        name: "spiInstance",
        displayName: "SPI",
        moduleName: "/ti/drivers/SPI"
    }]);
}

/*
 *  ======== getModuleInstances ========
 *  returns a non-shareable GPIO instance used for Chip Select
 */
function getModuleInstances(inst)
{
    return ([{
        name: "spiCsGpioInstance",
        displayName: "SDSPI Chip Select",
        moduleName: "/ti/drivers/GPIO",
        args: {
            comment: "%l    /* SDSPI Chip Select */",
            mode: "Output",
            outputType: "Standard",
            outputState:"High"
        }
    }]);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with SDHost
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinmuxRequirement object if SDHost is supported.
 */
function filterHardware(component) {
    /* Let SPI identify the component */
    let SPI = system.getScript("/ti/drivers/SPI");
    return (SPI.filterHardware(component));
}

exports = {
    displayName: "SDSPI",
    description: "SD Card Serial Peripheral Interface (SDSPI) Driver",
    defaultInstanceName: "Board_SDSPI",
    sharedModuleInstances: getSharedModuleInstances,
    moduleInstances: getModuleInstances,
    validate: validate,
    filterHardware: filterHardware
};

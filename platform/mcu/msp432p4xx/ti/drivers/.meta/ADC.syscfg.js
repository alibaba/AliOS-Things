/**
 *  ======== ADC configuration support ========
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "ADC");

/* generic configuration parameters for ADC instances */
let adcConfig = []; /* nothing (yet) beyond generic driver configs */

/* Intro splash on GUI */
let longDescription = "The ADC driver operates as a simplified " +
    "ADC module with only single channel sampling support. " +
    "It also operates on blocking only mode which means users " +
    "have to wait the current sampling finished before starting " +
    "another sampling. The sampling channel needs to be specified " +
    "in the ADC_open() before calling ADC_convert().";

/**
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - ADC instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with ADC
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinmuxRequirement object if ADC is supported.
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        if (component.signals[sig].type == "AIN") {
            return (true);
        }
    }
    return (false);
}

/**
 *  ======== base ========
 *  Define the base/common ADC properties and methods
 */
let base = {
    displayName: "ADC",
    description: "Analog to Digital Conversion (ADC) Input Driver",
    longDescription: longDescription,
    defaultInstanceName: "Board_ADC",
    config: adcConfig,
    validate: validate,
    filterHardware: filterHardware
};

/* get family-specific ADC module */
let deviceADC = system.getScript("/ti/drivers/adc/ADC" + family);

/* extend our common exports to include the family-specific content */
exports = deviceADC.extend(base);

/*!
 *  ======== I2S configuration ========
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "I2S");

/* generic configuration parameters for I2S instances */
let i2sConfig = []; /* nothing (yet) beyond generic driver configs */

/* Intro splash on GUI */
let longDescription = "The I2S driver provides a simplified " +
    "application interface to access peripherals on an I2S bus.";

/*!
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - I2S instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*!
 *  ======== base ========
 *  Define the base/common I2S properties and methods
 */
let base = {
    displayName: "I2S",
    description: "Inter-Integrated Circuit (I2S) Bus Driver",
    longDescription: longDescription,
    defaultInstanceName: "Board_I2S",
    config: i2sConfig,
    validate: validate,
    busModule: true
};

/* get family-specific I2S module */
let deviceI2S = system.getScript("/ti/drivers/i2s/I2S" + family + "DMA");

/* extend our common exports to include the family-specific content */
exports = deviceI2S.extend(base);

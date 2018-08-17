/**
 *  ======== Capture configuration support ========
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "Capture");

/* generic configuration parameters for Capture instances */
let captureConfig = []; /* nothing (yet) beyond generic driver configs */

/* Intro splash on GUI */
let longDescription = "The capture driver serves as the main interface " +
    "for a typical RTOS application. Its purpose is to redirect the " +
    "capture APIs to device specific implementations which are " +
    "specified using a pointer to a Capture_FxnTable. The device " +
    "specific implementations are responsible for creating all the " +
    "RTOS specific primitives to allow for thead-safe operation. " +
    "The capture driver utilizes the general purpose timer hardware.";

/**
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - ADCBuf instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/**
 *  ======== base  ========
 *  Define the base/common Capture properties and methods
 */
let base = {
    displayName: "Capture",
    description: "Input Capture Driver",
    longDescription: longDescription,
    defaultInstanceName: "Board_CAPTURE",
    config: captureConfig,
    validate: validate
};

/* get family-specific Capture module */
let deviceCapture = system.getScript("/ti/drivers/capture/Capture" + family);

/* extend our common exports to include the family-specific content */
exports = deviceCapture.extend(base);

/**
 *  ======== ADCBuf configuration support ========
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "ADCBuf");

/* generic configuration parameters for ADCBuf instances */
let adcbufConfig = []; /* nothing (yet) beyond generic driver configs */

/* Intro splash on GUI */
let longDescription = "The ADCBuf driver in TI-RTOS samples an " +
    "analogue waveform at a specified frequency. The resulting " +
    "samples are transferred to a buffer provided by the application. " +
    "The driver can either take n samples once, or continuously " +
    "sample by double-buffering and providing a callback to process " +
    "each finished buffer.";

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
 *  ======== base ========
 *  Define the base/common ADCBuf properties and methods
 */
let base = {
    displayName: "ADCBuf",
    description: "Buffered Analog to Digital Conversion (ADCBuf) Input Driver",
    longDescription: longDescription,
    defaultInstanceName: "Board_ADCBUF",
    config: adcbufConfig,
    validate: validate
};

/* get family-specific ADCBuf module */
let deviceADCBuf = system.getScript("/ti/drivers/adcbuf/ADCBuf" + family);

/* extend our common exports to include the family-specific content */
exports = deviceADCBuf.extend(base);

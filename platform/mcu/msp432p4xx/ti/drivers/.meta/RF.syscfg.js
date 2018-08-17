/*!
 *  ======== RF configuration ========
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "RF");

/* Intro splash on GUI */
let longDescription =
    "The RF driver provides access to the radio core on the CC13xx/CC26xx device " +
    "family. It offers a high-level interface for command execution and to the " +
    "radio timer (RAT). The RF driver ensures the lowest possible power consumption " +
    "by providing automatic power management that is fully transparent for the " +
    "application.";

/*!
 *  ======== base ========
 *  Define the base/common RF properties and methods
 */
let base = {
    displayName: "RF",
    description: "RF Driver",
    longDescription: longDescription,
    defaultInstanceName: "Board_RF"
};

/* get the family-specific RF module */
let deviceRF = system.getScript("/ti/drivers/rf/RF" + family);

/* extend our common exports to include the family-specific content */
exports = deviceRF.extend(base);

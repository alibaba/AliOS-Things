/*!
 *  ======== EMAC configuration ========
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "EMAC");

/* Intro splash on GUI */
let longDescription = "The Ethernet (EMAC) driver is used by the " +
        "Networking stack(NDK).";

/*!
 *  ======== base ========
 *  Define the base EMAC properties and methods
 */
let base = {
    displayName         : "EMAC",
    description         : "Ethernet Driver configuration for NDK",
    defaultInstanceName : "Board_EMAC",
    longDescription     : longDescription,
    validate            : validate
};

/*!
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  @param inst       - EMAC instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
}

/* get family-specific EMAC module */
let devEMAC = system.getScript("/ti/drivers/emac/EMAC" + family);
exports = devEMAC.extend(base);

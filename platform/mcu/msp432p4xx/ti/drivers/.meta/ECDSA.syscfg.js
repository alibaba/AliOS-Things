/*!
 *  ======== ECDSA configuration ========
 *  Elliptic Curve Digital Signature Algorithm
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "ECDSA");

/* Intro splash on GUI */
let longDescription = "The Elliptic Curve Digital Signature Algorithm " +
    "(ECDSA) module provides APIs for generating and verifying digital " +
    "signatures.";

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Public Key Acceleration module interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "ECDSA interrupt Swi handler priority";

/*!
 *  ======== base ========
 *  Define the base ECDSA properties and methods
 */
let base = {
    displayName         : "ECDSA",
    description         : "Elliptic Curve Digital Signature Algorithm Module",
    longDescription     : longDescription,
    validate            : validate,
    defaultInstanceName : "Board_ECDSA",
    maxInstances        : 1,
    moduleStatic        : {
        config          : [
            intPriority,
            swiPriority
        ]
    }
};

/*!
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  @param inst       - ECDSA instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
}

/* get family-specific ECDH module */
let devECDSA = system.getScript("/ti/drivers/ecdsa/ECDSA" + family);
exports = devECDSA.extend(base);

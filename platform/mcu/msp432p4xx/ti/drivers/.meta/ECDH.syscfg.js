/*!
 *  ======== ECDH configuration ========
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "ECDH");

/* Intro splash on GUI */
let longDescription = "The Elliptic Curve Diffie-Hellman (ECDH) " +
    "module provides APIs for shared secret and symmetric key generation " +
    " between two parties, based on the Diffie-Hellman key exchange " +
    "protocol.";

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Public Key Acceleration module interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "ECDH interrupt Swi handler priority";

/*!
 *  ======== base ========
 *  Define the base ECDH properties and methods
 */
let base = {
    displayName         : "ECDH",
    description         : "Elliptic Curve Diffie-Hellman Key Generation Module",
    longDescription     : longDescription,
    validate            : validate,
    defaultInstanceName : "Board_ECDH",
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
 *  @param inst       - ECDH instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
}

/* get family-specific ECDH module */
let devECDH = system.getScript("/ti/drivers/ecdh/ECDH" + family);
exports = devECDH.extend(base);

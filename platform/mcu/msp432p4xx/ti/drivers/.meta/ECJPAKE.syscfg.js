/*!
 *  ======== ECJPAKE configuration ========
 *  Elliptic Curve Password Authenticated Key Exchange by Juggling.
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "ECDH");

/* Intro splash on GUI */
let longDescription = "Elliptic Curve Password Authenticated Key Exchange by " +
    "Juggling (EC-JPAKE) is a key agreement scheme that establishes a secure channel " +
    "over an insecure network. It only requires sharing a password offline and does " +
    "not require public key infrastructure or trusted third parties such as certificate " +
    "authorities.";

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Public Key Acceleration module interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "ECJPAKE interrupt Swi handler priority";

/*!
 *  ======== base ========
 *  Define the base ECJPAKE properties and methods
 */
let base = {
    displayName         : "ECJPAKE",
    description         : "Elliptic Curve Password Authenticated Key Exchange Module",
    longDescription     : longDescription,
    validate            : validate,
    defaultInstanceName : "Board_ECJPAKE",
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
 *  @param inst       - ECJPAKE instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
}

/* get family-specific ECDH module */
let devECJPAKE = system.getScript("/ti/drivers/ecjpake/ECJPAKE" + family);
exports = devECJPAKE.extend(base);

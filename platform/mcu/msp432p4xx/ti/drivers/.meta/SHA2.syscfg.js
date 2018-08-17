/*!
 *  ======== SHA2 configuration ========
 *  Secure Hash Algorithm
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "SHA2");

/* Intro splash on GUI */
let longDescription = "Secure Hash Algorithm 2 (SHA2).  This is a cryptographic " +
    "hashing algorithm that maps an input of arbitrary length to a fixed-length output " +
    "with negligible probability of a collision.";

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Crypto peripheral interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "Crypto interrupt Swi handler priority";

/*!
 *  ======== base ========
 *  Define the base SHA2 properties and methods
 */
let base = {
    displayName         : "SHA2",
    description         : "Secure Hash Algorithm (SHA2)",
    longDescription     : longDescription,
    validate            : validate,
    defaultInstanceName : "Board_SHA2",
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
 *  @param inst       - SHA2 instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
}

/* get family-specific SHA2 module */
let devSHA2 = system.getScript("/ti/drivers/sha2/SHA2" + family);
exports = devSHA2.extend(base);

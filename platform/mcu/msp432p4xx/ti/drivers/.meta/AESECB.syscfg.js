/*!
 *  ======== AESECB configuration ========
 *  Advanced Encryption Standard (AES) module, electronic code book mode.
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "AESECB");

/* Intro splash on GUI */
let longDescription = "The Electronic Code Book (ECB) mode of operation is a generic " +
    "encryption block cipher mode. It can be used with any block cipher. " +
    "AESECB encrypts or decrypts one or multiple blocks of plaintext or ciphertext " +
    "using the Advanced Encryption Standard (AES) block cipher.";

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Crypto peripheral interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "Crypto interrupt Swi handler priority";

/*!
 *  ======== base ========
 *  Define the base ECDH properties and methods
 */
let base = {
    displayName         : "AESECB",
    description         : "AES ECB mode ",
    longDescription     : longDescription,
    validate            : validate,
    defaultInstanceName : "Board_AESECB",
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
 *  @param inst       - AESECB instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
}

/* get family-specific AESECB module */
let devAESECB = system.getScript("/ti/drivers/aesecb/AESECB" + family);
exports = devAESECB.extend(base);

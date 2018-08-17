/*!
 *  ======== AESCCM configuration ========
 *  Advanced Encryption Standard (AES) module, counter (CTR) mode with cypher
 *  block chaining message authentication (CBC-MAC).
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "AESCCM");

/* Intro splash on GUI */
let longDescription = "AESCCM combines CBC-MAC with an AES block cipher in CTR mode " +
    "of operation.  This combination of block cipher modes enables CCM to encrypt " +
    "messages of any length and not only multiples of the block cipher block size.";

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Crypto peripheral interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "Crypto interrupt Swi handler priority";

/*!
 *  ======== base ========
 *  Define the base ECDH properties and methods
 */
let base = {
    displayName         : "AESCCM",
    description         : "AES CCM mode ",
    longDescription     : longDescription,
    validate            : validate,
    defaultInstanceName : "Board_AESCCM",
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
 *  @param inst       - AESCCM instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
}

/* get family-specific AEECCM module */
let devAESCCM = system.getScript("/ti/drivers/aesccm/AESCCM" + family);
exports = devAESCCM.extend(base);

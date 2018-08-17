/*!
 *  ======== Crypto configuration support ========
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let family = Common.device2Family(system.deviceData, "Crypto");

/* Intro splash on GUI */
let longDescription = "This should be overridden by device crypto module.";

/* The base exports for the Crypto module */
let base = {
    displayName         : "Crypto",
    description         : "Crypto Driver",
    longDescription     : longDescription,
    defaultInstanceName : "Board_Crypto",
    maxInstances        : 1
};

/* Device-specific Crypto module */
let devCrypto = system.getScript("/ti/drivers/crypto/Crypto" + family);
exports = devCrypto.extend(base);

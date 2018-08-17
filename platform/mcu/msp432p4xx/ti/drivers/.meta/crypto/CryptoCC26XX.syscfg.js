/*!
 *  ======== CC26XX Crypto ========
 *
 *  Device-specific Crypto configuration support
 */
"use strict";
/* global exports, system */

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* Common interrupt priority element added to Crypto static configs */
let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Crypto peripheral interrupt priority";

let longDescription =  "The CryptoCC26XX driver simplifies reading and " +
        "writing to the CryptoCC26XX peripheral on the board with multiple " +
        "modes of operation and performance.  These include blocking and " +
        "polling. A timeout can be configured in blocking mode.  The " +
        "driver supports encryption and decryption for both AES-ECB and " +
        "AES-CCM.";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Crypto configuration
 */
let devSpecific = {
    config:
    [
        intPriority
    ],

    longDescription : longDescription,

    templates : {
        boardc: "/ti/drivers/crypto/CryptoCC26XX.Board.c.xdt",
        boardh: "/ti/drivers/crypto/Crypto.Board.h.xdt"
    }
}

/*!
 * The base object is passed to this function and allows the device
 * specific object to incorporate properties from the base object.
 *
 * @param base - the exports from the base object
 *
 * @returns - a composite object of the base + device specific modules
 */
function extend(base) {
    return Object.assign({}, base, devSpecific);
}

/* Export device-specific extensions to base exports */
exports = { extend : extend };

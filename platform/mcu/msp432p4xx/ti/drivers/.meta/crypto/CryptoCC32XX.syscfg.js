/*!
 *  ======== CC32XX Crypto ========
 *
 *  Device-specific Crypto configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let longDescription = "The CryptoCC32XX module provides security methods " +
    "(AES and DES) for data encryption and decryption.";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Crypto configuration
 */
let devSpecific = {
    maxInstances: 1,
    longDescription : longDescription,

    templates : {
        boardc: "/ti/drivers/crypto/CryptoCC32XX.Board.c.xdt",
        boardh: "/ti/drivers/crypto/Crypto.Board.h.xdt"
    }
}

/**
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

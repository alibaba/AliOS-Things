/*!
 *  ======== CC26XX ECDSA ========
 *
 *  Device-specific ECDSA (Elliptic Curve Digital Signature Algorithm)
 *  configuration support
 */
"use strict";
/* global exports, system */

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* Common interrupt priority element added to ECDSA static configs */
let intPriority = Common.newIntPri()[0];
intPriority.displayName = "ECDSA peripheral interrupt priority";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base ECDSA configuration
 */
let devSpecific = {
    templates : {
        boardc: "/ti/drivers/ecdsa/ECDSACC26X2.Board.c.xdt",
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

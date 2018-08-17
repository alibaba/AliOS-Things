/*!
 *  ======== CC26XX ECJPAKE ========
 *
 *  Device-specific ECJPAKE (Elliptic Curve Password Authenticated Key
 *  Exchange by Juggling) configuration support
 */
"use strict";
/* global exports, system */

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* Common interrupt priority element added to ECJPAKE static configs */
let intPriority = Common.newIntPri()[0];
intPriority.displayName = "ECJPAKE peripheral interrupt priority";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base ECJPAKE configuration
 */
let devSpecific = {
    templates : {
        boardc: "/ti/drivers/ecjpake/ECJPAKECC26X2.Board.c.xdt",
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

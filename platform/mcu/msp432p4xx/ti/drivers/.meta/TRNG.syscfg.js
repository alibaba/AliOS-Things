/*!
 *  ======== TRNG configuration ========
 *  True Random Number Generator
 */
"use strict";
/* global exports, system */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "TRNG");

/* Intro splash on GUI */
let longDescription = "The True Random Number Generator (TRNG) module generates " +
    "numbers of variable lengths from a source of entropy. The output is suitable " +
    "for applications requiring cryptographically random numbers such as keying " +
    "material for private or symmetric keys.";

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "TRNG peripheral interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "TRNG interrupt Swi handler priority";

/*!
 *  ======== base ========
 *  Define the base TRNG properties and methods
 */
let base = {
    displayName         : "TRNG",
    description         : "True Random Number Generator (TRNG)",
    longDescription     : longDescription,
    validate            : validate,
    defaultInstanceName : "Board_TRNG",
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
 *  @param inst       - TRNG instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
}

/* get family-specific TRNG module */
let devTRNG = system.getScript("/ti/drivers/trng/TRNG" + family);
exports = devTRNG.extend(base);

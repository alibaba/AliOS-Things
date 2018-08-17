/*!
 *  PIN IOID config instances
 */
"use strict";
/*global exports, system*/

let PIN = system.getScript("/ti/drivers/PIN.syscfg.js");

/*
 *  ======== base ========
 *  Define the base/common PIN property and method exports
 */
let base = {
    displayName: "PIN Config",
    defaultInstanceName: "Board_PIN_IOID",
    config: PIN.pinConfig
};

exports = base;

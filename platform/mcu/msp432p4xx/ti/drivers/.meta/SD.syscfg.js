/**
 *  ======== SD configuration support ========
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let family = Common.device2Family(system.deviceData, "SD");

/* Intro splash on GUI */
let longDescription = "The SD driver provides a simplified " +
    "application interface to access peripherals on an SD bus.";

/* generic configuration parameters for SD instances */
let config = [
    {
        name: "useFATFS",
        displayName: "Use FatFS",
        default: false,
    }
];

/**
 *  ======== validate ========
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

let base = {
    displayName: "SD",
    description: "SD Driver",
    longDescription: longDescription,
    defaultInstanceName: "Board_SD",
    config: config,
    validate: validate
};

let deviceSD = system.getScript("/ti/drivers/sd/SD" + family);
exports = deviceSD.extend(base);
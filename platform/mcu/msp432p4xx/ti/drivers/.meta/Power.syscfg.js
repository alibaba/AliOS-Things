"use strict";
/* global exports, system */

let Common = system.getScript("/ti/drivers/Common.js");
let family = Common.device2Family(system.deviceData, "Power");

/* Intro splash on GUI */
let longDescription = "The Power manager facilitates the transition " +
    "of the MCU from active state to one of the sleep states and vice " +
    "versa. It provides drivers the ability to set and release dependencies " +
    "on hardware resources and keeps a reference count on each resource to " +
    "know when to enable or disable the peripheral clock to the resource. " +
    "It provides drivers the ability to register a callback function upon a " +
    "specific power event. In addition, drivers and apps can set or release " +
    "constraints to prevent the MCU from transitioning into a particular " +
    "sleep state.";

/* The base exports for the Power module */
let base = {
    displayName         : "Power",
    description         : "Power Driver",
    longDescription     : longDescription,
    defaultInstanceName : "Board_Power",
    maxInstances        : 1,
    initPriority        : -1 /* lower numbers init earlier */
};

/* Compose a device specific Power from this base Power definition and
 * export the device specific Power
 */
let devPower = system.getScript("/ti/drivers/power/Power" + family);
exports = devPower.extend(base);

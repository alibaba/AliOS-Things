/*!
 *  ======== MSP432E4 EMAC ========
 *
 *  Device-specific EMAC configuration support
 */
"use strict";
/* global exports, system */

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

/* Common interrupt priority element added to EMAC static configs */
let intPriority = Common.newIntPri()[0];
intPriority.displayName = "EMAC peripheral interrupt priority";

let longDescription = "The Ethernet (EMAC) driver is used by the " +
        "Networking stack(NDK).";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base EMAC configuration
 */
let devSpecific = {
    longDescription : longDescription,

    config: [
        intPriority,
        {
            name: "macAddress",
            displayName: "MAC Address",
            description: "By default, when the MAC address is " +
                    "ff:ff:ff:ff:ff:ff, the EMAC driver reads the MAC " +
                    "address that is stored in flash.  To override this " +
                    "behavior, manually set the MAC address you wish to " +
                    "use.  The format should be in the form " +
                    "xx:xx:xx:xx:xx:xx, for example, 01:1a:b6:02:c4:e5",
            default: "ff:ff:ff:ff:ff:ff"
        }
    ],

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,
    filterHardware: filterHardware,

    /* override device-specific templates */
    templates : {
        boardc: "/ti/drivers/emac/EMACMSP432E4.Board.c.xdt",
    },

    /* override generic validation with ours */
    validate: validate
}

/*!
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let emac = {
        /* name is the name of a property that will be added to inst;
         * this property's value is an object returned by the
         * pinmux solver
         */
        name: "emac",
        displayName: "EMAC Peripheral",
        interfaceName: "ETHERNET",

        /* request specific signals for EMAC */
        resources: [
            {
                name: "led0Pin",
                displayName: "Ethernet Link OK",
                interfaceNames: ["EN0LED0"]
            },
            {
                name: "led1Pin",
                displayName: "Ethernet TX/RX Activity",
                interfaceNames: ["EN0LED1"]
            }
        ],
        signalTypes: {
            led0Pin: ["EN0LED0"],
            led1Pin: ["EN0LED1"]
        }
    };

    return ([emac]);
}

/*!
 *  ======== filterHardware ========
 *  Control LED Pin usage.
 *
 *  @param component - a hardware componenet
 *
 *  @returns a mapping of component names to pin names
 *  @returns null if the component does not match this module
 */
function filterHardware(component) {
    return (Common.typeMatches(component.type, ["EthernetLED"]));
}

/*!
 *  ======== validate ========
 *  Perform MSP432E4-specific validation
 *
 *  @param inst       - EMACSP432E4 instance to be validated
 *  @param vo   - Issue reporting object
 */
function validate(inst, vo)
{
    if ($super.validate) {
        $super.validate(inst, vo);
    }
}

/*!
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic EMAC module to
 *  allow us to augment/override as needed for the MSP432E4
 *
 *  @param base - the exports from the base object
 *
 *  @returns - a composite object of the base + device specific modules
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/*!
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ENAC module */
    extend: extend
};

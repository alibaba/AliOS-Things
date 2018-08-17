"use strict";
/* global exports, system */

let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let isCName  = Common.isCName;
let $super;

/**
 * config
 *
 * These are the module wide configurations for the Power Module on MSP432E4
 */
let config = [
    /* The top level Enablers */
    {
        name        : "enablePolicy",
        displayName : "Enable Power Policy",
        description : "Enable the power policy to run when the CPU is idle.",
        default     : true
    },

    /* Policy function handling */
    {
        name        : "policyFxn",
        displayName : "Policy Function",
        default     : "PowerMSP432E4_sleepPolicy",
        description : "Power policy function called from the idle loop.",
        options     :
        [
            { name: "PowerMSP432E4_sleepPolicy" },
            { name: "PowerMSP432E4_deepSleepPolicy" },
            { name: "Custom" }
        ]
    },

    {
        name        : "policyFxnCustom",
        displayName : "Policy Custom Function",
        description : "User provided Power policy function. Usage not typical.",
        default     : "customPolicyFxn",
        hidden      : true
    }
];

/**
 * devSpecific
 *
 * The device specific Power module definitino
 */
let devSpecific = {
    displayName         : "Power",

    moduleStatic : {
        config          : config,
        validate        : validate
    },

    templates : {
        boardc     : "/ti/drivers/power/PowerMSP432E4.Board.c.xdt",
        board_initc: "/ti/drivers/power/PowerMSP432E4.Board_init.c.xdt"
    }
};

/**
 * Validate this module's configuration
 *
 * @param inst  - Power instance containing the config that changed
 * @param vo    - Issue reporting object
 *
 */
function validate(inst, vo)
{
}

/**
 * Provides a numeric comparison function for use in sort algorithms
 *
 * @param a         - The first  item to compare
 * @param b         - The second item to compare
 *
 * @returns integer - A signed value, 0 if items are equal, positive if a > b,
 *                    negative if a < b
 */
function numericCompare(a, b) 
{
    return a - b;
}

/**
 * The base object is passed to this function and allows the device
 * specific object to incorporate properties from the base object.
 *
 * @param base - the exports from the base object
 *
 * @returns - a composite object of the base + device specific modules
 */
function extend (base)
{
    $super = base;
    return Object.assign({}, base, devSpecific);
}

exports = { extend : extend };

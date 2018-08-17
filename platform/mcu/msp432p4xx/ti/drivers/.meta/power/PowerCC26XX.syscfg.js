/*!
 * ======== PowerCC26XX.js ========
 * CC26XX Power module
 *
 * Device-specific Power configuration support
 */

"use strict";
/* global exports, system */

let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let isCName  = Common.isCName;
let $super;

/* Array of power configurables that are for this device family */
let config = [
    {
        name        : "enablePolicy",
        displayName : "Enable Power Policy",
        description : "Enable the power policy to run when the CPU is idle.",
        onChange    : onChangeEnablePolicy,
        default     : true
    },

     /* Policy function handling */
    {
        name        : "policyFxn",
        displayName : "Policy Function",
        default     : "PowerCC26XX_standbyPolicy",
        onChange    : onChangePolicyFxn,
        description : "Power policy function called from the idle loop.",
        options     :
        [
            { name: "PowerCC26XX_standbyPolicy" },
            { name: "PowerCC26XX_doWFI" },
            { name: "Custom" }
        ]
    },

    {
        name        : "policyFxnCustom",
        displayName : "Policy Custom Function",
        description : "User provided Power policy function. Usage not typical.",
        default     : "customPolicyFxn",
        hidden      : true
    },

    {
        name        : "policyInitFxn",
        displayName : "Policy Init Function",
        default     : "Not Required",
        onChange    : onChangePolicyInitFxn,
        description : "The initialization function for the Power policy.",
        options     :
        [
            { name: "Not Required" },
            { name: "Custom" }
        ]
    },

    {
        name        : "policyInitFxnCustom",
        displayName : "Policy Init Custom Function",
        description : "User provided Power policy init function. " +
                      "Usage not typical.",
        default     : "customPolicyInitFxn",
        hidden      : true
    },

    {
        name        : "calibrateFxn",
        displayName : "Calibrate function",
        description : "Optional function. ",
        default     : "PowerCC26XX_calibrate",
        options     :
        [
            { name: "PowerCC26XX_calibrate" },
            { name: "PowerCC26XX_noCalibrate" },
        ]
    },

    {
        name        : "vddrRechargeMargin",
        displayName : "vddr Recharge Margin",
        default     : 0
    },

    {
        name        : "calibrateRCOSC_LF",
        displayName : "Calibrate RCOSC_LF",
        default     : true
    },

    {
        name        : "calibrateRCOSC_HF",
        displayName : "Calibrate RCOSC_HF",
        default     : true
    }
];

/*!
 * onChange callback function for the enablePolicy config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangeEnablePolicy(inst, ui)
{
    let subState = (!inst.enablePolicy);
    ui.policyInitFxn.hidden             = subState;
    ui.policyFxn.hidden                 = subState;
    onChangePolicyInitFxn(inst,ui);
    onChangePolicyFxn(inst,ui);
}

/*!
 * onChange callback function for the policyInitFxn config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangePolicyInitFxn(inst, ui)
{
    let subState = (inst.policyInitFxn !== 'Custom');
    ui.policyInitFxnCustom.hidden = subState;
}

/*!
 * onChange callback function for the policyFxn config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangePolicyFxn(inst, ui)
{
    let subState = (inst.policyFxn !== 'Custom');
    ui.policyFxnCustom.hidden = subState;
}

/* The device specific exports for the power module */
let devSpecific = {
    getClockFrequencies : getClockFrequencies,
    moduleStatic        : {
        config   : config,
        validate : validate
    },
    templates           : {
        boardc      : "/ti/drivers/power/PowerCC26XX.Board.c.xdt",
        board_initc : "/ti/drivers/power/PowerCC26XX.Board_init.c.xdt"
    }
};

/*!
 * Validate this module's configuration
 *
 * @param inst       - Power instance to be validated
 * @param validation - Issue reporting object
 *
 */
function validate(inst, vo)
{
    if (inst.enablePolicy) {
        if (inst.policyInitFxn === 'Custom') {
            if (!isCName(inst.policyInitFxnCustom)) {
                logError(vo, inst, "policyInitFxnCustom",
                    "Not a valid C identifier");
            }
            if (inst.policyInitFxnCustom === '') {
                logError(vo, inst, "policyInitFxnCustom",
                    "Must contain a valid function name if the " +
                    "Policy Init Function field == 'Custom'");
            }
        }
        if (inst.policyFxn === 'Custom') {
            if (!isCName(inst.policyFxnCustom)) {
                logError(vo, inst, "policyFxnCustom",
                    "is not a valid C identifier");
            }
            if (inst.policyFxnCustom === '') {
                logError(vo, inst, "policyFxnCustom",
                    "Must contain a valid function name if the " +
                    "Policy Function field == 'Custom'");
            }
        }
    }
}

/*!
 * Return the value of the CC26XX main CPU clock frequency
 *
 * @param clock       - The clock name to use.  Ignored in this implementation.
 *
 * @returns frequency - scalar frequency of CPU during RUN state.
 */
function getClockFrequencies(clock)
{
    return [ 48000000 ];
}

/*!
 * The base object is passed to this function and allows the device
 * specific object to incorporate properties from the base object.
 *
 * @param base - the exports from the base object
 *
 * @returns - a composite object of the base + device specific modules
 */
function extend(base)
{
    $super = base;
    return Object.assign({}, base, devSpecific);
}

exports = { extend : extend };

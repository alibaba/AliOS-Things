/*!
 * ======== PowerCC32XX.js ========
 * CC32XX Power module
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
        default     : false
    },

     /* Policy function handling */
    {
        name        : "policyFxn",
        displayName : "Policy Function",
        default     : "PowerCC32XX_sleepPolicy",
        onChange    : onChangePolicyFxn,
        description : "Power policy function called from the idle loop.",
        options     :
        [
            { name: "PowerCC32XX_sleepPolicy" },
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
        default     : "PowerCC32XX_initPolicy",
        onChange    : onChangePolicyInitFxn,
        description : "The initialization function for the Power policy.",
        options     :
        [
            { name: "PowerCC32XX_initPolicy" },
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
        name        : "enterLPDSHookFxn",
        displayName : "Enable hook function call on entry to LPDS",
        description : "Optional function. ",
        placeholder : "Enter a function name to enable",
        default     : ""
    },

    {
        name        : "resumeLPDSHookFxn",
        displayName : "Enable hook function call on resume from LPDS",
        description : "Optional function. ",
        placeholder : "Enter a function name to enable",
        default     : ""
    },


    {
        name        : "enableNetworkWakeupLPDS",
        displayName : "Enable Network Activity as a Wakeup Source for LPDS",
        default     : true
    },

    {
        name         : "ramRetentionMaskLPDS",
        hidden       : true,
        displayName  : "SRAM retention mask for LPDS",
        default      : ["SRAM_COL_1", "SRAM_COL_2", "SRAM_COL_3", "SRAM_COL_4"],
        minSelections: 0,
        options      :
        [
            { name: "SRAM_COL_1" },
            { name: "SRAM_COL_2" },
            { name: "SRAM_COL_3" },
            { name: "SRAM_COL_4" }
        ]
    },

    {
        name        : "keepDebugActiveDuringLPDS",
        displayName : "Keep debug interface active during LPDS",
        default     : false
    },

    {
        name        : "enableGPIOWakeupLPDS",
        displayName : "Enable GPIO as a Wakeup Source for LPDS",
        onChange    : onChangeEnableGPIOWakeupLPDS,
        default     : true
    },

    {
        name        : "wakeupGPIOSourceLPDS",
        displayName : "GPIO Source for wakeup from LPDS",
        default     : "GPIO13",
        hidden      : true,
        options     :
        [
            { name: "GPIO2" },
            { name: "GPIO4" },
            { name: "GPIO11" },
            { name: "GPIO13" },
            { name: "GPIO17" },
            { name: "GPIO24" },
            { name: "GPIO26" }
        ]
    },
    {
        name        : "wakeupGPIOTypeLPDS",
        hidden      : true,
        displayName : "GPIO Trigger Type for wakeup from LPDS",
        default     : "FALL_EDGE",
        options     :
        [
            { name: "LOW_LEVEL"  },
            { name: "HIGH_LEVEL" },
            { name: "FALL_EDGE"  },
            { name: "RISE_EDGE"  }
        ]
    },
    {
        name        : "wakeupGPIOFxnLPDS",
        hidden      : true,
        displayName : "Enable hook function call on LPDS wakeup triggered by GPIO",
        description : "Optional function. ",
        onChange    : onChangeWakeupGPIOFxnLPDS,
        placeholder : "Enter a function name to enable",
        default     : ""
    },
    {
        name        : "wakeupGPIOFxnLPDSArg",
        hidden      : true,
        displayName : "Integer argument to Function called on GPIO " +
                      "triggered wakeup from LPDS",
        description : "",
        default     : 0
    },

    {
        name        : "enableGPIOWakeupShutdown",
        displayName : "Enable GPIO as a Wakeup Source for Shutdown",
        onChange    : onChangeEnableGPIOWakeupShutdown,
        default     : true
    },

    {
        name        : "wakeupGPIOSourceShutdown",
        hidden      : true,
        displayName : "GPIO Source for wakeup from Shutdown",
        default     : "GPIO13",
        options     :
        [
            { name: "GPIO2" },
            { name: "GPIO4" },
            { name: "GPIO11" },
            { name: "GPIO13" },
            { name: "GPIO17" },
            { name: "GPIO24" },
            { name: "GPIO26" }
        ]
    },

    {
        name        : "wakeupGPIOTypeShutdown",
        hidden      : true,
        displayName : "GPIO Trigger Type for wakeup from Shutdown",
        default     : "RISE_EDGE",
        options     :
        [
            { name: "LOW_LEVEL"  },
            { name: "HIGH_LEVEL" },
            { name: "FALL_EDGE"  },
            { name: "RISE_EDGE"  }
        ]
    },

    {
        name         : "ioRetentionShutdown",
        displayName  : "IO retention mask for Shutdown",
        default      : [ "GRP_0", "GRP_1", "GRP_2", "GRP_3" ],
        minSelections: 0,
        options      :
        [
            { name: "GRP_0" },
            { name: "GRP_1" },
            { name: "GRP_2" },
            { name: "GRP_3" }
        ]
    }
];

/*!
 * onChange callback function for the wakeupGPIOFxnLPDS config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangeWakeupGPIOFxnLPDS(inst, ui)
{
    let subState = (inst.wakeupGPIOFxnLPDS == '');
    ui.wakeupGPIOFxnLPDSArg.hidden = subState;
}

/*!
 * onChange callback function for the enableGPIOWakeupLPDS config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangeEnableGPIOWakeupLPDS(inst, ui)
{
    let subState = (!inst.enableGPIOWakeupLPDS);
    ui.wakeupGPIOTypeLPDS.hidden   = subState;
    ui.wakeupGPIOSourceLPDS.hidden = subState;
    ui.wakeupGPIOFxnLPDS.hidden    = subState;
}

/*!
 * onChange callback function for the enableGPIOWakeupShutdown config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangeEnableGPIOWakeupShutdown(inst, ui)
{
    let subState = (!inst.enableGPIOWakeupShutdown);
    ui.wakeupGPIOTypeShutdown.hidden   = subState;
    ui.wakeupGPIOSourceShutdown.hidden = subState;
}

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
        config           : config,
        moduleInstances  : modulesInstances,
        validate         : validate
    },
    templates           : {
        boardc      : "/ti/drivers/power/PowerCC32XX.Board.c.xdt",
        board_initc : "/ti/drivers/power/PowerCC32XX.Board_init.c.xdt"
    }
};

/*!
 * Return a array of dependent modules for PowerCC32XX
 *
 * @param inst - PowerCC32XX instance to
 *
 * @returns [] - Array of dependent modules
 *
 */
function modulesInstances(inst)
{
    return ([{
        name : 'parkPins',
        moduleName : '/ti/drivers/power/PowerCC32XXPins',
        collapsed : true
    }]);
}

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

    if (!isCName(inst.enterLPDSHookFxn)) {
            logError(vo, inst, 'enterLPDSHookFxn', 'Not a valid C identifier');
    }

    if (!isCName(inst.resumeLPDSHookFxn)) {
            logError(vo, inst, 'resumeLPDSHookFxn', 'Not a valid C identifier');
    }

    if (!isCName(inst.wakeupGPIOFxnLPDS)) {
            logError(vo, inst, 'wakeupGPIOFxnLPDS', 'Not a valid C identifier');
    }
}

/*!
 * Return the value of the CC3220SF main CPU clock frequency
 *
 * @param clock       - The clock name to use.  Ignored in this implementation.
 *
 * @returns frequency - scalar frequency of CPU during RUN state.
 */
function getClockFrequencies(clock)
{
    return [ 80000000 ];
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

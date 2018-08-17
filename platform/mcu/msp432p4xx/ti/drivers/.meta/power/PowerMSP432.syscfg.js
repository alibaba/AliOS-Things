/*!
 * ======== PowerMSP432.js ========
 * MSP432 Power module
 *
 * Device-specific Power configuration support
 */

"use strict";
/* global exports, system */

let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let isCName  = Common.isCName;
let $super;

let VLOCLK  = 9400;     // 9.4 kHz
let REFOCLK = 32768;    // selectable to alternative 128000
let MODCLK  = 25000000; // 25 MHz;

let priIntCS             = Common.newIntPri()[0];
    priIntCS.name        = "priorityInterruptsCS";
    priIntCS.displayName = "Priority of Clock System Interrupt";
    priIntCS.hidden      = true;

/*!
 * config
 *
 * These are the module wide configurations for the Power Module on MSP432
 */
let config = [

    /* The following frequencies are for informational display only.  They
     * are marked read only and will be updated when the initialPerformance
     * config is updated.
     */
    {
        name        : "MCLK",
        displayName : "Initial Frequency MCLK",
        default     : 48000000,
        readOnly    : true
    },
    {
        name        : "HSMCLK",
        displayName : "Initial Frequency HSMCLK",
        default     : 24000000,
        readOnly    : true
    },
    {
        name        : "SMCLK",
        displayName : "Initial Frequency SMCLK",
        default     : 12000000,
        readOnly    : true
    },
    {
        name        : "BCLK",
        displayName : "Initial Frequency BCLK",
        default     : 32768,
        readOnly    : true
    },
    {
        name        : "ACLK",
        displayName : "Initial Frequency ACLK",
        default     : 32768,
        readOnly    : true
    },

    /* The top level Enablers */
    {
        name        : "enablePerf",
        displayName : "Enable Performance Levels",
        description : "Enable performance level control. Strongly recommended.",
        onChange    : onChangeEnablePerf,
        default     : true
    },

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
        default     : "PowerMSP432_sleepPolicy",
        onChange    : onChangePolicyFxn,
        description : "Power policy function called from the idle loop.",
        options     :
        [
            { name: "PowerMSP432_sleepPolicy" },
            { name: "PowerMSP432_deepSleepPolicy" },
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
        default     : "PowerMSP432_initPolicy",
        onChange    : onChangePolicyInitFxn,
        description : "The initialization function for the Power policy.",
        options     :
        [
            { name: "PowerMSP432_initPolicy" },
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
        name        : "enableParking",
        displayName : "Enable Pin Parking",
        description : "Enable automatic pin parking during LPM3 and LPM4.",
        default     : false
    },

    {
        name        : "resumeShutdownHookFxn",
        displayName : "Enable hook function call on resume from shutdown",
        description : "Optional function name.",
        placeholder : "Enter a function name to enable",
        default     : ""
    },

    {
        name        : "customLevels",
        displayName : "Enable Custom Performance Levels",
        description : "Advanced option.",
        default     : 0,
        onChange    : onChangeCustomLevels,
        options     :
        [
            { name: 0 },
            { name: 1 },
            { name: 2 },
            { name: 3 },
            { name: 4 }
        ]
    },


    /* Initial Perf Level */
    {
        name        : "initialPerfLevel",
        displayName : "Initial Performance Level",
        description : "The performance level at system start up. " +
                      "Levels 0-3 are always available. " +
                      "Levels 4-7 are available if custom levels are defined.",
        default     : 2,
        onChange    : onChangeInitialPerfLevel,
        options     :
        [
            { name: 0 },
            { name: 1 },
            { name: 2 },
            { name: 3 },
            { name: 4 },
            { name: 5 },
            { name: 6 },
            { name: 7 }
        ]
    },

    {
        name        : "useExtendedPerf",
        displayName : "Show Advanced Power Configuration",
        onChange    : onChangeUseExtendedPerf,
        default     : false
    },

    {
        name        : "isrCS",
        displayName : "Enable clock system fault interrupt handler",
        description : "Optional function name",
        onChange    : onChangeIsrCS,
        hidden      : true,
        placeholder : "Enter a function name to enable",
        default     : ""
    },

    priIntCS,

    {
        name        : "configurePinHFXT",
        displayName : "Enable HFXT Clock",
        onChange    : onChangeConfigurePinHFXT,
        hidden      : true,
        default     : false
    },

    {
        name        : "bypassHFXT",
        displayName : "Configure HFXT Pin for HFXT Bypass",
        hidden      : true,
        default     : false
    },

    {
        name        : "HFXTFREQ",
        displayName : "HFXT External Crystal Frequency",
        hidden      : true,
        default     : 48000000,
        options     :
        [
            { name:  1000000, displayName : " 1   MHz" },
            { name:  1500000, displayName : " 1.5 MHz" },
            { name:  3000000, displayName : " 3   MHz" },
            { name:  4000000, displayName : " 4   MHz" },
            { name:  6000000, displayName : " 6   MHz" },
            { name: 12000000, displayName : "12   MHz" },
            { name: 16000000, displayName : "16   MHz" },
            { name: 24000000, displayName : "24   MHz" },
            { name: 32000000, displayName : "32   MHz" },
            { name: 40000000, displayName : "40   MHz" },
            { name: 48000000, displayName : "48   MHz" }
        ]
    },

    {
        name        : "configurePinLFXT",
        displayName : "Enable LFXT Clock",
        onChange    : onChangeConfigurePinLFXT,
        hidden      : true,
        default     : false
    },

    {
        name        : "bypassLFXT",
        displayName : "Configure LFXT Pin for LFXT Bypass",
        hidden      : true,
        default     : false
    },

    {
        name        : "LFXTDRIVE",
        displayName : "LFXT Drive Level",
        default     : 0,
        hidden      : true,
        options     :
        [
            { name: 0 },
            { name: 1 },
            { name: 2 },
            { name: 3 }
        ]
    }

];

/*!
 * devSpecific
 *
 * The device specific Power module definitino
 */
let devSpecific = {
    displayName         : "Power",
    getClockFrequencies : getClockFrequencies,

    moduleStatic : {
        config          : config,
        validate        : validate,
        moduleInstances : moduleInstances
    },

    templates : {
        boardc     : "/ti/drivers/power/PowerMSP432.Board.c.xdt",
        board_initc: "/ti/drivers/power/PowerMSP432.Board_init.c.xdt"
    }
};

/*!
 *  Return array of other module instances required by the PowerMSP432 module
 *  The first four instances corresopond to the pre-defined performance levels
 *  defined in the driver source.  Additional instances are for any custom
 *  levels defined by the user.
 *
 *  @params inst - the static Power instance
 *
 *  @returns - Array owned modules for Power.  In this case an array of
 *            custom performance levels.
 *
 */
 function moduleInstances(inst)
 {

    let moduleName = "/ti/drivers/power/PowerMSP432PerfLevels";
    let levelDefs  = [
        {
            moduleName : moduleName,
            name       : "perfLevel0",
            readOnly   : true,
            args       : {
                MCLK              : 12000000,
                HSMCLK            : 3000000,
                SMCLK             : 3000000,
                BCLK              : 32768,
                ACLK              : 32768,
                activeState       : 'DCDC',
                VCORE             : 0,
                DCORESEL          : 12000000,
                SELM              : 'DCO',
                DIVM              : 1,
                SELS              : 'DCO',
                DIVHS             : 4,
                DIVS              : 4,
                SELB              : 'REFO',
                SELA              : 'REFO',
                DIVA              : 1,
                flashWaitStates   : 0,
                enableFlashBuffer : false
            }
        },
        {
            moduleName : moduleName,
            name       : "perfLevel1",
            readOnly   : true,
            args       : {
                MCLK              : 24000000,
                HSMCLK            : 6000000,
                SMCLK             : 6000000,
                BCLK              : 32768,
                ACLK              : 32768,
                activeState       : 'DCDC',
                VCORE             : 0,
                DCORESEL          : 24000000,
                SELM              : 'DCO',
                DIVM              : 1,
                SELS              : 'DCO',
                DIVHS             : 4,
                DIVS              : 4,
                SELB              : 'REFO',
                SELA              : 'REFO',
                DIVA              : 1,
                flashWaitStates   : 1,
                enableFlashBuffer : true
            }
        },
        {
            moduleName : moduleName,
            name       : "perfLevel2",
            readOnly   : true,
            args       : {
                MCLK              : 48000000,
                HSMCLK            : 24000000,
                SMCLK             : 12000000,
                BCLK              : 32768,
                ACLK              : 32768,
                activeState       : 'DCDC',
                VCORE             : 1,
                DCORESEL          : 48000000,
                SELM              : 'DCO',
                DIVM              : 1,
                SELS              : 'DCO',
                DIVHS             : 2,
                DIVS              : 4,
                SELB              : 'REFO',
                SELA              : 'REFO',
                DIVA              : 1,
                flashWaitStates   : 1,
                enableFlashBuffer : true
            }
        },
        {
            moduleName : moduleName,
            readOnly   : true,
            name       : "perfLevel3",
            args       : {
                MCLK              : 48000000,
                HSMCLK            : 48000000,
                SMCLK             : 24000000,
                BCLK              : 32768,
                ACLK              : 32768,
                activeState       : 'DCDC',
                VCORE             : 1,
                DCORESEL          : 48000000,
                SELM              : 'DCO',
                DIVM              : 1,
                SELS              : 'DCO',
                DIVHS             : 1,
                DIVS              : 2,
                SELB              : 'REFO',
                SELA              : 'REFO',
                DIVA              : 1,
                flashWaitStates   : 1,
                enableFlashBuffer : true
            }
        }
    ];

    for (let i = 1; i <= inst.customLevels; ++i) {
        let customNumber = i + 3;
        levelDefs.push({
            moduleName : moduleName,
            name       : "perfLevel" + customNumber
        });
    }

    return levelDefs;
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
    let subState = !inst.enablePolicy;

    ui.enableParking.hidden = subState;
    ui.policyInitFxn.hidden = subState;
    ui.policyFxn.hidden     = subState;

    onChangePolicyInitFxn(inst,ui);
    onChangePolicyFxn    (inst,ui);
}

/*!
 * onChange callback function for the enablePerf config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangeEnablePerf(inst, ui)
{
    let subState = !inst.enablePerf;

    ui.initialPerfLevel.hidden = subState;
    ui.customLevels.hidden     = subState;
    ui.useExtendedPerf.hidden  = subState;

    onChangeInitialPerfLevel(inst, ui);
    onChangeCustomLevels    (inst, ui);
    onChangeUseExtendedPerf (inst, ui);

    for (let i = 0; i < (4 + Number(inst.customLevels)); i++) {
        ui["perfLevel" + i].hidden = subState;
    }
}

/*
 * onChange callback function for the onChangeUseExtendedPerf config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangeUseExtendedPerf(inst,ui)
{
    let subState = !inst.useExtendedPerf;

    ui.isrCS.hidden             = subState;
    ui.configurePinHFXT.hidden  = subState;
    ui.configurePinLFXT.hidden  = subState;

    onChangeConfigurePinHFXT   (inst, ui);
    onChangeConfigurePinLFXT   (inst, ui);
    onChangeIsrCS              (inst, ui);
}

/*
 * onChange callback function for the onChangeConfigurePinHFXT config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
 function onChangeConfigurePinHFXT (inst, ui)
 {
    let subState = !inst.configurePinHFXT;

    ui.bypassHFXT.hidden = subState;
    ui.HFXTFREQ.hidden   = subState;
 }

/*
 * onChange callback function for the onChangeConfigurePinLFXT config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
 function onChangeConfigurePinLFXT (inst, ui)
 {
    let subState = !inst.configurePinLFXT;

    ui.bypassLFXT.hidden  = subState;
    ui.LFXTDRIVE.hidden   = subState;
 }

/*
 * onChange callback function for the onChangeIsrCS config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
 function onChangeIsrCS (inst, ui)
 {
    let subState = (inst.isrCS == '');
    ui.priorityInterruptsCS.hidden = subState;
 }


/*!
 * onChange callback function for the initialPerfLevel config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangeInitialPerfLevel(inst, ui)
{
    displayFrequencies(inst);
}

/*!
 * onChange callback function for the customLevles config
 *
 * @param inst  - Power instance containing the config that changed
 * @param ui    - The User Interface object
 *
 */
function onChangeCustomLevels(inst, ui)
{
    /* TODO: Need to update the enum for Initial Performance Level */
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
    let subState = inst.policyInitFxn !== 'Custom';

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
    let subState = inst.policyFxn !== 'Custom';

    ui.policyFxnCustom.hidden = subState;
}


/*!
 * Validate this module's configuration
 *
 * @param inst  - Power instance containing the config that changed
 * @param vo    - Issue reporting object
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


    if ((inst.configurePinHFXT)) {
        if ((inst.HFXTFREQ == '0')) {
            if (inst.bypassHFXT) {
                logError(vo, inst, 'HFXTFREQ',
                         'If HFXT is enabled in bypass mode, please specify ' +
                         'the external crystal square wave frequency.');
            } else {
                logError(vo, inst, 'HFXTFREQ',
                         'Specify the desired HFXT oscillator frequency.');
            }
        }
    }

    if ((inst.initialPerfLevel - inst.customLevels) > 3) {
        logError(vo, inst, ['initialPerfLevel', 'customLevels'],
                'The initial performance level refers to an undefined custom ' +
                'performace level.');
    }

    if (!isCName(inst.resumeShutdownHookFxn)) {
            logError(vo, inst, "resumeShutdownHookFxn", 'Not a valid C identifier');
    }

    if (inst.useExtendedPerf) {
        if (!isCName(inst.isrCS)) {
                logError(vo, inst, "isrCS", 'Not a valid C identifier');
        }
    }
}

/*!
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

/*!
 * Determine if a list element is the 1st unique copy of that element value.
 *
 * Provides a function suitable for use in the filter function. It determines if
 * the element is the first unique version of the element in the list.
 *
 * @param element   - The element from the list
 * @param index     - The index of the element in the list
 * @param list      - The list
 *
 * @returns integer - A signed value, 0 if items are equal, positive if a > b,
 *                    negative if a < b
 */
function firstUniq (element, index, list)
{
    return list.indexOf(element) === index;
}

/*!
 * Returns a sorted list of unique frequencies that a name clock source may
 * have across any supported performance level.
 *
 * @param clock     - The clock source name
 *
 * @returns freqs[] - A sorted list of unique frequencies that the clock source
 *                    may have across any supported performance level.
 */
function getClockFrequencies(clock)
{
    let freqArray  = [];
    let enablePerf = true;
    let inst = this.$static;

    if ("/ti/drivers/Power" in system.modules) {
        enablePerf = system.modules["/ti/drivers/Power"].$static.enablePerf;
    }
    if (inst && enablePerf) {
        for (let level = 0; level < 8; ++level) {
            let levelName = "perfLevel" + level;

            if (levelName in inst) {
                freqArray.push(getClock(inst, level, clock));
            }
            else {
                break;
            }
        }
    } else {
        freqArray.push(rawBootClock(clock));
    }

    freqArray.sort(numericCompare);
    return freqArray.filter(firstUniq);
}

/*!
 * Return the raw (non driver modified) frequency of a clock on boot.
 *
 * @param clock  - A clock name
 *
 * @returns freq - The device reset (boot) frequency of that clock
 *
 */
 function rawBootClock(clock)
 {
    switch (clock) {
        case 'ACLK':
        case 'BCLK': return 32768;
        default:     return 3000000;
    }
}

/*!
 * Return the raw (non driver modified) frequency of a clock on boot.
 *
 * @param inst   - Power instance
 * @param level  - a performance level
 * @param clock  - A clock name
 *
 * @returns freq - The frequency of clock in the specified performance level.
 *
 */
function getClock(inst, level, clock)
{
    if (level === 'none') {
        return rawBootClock(clock);
    }

    let perfLevelName = 'perfLevel' + level;
    return inst[perfLevelName][clock];
}

/*!
 * Given a performance level.  Update the UI's readonly clock fields for the
 * module.
 *
 * @param inst   - Power instance
 * @param level  - a performance level
 *
 */
function displayFrequencies(inst)
{
    let clocks = ['MCLK', 'HSMCLK', 'SMCLK', 'BCLK', 'ACLK'];
    let level  = inst.enablePerf ? inst.initialPerfLevel : 'none';

    if ((inst.initialPerfLevel - inst.customLevels) > 3) {
        level = 'none';
    }

    for (let i = 0; i < clocks.length; i++) {
        inst[clocks[i]] = getClock(inst, level, clocks[i]);
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
function extend (base)
{
    $super = base;
    return Object.assign({}, base, devSpecific);
}

exports = { extend : extend };

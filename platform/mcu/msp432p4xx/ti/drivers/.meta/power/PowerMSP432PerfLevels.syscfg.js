"use strict";
/*global exports, system*/

let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

let VLOCLK  = 9400;     // 9.4 kHz
let REFOCLK = 32768;    // selectable to alternative 128000
let LFXTCLK = 32768;    // Assumed freq if enabled.
let MODOSC  = 25000000; // 25 MHz;

let divChoices = [
    { name:   1 },
    { name:   2 },
    { name:   4 },
    { name:   8 },
    { name:  16 },
    { name:  32 },
    { name:  64 },
    { name: 128 }
];

/**
 * config
 *
 * These are the Performance Level configurations for MSP432
 */
let config =  [
    {
        name        : "MCLK",
        displayName : "This Performance Level Frequency MCLK",
        default     : 3000000,
        readOnly    : true
    },
    {
        name        : "HSMCLK",
        displayName : "This Performance Level Frequency HSMCLK",
        default     : 3000000,
        readOnly    : true
    },
    {
        name        : "SMCLK",
        displayName : "This Performance Level Frequency SMCLK",
        default     : 3000000,
        readOnly    : true
    },
    {
        name        : "BCLK",
        displayName : "This Performance Level Frequency BCLK",
        default     : 32768,
        readOnly    : true
    },
    {
        name        : "ACLK",
        displayName : "This Performance Level Frequency ACLK",
        default     : 32768,
        readOnly    : true
    },


    {
        name        : "activeState",
        displayName : "Active State for the Device",
        default     : "LDO",
        options     :
        [
            { name: "LDO",  description: "Low Dropout"},
            { name: "DCDC", description: "DC-DC" },
            { name: "LF",   description: "Low Frequency"}
        ]
    },
    {
        name        : "VCORE",
        displayName : "Core Voltage Level",
        default     : 0,
        options     :
        [
            { name: 0, displayName : "VCORE0" },
            { name: 1, displayName : "VCORE1" }
        ]
    },
    {
        name        : "DCORESEL",
        displayName : "DCO Frequency Range Selection",
        default     : 3000000,
        onChange    : calcOutClockFreq,
        options     :
        [
            { name: 1500000,   description: "1  MHz to 2  MHz" },
            { name: 3000000,   description: "2  MHz to 4  MHz" },
            { name: 6000000,   description: "4  MHz to 8  MHz" },
            { name: 12000000,  description: "8  MHz to 16 MHz" },
            { name: 24000000,  description: "16 MHz to 32 MHz" },
            { name: 48000000,  description: "32 MHz to 64 MHz" }
        ]
    },

    /* Source Configuration */
    {
        name        : "SELM",
        displayName : "MCLK Source",
        default     : "DCO",
        onChange    : calcOutClockFreq,
        options     :
        [
            { name: "DCO"    },
            { name: "HFXT"   },
            { name: "LFXT"   },
            { name: "VLO"    },
            { name: "REFO"   },
            { name: "MODOSC" }
        ]
    },
    {
        name        : "SELS",
        displayName : "HSMCLK / SMCLK Shared Source",
        default     : "DCO",
        onChange    : calcOutClockFreq,
        options     :
        [
            { name: "DCO"    },
            { name: "HFXT"   },
            { name: "LFXT"   },
            { name: "VLO"    },
            { name: "REFO"   },
            { name: "MODOSC" }
        ]
    },
    {
        name        : "SELB",
        displayName : "BCLK Source",
        default     : "REFO",
        onChange    : calcOutClockFreq,
        options     :
        [
            { name: "LFXT" },
            { name: "REFO" }
        ]
    },

    {
        name        : "SELA",
        displayName : "ACLK Source",
        default     : "REFO",
        onChange    : calcOutClockFreq,
        options     :
        [
            { name: "LFXT" },
            { name: "VLO"  },
            { name: "REFO" }
        ]
    },

    {
        name        : "DIVM",
        displayName : "MCLK Divider",
        default     : 1,
        onChange    : calcOutClockFreq,
        options     : divChoices
    },

    /* Divider Configuration */
    {
        name        : "DIVHS",
        displayName : "HSMCLK Divider",
        default     : 1,
        onChange    : calcOutClockFreq,
        options     : divChoices
    },
    {
        name        : "DIVS",
        displayName : "SMCLK Divider",
        default     : 1,
        onChange    : calcOutClockFreq,
        options     : divChoices
    },

    {
        name        : "DIVA",
        displayName : "ACLK Divider",
        default     : 1,
        onChange    : calcOutClockFreq,
        options     : divChoices
    },

    /* FLASH configuration */
    {
        name        : "flashWaitStates",
        displayName : "Number of Flash Wait-States",
        default     : 0
    },

    {
        name        : "enableFlashBuffer",
        displayName : "Enable Flash Read Buffering",
        default     : false
    }
];

function getClockSourceFreq(clk, perfInst)
{
    let pow = system.getScript("/ti/drivers/Power").$static;

    switch (clk) {
        case 'VLO'   : { return VLOCLK;                                  }
        case 'REFO'  : { return REFOCLK;                                 }
        case 'MODOSC': { return MODOSC;                                  }
        case 'DCO'   : { return perfInst.DCORESEL;                       }
        case 'HFXT'  : { return pow.configurePinHFXT ? pow.HFXTFREQ : 0; }
        case 'LFXT'  : { return pow.configurePinLFXT ? LFXTCLK      : 0; }
    }
    return 0;
}

function calcOutClockFreq(inst, ui)
{
    let divm  = inst.DIVM;
    let divhs = inst.DIVHS;
    let divs  = inst.DIVS;
    let diva  = inst.DIVA;

    inst.MCLK    = getClockSourceFreq(inst.SELM, inst) / divm;
    inst.HSMCLK  = getClockSourceFreq(inst.SELS, inst) / divhs;
    inst.SMCLK   = getClockSourceFreq(inst.SELS, inst) / divs;
    inst.BCLK    = getClockSourceFreq(inst.SELB, inst);
    inst.ACLK    = getClockSourceFreq(inst.SELA, inst) / diva;
}

/**
 * Validate this module's configuration
 *
 * @param inst       - PowerMSP432PerfLevels instance to be validated
 * @param validation - Issue reporting object
 */
function validate(inst, validation)
{
    let divm  = inst.DIVM;
    let divhs = inst.DIVHS;
    let divs  = inst.DIVS;
    let diva  = inst.DIVA;

    /* activeState, VCORE, DCORESEL, flashWaitStates, enableFlashBuffer
     * DIV:  M, HS, S, A
     * SEL:  M, S, B, A
     * CLK:  M, HSM, SM, B, A
     */

    if (inst.ALCK > 128000) {
        logError(validation, inst, "SELA", 'ACLK frequency must be 128kHz or lower.');
    }

    if (inst.BLCK > 32768) {
        logError(validation, inst, "SELB", 'BCLK frequency must be 32768 or lower.');
    }

    if (inst.SMCLK > 24000000) {
        logError(validation, inst, ["SELS", "DIVS"],
                 'SMCLK frequency must be 24000000 or lower.');
    }

    if (inst.VCORE === '0') {
        if (inst.MCLK > 24000000) {
            logError(validation, inst, "VCORE",
                'MCLK frequency must be 24000000 or lower, if VCORE is 0.');
        }
        if (inst.SMCLK > 12000000) {
            logError(validation, inst, "VCORE",
                'SMCLK frequency must be 12000000 or lower, if VCORE is 0.');
        }
    }

    if (inst.activeState === 'LF') {
        if (inst.MCLK   > 128000) {
            logError(validation, inst, ["activeState"],
            'MCLK frequency must be 128000 or lower, if active state is LF.');
        }
        if (inst.SMCLK  > 128000) {
            logError(validation, inst, ["activeState"],
            'SMCLK frequency must be 128000 or lower, if active state is LF.');
        }
        if (inst.HSMCLK > 128000) {
            logError(validation, inst, ["activeState"],
            'HSMCLK frequency must be 128000 or lower, if active state is LF.');
        }
    }

    if (getClockSourceFreq(inst.SELM, inst) === 0) {
        logError(validation, inst, ["SELM"],
            'Selected clock source has not been configured.');
    }

    if (getClockSourceFreq(inst.SELS, inst) === 0) {
        logError(validation, inst, ["SELS"],
            'Selected clock source has not been configured.');
    }

    if (getClockSourceFreq(inst.SELB, inst) === 0) {
        logError(validation, inst, ["SELB"],
            'Selected clock source has not been configured.');
    }

    if (getClockSourceFreq(inst.SELA, inst) === 0) {
        logError(validation, inst, ["SELA"],
            'Selected clock source has not been configured.');
    }
}

/* The exported object for this module */
exports = {
    name        : "perfLevel",
    displayName : "Performance Levels",
    description : "MSP432 performance levels used by Power",
    validate    : validate,
    maxInstances: 8,
    config      : config
};

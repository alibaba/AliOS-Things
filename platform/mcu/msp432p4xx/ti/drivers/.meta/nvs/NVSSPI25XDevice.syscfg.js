"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let logWarning = Common.logWarning;

/* NVSSPI25X config parameters */
let nvsConfig = [
    {
        name: "spiManager",
        displayName: "SPI Management",
        default: "NVS",
        options: [
            { name: "NVS",
              displayName: "NVS driver opens SPI Instance" },
            { name: "User",
              displayName: "User Opens SPI Instance" }
        ],
        onChange: updateConfigs
    },
    {
        name: "spiCsManager",
        displayName: "SPI Flash Chip Select Management",
        default: "NVS",
        options: [
            { name: "NVS",
              displayName: "NVS driver asserts SPI flash chip select" },
            { name: "SPI",
              displayName: "SPI driver asserts SPI flash chip select" }
        ],
        onChange: updateConfigs
    },
    {
        name: "spiHandle",
        displayName: "SPI Handle",
        hidden: true,
        description: "The symbol name of the user provided SPI Handle",
        placeholder : "Enter the SPI Handle name",
        default: ""
    },
    {
        name: "spiBitRate",
        displayName: "SPI Bit Rate",
        default: 4000000
    }
];

/*
 * find matching components
 */
function filterHardware(component) {
    /* let the SPI module make the decision */
    let SPI = system.getScript("/ti/drivers/SPI");
    return (SPI.filterHardware(component));
}

/*
 * Called on various config changes.
 */
function updateConfigs(inst, ui) {
    if (inst.spiManager == "User") {
        ui.spiHandle.hidden = false;
        ui.spiBitRate.hidden = true;
    } else {
        ui.spiHandle.hidden = true;
        ui.spiBitRate.hidden = false;
    }
}

/**
 * Validate this module's configuration
 *
 * @param inst       - NVS instance to be validated
 * @param validation - map of declared members to inst which holds
 *                     arrays for errors, warnings, infos
 */
function validate(inst, validation) {
    if (inst.spiManager == "User") {
        if ((inst.spiHandle == "NULL") || (inst.spiHandle == "")) {
            let message = "Must be the name of a global SPI Handle.";
            logWarning(validation, inst, "spiHandle", message);
        }
    }
    else if (inst.spiHandle != "") {
        let message = "Must be NULL when the NVS driver manages the SPI instance.";
        logWarning(validation, inst, "spiHandle", message);
    }

}

/*
 *  ======== gpioInit ========
 *  Initialize GPIO CS instance
 */
function gpioInit() {
    return ({
        comment: "%l    /* SPI Flash Chip Select GPIO Instance */",
        mode: "Output",
        outputType: "Standard",
        outputState: "High"
    });
}

/*
 *  ======== moduleInstances ========
 *  returns a SPI CS GPIO instance
 */
function moduleInstances(inst) {
    let modules = new Array();
    /* Add GPIO instance if NVS driver asserts CS */
    if (inst.spiCsManager == "NVS") {
        modules.push(
            {
                name: "spiCsnGpioInstance",
                displayName: "SPI Flash Chip Select GPIO Instance",
                moduleName: "/ti/drivers/GPIO",
                args: gpioInit(),
                hardware: inst.$hardware ? inst.$hardware : null
            }
        );
    }

    return (modules);
}

/*
 *  ======== sharedModuleInstances ========
 *  returns a shared SPI instance
 */
function sharedModuleInstances(inst) {
    let sharedModules = new Array();

    /* Add SPI instance if NVS driver opens the SPI */
    if (inst.spiManager == "NVS") {
        sharedModules.push(
            {
                name: "spiInstance",
                displayName: "SPI Flash SPI Instance",
                moduleName: "/ti/drivers/SPI",
                hardware: inst.$hardware ? inst.$hardware : null
            }
        );
    }

    return (sharedModules);
}

exports = {
    config: nvsConfig,

    defaultInstanceName: "SPI_Flash_Device",

    groups: [
        {
            name: "userSPI",
            displayName: "User SPI Handle",
            hidden: true,
            members: ["spiHandle"]
        },
        {
            name: "nvsSPI",
            displayName: "SPI Instance Parameters",
            members: ["spiBitRate"]
        }
    ],

    layout: [
        "regionBaseOffset", "regionSize", "sectorSize",
        "verifyBufSize", "spiManager"
    ],

    members: [
        "regionBaseOffset", "regionSize", "sectorSize",
        "verifyBufSize", "spiManager"
    ],

    filterHardware: filterHardware,

    sharedModuleInstances: sharedModuleInstances,
    moduleInstances: moduleInstances,

    validate: validate
};

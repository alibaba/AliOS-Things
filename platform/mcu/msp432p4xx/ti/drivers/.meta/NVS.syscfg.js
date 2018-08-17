/**
 *  NVS configuration
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* Intro splash on GUI */
let longDescription = "The NVS module allows you to manage non-volatile " +
    "memory. Using the NVS APIs, you can read and write data from and to " +
    "persistent storage." +
    "Each NVS object manages a 'region' of non-volatile memory of a size " +
    "specified in the NVS object's hardware attributes. A 'sector' refers " +
    "to the smallest unit of non-volatile storage that can be erased at " +
    "one time, and the 'sectorSize' is the size of this unit. Flash sector " +
    "size is hardware specific and may be meaningless for some persistent " +
    "storage systems. However, in the case of flash memory devices, the " +
    "size of a managed region must be an integer multiple of the sector size.";

/* top level NVS Type selector config */
let nvsConfigs = [
    {
        name: "nvsType",
        displayName: "NVS Region Type",
        default: "Internal",
        options: [
            { name: "Internal" },
            { name: "External" },
            { name: "RAM" }
        ]
    }
];

/* compute /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "NVS");

/*
 *  ======== filterHardware ========
 *  Find matching components
 */
function filterHardware(component)
{
    if (component.type) {
        if (Common.typeMatches(component.type, ["SPI_FLASH"])) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {
        inst.nvsType = "External";
        ui.nvsType.readOnly = true;
    } else {
        ui.nvsType.readOnly = false;
    }
}

/**
 * Validate an instance's configuration
 *
 * @param inst       - NVS instance to be validated
 * @param validation - NVS config validation object
 *                     to fill with error strings
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

let nvsIntFlashModuleInstance = [{
    name: "intFlash",
    displayName: "Internal Flash Region",
    moduleName: "/ti/drivers/nvs/NVS" + family
}];

let nvsRamModuleInstance = [{
    name: "ramFlash",
    displayName: "RAM Region",
    moduleName: "/ti/drivers/nvs/NVSRAM"
}];

function moduleInstances(inst)
{
    let nvsSpiFlashModuleInstance = [{
        name: "spiFlash",
        displayName: "External (SPI) Flash Region",
        moduleName: "/ti/drivers/nvs/NVSSPI25X",
        hardware: inst.$hardware
    }];
    switch (inst.nvsType) {
        case "External":
            return (nvsSpiFlashModuleInstance);

        case "Internal":
            return (nvsIntFlashModuleInstance);

        case "RAM":
            return (nvsRamModuleInstance);
    }
    return ([]);
}

exports = {
    displayName: "NVS",
    defaultInstanceName: "Board_NVS",
    description: "Non Volatile Storage Driver",
    longDescription: longDescription,
    config: nvsConfigs,
    validate: validate,
    moduleInstances: moduleInstances,
    filterHardware: filterHardware,
    onHardwareChanged: onHardwareChanged,
    templates: {
        boardc: "/ti/drivers/nvs/NVS.Board.c.xdt",
        boardh: "/ti/drivers/nvs/NVS.Board.h.xdt"
    }
};

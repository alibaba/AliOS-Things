"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let logWarning = Common.logWarning;

/* NVSSPI25X config parameters */
let nvsConfig = [
    {
        name: "regionBaseOffset",
        displayName: "Base Offset of Region",
        default: "0"
    },
    {
        name: "regionSize",
        displayName: "Size of Region (in bytes)",
        default: "0x1000"
    },
    {
        name: "sectorSize",
        displayName: "Size of Flash Erase Sector (in bytes)",
        default: "0x1000"
    },
    {
        name: "verifyBufSize",
        displayName: "Write Verification Buffer size",
        description: "Size of Write Verification Buffer (in bytes)",
        default: "0"
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
    if (inst.spiDevice.spiManager == "User") {
        ui.spiDevice.spiHandle.hidden = false;
        ui.spiDevice.spiBitRate.hidden = true;
    } else {
        ui.spiDevice.spiHandle.hidden = true;
        ui.spiDevice.spiBitRate.hidden = false;
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
    let regionBaseOffset = Number(inst.regionBaseOffset);
    let regionSize = Number(inst.regionSize);
    let sectorSize = Number(inst.sectorSize);

    /* sanity test region attributes */
    if (sectorSize == 0) {
        let message = "Invalid sectorSize value: " + inst.sectorSize + ".";
        logError(validation, inst, "sectorSize", message);
    } else {
        if ((regionSize == 0) ||
            ((regionSize % sectorSize) != 0)) {
            let message = "Invalid regionSize value: " + inst.regionSize + ". " +
                "Must be a non-zero multiple of region.sectorSize [" +
                inst.sectorSize + "] bytes.";
            logError(validation, inst, "regionSize", message);
        }
    }

    if (regionBaseOffset % sectorSize) {
        let message = "Base offset must be an integer multiple of the sector size.";
        logError(validation, inst, "regionBaseOffset", message);
    }

    /*
     * verify that this region does not overlap within any other region
     * on the same SPI flash device
     */
    for (let i = 0; i < inst.$module.$instances.length; i++) {
        let tinst = inst.$module.$instances[i];
        if (inst == tinst) continue;

        /* only compare regions on the same SPI flash device */
        if (inst.spiDevice != tinst.spiDevice) continue;

        let tRegionBaseOffset = Number(tinst.regionBaseOffset);
        let tRegionSize = Number(tinst.regionSize);
        if ((regionBaseOffset >= tRegionBaseOffset) &&
            (regionBaseOffset < (tRegionBaseOffset + tRegionSize))) {
            let message = "Region Base Offset overlaps with NVS region: " + tinst.$name + ".";
            logWarning(validation, inst, "regionBaseOffset", message);
            break;
        }
        if (((regionBaseOffset + regionSize) > tRegionBaseOffset) &&
            ((regionBaseOffset + regionSize) <= (tRegionBaseOffset + tRegionSize))) {
            let message = "Region Base Offset + Region Size overlaps with NVS region: " + tinst.$name + ".";
            logWarning(validation, inst, "regionBaseOffset", message);
            break;
        }
    }
}

/*
 *  ======== sharedModuleInstances ========
 *  returns a shared SPI Flash Device instance
 */
function sharedModuleInstances(inst) {
    let sharedModules = new Array();

    sharedModules.push(
        {
            name: "spiDevice",
            displayName: "SPI Flash Device",
            moduleName: "/ti/drivers/nvs/NVSSPI25XDevice",
            hardware: inst.$hardware ? inst.$hardware : null
        }
    );

    return (sharedModules);
}

exports = {
    config: nvsConfig,

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

    validate: validate
};

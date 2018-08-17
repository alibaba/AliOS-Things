"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

/* NVSRAM config parameters */
let nvsConfig = [
    {
        name        : "regionBase",
        displayName : "Base Address of Region",
        default     : ""
    },
    {
        name        : "regionSize",
        displayName : "Size of Region (in bytes)",
        default     : "4096"
    },
    {
        name        : "sectorSize",
        displayName : "Region Sector Size (in bytes)",
        default     : "4096"
    }
];

/**
 * Validate an instance's configuration
 *
 * @param inst       - NVS instance to be validated
 * @param validation - NVS config validation object
 *                     to fill with error strings
 */
function validate(inst, validation)
{
    let regionBase = Number(inst.regionBase);
    let regionSize = Number(inst.regionSize);
    let sectorSize = Number(inst.sectorSize);

    /* sanity test region attributes */
    if ((regionSize == 0) ||
        ((regionSize % sectorSize) != 0)) {
        let message = "Invalid regionSize value: " + inst.regionSize + "." +
            "\nMust be a non-zero multiple of " + sectorSize + " bytes.";
        logError(validation, inst, "regionSize", message);
    }

    if (regionBase % sectorSize) {
        let message = "Region Base address must be aligned on a 4K page boundary.";
        logError(validation, inst, "regionBase", message);
    }

    /* verify that this region does not overlap within any other region */
    for (let i = 0; i < inst.$module.$instances.length; i++) {
        let tinst = inst.$module.$instances[i];
        if ((inst == tinst)) continue;
        let tRegionBase = Number(tinst.regionBase);
        let tRegionSize = Number(tinst.regionSize);
        if ((regionBase >= tRegionBase) &&
            (regionBase < (tRegionBase + tRegionSize))) {
            let message = "Region Base overlaps with NVS region: " + tinst.$name + ".";
            logError(validation, inst, "regionBase", message);
            break;
        }
        if (((regionBase + regionSize) > tRegionBase) &&
            ((regionBase + regionSize) <= (tRegionBase + tRegionSize))) {
            let message = "Region Base + Region Size overlaps with NVS region: " + tinst.$name + ".";
            logError(validation, inst, "regionBase", message);
            break;
        }
    }
}

exports = {
    config: nvsConfig,
    validate: validate
};

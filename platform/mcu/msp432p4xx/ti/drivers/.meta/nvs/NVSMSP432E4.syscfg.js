"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let logWarning = Common.logWarning;

const SECTORSIZE = "0x4000";

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

    /* sanity test region attributes */
    if ((regionSize == 0) ||
        ((regionSize % inst.sectorSize) != 0)) {
        let message = "Invalid regionSize value: " + inst.regionSize + "." +
            "\nMust be a non-zero multiple of " + inst.sectorSize + " bytes.";
        logError(validation, inst, "regionSize", message);
    }

    if (regionBase % inst.sectorSize) {
        let message = "Region Base address must be aligned on a "
            + inst.sectorSize + " page boundary.";
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
            logWarning(validation, inst, "regionBase", message);
            break;
        }
        if (((regionBase + regionSize) > tRegionBase) &&
            ((regionBase + regionSize) <= (tRegionBase + tRegionSize))) {
            let message = "Region Base + Region Size overlaps with NVS region: " + tinst.$name + ".";
            logWarning(validation, inst, "regionBase", message);
            break;
        }
    }
}

/* NVSMSP432E4 config parameters */
let nvsConfig = [
    {
        name        : "regionBase",
        displayName : "Base Address of Region",
        default     : "0"
    },
    {
        name        : "regionSize",
        displayName : "Size of Region (in bytes)",
        default     : SECTORSIZE
    },
    {
        name        : "sectorSize",
        default     : SECTORSIZE,
        hidden      : true,
        readOnly    : true
    }
];

exports = {
    config: nvsConfig,
    validate: validate
};

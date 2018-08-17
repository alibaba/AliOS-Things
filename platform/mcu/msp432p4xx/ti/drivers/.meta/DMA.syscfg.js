/*
 *  ======== DMA configuration support ========
 */
"use strict";
/*global exports, system*/

let Common      = system.getScript("/ti/drivers/Common.js");
let family      = Common.device2Family(system.deviceData, "DMA");
let logError    = Common.logError;
let intPriority = Common.newIntPri()[0];
intPriority.displayName = "DMA error interrupt priority";

/**
 *  ======== base ========
 *  Define the base DMA properties and methods
 */
let base = {
    displayName         : "DMA",
    description         : "On-chip DMA resource allocation",
    longDescription     : "The DMA module provides support to other drivers that need "
                          + "to allocate exclusive access to a DMA resource to support "
                          + "CPU-less peripheral data transfers.",
    validate            : validate,
    defaultInstanceName : "Board_DMA",
    maxInstances        : 1,
    moduleStatic        : {
        config          : [
            {
                /*
                 *  This name is used by UDMACC26XX.syscfg.js.  If it is
                 *  changed, make sure to update UDMACC26XX.syscfg.js.
                 */
                name        : "dmaErrorFxn",
                displayName : "DMA error interrupt handler",
                default     : "dmaErrorFxn",
                hidden      : false
            },
            intPriority
        ]
    }
};

/**
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  @param inst       - DMA instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if (!Common.isCName(inst.$module.$static.dmaErrorFxn)) {
        logError(validation, inst, 'dmaErrorFxn', 'Not a valid C identifier.');
    }
}

/* get family-specific DMA module */
let devDMA = system.getScript("/ti/drivers/dma/UDMA" + family);
exports = devDMA.extend(base);

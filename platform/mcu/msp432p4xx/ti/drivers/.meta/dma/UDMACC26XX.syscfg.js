/*!
 *  ======== UDMACC26XX.js ========
 *  CC26XX UDMA module
 *
 *  Device-specific Power configuration support
 */

"use strict";
/* global exports, system */

/*!
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base DMA configuration
 */
let devSpecific = {
    templates : {
        boardc: "/ti/drivers/dma/UDMACC26XX.Board.c.xdt"
    }
};

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
    /*
     *  Hide the dmaErrorFxn for UDMACC26XX, since this in not configurable
     *  by the user.  It is hardcoded in the UDMACC26XX driver.
     */
    for (let i = 0; i < base.moduleStatic.config.length; i++) {
        if (base.moduleStatic.config[i].name.match(/dmaErrorFxn/)) {
            base.moduleStatic.config[i].hidden = true;
        }
    }

    return Object.assign({}, base, devSpecific);
}

/* Export device-specific extensions to base exports */
exports = { extend : extend };

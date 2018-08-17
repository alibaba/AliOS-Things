/*!
 *  ======== UDMAMSP432.js ========
 *  MSP432 UDMA module
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
        boardc: "/ti/drivers/dma/UDMAMSP432.Board.c.xdt"
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
    return Object.assign({}, base, devSpecific);
}

/* Export device-specific extensions to base exports */
exports = { extend : extend };

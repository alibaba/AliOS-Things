/*
 *  ======== SPI configuration ========
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "SPI");

/* Intro splash on GUI */
let longDescription = "The Serial Peripheral Interface (SPI) "
    + "driver is a generic, full-duplex driver that transmits "
    + "and receives data on a SPI bus. SPI is sometimes called SSI "
    + "(Synchronous Serial Interface). The SPI protocol defines "
    + "the format of a data transfer over the SPI bus, but it "
    + "leaves flow control, data formatting, and handshaking "
    + "mechanisms to higher-level software layers.";

/* generic configuration parameters for SPI instances */
let spiConfig = [
    {
        name: "mode",
        displayName: "Mode",
        default: "Three Pin",
        options: [
            { name: "Three Pin" },
            { name: "Four Pin, Select active low" },
            { name: "Four Pin, Select active high" }
        ]
    },
    {
        name: "defaultTxBufValue",
        displayName: "Default Tx Buffer Value (Provide in 0xh format)",
        default: "~0"
    },
    {
        name: "minDmaTransferSize",
        displayName: "Minimum DMA Transfer Size",
        default: 10
    },
    {
        name: "duplex",
        displayName: "Duplex",
        hidden : true,
        description: "Useful in situations where the SPI bus master will"
            + " transmit data to the slaves but is not interested in the data"
            + " returned by the slaves OR where the master will not"
            + " transmit meaningful data but is interested in receiving data"
            + " from slaves. Slaves must ignore incoming data from master.",
        default: "full",
        options: [
            { name: "full", displayName: "Full Duplex" },
            { name: "txOnly", displayName: "Transmit Only" },
            { name: "rxOnly", displayName: "Receive Only" }

        ]
    }
];

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with SPI
 *
 *  param component - hardware object describing signals and
 *                    resources they're attached to
 *
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component)
{
    let clk, simo, somi, ste;

    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["SPI_CLK"])) clk = sig;
        if (Common.typeMatches(type, ["SPI_SIMO"])) simo = sig;
        if (Common.typeMatches(type, ["SPI_SOMI"])) somi = sig;
        if (Common.typeMatches(type, ["DOUT"])) ste = sig;
        if (Common.typeMatches(type, ["SPI_STE"])) ste = sig;
    }

    if (clk && simo && somi) {
        return (true);
    }

    return (false);
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - SPI instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);

    /* Allow an input of 0 or ~0 */
    if (inst.defaultTxBufValue.match( /((^0$)|(^~0$))/ )) {
        return;
    }

    if (inst.defaultTxBufValue.match(/^0[xX]/) === null) {
        logError(validation, inst, 'defaultTxBufValue',
            'Must be in hex format.');
    }
    else if (inst.defaultTxBufValue.length === 2) {
        logError(validation, inst, 'defaultTxBufValue',
            'Must be in hex format.');
    }
}

/*
 *  ======== base ========
 *  Define the base/common SPI properties and methods
 */
let base = {
    displayName: "SPI",
    description: "Serial Peripheral Interface (SPI) Bus Driver",
    longDescription: longDescription,
    defaultInstanceName: "Board_SPI",
    config: spiConfig,
    moduleStatic: { /* issue: PMUX-1054 */
        displayName: "SPI Module Configuration"
    },
    filterHardware: filterHardware,
    validate: validate,
    busModule: true,      /* allow HW components to reference same SPI pins */
    allowStaticOnly: true /* allow addModule(SPI) without any instances */
};

/* get family-specific SPI module */
let deviceSPI = system.getScript("/ti/drivers/spi/SPI" + family);

/* extend our common exports to include the family-specific content */
exports = deviceSPI.extend(base);

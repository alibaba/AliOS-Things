/*
 *  SDCC32XX.syscfg.js
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "SDHost Interrupt Priority";
intPriority.hidden = true;

let devSpecific = {
    config: [
        {
            name: "clockRate",
            displayName: "Clock Rate",
            default: 8000000
        },
        {
            name: "interfaceType",
            displayName: "Interface Type",
            default: "SD Host",
            options: [
                { name: "SD SPI" },
                { name: "SD Host" }
            ],
            onChange: interfaceChange
        },
        intPriority
    ],

    validate: validate,

    pinmuxRequirements: pinmuxRequirements,

    sharedModuleInstances : sharedModuleInstances,

    moduleInstances : moduleInstances,

    modules: Common.autoForcePowerAndDMAModules,

    filterHardware: filterHardware,

    templates: {
        boardc: "/ti/drivers/sd/SDCC32XX.Board.c.xdt",
        boardh: "/ti/drivers/sd/SD.Board.h.xdt"
    }
};

/**
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - SPI instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== sharedModuleInstances ========
 */
function sharedModuleInstances(inst)
{
    if (inst.interfaceType === "SD SPI") {
        return ([{
            name: "spiInstance",
            displayName: "SPI",
            moduleName: "/ti/drivers/SPI",
            hardware: inst.$hardware
        }]);
    }
    return ([]);
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    if (inst.interfaceType === "SD SPI") {
        return ([{
            name: "spiCsGpioInstance",
            displayName: "SDSPI Chip Select",
            moduleName: "/ti/drivers/GPIO",
            hardware: inst.$hardware,
            args: {
                comment: "%l    /* SDSPI Chip Select */",
                mode: "Output",
                outputType: "Standard",
                outputState:"High"
            }
        }]);
    }
    return ([]);
}

/*
 *  ======== pinmuxRequirements ========
 */
function pinmuxRequirements(inst)
{
    if (inst.interfaceType === "SD Host") {
        let sdHost = {
            name: "sdHost",
            displayName: "SD Host Peripheral",
            interfaceName: "SDHost",
            hidden: true,
            resources: [
                {
                    name: "clkPin",
                    displayName: "CLK",
                    interfaceNames: ["CLK"]
                },
                {
                    name: "cmdPin",
                    displayName: "CMD",
                    interfaceNames: ["CMD"]
                },
                {
                    name: "dataPin",
                    displayName: "DATA",
                    interfaceNames: ["DATA"]
                },
                {
                    name: "dmaTxChannel",
                    displayName: "TX DMA CHANNEL",
                    interfaceNames: ["DMA_TX"]
                },
                {
                    name: "dmaRxChannel",
                    displayName: "RX DMA CHANNEL",
                    interfaceNames: ["DMA_RX"]
                }
            ]
        };
        return ([sdHost]);
    }

    return ([]);
}

/*
 *  ========= filterHardware ========
 */
function filterHardware(component)
{

    let clk, cmd, data;

    /* component must have SD Host signals */
    for (let sig in component.signals) {
        if (component.signals[sig].type) {
            let type = component.signals[sig].type;
            if (Common.typeMatches(type,["SDHost_CLK"])) clk = sig;
            if (Common.typeMatches(type,["SDHost_CMD"])) cmd = sig;
            if (Common.typeMatches(type,["SDHost_DATA"])) data = sig;
        }
    }

    /*
     * confirm that this component has all the signals required
     * to be an SDHost
     */
    if (clk && cmd && data) {
        return (true);
    }

    return (false);
}

function interfaceChange(inst, ui)
{
    if (inst.interfaceType === "SD Host") {
        ui.intPriority.hidden = false;
    }
    else {
        ui.intPriority.hidden = true;
    }
}

function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {
        let component = inst.$hardware;
        for (let sig in component.signals) {
            if (component.signals[sig].type) {
                let type = component.signals[sig].type;
                if (Common.typeMatches(type, ["SDHost_CLK"])) {
                    inst.interfaceType = "SD Host";
                    ui.interfaceType.readOnly = true;
                    return;
                }
                if (Common.typeMatches(type, ["SPI_CLK"])) {
                    inst.interfaceType = "SD SPI";
                    ui.interfaceType.readOnly = true;
                    return;
                }
            }
        }
    }

    ui.interfaceType.readOnly = false;
}

/*
 *  ======== extend ========
 */
function extend(base)
{
    devSpecific.config = base.config.concat(devSpecific.config);

    return (Object.assign({}, base, devSpecific));
}

exports = {
    extend: extend
};

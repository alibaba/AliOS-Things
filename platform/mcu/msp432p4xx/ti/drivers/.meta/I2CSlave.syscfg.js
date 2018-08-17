/*
 *  ======== I2CSlave configuration support ========
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "I2CSlave");

/* generic configuration parameters for I2CSlave instances */
let i2cSlaveConfig = []; /* nothing (yet) beyond generic driver configs */

/* Intro splash on GUI */
let longDescription = "The I2CSlave driver operates as a slave " +
    "on an I2C bus in either I2CSLAVE_MODE_BLOCKING or " +
    "I2CSLAVE_MODE_CALLBACK. In blocking mode, the task's " +
    "execution is blocked during the I2CSlave read/write transfer. " +
    "When the transfer has completed, code execution will resume. " +
    "In callback mode, the task's execution is not blocked, allowing " +
    "for other transactions to be queued up or to process some other " +
    "code. When the transfer has completed, the I2CSlave driver will " +
    "call a user-specified callback function (from a HWI context).";

/**
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let i2cSlave = {
        /* name is the name of a property that will be added to inst;
         * this property's value is an object returned by the
         * pinmux solver
         */
        name: "i2c",

        interfaceName: "I2C",

        /* request specific signals for I2C */
        resources: [
            {
                name: "sdaPin",            /* config script name */
                displayName: "SDA",        /* GUI name */
                interfaceNames: ["SDA"] /* pinmux tool name */
            },
            {
                name: "sclPin",
                displayName: "SCL",
                interfaceNames: ["SCL"]
            }
        ]

    };

    return ([i2cSlave]);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with I2CSlave
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinRequirement object if I2CSlave is supported.
 */
function filterHardware(component)
{
    let sda, scl;

    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["I2C_SCL"])) scl = sig;
        if (Common.typeMatches(type, ["I2C_SDA"])) sda = sig;
    }

    if (scl && sda) {
        return ({
            i2c: {
                sdaPin: [sda],
                sclPin: [scl]
            }
        });
    }

    return null;
}

/*
 *  ======== modules ========
 *  Return all used modules
 */
function modules(inst)
{
    return [
        {name : "Power", moduleName: "/ti/drivers/Power"}
    ];
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - I2CSlave instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/**
 *  ======== base ========
 *  Define the base/common I2CSlave properties and methods
 */
let base = {
    /* generic sysconfig module interface */
    displayName: "I2CSlave",
    description: "Slave Inter-Integrated Circuit (I2C) Bus Driver",
    longDescription: longDescription,
    defaultInstanceName: "Board_I2CSLAVE",
    config: i2cSlaveConfig,
    validate: validate,

    /* used modules */
    modules: modules,

    /* pinmux interface */
    busModule: true, /* true => instances of this module can be shared */
    filterHardware: filterHardware,
    pinmuxRequirements: pinmuxRequirements
};

/* get family-specific I2CSlave module */
let deviceI2CSlave = system.getScript("/ti/drivers/i2cslave/I2CSlave" + family);

/* extend our common exports to include the family-specific content */
exports = deviceI2CSlave.extend(base);

/*!
 *  CC26XX ADCBuf module
 *
 *  Device-specific ADCBuf configuration support
 */
"use strict";
/*global exports, system*/

/* $super is used to call generic module's methods */
let $super;

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Hardware interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "Software interrupt priority";

/*!
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base ADCBuf configuration
 */
let devSpecific = {
    maxInstances: 1,

    config: [
        intPriority,
        swiPriority,
        {
            name: "numChans",
            displayName: "Number of External Input Pins",
            description: "Maximum of 8 External Input Pins",
            default: 1,
            options: [
                { name: 1 },  { name: 2 },  { name: 3 },  { name: 4 },
                { name: 5 },  { name: 6 },  { name: 7 },  { name: 8 }
            ]
        },
        {
            name: "includeVDDS",
            displayName: "Include VDDS in Channel List",
            default: false
        },
        {
            name: "includeDCOUPL",
            displayName: "Include DCOUPL in Channel List",
            default: false
        },
        {
            name: "includeVSS",
            displayName: "Include VSS in Channel List",
            default: false
        }
    ],

    moduleInstances: moduleInstances,

    modules: Common.autoForceDMAModule,

    /* override generic pin requirements with device-specific pin reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/drivers/adcbuf/ADCBufCC26XX.Board.c.xdt",
        boardh: "/ti/drivers/adcbuf/ADCBufCC26XX.Board.h.xdt"
    },

    /* override generic validation with ours */
    validate: validate,

    filterHardware: filterHardware
};

/*!
 *  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  @param inst    - a fully configured ADC instance
 *
 *  @returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let pinArray = [];

    let adcChanNames = [
        "PIN0", "PIN1", "PIN2", "PIN3",
        "PIN4", "PIN5", "PIN6", "PIN7"
    ];

    for (let i = 0; i < inst.numChans; i++) {
        pinArray[i] = {
            name: "ADCBufChan" + i,
            displayName: "ADCBuf Channel " + i + " ",
            interfaceNames: adcChanNames
        };
    }

    let adcbuf = {
        name: "adc",
        hidden: true,
        displayName: "ADC Pin",
        interfaceName: "ADC",
        canShareWith: "ADC",
        resources: pinArray,
        signalTypes: { adcPin: ["AIN"] }
    };

    return ([adcbuf]);
}

/*!
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with ADC
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinRequirement object if ADC is supported.
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        if (component.signals[sig].type == "AIN") {
            return (true);
        }
    }
    return (false);
}

/*!
 *  ======== moduleInstances ========
 *  returns a GPTimer instance
 */
function moduleInstances(inst)
{
    let modInstances = new Array();

    modInstances.push(
        {
            name: "timerObject",
            displayName: "GP Timer Instance",
            moduleName: "/ti/drivers/Timer",
            hidden: false,
            hardware: inst.$hardware ? inst.$hardware : null,
            args: {
                timerType: "16 Bits",
                pinType: "ADCBuf"
            }
        }
    );

    for (let i = 0; i < inst.numChans; i++) {
        modInstances.push(
            {
                name: "adcbufPinInstance" + i,
                displayName: "ADCBuf Input PIN Configuration While Pin is Not In Use",
                moduleName: "/ti/drivers/PIN",
                collapsed: true,
                args: { parentMod: "/ti/drivers/ADCBuf",
                        parentInstanceName: inst.$name + " : ADCBuf Input",
                        parentInterfaceName: "adc",
                        parentSignalName:  "ADCBufChan" + i,
                        mode: "Input",
                        pull: "None" }
            }
        );
    }

    return (modInstances);
}

/*!
 *  ======== validate ========
 *  Perform CC26XX-specific validation
 *
 *  @param inst       - ADCBufCC26XX instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if ($super.validate) {
        $super.validate(inst, validation);
    }
}

/*!
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic ADCBuf module to
 *  allow us to augment/override as needed for the CC26XX
 */
function extend(base)
{
    /* save base properies/methods, to use in our methods */
    $super = base;

    /* concatenate device-specific configs */
    devSpecific.config = base.config.concat(devSpecific.config);

    /* merge and overwrite base module attributes */
    return (Object.assign({}, base, devSpecific));
}

/*!
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ADCBuf module */
    extend: extend
};

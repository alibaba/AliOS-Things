/*!
 * ======== PowerCC32XXPins.js ========
 * CC32XX Pins module (owned by PowerCC32XX)
 *
 * Device-specific Pins configuration support
 */

"use strict";
/* global exports, system */

let Common   = system.getScript("/ti/drivers/Common.js");
let pinTable = [1,2,3,4,5,6,7,8,13,15,16,17,18,19,20,21,29,30,
                45,50,52,53,55,57,58,59,60,61,62,63,64];
let pinOptions  = [
    {
        name: 'NO_PULL_HIZ',
        description: 'No pull resistor, leave pin in a HIZ state'
    },
    {
        name: 'WEAK_PULL_UP_STD',
        description: 'Pull-up resistor for standard pin type'
    },
    {
        name: 'WEAK_PULL_DOWN_STD',
        description: 'Pull-down resistor for standard pin type'
    },
    {
        name: 'WEAK_PULL_UP_OPENDRAIN',
        description: 'Pull-up resistor for open drain pin type'
    },
    {
        name: 'WEAK_PULL_DOWN_OPENDRAIN',
        description: 'Pull-down resistor for open drain pin type'
    },
    {
        name: 'DRIVE_LOW',
        description: 'Drive pin to a low logic state'
    },
    {
        name: 'DRIVE_HIGH',
        description: 'Drive pin to a high logic state'
    },
    {
        name: 'DONT_PARK',
        description: 'Take no action; do not park the pin'
    }
];

function padStart(x)
{
    if (x < 10) {
        x = "0" + x;
    }
    return x;
}

function resources()
{
    let resources = [
        {
            name          : 'default',
            displayName   : 'Change ALL Pins to:',
            onChange      : onChangeDefault,
            default       : 'WEAK_PULL_DOWN_STD',
            options       : pinOptions
        }
    ];

    for (let idx = 0; idx < pinTable.length; idx++) {
        let pinNum = padStart(pinTable[idx].toString());
        let defOpt = 'WEAK_PULL_DOWN_STD';
        if (pinNum == '55' || pinNum == '57') {
            defOpt = 'WEAK_PULL_UP_STD';
        }
        resources.push({
            name          : 'PIN'+pinNum,
            displayName   : 'PIN'+pinNum,
            default       : defOpt,
            options       : pinOptions
        });
    }
    return resources;
}

let config = resources();
function onChangeDefault(inst, ui)
{
    let initVal = inst.default;

    for (let idx = 0; idx < pinTable.length; idx++) {
        let pinName = 'PIN' + padStart(pinTable[idx].toString());
        inst[pinName] = initVal;
    }
}

/* The device specific exports for the power module */
exports = {
    name         : 'parkPins',
    displayName  : 'parkPins',
    maxInstances : 1,
    config       : config,
    templates    : {
        boardc      : "/ti/drivers/power/PowerCC32XXPins.Board.c.xdt",
    }
};

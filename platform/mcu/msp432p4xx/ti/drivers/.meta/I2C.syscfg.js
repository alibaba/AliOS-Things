/*
 *  ======== I2C ========
 */
"use strict";
/*global exports, system*/

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let logWarning = Common.logWarning;

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "I2C");

/* generic configuration parameters for I2C instances */
let i2cConfig = []; /* nothing (yet) beyond generic driver configs */

/* Intro splash on GUI */
let longDescription = "The I2C driver provides a simplified "
    + "application interface to access peripherals on an I2C bus.";

/**
 *  ======== getAddresses ========
 *  Get the range of valid I2C addresses to which a component may respond
 *
 *  Given an I2C component, extract and return the "actual" range of
 *  I2C addresses this component supports (as specified by the HW component).
 *  This is often a much smaller subset of the "full" range of addresses
 *  supported by the underlying peripheral that's documented by the
 *  peripheral's datasheet.  Boards often tie the peripheral's address
 *  selection pins to fixed levels that ensure all peripherals on the same
 *  I2C bus have unique addresses.
 *
 *  @param component - a board component assigned to $hardware; note, a bus
 *                     modules $hardware is an array of such components.
 *
 *  @returns - Array of numeric addresses declared in by the component's
 *             meta-data specification.  If a component is not an I2C
 *             component or no addresses are declared in the the componnet's
 *             meta-data, an empty array is returned.
 */
function getAddresses(component)
{
    let result = null;

    /* I2C addresses are declared in the component's settings object */
    if (component != null && component.settings != null) {

        /* find the I2C signals in the component */
        if (component.settings.address != null) {

            //console.log("I2C.getAddresses(" + component.name + "): address: "
            //        + component.settings.address);

            /* address settings can be a single string or an array */
            result = component.settings.address;
            if (!(result instanceof Array)) {
                result = [result];
            }
        }
    }

    return (result == null ? [] : result);
}

/*
 *  ======== modules ========
 *  Return all used modules
 */
function modules(inst)
{
    return [
        {name : "Power", moduleName: "/ti/drivers/Power", hidden: true}
    ];
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  In general, HW components can specify a range of
 *  addresses that are initialized at runtime; e.g., the INA226
 *  on the BOOSTXL-TLV8544PIR.  We use a simple solver to tell
 *  us whether
 *      o there exists any solution (involving unused components) and
 *      o if there is, which components need to be actively
 *        "managed" to properly initialize the HW component so
 *        as to not respond to addresses being actively used by
 *        the application.
 *
 *  Assumptions:
 *      o each module instance using an I2C bus validates that its I2C
 *        address is in the set of addresses allowed by the HW component.
 *      o the first entry in a HW component's I2C address array is its
 *        default address; i.e., in the absence of any SW initialization
 *        of the HW component, the device will respond to this I2C
 *        address.
 *  Checks:
 *      o all components connected to the I2C bus can be assigned unique
 *        addresses.
 *            - detects bogus booster pack combos
 *      o the set of default addresses of all unused components is disjoint
 *        from the set of addresses assigned to the used components.
 *            - detects an initialization problem (unused HW interfering with
 *              properly configured instances)
 *      o all components connected to the I2C bus can be assigned unique
 *        addresses using the addresses specified by the instances bound to
 *        the components
 *            - detects probable user error (requires runtime I2C addr control)
 *      o all instances bound to a common HW component on the I2C bus are
 *        configured with a common I2C address.
 *            - detects probable user error (requires runtime I2C addr control)
 *
 *  @param inst       - I2C instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if (inst.$hardware != null) {
        /* get _all_ HW components attached to this I2C bus */
        let components = inst.$hardware != null ? inst.$hardware.$parents : [];

        //Common.print(components, "I2C_validate(" + inst.$name + "), comp: ");

        /* gather all component address and usage information */
        let ca = _makeCompAddrArray(components, true);

        /* check for different addrs for the same component */
        for (let i = 0; i < ca.length; i++) {
            let usedBy = ca[i].usedBy;
            if (usedBy.length < 2) {
                continue;
            }
            let addr = usedBy[0].i2cAddress;
            let msg = "";
            for (let j = 1; j < usedBy.length; j++) {
                if (addr != usedBy[j].i2cAddress) {
                    msg += usedBy[j].$name + ":" + usedBy[j].i2cAddress + " ";
                }
            }
            if (msg.length > 0) {
                msg = "multiple addresses used to reference the component "
                      + ca[i].name + ": "
                      + usedBy[0].$name + ":" + usedBy[0].i2cAddress + " "
                      + msg;
                logError(validation, inst, "$hardware", msg);
                return;
            }
        }

        /* check for conflicts using addresses specified by instances */
        if (ca.length > 0 && _solve(ca) == false) {
            let msg = _genConflictMsg(ca);
            logError(validation, inst, "$hardware", msg);
        }

        /* check for conflicts between unused components and instances */
        let unused = {}; /* map addr -> unused component name with this addr */
        for (let i = 0; i < ca.length; i++) {
            let comp = ca[i];
            if (comp.usedBy.length == 0) {
                /* accumulate the default addresses of unused components */
                unused[comp.addrs[0]] = comp.name;
                //console.log("unused component " + comp.name + " addr: "
                //            + comp.addrs[0]);
            }
        }
        for (let i = 0; i < ca.length; i++) {
            let comp = ca[i];
            if (comp.usedBy.length > 0 && comp.index >= 0) {
                /* check that used addrs differ from the unused from above */
                let addr = comp.addrs[comp.index];
                //console.log("used component " + comp.name + " addr: "+addr);
                if (unused[addr] != null) {
                    logError(validation, inst, "$hardware",
                        comp.name
                        + " and the unused device " + unused[addr]
                        + " share the same address: 0x" + addr.toString(16));
                }
            }
        }
    }
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with I2C
 *
 *  @param component - hardware object describing signals and
 *                     pins to which they're attached
 *
 *  @returns true iff component is recognized as an I2C bus
 */
function filterHardware(component)
{
    let sda, scl;

    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["I2C_SCL"])) scl = sig;
        if (Common.typeMatches(type, ["I2C_SDA"])) sda = sig;
    }
    let result = (scl && sda) ? true : false;

    return (result);
}

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of an instance's config
 */
function pinmuxRequirements(inst)
{
    let i2c = {
        /* name is the name of a property that will be added to inst;
         * this property's value is an object returned by the
         * pinmux solver
         */
        name: "i2c",
        displayName: "I2C Peripheral",

        interfaceName: "I2C",            /* pinmux interface name */

        /* request specific signals for I2C */
        resources: [
            {
                name: "sdaPin",          /* config script name */
                displayName: "SDA Pin",  /* GUI name */
                interfaceNames: ["SDA"]  /* pinmux tool name */
            },
            {
                name: "sclPin",
                displayName: "SCL Pin",
                interfaceNames: ["SCL"]
            }
        ]
    };

    /* map injected pinmux solution names to our standard sig types */
    i2c.signalTypes = {
        sdaPin: ["I2C_SDA"],
        sclPin: ["I2C_SCL"]
    };

    return ([i2c]);
}

/*
 *  ======== _addrsOverlap ========
 *  Returns true iff two address arrays have a common element
 *
 *  @param a - an array of I2C addresses
 *  @param b - another array of I2C addresses

 *  @returns true iff an element of a[] equals an element of b[]
 */
function _addrsOverlap(a, b)
{
    let addrs = {};
    let i;

    for (i = 0; i < a.length; i++) {
        addrs[a[i]] = true;
    }
    for (i = 0; i < b.length; i++) {
        if (addrs[b[i]]) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== _genConflictMsg ========
 *  Generate an I2C address conflict error message
 *
 *  When _solve() fails to find a solution, use its output to generate a
 *  meaningful/actionable message.
 *
 *  @param ca - the array of objects returned from _solve()
 *  @returns  - a string containing an informative error message,
 *              or "" in the case there is no error.
 */
function _genConflictMsg(ca, header)
{
    /* if there's no conflict, return "" */
    if (ca[0].index >= 0) {
        return ("");
    }

    let msg = (header == null ? "" : header) 
              + "Conflicting I2C addresses between ";

    /* compute the last fatal conflict's addresses (solver sets index = -1) */
    let i;
    for (i = 0; i < ca.length; i++) {
        let comp = ca[i];
        if (comp.index >= 0) {
            break;
        }
    }
    let caddrs = ca[i - 1].addrs;

    /* append only components that overlap with these addresses (caddrs) */
    let overlaps = {};
    let prefix = "";
    for (let j = 0; j < i; j++) {
        let comp = ca[j];
        if (_addrsOverlap(caddrs, comp.addrs)) {
            msg += prefix + comp.name + ":[" + comp.addrs + "]";
            prefix = ", ";
            overlaps[comp.name] = comp.usedBy;
        }
    }

    /* reset ca, to look for _any_ solution supported by HW */
    for (let i = 0; i < ca.length; i++) {
        let comp = ca[i];
        comp.index = 0;          /* reset previous search state */
        comp.addrs = comp.range; /* search the full range of HW addresses */
    }
    
    /* append a suggestion: a solution supported by the HW */
    if (_solve(ca) == false) {
        msg += "; this appears to be in irreconcilable conflict. "
               + "Try not using one or more of the following components:\n";
        for (let j = 0; j < ca.length; j++) {
            let comp = ca[j];
            let refs = [];
            if (comp.usedBy.length > 0) {
                for (let k = 0; k < comp.usedBy.length; k++) {
                    refs.push(comp.usedBy[k].$name);
                }
                msg += "    " + comp.name + " (used by: " + refs + ")\n";
            }
        }
    }
    else {
        msg += "; try the following address assignments:\n";
        for (let cname in overlaps) {
            msg += "\t" + cname;
            let usedBy = overlaps[cname];
            if (usedBy.length > 0) {
                msg += "(used by " + usedBy[0].$name + ")";
            }
            msg += ": ";
            for (let j = 0; j < ca.length; j++) {
                let comp = ca[j];
                if (comp.name == cname) {
                    msg += "0x" + comp.addrs[comp.index].toString(16) + "\n";
                    break;
                }
            }
        }
    }

    return (msg);
}

/*
 *  ======== _genSolutionMsg ========
 *  Generate a printable I2C address assignment
 *
 *  When _solve() finds a solution, use its output to generate a
 *  meaningful message.
 *
 *  @param ca - the array of objects returned from _solve()
 *  @returns  - a string containing an informative message,
 *              or "" in the case there is novalid solution.
 */
function _genSolutionMsg(ca, header)
{
    /* if there's a conflict, return "" */
    if (ca[0].index < 0) {
        return ("");
    }

    /* sort used components ahead of unused then sort by address choices */
    ca.sort(function (a,b) {
        if (b.usedBy.length == a.usedBy.length) {
            return a.addrs.length - b.addrs.length;
        }
        return b.usedBy.length - a.usedBy.length;
    });

    let msg = header == null ? "" : header;
    for (let i = 0; i < ca.length; i++) {
        let comp = ca[i];
        msg += "    " + comp.name + ": 0x" + comp.addrs[comp.index].toString(16)
            + ((comp.usedBy.length == 0)
                ? " (unused)" : (" (used by " + comp.usedBy[0].$name + ")"))
            + "\n";
    }

    return (msg);
}

/*
 *  ======== _makeCompAddrArray ========
 *  Return an array of component address objects, one per HW component
 * 
 *  The returned array is used to find valid I2C address assignments for
 *  each component.  Specifically:
 *     o all used components must have unique addresses allowed by the HW
 *     o unused components may share addresses with other unused components
 *     o no unused component's address can be shared with a used component
 *
 *  Components are "used" (i.e., referenced) by an instance object's
 *  $hardware config _or_ by a module's static configs. An instance, say 
 *  inst, uses a component comp iff
 *      inst.$hardware.name = comp.name
 *
 *  and a module, say Mod, uses comp iff
 *      Mod.$static.$hardware.name == comp.name
 *
 *  All modules are found via the map system.modules.  Given a module, Mod,
 *  all its instances are provided via the array Mod.$instances
 *
 *  @param components  - an array of HW components connected to a common set
 *                       of I2C resources
 *
 *  @param useInstAddr - true => a referencing instance's I2C
 *                          address is taken as the _only_ allowed address for
 *                          the referenced HW component; all other components
 *                          have the full range of addresses allowed by the
 *                          board's HW.
 *                       false => the full range of addresses are used for
 *                           all components
 *
 *  @returns an array of objects, one for each component:
 *   {   name: "<comp name>",   // comp name
 *       addrs: [<addr> ...],   // allowable addresses for comp
 *       index: <addrs index>,  // addrs[index] is the addr for comp
 *       usedBy: []             // array of all instances that use comp or
 *                              // undefined (when useInstAddr == false)
 *   }
 */
function _makeCompAddrArray(components, useInstAddr)
{
    let c2i = {}; /* map comp names -> array of instances bound to comp */

    /* create map of comp names to array of instances bound to it */
    for (let j = 0; j < components.length; j++) {
        let comp = components[j];

        /* issue: should pre-compute "I2C instances" bound to HW */
        for (let moduleName in system.modules) {
            let mod = system.modules[moduleName];
            let instances = mod.$instances;
            for (let i = 0; i < instances.length; i++) {
                let xinst = instances[i];
                let xinstHW = xinst.$hardware ? xinst.$hardware.name : null;

                /* issue: assuming i2cAddress always has default */
                if (xinst.i2cAddress && xinstHW == comp.name) {
                    if (c2i[comp.name] == null) {
                        c2i[comp.name] = [];
                    }
                    c2i[comp.name].push(xinst);
                    //console.log(comp.name + " used by " + xinst.$name);
                }
            }
        }
    }

    /* create component addr solution array */
    let ca = [];
    for (let i = 0; i < components.length; i++) {
        let comp = components[i];

        /* get actual range of allowable addresses */
        let range = getAddresses(comp);

        /* compute a specified subset of range, addrs, to pass to the solver */
        let ia = c2i[comp.name];
        let addr = null;
        if (useInstAddr && ia != null) {
            /* if inst sets an addr, it's the only choice */
            addr = Number(ia[0].i2cAddress);
        }
        let addrs = addr ? [addr] : range;
        if (addrs.length > 0) {
            ca.push({
                name: comp.name, 
                addrs: addrs, 
                index: 0, 
                range: range,
                usedBy: (ia == null) ? [] : ia
            });
        }
    }

    return (ca);
}

/*
 *  ======== _solve ========
 *  Find a valid assignment of I2C addresses for the specified components
 *
 *  @param ca  - an array of components and their allowable addresses 
 *               returned by _makeCompAddrArray
 *
 *  @returns   - true => there is a solution, false => no solution
 *               In addition, the ca array contains the solution or a
 *               set of conflicting components
 * 
 *  Elements of ca are objects of the form:
 *  {
 *      name: <comp_name>,    // name of HW component
 *      addrs: [<addr>, ...], // set of allowable addresses
 *      index: 0,             // set by solver to index into addrs or -1
 *      usedBy: []            // may be null  must
 *  ]
 *
 */
function _solve(ca)
{
    //Common.print(ca, "_solve ca:\n");

    /* sort to maximize search tree pruning in backtrack */
    ca.sort(function (a,b) {return a.addrs.length - b.addrs.length;});

    let compIndex = 0;  /* curr comp to which to give an addr */
    let failIndex = 0;  /* max index considered before failing */
    let usedAddrs = {}; /* currently used addrs */
    while (compIndex >= 0 && compIndex < ca.length) {
        let comp = ca[compIndex];
        let caddr = comp.addrs[comp.index];

        /* if comp addr is unique, add a reference and move on to next comp */
        if (usedAddrs[caddr] == null || usedAddrs[caddr].length == 0) {
            usedAddrs[caddr] = [compIndex];
            compIndex++;
        }
        /* if caddr is used BUT both comp and caddr's owner are unused ... */
        else if (ca[usedAddrs[caddr][0]].usedBy.length == 0
                 && comp.usedBy.length == 0) {
            /* an overlap is allowable, so add the reference and move on */
            usedAddrs[caddr].push(compIndex);
            compIndex++;
        }
        else {
            /* otherwise, try next addr choice */
            comp.index++;

            /* save backtrack start (in case we need to reset comp.index) */
            failIndex = compIndex; /* used to generate failure message */

            /* if necessary, backtrack */
            while (comp.index >= comp.addrs.length) {
                comp.index = 0; /* reset current comp index */

                /* move back to prev comp, if possible */
                compIndex--;
                if (compIndex < 0) {
                    break; /* fail, no solution */
                }
                comp = ca[compIndex];

                /* remove our reference to prev addr from the used hash */
                usedAddrs[comp.addrs[comp.index]].pop();

                /* try new addr choice */
                comp.index++;
            }
        }
    }

    /* if no solution, annotate irreconcilable components (for err message) */
    if (compIndex < 0) {
        for (let i = 0; i <= failIndex; i++) {
            ca[i].index = -1; /* -1 => ca[i] is in the conflicting set */
        }
        return (false);
    }

    return (true);
}

/*
 *  ======== base ========
 *  Define the base/common I2C properties and methods
 */
let base = {
    /* generic sysconfig module interface */
    displayName:         "I2C",
    description:         "Inter-Integrated Circuit (I2C) Bus Driver",
    longDescription:     longDescription,
    defaultInstanceName: "Board_I2C",

    /* instance properties and methods */
    config:              i2cConfig,
    modules:             modules,    /* used modules */
    validate:            validate,

    /* pinmux interface */
    busModule:           true, /* true => this module's insts are shareable */
    filterHardware:      filterHardware,
    pinmuxRequirements:  pinmuxRequirements,

    /* I2C interface exported for other modules */
    getAddresses:        getAddresses,

    /* internal methods exposed for testing only */
    _genConflictMsg:     _genConflictMsg,
    _genSolutionMsg:     _genSolutionMsg,
    _solve:              _solve
};

/* get family-specific I2C module */
let deviceI2C = system.getScript("/ti/drivers/i2c/I2C" + family);

/* extend our common exports to include the family-specific content */
exports = deviceI2C.extend(base);

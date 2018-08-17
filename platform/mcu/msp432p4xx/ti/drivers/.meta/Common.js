/*
 *  ======== Common ========
 *  Common utility functions for /ti/drivers
 */

exports = {
    boardName: boardName,  /* get /ti/boards name */
    device2Family: device2Family,  /* get /ti/drivers device family name */
    device2DeviceFamily: device2DeviceFamily, /* get driverlib DeviceFamly_xxx name */
    device2DevicesDir: device2DevicesDir, /* get driverlib 'devices' dir */
    getName: getName,              /* get C name for instance */
    getPort: getPort,              /* get pin port name: Px_y */
    getInstanceIndex: getInstanceIndex, /* returns mod.$instances array index of inst */
    isCName: isCName,              /* validate name is C identifier */
    genAliases: genAliases,        /* generate list of instance name aliases */
    pinToName: pinToName,          /* convert pin number to pin name */
    intPriority2Hex: intPriority2Hex, /* translate logical priority into a
                                       * valid hex value
                                       */
    swiPriority2Int: swiPriority2Int,
    logError: logError,           /* push error onto into validation obj */
    logWarning: logWarning,       /* push warning onto into validation obj */
    logInfo: logInfo,             /* push remark onto into validation obj */

    newConfig: newConfig,         /* create /ti/drivers config structure */

    newIntPri: newIntPri,         /* create a common 'intPriority' config */

    newSwiPri: newSwiPri,         /* create a common 'swiPriority' config */

    print: print,                 /* debug: print specified object */
    printModule: printModule,     /* debug: print module configs and methods */
    printPins: printPins,         /* debug: print pinmux solution */

    useModule: useModule,

    typeMatches: typeMatches,
    setDefaults: setDefaults,

    validateNames: validateNames, /* validate inst names are unique C names */

    autoForcePowerModule: autoForcePowerModule,
    autoForceDMAModule: autoForceDMAModule,
    autoForcePowerAndDMAModules: autoForcePowerAndDMAModules,
    genBoardHeader: genBoardHeader,

    init: init
};

/*
 *  ======== IModule ========
 *  Names and types defined by _all_ sysconfig modules
 */
let IModule = {
    config               : [],
    defaultInstanceName  : "",
    description          : "",
    displayName          : "",
    filterHardware       : function (component){return false;},
    longDescription      : "",
    groups               : [], /* unused?? */
    layout               : [], /* unused?? */
    maxInstances         : 0,
    moduleInstances      : function (inst){return [];},
    moduleStatic         : {/* IModule declaration */},
    modules              : function (inst){return [];},
    onHardwareChanged    : function (inst, ui){},
    pinmuxRequirements   : function (inst){return [];},
    sharedModuleInstances: function (inst){return [];},
    templates            : {},
    validate             : function (inst, validation){}
};

/*
 *  ======== init ========
 */
function init()
{
}

/*
 *  ======== useModule ========
 */
function useModule(name)
{
    let mod = {};
    mod.$mod = scripting.addModule(name);

    /* hoist module statics to mod */
    let spec = system.getScript(mod.$mod.$name.split('$')[0]);
    if (spec.moduleStatic != null && spec.moduleStatic.config != null) {
        let configs = spec.moduleStatic.config;
        for (var i = 0; i < configs.length; i++) {
            let p = configs[i].name;
            let setter = function(x) { this.$mod[p] = x; };
            Object.defineProperty(mod, p, {set: setter});
        }
    }

    /* add create method */
    mod.create = function (name, params) {
        if (params == null) {
            params = {};
        }
        let inst = mod.$mod.addInstance();
        if (name) {
            inst.$name = name;
        }
        for (let p in params) {
            if ("$hardware" == p && params[p] === undefined) {
                console.log("warning: " + name
                            + ".$hardware was set to undefined");
            }
            if (params[p] !== undefined) {
                inst[p] = params[p];
            }
        }
        return (inst);
    };
    return (mod);
}


/*!
 *  ======== boardName ========
 *  Get the name of the board (or device)
 *
 *  @returns String - Name of the board with prefix /ti/boards and
 *                    suffix .syscfg.json stripped off.  If no board
 *                    was specified, the device name is returned.
 */
function boardName()
{
    let boardName = system.deviceData.deviceId;

    if (system.deviceData.board != null) {
        boardName = system.deviceData.board.source;

        /* Strip off everything up to and including the last '/' */
        boardName = boardName.replace(/.*\//, '');

        /* Strip off everything after and including the first '.' */
        boardName = boardName.replace(/\..*/, '');
    }
    return (boardName);
}

/*!
 *  ======== device2Family ========
 *  Map a pimux device object to a TI-driver device family string
 *
 *  @param device   - a pinmux device object (system.deviceData)
 *  @param mod      - module name ("ADC", "PWM", etc)
 *
 *  @returns String - the name of a device family that's used to
 *                    create family specifc-implementation module
 *                    names.  Returns null, in the event of an
 *                    unsupported device.
 */
function device2Family(device, mod)
{
    /* device.deviceId prefix -> /ti/drivers family name */
    let DEV2FAMILY = [
        {prefix: "CC13.2",   family: "CC26X2"},
        {prefix: "CC26.2",   family: "CC26X2"},
        {prefix: "CC13",     family: "CC26XX"},
        {prefix: "CC26",     family: "CC26XX"},
        {prefix: "CC32",     family: "CC32XX"},
        {prefix: "MSP432E",  family: "MSP432E4"},
        {prefix: "MSP432",   family: "MSP432"}
    ];

    /* Agama (CC26X2) specific module delegates */
    let cc26x2Mods = {
        "ECDH" : 1,
        "ECDSA" : 1,
        "ECJPAKE" : 1,
        "SHA2" : 1
    };

    /* deviceId is the directory name within the pinmux/deviceData */
    let deviceId = device.deviceId;

    for (let i = 0; i < DEV2FAMILY.length; i++) {
        let d2f = DEV2FAMILY[i];

        if (deviceId.match(d2f.prefix)) {
            /* trap Agama specific mods */
            if (d2f.family == "CC26X2") {
                if (cc26x2Mods[mod] == 1) {
                    return ("CC26X2");
                }
                else {
                    return ("CC26XX");
                }
            }
            else {
                return (d2f.family);
            }
        }
    }
    return (null);
}

/*!
 *  ======== device2DeviceFamily ========
 *  Map a pimux deviceID to a TI-driver device family string
 *
 *  @param deviceId  - a pinmux deviceId (system.deviceData)
 *
 *  @returns String - the corresponding "DeviceFamily_xxxx" string
 *                    used by driverlib header files.
 */
function device2DeviceFamily(deviceId)
{
    var driverString;

    /* Determine libraries required by device name. */
    if (deviceId.match(/CC13.2/)) {
        driverString = "DeviceFamily_CC13X2";
    }
    else if (deviceId.match(/CC13.0/)) {
        driverString = "DeviceFamily_CC13X0";
    }
    else if (deviceId.match(/CC26.0R2/)) {
        driverString = "DeviceFamily_CC26X0R2";
    }
    else if (deviceId.match(/CC26.2/)) {
        driverString = "DeviceFamily_CC26X2";
    }
    else if (deviceId.match(/CC26.0/)) {
        driverString = "DeviceFamily_CC26X0";
    }
    else if (deviceId.match(/CC3220/)) {
        driverString = "DeviceFamily_CC3220";
    }
    else if (deviceId.match(/MSP432E.*/)) {
        driverString = "DeviceFamily_MSP432E401Y";
    }
    else if (deviceId.match(/MSP432P4.1.I/)) {
        driverString = "DeviceFamily_MSP432P4x1xI";
    }
    else if (deviceId.match(/MSP432P4.1.T/)) {
        driverString = "DeviceFamily_MSP432P4x1xT";
    }
    else if (deviceId.match(/MSP432P401/)) {
        driverString = "DeviceFamily_MSP432P401x";
    }
    else {
        driverString = "";
    }

    return (driverString);
}

/*!
 *  ======== device2DevicesDir ========
 *  Map a pimux deviceId to a driverlib 'devices' dir
 *
 *  @param deviceId  - a pinmux deviceId (system.deviceData)
 *
 *  @returns String - the corresponding ti/devices/--- string
 */
function device2DevicesDir(deviceId)
{
    var devicesDir;

    /* Determine libraries required by device name. */
    if (deviceId.match(/CC13.2/)) {
        devicesDir = "cc13x2_cc26x2_v1";
    }
    else if (deviceId.match(/CC13.0/)) {
        devicesDir = "cc13x0";
    }
    else if (deviceId.match(/CC26.0R2/)) {
        devicesDir = "cc26x0r2";
    }
    else if (deviceId.match(/CC26.2/)) {
        devicesDir = "cc13x2_cc26x2_v1";
    }
    else if (deviceId.match(/CC26.0/)) {
        devicesDir = "cc26x0";
    }
    else if (deviceId.match(/CC3220/)) {
        devicesDir = "cc32xx";
    }
    else if (deviceId.match(/MSP432E.*/)) {
        devicesDir = "msp432e4";
    }
    else if (deviceId.match(/MSP432P4.1.I/)) {
        devicesDir = "msp432p4x1xi";
    }
    else if (deviceId.match(/MSP432P4.1.T/)) {
        devicesDir = "msp432p4x1xt";
    }
    else if (deviceId.match(/MSP432P401/)) {
        devicesDir = "msp432p401x";
    }
    else {
        devicesDir = "";
    }

    return (devicesDir);
}

/*!
 *  ======== getInstanceIndex ========
 *  returns module.$instances array index of this inst
 */
function getInstanceIndex(inst)
{
    let instances = inst.$module.$instances;
    for (let i = 0; i < instances.length; i++) {
        if (inst == instances[i]) return (i);
    }
    return (-1);
}

/*
 *  ======== getName ========
 *  Compute C name for specified instance
 */
function getName(inst, index)
{
    return (inst.$name);
}

/*
 *  ======== getPort ========
 *  Return pin port name or ""
 *
 *  @param pin - pin object representing the
 *               solution found by pinmux
 *  @returns string of the form P<port>_<index>
 */
function getPort(pin)
{
    let port = "";
    if (pin != null && pin.$solution != null) {
        let pname = pin.$solution.devicePinName;
        port = pname.match(/P\d+\.\d+/);
        port = port ? port[0] : "";

    }
    return port.replace(".", "_");
}

/*
 *  ======== isCName ========
 *  Determine if specified id is either empty or a valid C identifier
 *
 *  @param id  - String that may/may not be a valid C identifier
 *
 *  @returns true if id is a valid C identifier OR is the empty
 *           string; otherwise false.
 */
function isCName(id)
{
    if ((id != null && id.match(/^[a-zA-Z_][0-9a-zA-Z_]*$/) != null)
            || id == '') { /* '' is a special value that means "default" */
        return true;
    }
    return false;
}

/*
 *  ======== genAliases ========
 */
function genAliases(inst, name)
{
    let lines = [];
    if (inst.cAliases != null && inst.cAliases.length > 0) {
        let aliases = inst.cAliases.split(/[,;\s]+/);
        for (let i = 0; i < aliases.length; i++) {
            let alias = aliases[i];
            lines.push("    " + alias + " = " + name + ",");
        }
        lines.push('');
    }
    return (lines.join('\n'));
}

/*
 *  ======== logError ========
 *  Log a new error
 *
 *  @param vo     - validation object associated with inst
 *  @param inst   - module instance object
 *  @param field  - instance property name with which this error is associated
 */
function logError(vo, inst, field, msg)
{
    if (typeof(field) === 'string') {
        vo[field].errors.push(msg);
    }
    else {
        for (let i = 0; i < field.length; i++) {
            vo[field[i]].errors.push(msg);
        }
    }
}

/*
 *  ======== logInfo ========
 *  Log a new remark
 *
 *  @param vo     - validation object associated with inst
 *  @param inst   - module instance object
 *  @param field  - instance property name with which this remark is associated
 */
function logInfo(vo, inst, field, msg)
{
    if (typeof(field) === 'string') {
        vo[field].infos.push(msg);
    }
    else {
        for (let i = 0; i < field.length; i++) {
            vo[field[i]].infos.push(msg);
        }
    }
}

/*
 *  ======== logWarning ========
 *  Log a new warning
 *
 *  @param vo     - validation object associated with inst
 *  @param inst   - module instance object
 *  @param field  - instance prop name with which this warning is associated
 */
function logWarning(vo, inst, field, msg)
{
    if (typeof(field) === 'string') {
        vo[field].warnings.push(msg);
    }
    else {
        for (let i = 0; i < field.length; i++) {
            vo[field[i]].warnings.push(msg);
        }
    }
}

/*
 *  ======== newConfig ========
 */
function newConfig()
{
    let config = [
        /* issue: how to provide help tooltip? */
        {
            name: "cAliases",
            /* first arg to GPIO functions */
            displayName: "Alternate Names",
            description: "Must be a comma separated list of valid C identifiers",
            /* allow special format char */
            default: ""
        }
    ];

    return (config);
}

/*
 *  ======== newIntPri ========
 *  Create a new intPriority config parameter
 *
 *  Presents a constrained set of logical NVIC interrupt priorities,
 *  excluding priority 0, which is the "Zero Latency"
 *  interrupt priority.
 */
function newIntPri()
{
    let intPri = [{
        name: "intPriority",
        displayName: "Interrupt Priority",
        default: "7",
        options: [
            { name: "7", displayName: "7 - Lowest Priority" },
            { name: "6" },
            { name: "5" },
            { name: "4" },
            { name: "3" },
            { name: "2" },
            { name: "1", displayName: "1 - Highest Priority" }
        ]
    }];

    return (intPri);
}

/*
 * ======== intPriority2Hex ========
 * translate user readable priority into NVIC priority value
 */
function intPriority2Hex(intPri)
{
    return ("0x" + (intPri << 5).toString(16));
}

/*
 *  ======== newSwiPri ========
 *  Create a new swiPriority config parameter
 */
function newSwiPri()
{
    let swiPri = {
        name: "swiPriority",
        displayName: "Swi Priority",
        default: "0",
        options: [
            { name: "0", displayName: "0 - Lowest Priority" },
            { name: "1" },
            { name: "2" },
            { name: "3" },
            { name: "4" },
            { name: "5" },
            { name: "6" },
            { name: "7" },
            { name: "8" },
            { name: "9" },
            { name: "10" },
            { name: "11" },
            { name: "12" },
            { name: "13" },
            { name: "14" },
            { name: "15", displayName: "15 - Highest Priority" }
        ]
    };

    return (swiPri);
}

/*
 * ======== swiPriority2Int ========
 * translate user priority to integer
 */
function swiPriority2Int(swiPri)
{
    return (swiPri);
}

/*
 *  ======== pinToName ========
 * Translate device pin number into pin name
 */
function pinToName(pinNum)
{
    for (let x in system.deviceData.devicePins) {
        if (system.deviceData.devicePins[x].ball == pinNum) {
            let desc = String(system.deviceData.devicePins[x].description);
            return (desc);
        }
    }

    return ("");
}

/*
 *  ======== print ========
 *  Print specified obj
 */
function print(obj, header, depth, indent)
{
    if (indent == null) indent = "";
    if (header == null) header = "";
    if (depth == null) depth = 4;
    if (indent.length > 2 * depth) return;

    if (obj == null) {
        console.log(header + (obj === null ? "null" : "undefined"));
        return;
    }
    if (indent == "") {
        if (obj.$name != null) {
            console.log(header + obj.$name + ":");
        }
        else {
            console.log(header + obj + " (nameless):");
        }
    }

    for (let p in obj) {
        /* print the enumerable properties of obj and their value */
        let line = indent + "  " + p + ": ";

        let value = obj[p];

        let fxn = null;
        if (typeof value == "function") {
            try {
                fxn = String(value).match(/.*\n/);
            } catch (x){/* ignore any exception */}
        }
        if (fxn != null) {
            /* only print declaration of functions (not their entire source) */
            console.log(line + fxn);
        }
        else if (value != null && (typeof value == "object")) {
            console.log(line
                + (("$name" in value) ? (" (" + value.$name + ")") : ""));

            if (p[0] != '$') {
                /* recursively print (non-tool) object properties */
                print(value, "", depth, indent + "  ");
            }
        }
        else {
            /* print non-object values */
            console.log(line + value);
        }
    }
}

/*
 *  ======== printModule ========
 */
function printModule(mod, header, indent)
{
    if (indent == null) indent = "";
    if (header == null) header = "";

    function printConfigs(configs, indent) {
        if (configs != null && configs.length != 0) {
            /* maxPad is the maximum field width */
            let maxPad = "                           ";

            /* compute max config name field width */
            let maxName = 1;
            for (let i = 0; i < configs.length; i++) {
                let len = configs[i].name.length;
                if (len > maxName) {
                    maxName = len;
                }
            }
            if (maxName > maxPad.length) {
                maxName = maxPad.length;
            }

            /* compute max config default value field width */
            let maxValue = 1;
            for (let i = 0; i < configs.length; i++) {
                let len = String(configs[i].default).length;
                if (len > maxValue) {
                    maxValue = len;
                }
            }
            if (maxValue > maxPad.length) {
                maxValue = maxPad.length;
            }

            /* print all configs */
            for (let i = 0; i < configs.length; i++) {
                let nLen = configs[i].name.length;
                let vLen = String(configs[i].default).length;
                var npad = maxPad.substring(0, maxName - nLen);
                var vpad = maxPad.substring(0, maxValue - vLen);
                printCfg(configs[i], indent + "  ", npad, vpad);
            }
        }
        else {
            console.log(indent + "  <none>");
        }
    }

    function printCfg(cfg, indent, npad, vpad) {
        npad = npad == null ? "" : npad;
        vpad = vpad == null ? "" : vpad;
        let quote = " ";
        if (typeof cfg.default == "string" || cfg.default instanceof String) {
            quote = "'";
        }
        let desc = cfg.description != null ? (" - " + cfg.description) : "";
        console.log(indent + cfg.name + npad
                    + ": " + quote + cfg.default + quote + vpad
                    + " <" + cfg.displayName + desc + ">"
        );
        printCfgOptions(cfg, indent, quote);
    }

    function printCfgOptions(cfg, indent, quote) {
        /* maxPad is the maximum field width */
        let maxPad = "                           ";

        if (cfg.options != null) {
            let maxName = 0;
            for (let i = 0; i < cfg.options.length; i++) {
                let name = cfg.options[i].name;
                if (maxName < name.length) {
                    maxName = name.length;
                }
            }

            let prefix = indent + "   " + quote;
            for (let i = 0; i < cfg.options.length; i++) {
                let opt = cfg.options[i];
                let desc = opt.description != null ? (" - " + opt.description) : "";
                let displayName = opt.displayName == null ? opt.name : opt.displayName;
                let suffix = ((opt.displayName == opt.name || opt.displayName == null) && desc.length == 0) ? "" : (" <" + displayName + desc +  ">");
                let namePad = maxPad.substring(0, maxName - opt.name.length + 1);
                console.log(prefix + opt.name + quote + namePad + suffix );
            }
        }
    }

    function printLine(line, indent, maxLen) {
        let curline = line;
        while (curline.length > 0) {
            let lastSpace;
            let i;

            /* trim leading space */
            for (i = 0; (curline[i] == ' ') && (i < curline.length);) {
                i++;
            }
            curline = curline.substring(i);
            if (curline.length == 0) return;

            /* find last white space within maxLen characters */
            lastSpace = 0;
            for (i = 0; (i < curline.length) && (i < maxLen); i++) {
                if (curline[i] == ' ') {
                    lastSpace = i;
                }
            }
            if (i == curline.length) {
                lastSpace = i;
            }
            else if (i == maxLen && lastSpace == 0) {
                /* advance i to first space or end of string */
                while ((i < curline.length) && (curline[i] != ' ')) {
                    i++;
                }
                lastSpace = i;
            }

            /* print up to lastSpace and reset curline */
            console.log(indent + curline.substring(0, lastSpace));
            curline = curline.substring(lastSpace);
        }
    }

    /* get the real module object (gack!) */
    mod = system.getScript(mod.$name.split('$')[0]);
    console.log(header + mod.$name + " - " + mod.description);

    indent += "  ";

    console.log(indent + "Module Description");
    if  (mod.longDescription) {
        let lines = mod.longDescription.split('\n');
        for (let i = 0; i < lines.length; i++) {
            printLine(lines[i], indent + "  ", 60);
        }
    }
    else {
        console.log(indent + "  <none>");
    }

    console.log("\n" + indent + "Module Configuration Options");
    if (mod.moduleStatic != null) {
        printConfigs(mod.moduleStatic.config, indent);
    }
    else {
        console.log(indent + "  <none>");
    }

    console.log("\n" + indent + "Instance Configuration Options");
    printConfigs(mod.config, indent);

    console.log("\n" + indent + "Module Methods");
    let methods = [];
    for (let m in mod) {
        if (typeof mod[m] == "function") {
            let mname = String(mod[m]).match(/.*\n/);
            mname = mname[0].replace(m + "(", "(");
            let prefix = "";
            if (m in IModule && typeof IModule[m] == "function") {
                prefix = "IModule::";
            }
            methods.push(indent + "  " + prefix + m + ": " + mname);
        }
    }
    console.log(methods.sort().join('\n'));
}

/*
 *  ======== printPins ========
 *  Print specified pinmux solution
 */
function printPins(obj, indent)
{
    if (indent == null) {
        indent = "    ";
    }
    if (obj == null) {
        console.log(obj === null ? "null" : "udefined");
        return;
    }
    let solution = obj.$solution;
    let resources = obj.$resources;
    let peripheral = system.deviceData.peripherals[solution.peripheralName];
    console.log(indent + "peripheral.name = " + peripheral.name);
    for (let p in resources) {
        console.log(indent + "    resources." + p
                    + " = " + resources[p].$solution.devicePinName);
        //            for (let q in resources[p].$solution) {
        //                console.log("          " + q + ": "
        //                    + resources[p].$solution[q]);
        //            }
    }
}

/*
 *  ======== typeMatches ========
 *  Check that HW signal type matches a specified array of types
 *
 *  Example: within a module's filterHardware(component) method:
 *      for (sig in component.signals) {
 *          let type = component.signals[sig].type;
 *          if (Common.typeMatches(type, ["PWM", "DOUT"])) {
 *              :
 *          }
 *      }
 *
 *  type      - a string or array of strings that are valid signal types
 *  nameArray - array of signal name types that must match one of the signal
 *              types named by type
 *
 *  Returns true iff nameArray contains at least one type name that's
 *          specified the type parameter.
 */
function typeMatches(type, nameArray)
{
    let options = {};

    if (type instanceof Array || typeof type == "object") {
        for (let i = 0; i < type.length; i++) {
            options[type[i]] = 1;
        }
    }
    else if (typeof type == "string" || type instanceof String) {
        options[type] = 1;
    }

    for (let i = 0; i < nameArray.length; i++) {
        let name = nameArray[i];
        if (name in options) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== setDefaults ========
 *  Copy properties from a signal's settings into inst
 *
 *  inst - the intance whose properties need to be modified
 *  signal - a signal object defined by a component
 *  type - a specific type for the signal (DOUT, PWM, ...)
 *
 *  If type is undefined or null, no properties of inst are modified.
 */
function setDefaults(inst, signal, type)
{
    let settings = {};

    /* populate settings hash from the specified signal settings */
    if (signal.settings != null) {
        if (signal.type instanceof Array && signal.type.length > 1) {
            settings = signal.settings[type];
            /* if settings == null, there aren't settings for this type */
        }
        else {
            settings = signal.settings;
        }
    }

    /* allow component settings to override signal settings */
    let comp = inst.$hardware;
    if (comp.settings != null) {
        let csettings = null;
        if (comp.type instanceof Array && comp.type.length > 1) {
            csettings = comp.settings[type];
            /* if settings == null, there aren't settings for this type */
        }
        else {
            csettings = comp.settings;
        }
        for (let cfg in csettings) {
            settings[cfg] = csettings[cfg];
        }
    }

    /* apply any settings to the instance */
    for (let cfg in settings) {
        try {
            inst[cfg] = settings[cfg];
        }
        catch (x) {
            console.log("warning: signal '" + signal.name 
                + "' of component " + comp.name 
                + " specified an unknown setting (" + cfg + ") for " 
                + inst.$name);
        }
    }
}

/*
 *  ======== validateNames ========
 *  Validate that all names defined by inst are globally unique and
 *  valid C identifiers.
 */
function validateNames(inst, validation)
{
    let myNames = {}; /* all C identifiers defined by inst) */

    /* check that $name is a C identifier */
    if (inst.$name != "") {
        let token = inst.$name;
        if (!isCName(token)) {
            logError(validation, inst, "$name",
                "'" + token + "' is not a valid a C identifier");
        }
        myNames[token] = 1;
    }

    /* check that cAliases are all C identifiers and there are no dups */
    let tokens = [];
    if ("cAliases" in inst && inst.cAliases != "") {
        tokens = inst.cAliases.split(/[,;\s]+/);
    }

    for (let i = 0; i < tokens.length; i++) {
        let token = tokens[i];
        if (!isCName(token)) {
            logError(validation, inst, "cAliases",
                "'" + token + "' is not a valid a C identifier");
        }
        if (myNames[token] != null) {
            logError(validation, inst, "cAliases",
                "'" + token + "' is defined twice");
        }
        myNames[token] = 1;
    }

    /* ensure all inst C identifiers are globally unique */
    let mods = system.modules;
    for (let i in mods) {
        /* for all instances in all modules in /ti/drivers ... */
        let instances = mods[i].$instances;
        for (let j = 0; j < instances.length; j++) {
            let other = instances[j];

            /* skip self */
            if (inst.$name == other.$name) {
                continue;
            }

            /* compute all other names */
            let name = other.$name;
            if (name != "" && name in myNames) {
                logError(validation, inst, "cAliases",
                    "multiple instances with the same name: '"
                         + name + "': " + inst.$name + " and " + other.$name);
                break;
            }
            if (other.cAliases != null && other.cAliases != "") {
                let tokens = other.cAliases.split(/[,;\s]+/);
                for (let k = 0; k < tokens.length; k++) {
                    name = tokens[k];
                    if (name != "" && name in myNames) {
                        logError(validation, inst, "cAliases",
                            "multiple instances with the same name: '" + name
                                 + "': " + inst.$name + " and " + other.$name);
                        break;
                    }
                }
            }
        }
    }
}

/*
 *  ======== autoForcePowerModule ========
 *  Return an Array with the Power module in it,
 *  hidden from the GUI.
 *
 *  Using this function as the value for a key of "modules" in the
 *  definition of module X, will automatically force the Power module to be
 *  added, when the module X is added. Like the following:
 *
 *    modules: Common.autoForcePowerModule,
 */
function autoForcePowerModule(inst)
{
    return [
        {
            name : "Power",
            moduleName: "/ti/drivers/Power",
            hidden: true
        }
    ];
}

/*
 *  ======== autoForceDMAModule ========
 *  Return an Array with the DMA module in it,
 *  hidden from from the GUI.
 *
 *  Using this function as the value for a key of "modules" in the
 *  definition of module X, will automatically force the DMA module to be
 *  added, when the module X is added. Like the following:
 *
 *    modules: Common.autoForceDMAModule,
 */
function autoForceDMAModule(inst)
{
    return [
        {
            name : "DMA",
            moduleName: "/ti/drivers/DMA",
            hidden: true
        }
    ];
}

/*
 *  ======== autoForcePowerAndDMAModules ========
 *  Return an Array with the Power and DMA modules in it,
 *  hidden from the GUI.
 *
 *  Using this function as the value for a key of "modules" in the
 *  definition of module X, will automatically force the Power and
 *  DMA modules to be added, when the module X is added. Like
 *  the following:
 *
 *    modules: Common.autoForcePowerAndDMAModules,
 */
function autoForcePowerAndDMAModules(inst)
{
    let modArray = autoForcePowerModule(inst);
    modArray.push({
        name : "DMA",
        moduleName: "/ti/drivers/DMA",
        hidden: true
    });
    return (modArray);
}

/*
 *  ======== genBoardHeader ========
 *  Common Board.h.xdt function to generate standard module header
 *  including instance #defines
 *
 *  instances = array of module instances
 *  mod       = module object
 */
function genBoardHeader(instances, mod)
{
    let padding = Array(80).join(' ');
    let maxLineLength = 30;
    let lines = [];
    let banner = [];
    banner.push("/*");
    banner.push(" *  ======== " + mod.displayName + " ========");
    banner.push(" */");
    banner.push("");
    let line;
    for (let i = 0; i < instances.length; i++) {
        let inst = instances[i];
        line = "#define " + inst.$name;
        lines.push(line);
        if (line.length > maxLineLength) maxLineLength = line.length;
    }
    maxLineLength = ((maxLineLength + 3) & 0xfffc) + 4;
    for (let i = 0; i < instances.length; i++) {
        lines[i] += padding.substring(0, maxLineLength - lines[i].length);
        lines[i] += i;
    }
    return ((banner.concat(lines)).join("\n"));
}

/*
 *  ======== xdcArgParser.xs ========
 */

/*
 *  ======== getDeviceWareOpts========
 *  Parses XDCARGS returning an object with paths to device *wares.
 */
function getDeviceWareOpts(xdcArgs)
{
    /* Get paths from XDCARGS options */
    var argMap = parseArgs(xdcArgs);

    return ({
        "cc26xxware": argMap["cc26xxware"],
        "cc32xxware": argMap["cc32xxware"],
        "msp432DriverLib": argMap["msp432DriverLib"],
        "msp432e4DriverLib": argMap["msp432e4DriverLib"]
    });
}

/* Returns a JSON map object of XDCARGS */
function parseArgs(xdcArgs) {
    var argMap = {};

    /* Parse product versions from XDCARGS options */
    for each (var arg in xdcArgs.split(" ")) {
        if (arg.split('=').length < 2) {
            /* skip undefined arguments */
            continue;
        }

        var path = String(arg.split("=").pop());

        if (arg.match(/^CC26XXWARE=/)) {
            argMap["cc26xxware"] = path;
        }

        if (arg.match(/^CC32XXWARE=/)) {
            argMap["cc32xxware"] = path;
        }

        if (arg.match(/^MSP432DRIVERLIB=/)) {
            argMap["msp432DriverLib"] = path;
        }

        if (arg.match(/^MSP432E4DRIVERLIB=/)) {
            argMap["msp432e4DriverLib"] = path;
        }
    }

    return argMap;
}

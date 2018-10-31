/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== Config.xs ========
 */

var ClockFreqs;
var Config;
var Program;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    Config = xdc.module('ti.drivers.Config');
    Program = xdc.module('xdc.cfg.Program');

    /* Prevent false failure when building the module. */
    if (Program.cpu) {
        determineSupport();
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    var warning = "The ti.drivers.Config module is deprecated " +
        "and will not be supported in future releases.\n" +
        "    [1] Remove this module from your .cfg file\n" +
        "    [2] Add the driver-related libraries directly to the link line " +
        "in your project or makefile.\n" +
        "    [3] Add the following lines to your .cfg file to add " +
        "the Power module's idle function to the idle loop:\n\n" +
        "        Idle = xdc.useModule('ti.sysbios.knl.Idle');\n" +
        "        Idle.addFunc('&Power_idleFunc');\n";

    /* Add driver dependencies & requirements */
    Program = xdc.module('xdc.cfg.Program');

    /* Add ports library */
    xdc.loadPackage('ti.dpl');

    /*
     * Power driver (ti.drivers.Power) is supported on CC13xx, CC26xx,
     * CC32XX & MSP432
     */
    if ((Program.cpu.deviceName.match(/CC32/)) ||
        (Program.cpu.deviceName.match(/CC13/)) ||
        (Program.cpu.deviceName.match(/CC26/)) ||
        (Program.cpu.deviceName.match(/MSP432/))) {
        xdc.useModule('ti.drivers.Power');
    }

    if (Program.cpu.deviceName.match(/CC13/) ||
        Program.cpu.deviceName.match(/CC26/)) {
        xdc.loadPackage("ti.drivers.pdm");
        xdc.loadPackage("ti.drivers.rf");
    }

    /* Add ClockFreqs module to MSP432 */
    if (Program.cpu.deviceName.match(/MSP432/)) {
        ClockFreqs = xdc.useModule('ti.sysbios.family.arm.msp432.ClockFreqs');
        warning += "\n    [4] Also add the following to your .cfg file to link in " +
                   "the ClockFreqs module required by the Power driver:\n\n" +
                   "        xdc.useModule('ti.sysbios.family.arm.msp432.ClockFreqs');\n";
    }

    Config.$logWarning(warning, Config);
}

/*
 *  ======== determineSupport ========
 */
function determineSupport()
{
    if (Program.cpu.deviceName.match(/CC[12].*/)) {
        Config.supportsRFDriver = true;
    }
    else {
        Config.supportsRFDriver = false;
    }
}

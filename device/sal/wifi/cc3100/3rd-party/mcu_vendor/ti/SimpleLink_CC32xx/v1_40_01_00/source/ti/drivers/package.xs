/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
 *  ======== package.xs ========
 */

/*
 *  ======== getLibs ========
 *  This package does not build any libraries.
 */
function getLibs()
{
    var driversConfig = xdc.module("ti.drivers.Config");
    var driversBase = xdc.loadPackage("ti.drivers").packageBase;

    var driverString = "";
    var instrumentedString = "";

    var retString = "";
    var targetSuffix = "";

    if (!driversConfig.$used) {
        return (null);
    }

    /* Determine libraries required by device name. */
    if (Program.cpu.deviceName.match(/CC32.*/)) {
        driverString = "_cc32xx";
    }
    else if (Program.cpu.deviceName.match(/CC13.*/)) {
        driverString = "_cc13x0";
    }
    else if (Program.cpu.deviceName.match(/CC26.*R2/)) {
        driverString = "_cc26x0r2";
    }
    else if (Program.cpu.deviceName.match(/CC2652.*/)) {
        libName = "_eng_cc26x2_cc26x4_fpga";
    }
    else if (Program.cpu.deviceName.match(/CC2654.*/)) {
        libName = "_eng_cc26x2_cc26x4_fpga";
    }
    else if (Program.cpu.deviceName.match(/CC26.*/)) {
        driverString = "_cc26x0";
    }
    else if (Program.cpu.deviceName.match(/MSP432.*/)) {
        driverString = "_msp432p4xx";
    }
    else {
        throw ("Driver not found for this device " + Program.cpu.deviceName +
               " and target " + Program.build.target.suffix);
    }

    /* Get instrumentation preference */
    if (driversConfig.libType == driversConfig.LibType_Instrumented) {
        instrumentedString = "_instrumented";
    }

    targetSuffix = ".a" + Program.build.target.suffix + ";";

    retString += driversBase + "/lib/drivers" + driverString +
        instrumentedString + targetSuffix;

    /* strip duplicate '//' characters from the library list */
    return (retString.replace(/\/\/+/g, '/'));
}

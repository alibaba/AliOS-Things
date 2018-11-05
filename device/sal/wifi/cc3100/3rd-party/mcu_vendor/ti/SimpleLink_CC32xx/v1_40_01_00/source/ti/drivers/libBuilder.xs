/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 *  ======== libBuilder.xs ========
 */

var buildInstrumentedLibrary = false; /* don't build instrumented libraries */

/*
 *  ======== makeLibs ========
 */
function makeLibs(name, targets, objects, cOpts)
{
    var lib;
    var target;
    var attrs = {
        copts: "",
        profile: "release"
    };

    for each (var targetName in targets) {
        /* Only proceed if target already in Build object */
        if (Build.printBuildTargets().indexOf(targetName + "\n") < 0) {
            continue;
        }

        /* Load target module */
        var target = xdc.module(targetName);

        /* Disable asserts & logs for the non-instrumented library */
        attrs.copts = cOpts +
            " -Dxdc_runtime_Log_DISABLE_ALL -Dxdc_runtime_Assert_DISABLE_ALL";
        lib = Pkg.addLibrary(name, target, attrs);
        lib.addObjects(objects);

        if (buildInstrumentedLibrary) {
            /* Enable asserts & logs for the instrumented library */
            attrs.copts = cOpts +
                " -Dxdc_runtime_Assert_DISABLE_CONDITIONAL_ASSERT" +
                " -DDebugP_ASSERT_ENABLED -DDebugP_LOG_ENABLED";
            lib = Pkg.addLibrary(name + "_instrumented", target, attrs);
            lib.addObjects(objects);
        }
    }
}

var m3Targets = [
    "ti.targets.arm.elf.M3",
    "iar.targets.arm.M3",
    "gnu.targets.arm.M3",
];

var m4Targets = [
    "ti.targets.arm.elf.M4",
    "gnu.targets.arm.M4",
    "iar.targets.arm.M4",
];

var m4fTargets = [
    "ti.targets.arm.elf.M4F",
    "gnu.targets.arm.M4F",
    "iar.targets.arm.M4F",
];

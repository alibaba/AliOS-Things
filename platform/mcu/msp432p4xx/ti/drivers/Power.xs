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
 *  ======== Power.xs ========
 */

var Clock;
var Idle;
var Timestamp;
var Swi;

/*
 *  ======== module$use ========
 *  This function will automatically add the Power policy to the idle loop.
 */
function module$use()
{
    Power = xdc.module("ti.drivers.Power");
    BIOS = xdc.module('ti.sysbios.BIOS');

    /*
     *  Warn if trying to add Power to anything other than CC32XX, MSP432,
     *  CC26XX, or CC13XX
     */
    if (!Program.cpu.deviceName.match(/CC32/) &&
        !Program.cpu.deviceName.match(/CC26/) &&
        !Program.cpu.deviceName.match(/CC13/) &&
        !Program.cpu.deviceName.match(/MSP432/)) {
        Power.$logWarning("Power driver is not supported on the " +
            Program.cpu.deviceName + " device.",  Power);

        return;
    }

    /* Swi is used in Power policy to disable scheduling */
    if (BIOS.swiEnabled == true) {
        Swi = xdc.useModule('ti.sysbios.knl.Swi');
    }

    /* Add Idle module and Power idle fxn */
    Idle = xdc.useModule('ti.sysbios.knl.Idle');
    Idle.addFunc('&Power_idleFunc');
}

/*
 *  ======== module$validate ========
 */
function module$validate()
{
    Power = xdc.module("ti.drivers.Power");
    BIOS = xdc.module('ti.sysbios.BIOS');

    /* If CC32XX, check that Clock timer delegate is compatible with sleep */
    if (Program.cpu.deviceName.match(/CC32/)) {

        if (BIOS.clockEnabled) {
            Clock = xdc.module('ti.sysbios.knl.Clock');
            if (Clock.TimerProxy != null && !Clock.TimerProxy.delegate$.$name.match(
                /ti.sysbios.family.arm.m3.Timer/)) {
                Power.$logWarning("The specified Clock.TimerProxy (" + Clock.TimerProxy.delegate$.$name + ") is not compatible with the reference CC32XX sleep policy.", this);
            }
        }
    }
}

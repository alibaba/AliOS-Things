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
 *  ======== Config.xdc ========
 *  This module is used to tie together the xGConf page for TI Drivers product.
 */

/*!
 *  ======== Config ========
 */

metaonly module Config {
    /*!
     *  ======== LibType ========
     *  Configuration options for TI Drivers library
     *
     *  You can select the library type by setting
     *  the {@link #libType Config.libType} configuration parameter.
     *
     *  @field(LibType_Instrumented) The library supplied is pre-built with
     *  logging and assertions enabled.
     *  Diags_USER1 mask logs basic information
     *  Diags_USER2 mask logs more detailed information
     *
     *  @field(LibType_NonInstrumented) The library supplied is pre-built
     *  with logging and assertions disabled.
     */
    enum LibType {
        LibType_Instrumented,           /*! instrumented (with asserts and logs)*/
        LibType_NonInstrumented         /*! non-instrumented */
    };

    /*!
     *  ======== libType ========
     *  TI Drivers library type
     *
     *  The driver libraries are provided to link with your application.
     *  Two forms of this library are provided with the TI Drivers product.
     *  This configuration parameter allows you to select the form of the
     *  driver libraries to use.
     *
     *  The default value of libType is
     *  {@link #LibType_NonInstrumented LibType_NonInstrumented}.  For a
     *  complete list of options and what they offer see {@link #LibType}.
     */
    metaonly config LibType libType = LibType_NonInstrumented;

    /*!
     *  ======== RFDriverMode ========
     *  TI-Drivers RF Driver mode
     *
     *  You can select the RF driver mode by setting the
     *  {@link #rfDriverMode Config.rfDriverMode} configuration parameter.
     *
     *  @field(RF_SingleMode) A single client/protocol will have exclusive
     *  access to the radio.
     *
     *  @field(RF_MultiMode) Supports multiple clients/protocols accessing
     *  the radio simultaneously.
     */
    enum RFDriverMode {
        RF_MultiMode,        /*! Multiple clients/protocols run simultaneously */
        RF_SingleMode        /*! Only a single client/protocol */
    };

    /*!
     *  ======== rfDriverMode ========
     *  TI-Drivers RF Driver mode
     *
     *  The RF driver supports single & multi modes of operation.  In
     *  single-mode only a single client/protocol can interact  with the
     *  radio, meaning if another protocol is required, the RF driver must be
     *  closed & re-opened.
     *
     *  In multi-mode more than one client/protocol can interact with the radio
     *  simultaneously.
     *
     *  The default value of rfDriverMode is:
     *  {@link #RF_SingleMode RF_SingleMode}.  For a
     *  complete list of options and what they offer see {@link #RFDriverMode}.
     */
    metaonly config RFDriverMode rfDriverMode = RF_SingleMode;

    /*!
     *  ======== supportsRFDriver ========
     *  @_nodoc
     */
    metaonly config Bool supportsRFDriver = false;
}

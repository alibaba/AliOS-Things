/*
 * The Clear BSD License
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "evkbimxrt1050_flexspi_nor_config.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
#if defined(__CC_ARM) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf")))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.conf"
#endif

const flexspi_nor_config_t hyperflash_config = {
    .memConfig =
        {
            .tag = FLEXSPI_CFG_BLK_TAG,
            .version = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc = kFlexSPIReadSampleClk_ExternalInputFromDqsPad,
            .csHoldTime = 3u,
            .csSetupTime = 3u,
            .columnAddressWidth = 3u,
            .deviceType = kFlexSpiDeviceType_SerialNOR,
            // Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
            .controllerMiscOption =
                (1u << kFlexSpiMiscOffset_DdrModeEnable) | (1u << kFlexSpiMiscOffset_WordAddressableEnable) |
                (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable) | (1u << kFlexSpiMiscOffset_DiffClkEnable),
            .sflashPadType = kSerialFlash_8Pads,
            .serialClkFreq = kFlexSpiSerialClk_133MHz,
            .lutCustomSeqEnable = true,
            .sflashA1Size = 64u * 1024u * 1024u,
            .busyOffset = 15,
            .busyBitPolarity = 1,
            .lookupTable =
                {
                    /* Read */
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xA0, RADDR_DDR, FLEXSPI_8PAD, 0x18),
                    FLEXSPI_LUT_SEQ(CADDR_DDR, FLEXSPI_8PAD, 0x10, DUMMY_DDR, FLEXSPI_8PAD, 0x06),
                    FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP, FLEXSPI_1PAD, 0x0),
                    0,

                    /* Read Status */
                    // 0
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x05),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x70),
                    // 1
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xA0, RADDR_DDR, FLEXSPI_8PAD, 0x18),
                    FLEXSPI_LUT_SEQ(CADDR_DDR, FLEXSPI_8PAD, 0x10, DUMMY_RWDS_DDR, FLEXSPI_8PAD, 0x0B),
                    FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP, FLEXSPI_1PAD, 0x00),
                    0,

                    // Write Enable
                    // 0
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x05),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    // 1
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x55),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x02),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x55),

                    // Erase Sector
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x05),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x80),
                    // 1
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x05),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    // 2
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x55),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x02),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x55),
                    // 3
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, RADDR_DDR, FLEXSPI_8PAD, 0x18),
                    FLEXSPI_LUT_SEQ(CADDR_DDR, FLEXSPI_8PAD, 0x10, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x30, STOP, FLEXSPI_1PAD, 0x0),
                    0,

                    // Page Program
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x05),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xA0),
                    // 1
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, RADDR_DDR, FLEXSPI_8PAD, 0x18),
                    FLEXSPI_LUT_SEQ(CADDR_DDR, FLEXSPI_8PAD, 0x10, WRITE_DDR, FLEXSPI_8PAD, 0x80),
                    0,
                    0,

                    // Erase Chip
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x05),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x80),
                    // 1
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x05),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    // 2
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x55),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x02),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x55),
                    // 3
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x00),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0xAA),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x05),
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x00, CMD_DDR, FLEXSPI_8PAD, 0x10),
                },
            .lutCustomSeq =
            {
                    // LUT customized sequence
                    {0, },
                    {.seqNum = 2, .seqId = NOR_CMD_LUT_SEQ_IDX_READSTATUS },
                    {.seqNum = 2, .seqId = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE },
                    {.seqNum = 4, .seqId = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR },
                    {.seqNum = 2, .seqId = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM },
                    {.seqNum = 4, .seqId = NOR_CMD_LUT_SEQ_IDX_CHIPERASE },
            }
        },
    .pageSize = 512u,
    .sectorSize = 256u * 1024u,
    .blockSize = 256u * 1024u,
    .isUniformBlockSize = true,
    .serialNorType = kSerialNorType_HyperBus,
    .ipcmdSerialClkFreq = kFlexSpiSerialClk_30MHz,
};
#endif /* XIP_BOOT_HEADER_ENABLE */

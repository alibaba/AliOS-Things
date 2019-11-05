/*******************************************************************************
Copyright � 2016, STMicroelectronics International N.V.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STMicroelectronics nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED.
IN NO EVENT SHALL STMICROELECTRONICS INTERNATIONAL N.V. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/


#ifndef _VL53L0X_INTERRUPT_THRESHOLD_SETTINGS_H_
#define _VL53L0X_INTERRUPT_THRESHOLD_SETTINGS_H_


#ifdef __cplusplus
extern "C"
{
#endif


    uint8_t InterruptThresholdSettings[] = {

        /* Start of Interrupt Threshold Settings */
        0x1, 0xff, 0x00, 0x1,  0x80, 0x01, 0x1, 0xff, 0x01, 0x1, 0x00, 0x00,
        0x1, 0xff, 0x01, 0x1,  0x4f, 0x02, 0x1, 0xFF, 0x0E, 0x1, 0x00, 0x03,
        0x1, 0x01, 0x84, 0x1,  0x02, 0x0A, 0x1, 0x03, 0x03, 0x1, 0x04, 0x08,
        0x1, 0x05, 0xC8, 0x1,  0x06, 0x03, 0x1, 0x07, 0x8D, 0x1, 0x08, 0x08,
        0x1, 0x09, 0xC6, 0x1,  0x0A, 0x01, 0x1, 0x0B, 0x02, 0x1, 0x0C, 0x00,
        0x1, 0x0D, 0xD5, 0x1,  0x0E, 0x18, 0x1, 0x0F, 0x12, 0x1, 0x10, 0x01,
        0x1, 0x11, 0x82, 0x1,  0x12, 0x00, 0x1, 0x13, 0xD5, 0x1, 0x14, 0x18,
        0x1, 0x15, 0x13, 0x1,  0x16, 0x03, 0x1, 0x17, 0x86, 0x1, 0x18, 0x0A,
        0x1, 0x19, 0x09, 0x1,  0x1A, 0x08, 0x1, 0x1B, 0xC2, 0x1, 0x1C, 0x03,
        0x1, 0x1D, 0x8F, 0x1,  0x1E, 0x0A, 0x1, 0x1F, 0x06, 0x1, 0x20, 0x01,
        0x1, 0x21, 0x02, 0x1,  0x22, 0x00, 0x1, 0x23, 0xD5, 0x1, 0x24, 0x18,
        0x1, 0x25, 0x22, 0x1,  0x26, 0x01, 0x1, 0x27, 0x82, 0x1, 0x28, 0x00,
        0x1, 0x29, 0xD5, 0x1,  0x2A, 0x18, 0x1, 0x2B, 0x0B, 0x1, 0x2C, 0x28,
        0x1, 0x2D, 0x78, 0x1,  0x2E, 0x28, 0x1, 0x2F, 0x91, 0x1, 0x30, 0x00,
        0x1, 0x31, 0x0B, 0x1,  0x32, 0x00, 0x1, 0x33, 0x0B, 0x1, 0x34, 0x00,
        0x1, 0x35, 0xA1, 0x1,  0x36, 0x00, 0x1, 0x37, 0xA0, 0x1, 0x38, 0x00,
        0x1, 0x39, 0x04, 0x1,  0x3A, 0x28, 0x1, 0x3B, 0x30, 0x1, 0x3C, 0x0C,
        0x1, 0x3D, 0x04, 0x1,  0x3E, 0x0F, 0x1, 0x3F, 0x79, 0x1, 0x40, 0x28,
        0x1, 0x41, 0x1E, 0x1,  0x42, 0x2F, 0x1, 0x43, 0x87, 0x1, 0x44, 0x00,
        0x1, 0x45, 0x0B, 0x1,  0x46, 0x00, 0x1, 0x47, 0x0B, 0x1, 0x48, 0x00,
        0x1, 0x49, 0xA7, 0x1,  0x4A, 0x00, 0x1, 0x4B, 0xA6, 0x1, 0x4C, 0x00,
        0x1, 0x4D, 0x04, 0x1,  0x4E, 0x01, 0x1, 0x4F, 0x00, 0x1, 0x50, 0x00,
        0x1, 0x51, 0x80, 0x1,  0x52, 0x09, 0x1, 0x53, 0x08, 0x1, 0x54, 0x01,
        0x1, 0x55, 0x00, 0x1,  0x56, 0x0F, 0x1, 0x57, 0x79, 0x1, 0x58, 0x09,
        0x1, 0x59, 0x05, 0x1,  0x5A, 0x00, 0x1, 0x5B, 0x60, 0x1, 0x5C, 0x05,
        0x1, 0x5D, 0xD1, 0x1,  0x5E, 0x0C, 0x1, 0x5F, 0x3C, 0x1, 0x60, 0x00,
        0x1, 0x61, 0xD0, 0x1,  0x62, 0x0B, 0x1, 0x63, 0x03, 0x1, 0x64, 0x28,
        0x1, 0x65, 0x10, 0x1,  0x66, 0x2A, 0x1, 0x67, 0x39, 0x1, 0x68, 0x0B,
        0x1, 0x69, 0x02, 0x1,  0x6A, 0x28, 0x1, 0x6B, 0x10, 0x1, 0x6C, 0x2A,
        0x1, 0x6D, 0x61, 0x1,  0x6E, 0x0C, 0x1, 0x6F, 0x00, 0x1, 0x70, 0x0F,
        0x1, 0x71, 0x79, 0x1,  0x72, 0x00, 0x1, 0x73, 0x0B, 0x1, 0x74, 0x00,
        0x1, 0x75, 0x0B, 0x1,  0x76, 0x00, 0x1, 0x77, 0xA1, 0x1, 0x78, 0x00,
        0x1, 0x79, 0xA0, 0x1,  0x7A, 0x00, 0x1, 0x7B, 0x04, 0x1, 0xFF, 0x04,
        0x1, 0x79, 0x1D, 0x1,  0x7B, 0x27, 0x1, 0x96, 0x0E, 0x1, 0x97, 0xFE,
        0x1, 0x98, 0x03, 0x1,  0x99, 0xEF, 0x1, 0x9A, 0x02, 0x1, 0x9B, 0x44,
        0x1, 0x73, 0x07, 0x1,  0x70, 0x01, 0x1, 0xff, 0x01, 0x1, 0x00, 0x01,
        0x1, 0xff, 0x00, 0x00, 0x00, 0x00
    };

#ifdef __cplusplus
}
#endif

#endif /* _VL53L0X_INTERRUPT_THRESHOLD_SETTINGS_H_ */

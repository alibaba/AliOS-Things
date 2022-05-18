/*
 *    Copyright (c) 2018, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Allows to encrypt spinel frames sent between Application Processor (AP) and Network Co-Processor (NCP).
 */

namespace SpinelEncrypter {

/**
 * Encrypts spinel frames before sending to AP/NCP.
 *
 * This method encrypts outbound frames in both directions, i.e. from AP to NCP and from NCP to AP.
 *
 * @param[in,out] aFrameBuf Pointer to buffer containing the frame, also where the encrypted frame will be placed.
 * @param[in] aFrameSize Max number of bytes in frame buffer (max length of spinel frame + additional data for
 * encryption).
 * @param[in,out] aFrameLength Pointer to store frame length, on input value is set to frame length,
 * on output changed to show the frame length after encryption.
 * @return \c true on success, \c false otherwise.
 */
bool EncryptOutbound(unsigned char *aFrameBuf, size_t aFrameSize, size_t *aFrameLength);

/**
 * Decrypts spinel frames received from AP/NCP.
 *
 * This method decrypts inbound frames in both directions, i.e. from AP to NCP and from NCP to AP.
 *
 * @param[in,out] aFrameBuf Pointer to buffer containing encrypted frame, also where the decrypted frame will be placed.
 * @param[in] aFrameSize Max number of bytes in frame buffer (max length of spinel frame + additional data for
 * encryption).
 * @param[in,out] aFrameLength Pointer to store frame length, on input value is set to encrypted frame length,
 * on output changed to show the frame length after decryption.
 * @return \c true on success, \c false otherwise.
 */
bool DecryptInbound(unsigned char *aFrameBuf, size_t aFrameSize, size_t *aFrameLength);

} // namespace SpinelEncrypter

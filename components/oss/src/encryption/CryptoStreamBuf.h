/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <alibabacloud/oss/encryption/Cipher.h>
#include "../utils/StreamBuf.h"

namespace AlibabaCloud
{
namespace OSS
{
    class CryptoStreamBuf : public StreamBufProxy
    {
    public:
        static const std::streamsize BLK_SIZE = 16;

        CryptoStreamBuf(std::iostream& stream,
            const std::shared_ptr<SymmetricCipher>& cipher,
            const ByteBuffer& key, const ByteBuffer& iv,
            const int skipCnt = 0);
        ~CryptoStreamBuf();
    protected:
        std::streamsize xsgetn(char * _Ptr, std::streamsize _Count);
        std::streamsize xsputn(const char *_Ptr, std::streamsize _Count);
        std::streampos seekoff(off_type _Off, std::ios_base::seekdir _Way, std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out);
        std::streampos seekpos(pos_type _Pos, std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out);
    private:
        std::streamsize read_from_encrypted_buffer(char * _Ptr, std::streamsize _Count);
        std::streamsize xsputn_with_skip(const char *_Ptr, std::streamsize _Count);
        std::shared_ptr<SymmetricCipher> cipher_;
        unsigned char encBuffer_[BLK_SIZE * 2];
        std::streamsize encBufferCnt_;
        std::streamsize encBufferOff_;
        unsigned char decBuffer_[BLK_SIZE * 2];
        std::streamsize decBufferCnt_;
        std::streamsize decBufferOff_;
        ByteBuffer key_;
        ByteBuffer iv_;
        bool initEncrypt;
        bool initDecrypt;
        std::streamsize skipCnt_; // for decrypt, must be less BLK_SIZE
        std::streampos StartPosForIV_;
    };
}
}

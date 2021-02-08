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
#include <openssl/evp.h>

namespace AlibabaCloud
{
namespace OSS
{
    class SymmetricCipherOpenssl : public  SymmetricCipher
    {
    public:
        ~SymmetricCipherOpenssl();
        SymmetricCipherOpenssl(const EVP_CIPHER* cipher, CipherAlgorithm algo, CipherMode mode, CipherPadding pad);

        virtual void EncryptInit(const ByteBuffer& key, const ByteBuffer& iv);
        virtual ByteBuffer Encrypt(const ByteBuffer& data);
        virtual int Encrypt(unsigned char * dst, int dstLen, const unsigned char* src, int srcLen);
        virtual ByteBuffer EncryptFinish();

        virtual void DecryptInit(const ByteBuffer& key, const ByteBuffer& iv);
        virtual ByteBuffer Decrypt(const ByteBuffer& data);
        virtual int Decrypt(unsigned char * dst, int dstLen, const unsigned char* src, int srcLen);
        virtual ByteBuffer DecryptFinish();

    private:
        EVP_CIPHER_CTX* encryptCtx_;
        EVP_CIPHER_CTX* decryptCtx_;
        const EVP_CIPHER* cipher_;
        int blockSize_;
    };

    class AsymmetricCipherOpenssl : public  AsymmetricCipher
    {
    public:
        ~AsymmetricCipherOpenssl();
        AsymmetricCipherOpenssl(CipherAlgorithm algo, CipherMode mode, CipherPadding pad);

        virtual ByteBuffer Encrypt(const ByteBuffer& data);
        virtual ByteBuffer Decrypt(const ByteBuffer& data);
    private:
    };
}
}

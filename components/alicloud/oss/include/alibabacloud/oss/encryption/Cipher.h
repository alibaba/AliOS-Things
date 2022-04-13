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
#include <memory>
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/Types.h>

namespace AlibabaCloud
{
namespace OSS
{
    enum class CipherAlgorithm {
        AES,
        RSA,
    };

    enum class CipherMode {
        NONE,
        ECB,
        CBC,
        CTR,
    };

    enum class CipherPadding {
        NoPadding,
        PKCS1Padding,
        PKCS5Padding,
        PKCS7Padding,
        ZeroPadding,
    };

    class ALIBABACLOUD_OSS_EXPORT SymmetricCipher
    {
    public:
        virtual ~SymmetricCipher() {};

        //algorithm/mode/padding format. ex. AES/CBC/NoPadding
        const std::string& Name() const { return name_; }
        CipherAlgorithm Algorithm() { return algorithm_; }
        CipherMode Mode() { return mode_; }
        CipherPadding Padding() { return padding_; }

        int BlockSize() { return blockSize_; }

        virtual void EncryptInit(const ByteBuffer& key, const ByteBuffer& iv) = 0;
        virtual ByteBuffer Encrypt(const ByteBuffer& data) = 0;
        virtual int Encrypt(unsigned char * dst, int dstLen, const unsigned char* src, int srcLen) = 0;
        virtual ByteBuffer EncryptFinish() = 0;

        virtual void DecryptInit(const ByteBuffer& key, const ByteBuffer& iv) = 0;
        virtual ByteBuffer Decrypt(const ByteBuffer& data) = 0;
        virtual int Decrypt(unsigned char * dst, int dstLen, const unsigned char* src, int srcLen) = 0;
        virtual ByteBuffer DecryptFinish() = 0;
    
    public:
        static ByteBuffer GenerateIV(size_t length);
        static ByteBuffer GenerateKey(size_t length);
        static ByteBuffer IncCTRCounter(const ByteBuffer& counter, uint64_t numberOfBlocks);

        static std::shared_ptr<SymmetricCipher> CreateAES128_CTRImpl();
        static std::shared_ptr<SymmetricCipher> CreateAES128_CBCImpl();
        static std::shared_ptr<SymmetricCipher> CreateAES256_CTRImpl();
    protected:
        SymmetricCipher(const std::string& impl, CipherAlgorithm algo, CipherMode mode, CipherPadding pad);
    private:
        std::string impl_;
        std::string name_;
        CipherAlgorithm algorithm_;
        CipherMode mode_;
        CipherPadding padding_;
        int blockSize_;
    };

    class ALIBABACLOUD_OSS_EXPORT AsymmetricCipher
    {
    public:
        virtual ~AsymmetricCipher() {};
        const std::string& Name() const { return name_; }
        CipherAlgorithm Algorithm() { return algorithm_; }
        CipherMode Mode() { return mode_; }
        CipherPadding Padding() { return padding_; }

        void setPublicKey(const std::string& key) { publicKey_ = key; }
        void setPrivateKey(const std::string& key) { privateKey_ = key; }

        const std::string& PublicKey() const { return publicKey_; }
        const std::string& PrivateKey() const { return privateKey_; }

        virtual ByteBuffer Encrypt(const ByteBuffer& data) = 0;
        virtual ByteBuffer Decrypt(const ByteBuffer& data) = 0;

    public:
        static std::shared_ptr<AsymmetricCipher> CreateRSA_NONEImpl();

    protected:
        AsymmetricCipher(const std::string& impl, CipherAlgorithm algo, CipherMode mode, CipherPadding pad);
    private:
        std::string impl_;
        std::string name_;
        CipherAlgorithm algorithm_;
        CipherMode mode_;
        CipherPadding padding_;
        std::string publicKey_;
        std::string privateKey_;
    };
}
}

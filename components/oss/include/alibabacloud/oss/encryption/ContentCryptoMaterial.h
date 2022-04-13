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
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/Types.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT ContentCryptoMaterial
    {
    public:
        ContentCryptoMaterial();
        ~ContentCryptoMaterial();
        std::string Tag() const { return "Material"; }

        const ByteBuffer& ContentKey() const { return contentKey_; }
        const ByteBuffer& ContentIV() const { return contentIV_; }
        const std::string& CipherName() const { return cipherName_; }

        const std::string& KeyWrapAlgorithm() const { return keyWrapAlgorithm_; }
        const std::map<std::string, std::string>& Description() const { return description_; }
        const ByteBuffer& EncryptedContentKey() const { return encryptedContentKey_; }
        const ByteBuffer& EncryptedContentIV() const { return encryptedContentIV_; }

        const std::string& MagicNumber() const { return magicNumber_; }

        void setContentKey(const ByteBuffer& key) { contentKey_ = key; }
        void setContentIV(const ByteBuffer& iv) { contentIV_ = iv; }
        void setCipherName(const std::string& name) { cipherName_ = name; }

        void setKeyWrapAlgorithm(const std::string& algo) { keyWrapAlgorithm_ = algo; }
        void setDescription(const std::map<std::string, std::string>& desc) { description_ = desc; }
        void setEncryptedContentKey(const ByteBuffer& key) { encryptedContentKey_ = key; }
        void setEncryptedContentIV(const ByteBuffer& iv) { encryptedContentIV_ = iv; }

        void setMagicNumber(const std::string& value) { magicNumber_ = value; }
    private:
        ByteBuffer contentKey_;
        ByteBuffer contentIV_;
        std::string cipherName_;

        std::string keyWrapAlgorithm_;
        std::map<std::string, std::string> description_;
        ByteBuffer encryptedContentKey_;
        ByteBuffer encryptedContentIV_;

        std::string magicNumber_;
    };
}
}

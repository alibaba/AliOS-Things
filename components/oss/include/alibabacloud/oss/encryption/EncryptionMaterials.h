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

#include <alibabacloud/oss/encryption/ContentCryptoMaterial.h>
#include <map>
#include <string>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT EncryptionMaterials
    {
    public:
        virtual ~EncryptionMaterials();

        virtual int EncryptCEK(ContentCryptoMaterial& contentCryptoMaterial) = 0;
        virtual int DecryptCEK(ContentCryptoMaterial& contentCryptoMaterial) = 0;
    };

    using RSAEncryptionMaterial = std::pair<std::pair<std::string, std::string>, std::map<std::string, std::string>>;
    class ALIBABACLOUD_OSS_EXPORT SimpleRSAEncryptionMaterials : public EncryptionMaterials
    {
    public:
        SimpleRSAEncryptionMaterials(const std::string& publicKey, const std::string& privateKey);
        SimpleRSAEncryptionMaterials(const std::string& publicKey, const std::string& privateKey,
            const std::map<std::string, std::string>& description);
        ~SimpleRSAEncryptionMaterials();
        void addEncryptionMaterial(const std::string& publicKey, const std::string& privateKey,
            const std::map<std::string, std::string>& description);
        virtual int EncryptCEK(ContentCryptoMaterial& contentCryptoMaterial);
        virtual int DecryptCEK(ContentCryptoMaterial& contentCryptoMaterial);
    private:
        int findIndexByDescription(const std::map<std::string, std::string>& description);
        std::string publicKey_;
        std::map<std::string, std::string> description_;
        std::vector<RSAEncryptionMaterial> encryptionMaterials_;
    };
}
}

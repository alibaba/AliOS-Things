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

#include <alibabacloud/oss/encryption/EncryptionMaterials.h>
#include <alibabacloud/oss/encryption/Cipher.h>
#include <map>
#include <string>
#include "utils/Utils.h"

using namespace AlibabaCloud::OSS;

EncryptionMaterials::~EncryptionMaterials()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleRSAEncryptionMaterials::SimpleRSAEncryptionMaterials(const std::string& publicKey, const std::string& privateKey):
    SimpleRSAEncryptionMaterials(publicKey, privateKey, std::map< std::string, std::string>())
{
}

SimpleRSAEncryptionMaterials::SimpleRSAEncryptionMaterials(const std::string& publicKey, const std::string& privateKey,
    const std::map<std::string, std::string>& description):
    publicKey_(publicKey),
    description_(description)
{
    encryptionMaterials_.push_back(RSAEncryptionMaterial(std::pair<std::string, std::string>(publicKey, privateKey), description));
}

SimpleRSAEncryptionMaterials::~SimpleRSAEncryptionMaterials()
{
}

void SimpleRSAEncryptionMaterials::addEncryptionMaterial(const std::string& publicKey, const std::string& privateKey,
    const std::map<std::string, std::string>& description)
{
    encryptionMaterials_.push_back(RSAEncryptionMaterial(std::pair<std::string, std::string>(publicKey, privateKey), description));
}

int SimpleRSAEncryptionMaterials::EncryptCEK(ContentCryptoMaterial& contentCryptoMaterial)
{
    auto cipher = AsymmetricCipher::CreateRSA_NONEImpl();
    cipher->setPublicKey(publicKey_);

    auto encrptedKey = cipher->Encrypt(contentCryptoMaterial.ContentKey());
    auto encrptedKeyIV = cipher->Encrypt(contentCryptoMaterial.ContentIV());

    if (encrptedKey.empty() || encrptedKeyIV.empty()) {
        return -1;
    }

    contentCryptoMaterial.setDescription(description_);
    contentCryptoMaterial.setKeyWrapAlgorithm(cipher->Name());
    contentCryptoMaterial.setEncryptedContentKey(encrptedKey);
    contentCryptoMaterial.setEncryptedContentIV(encrptedKeyIV);

    return 0;
}

int SimpleRSAEncryptionMaterials::DecryptCEK(ContentCryptoMaterial& contentCryptoMaterial)
{
    auto cipher = AsymmetricCipher::CreateRSA_NONEImpl();

    auto keyWarpAlgo = ToLower(contentCryptoMaterial.KeyWrapAlgorithm().c_str());
    auto cipherName  = ToLower(cipher->Name().c_str());

    if (keyWarpAlgo != cipherName) {
        return -1;
    }
    
    int index = findIndexByDescription(contentCryptoMaterial.Description());
    if (index < 0) {
        return -1;
    }
    const std::string& privateKey = encryptionMaterials_[index].first.second;

    cipher->setPrivateKey(privateKey);

    auto key = cipher->Decrypt(contentCryptoMaterial.EncryptedContentKey());
    auto iv = cipher->Decrypt(contentCryptoMaterial.EncryptedContentIV());

    if (key.empty() || iv.empty()) {
        return -1;
    }

    contentCryptoMaterial.setContentKey(key);
    contentCryptoMaterial.setContentIV(iv);

    return 0;
}

int SimpleRSAEncryptionMaterials::findIndexByDescription(const std::map<std::string, std::string>& description)
{
    bool found = false;
    int index = 0;
    for (const auto& item : encryptionMaterials_) {
        if (item.second == description) {
            found = true;
            break;
        }
        index++;
    }
    return found ? index: -1;
}

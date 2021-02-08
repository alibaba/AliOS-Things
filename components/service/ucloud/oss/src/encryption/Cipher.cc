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

#include "CipherOpenssl.h"
#include <openssl/rand.h>

using namespace AlibabaCloud::OSS;

inline static std::string toAlgorithmName(CipherAlgorithm algo)
{
    static const char* name[] = { "AES", "RSA"};
    return name[static_cast<int>(algo) - static_cast<int>(CipherAlgorithm::AES)];
}

inline static std::string toModeName(CipherMode mode)
{
    static const char* name[] = { "NONE", "ECB", "CBC", "CTR"};
    return name[static_cast<int>(mode) - static_cast<int>(CipherMode::NONE)];
}

inline static std::string toPaddingName(CipherPadding pad)
{
    static const char* name[] = { "NoPadding", "PKCS1Padding", "PKCS5Padding", "PKCS7Padding", "ZeroPadding"};
    return name[static_cast<int>(pad) - static_cast<int>(CipherPadding::NoPadding)];
}

SymmetricCipher::SymmetricCipher(const std::string& impl, CipherAlgorithm algo, CipherMode mode, CipherPadding pad):
    impl_(impl),
    algorithm_(algo),
    mode_(mode),
    padding_(pad),
    blockSize_(16)
{
    name_ = toAlgorithmName(algo);
    name_.append("/");
    name_.append(toModeName(mode));
    name_.append("/");
    name_.append(toPaddingName(pad));
}

ByteBuffer SymmetricCipher::GenerateIV(size_t length)
{
    //use openssl rand func
    ByteBuffer out = ByteBuffer(length);
    RAND_bytes((unsigned char *)out.data(), length);
    return out;
}

ByteBuffer SymmetricCipher::GenerateKey(size_t length)
{
    //use openssl rand func
    ByteBuffer out = ByteBuffer(length);
    RAND_bytes((unsigned char *)out.data(), length);
    return out;
}

template<class T>
typename std::enable_if<std::is_unsigned<T>::value, T>::type
bswap(T i, T j = 0u, std::size_t n = 0u)
{
    return n == sizeof(T) ? j :
        bswap<T>(i >> CHAR_BIT, (j << CHAR_BIT) | (i & (T)(unsigned char)(-1)), n + 1);
}

ByteBuffer SymmetricCipher::IncCTRCounter(const ByteBuffer& counter, uint64_t numberOfBlocks)
{
    ByteBuffer ctrCounter(counter);
    uint64_t *ctrPtr = (uint64_t*)(ctrCounter.data() + ctrCounter.size() - sizeof(uint64_t));

    uint64_t n = 1;
    if (*(char *)&n) {
        //little
        *ctrPtr = bswap<uint64_t>(bswap<uint64_t>(*ctrPtr) + numberOfBlocks);
    }
    else {
        //big
        *ctrPtr += numberOfBlocks;
    }

    return ctrCounter;
}

std::shared_ptr<SymmetricCipher> SymmetricCipher::CreateAES128_CTRImpl()
{
    return std::make_shared<SymmetricCipherOpenssl>(EVP_aes_128_ctr(), 
        CipherAlgorithm::AES, CipherMode::CTR, CipherPadding::NoPadding);
}

std::shared_ptr<SymmetricCipher> SymmetricCipher::CreateAES128_CBCImpl()
{
    return std::make_shared<SymmetricCipherOpenssl>(EVP_aes_128_cbc(), 
        CipherAlgorithm::AES, CipherMode::CBC, CipherPadding::PKCS5Padding);
}

std::shared_ptr<SymmetricCipher> SymmetricCipher::CreateAES256_CTRImpl()
{
    return std::make_shared<SymmetricCipherOpenssl>(EVP_aes_256_ctr(),
        CipherAlgorithm::AES, CipherMode::CTR, CipherPadding::NoPadding);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AsymmetricCipher::AsymmetricCipher(const std::string& impl, CipherAlgorithm algo, CipherMode mode, CipherPadding pad) :
    impl_(impl),
    algorithm_(algo),
    mode_(mode),
    padding_(pad)
{
    name_ = toAlgorithmName(algo);
    name_.append("/");
    name_.append(toModeName(mode));
    name_.append("/");
    name_.append(toPaddingName(pad));
}

std::shared_ptr<AsymmetricCipher> AsymmetricCipher::CreateRSA_NONEImpl()
{
    return std::make_shared<AsymmetricCipherOpenssl>(CipherAlgorithm::RSA, CipherMode::NONE, CipherPadding::PKCS1Padding);
}




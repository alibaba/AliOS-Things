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
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <cstring>

using namespace AlibabaCloud::OSS;

SymmetricCipherOpenssl::SymmetricCipherOpenssl(const EVP_CIPHER* cipher, CipherAlgorithm algo, CipherMode mode, CipherPadding pad):
    SymmetricCipher("openssl-impl", algo, mode, pad),
    encryptCtx_(nullptr),
    decryptCtx_(nullptr),
    cipher_(cipher)
{
    blockSize_  = EVP_CIPHER_block_size(cipher_);
}

SymmetricCipherOpenssl::~SymmetricCipherOpenssl()
{
    if (encryptCtx_) {
        EVP_CIPHER_CTX_cleanup(encryptCtx_);
        EVP_CIPHER_CTX_free(encryptCtx_);
        encryptCtx_ = nullptr;
    }

    if (decryptCtx_) {
        EVP_CIPHER_CTX_cleanup(decryptCtx_);
        EVP_CIPHER_CTX_free(decryptCtx_);
        decryptCtx_ = nullptr;
    }
}

void SymmetricCipherOpenssl::EncryptInit(const ByteBuffer& key, const ByteBuffer& iv)
{
    if (!encryptCtx_) {
        encryptCtx_ = EVP_CIPHER_CTX_new();
    }

    EVP_EncryptInit_ex(encryptCtx_, cipher_, nullptr, key.data(), iv.data());
}

ByteBuffer SymmetricCipherOpenssl::Encrypt(const ByteBuffer& data)
{
    if (data.empty()) {
        return ByteBuffer();
    }

    int outlen = static_cast<int>(data.size() + EVP_MAX_BLOCK_LENGTH);
    ByteBuffer out(static_cast<size_t>(outlen));

    if (!EVP_EncryptUpdate(encryptCtx_, out.data(), &outlen, data.data(), static_cast<int>(data.size()))) {
        return ByteBuffer();
    }
    out.resize(outlen);
    return out;
}

int SymmetricCipherOpenssl::Encrypt(unsigned char* dst, int dstLen, const unsigned char* src, int srcLen)
{
    if (!dst || !src) {
        return -1;
    }

    if (!EVP_EncryptUpdate(encryptCtx_, dst, &dstLen, src, srcLen)) {
        return -1;
    }

    return dstLen;
}


ByteBuffer SymmetricCipherOpenssl::EncryptFinish()
{
    ByteBuffer out(EVP_MAX_BLOCK_LENGTH);
    int outlen = 0;
    if (!EVP_EncryptFinal_ex(encryptCtx_, out.data(), &outlen)) {
        return ByteBuffer();
    }
    out.resize(static_cast<size_t>(outlen));
    return out;
}

void SymmetricCipherOpenssl::DecryptInit(const ByteBuffer& key, const ByteBuffer& iv)
{
    if (!decryptCtx_) {
        decryptCtx_ = EVP_CIPHER_CTX_new();
    }

    EVP_DecryptInit_ex(decryptCtx_, cipher_, nullptr, key.data(), iv.data());
}

ByteBuffer SymmetricCipherOpenssl::Decrypt(const ByteBuffer& data)
{
    if (data.empty()) {
        return ByteBuffer();
    }

    int outlen = static_cast<int>(data.size() + EVP_MAX_BLOCK_LENGTH);
    ByteBuffer out(static_cast<size_t>(outlen));

    if (!EVP_DecryptUpdate(decryptCtx_, out.data(), &outlen, data.data(), static_cast<int>(data.size()))) {
        return ByteBuffer();
    }
    out.resize(outlen);
    return out;
}

int SymmetricCipherOpenssl::Decrypt(unsigned char * dst, int dstLen, const unsigned char* src, int srcLen)
{
    if (!dst || !src) {
        return -1;
    }

    if (!EVP_DecryptUpdate(decryptCtx_, dst, &dstLen, (const unsigned char *)src, srcLen)) {
        return -1;
    }

    return dstLen;
}

ByteBuffer SymmetricCipherOpenssl::DecryptFinish()
{
    ByteBuffer out(EVP_MAX_BLOCK_LENGTH);
    int outlen = 0;
    if (!EVP_DecryptFinal_ex(decryptCtx_, (unsigned char *)out.data(), &outlen)) {
        return ByteBuffer();
    }
    out.resize(static_cast<size_t>(outlen));
    return out;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AsymmetricCipherOpenssl::AsymmetricCipherOpenssl(CipherAlgorithm algo, CipherMode mode, CipherPadding pad) :
    AsymmetricCipher("openssl-impl", algo, mode, pad)
{
}

AsymmetricCipherOpenssl::~AsymmetricCipherOpenssl()
{
}

ByteBuffer AsymmetricCipherOpenssl::Encrypt(const ByteBuffer& data)
{
    RSA* rsa = NULL;
    BIO* bio = NULL;
    EVP_PKEY* pkey = NULL;
    ByteBuffer enc;
    do {
        if (data.empty()) {
            break;
        }

        bio = BIO_new(BIO_s_mem());
        BIO_puts(bio, PublicKey().c_str());

        if (strncmp("-----BEGIN RSA", PublicKey().c_str(), 14) == 0) {
            rsa = PEM_read_bio_RSAPublicKey(bio, &rsa, NULL, NULL);
        }
        else {
            pkey = PEM_read_bio_PUBKEY(bio, &pkey, NULL, NULL);
            rsa = pkey? EVP_PKEY_get1_RSA(pkey) : NULL;
        }

        if (rsa == NULL) {
            break;
        }

        int rsa_len = RSA_size(rsa);
        enc.resize(rsa_len, 0);

        if (RSA_public_encrypt(data.size(), (unsigned char*)data.data(), (unsigned char*)enc.data(), rsa, RSA_PKCS1_PADDING) < 0) {
            enc.resize(0);
        }

    } while (0);

    if (bio) {
        BIO_free(bio);
    }

    if (pkey) {
        EVP_PKEY_free(pkey);
    }

    if (rsa) {
        RSA_free(rsa);
    }

    return enc;
}

ByteBuffer AsymmetricCipherOpenssl::Decrypt(const ByteBuffer& data)
{
    RSA* rsa = NULL;
    BIO* bio = NULL;
    EVP_PKEY* pkey = NULL;
    ByteBuffer dec;
    do {
        if (data.empty()) {
            break;
        }

        bio = BIO_new(BIO_s_mem());
        BIO_puts(bio, PrivateKey().c_str());

        if (strncmp("-----BEGIN RSA", PublicKey().c_str(), 14) == 0) {
            rsa = PEM_read_bio_RSAPrivateKey(bio, &rsa, NULL, NULL);
        }
        else {
            pkey = PEM_read_bio_PrivateKey(bio, &pkey, NULL, NULL);
            rsa = pkey ? EVP_PKEY_get1_RSA(pkey) : NULL;
        }

        if (rsa == NULL) {
            break;
        }

        int rsa_len = RSA_size(rsa);
        dec.resize(rsa_len, 0);

        auto dec_len = RSA_private_decrypt(rsa_len, (unsigned char*)data.data(), (unsigned char*)dec.data(), rsa, RSA_PKCS1_PADDING);
        dec.resize(dec_len < 0 ? 0 : static_cast<size_t>(dec_len));

    } while (0);

    if (bio) {
        BIO_free(bio);
    }

    if (pkey) {
        EVP_PKEY_free(pkey);
    }

    if (rsa) {
        RSA_free(rsa);
    }

    return dec;
}

/*
 * Copyright 1999-2019 Alibaba Cloud All rights reserved.
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

#include <alibabacloud/core/HmacSha1Signer.h>
#ifdef _WIN32
#include <wincrypt.h>
#include <windows.h>
#elif defined(USE_CRYPTO_MBEDTLS)
#include "mbedtls/compat-1.3.h"
#include "mbedtls/sha1.h"
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#else
#include <openssl/hmac.h>
#endif

namespace AlibabaCloud {

HmacSha1Signer::HmacSha1Signer() : Signer(HmacSha1, "HMAC-SHA1", "1.0") {}

HmacSha1Signer::~HmacSha1Signer() {}

std::string HmacSha1Signer::generate(const std::string &src,
                                     const std::string &secret) const {
  if (src.empty())
    return std::string();

#ifdef _WIN32
  typedef struct _my_blob {
    BLOBHEADER hdr;
    DWORD dwKeySize;
    BYTE rgbKeyData[];
  } my_blob;

  DWORD kbLen = sizeof(my_blob) + secret.size();
  my_blob *kb = (my_blob *)LocalAlloc(LPTR, kbLen);
  kb->hdr.bType = PLAINTEXTKEYBLOB;
  kb->hdr.bVersion = CUR_BLOB_VERSION;
  kb->hdr.reserved = 0;
  kb->hdr.aiKeyAlg = CALG_RC2;
  kb->dwKeySize = secret.size();
  memcpy(&kb->rgbKeyData, secret.c_str(), secret.size());

  HCRYPTPROV hProv = 0;
  HCRYPTKEY hKey = 0;
  HCRYPTHASH hHmacHash = 0;
  BYTE pbHash[32];
  DWORD dwDataLen = 32;
  HMAC_INFO HmacInfo;
  ZeroMemory(&HmacInfo, sizeof(HmacInfo));
  HmacInfo.HashAlgid = CALG_SHA1;

  CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL,
                      CRYPT_VERIFYCONTEXT | CRYPT_NEWKEYSET);
  CryptImportKey(hProv, (BYTE *)kb, kbLen, 0, CRYPT_IPSEC_HMAC_KEY, &hKey);
  CryptCreateHash(hProv, CALG_HMAC, hKey, 0, &hHmacHash);
  CryptSetHashParam(hHmacHash, HP_HMAC_INFO, (BYTE *)&HmacInfo, 0);
  CryptHashData(hHmacHash, (BYTE *)(src.c_str()), src.size(), 0);
  CryptGetHashParam(hHmacHash, HP_HASHVAL, pbHash, &dwDataLen, 0);

  LocalFree(kb);
  CryptDestroyHash(hHmacHash);
  CryptDestroyKey(hKey);
  CryptReleaseContext(hProv, 0);

  DWORD dlen = 0;
  CryptBinaryToString(pbHash, dwDataLen,
                      CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &dlen);
  char *dest = new char[dlen];
  CryptBinaryToString(pbHash, dwDataLen,
                      CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, dest, &dlen);

  std::string ret = std::string(dest, dlen);
  delete dest;
  return ret;

#elif defined(USE_CRYPTO_MBEDTLS)
	unsigned char md[20];
  unsigned int mdLen = 20;
	size_t olen = 0;
  mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1),
                  reinterpret_cast<const unsigned char*>(secret.c_str()),static_cast<int>(secret.size()),
                  reinterpret_cast<const unsigned char*>(src.c_str()), src.size(),
                  (unsigned char *)md);
  char encodedData[100];
  mbedtls_base64_encode((unsigned char*)encodedData, sizeof(encodedData), &olen, md, mdLen);

  return encodedData;
#else
  unsigned char md[EVP_MAX_BLOCK_LENGTH];
  unsigned int mdLen = EVP_MAX_BLOCK_LENGTH;

  if (HMAC(EVP_sha1(), secret.c_str(), secret.size(),
           reinterpret_cast<const unsigned char *>(src.c_str()), src.size(), md,
           &mdLen) == nullptr)
    return std::string();

  char encodedData[100];
  EVP_EncodeBlock(reinterpret_cast<unsigned char *>(encodedData), md, mdLen);
  return encodedData;
#endif
}

} // namespace AlibabaCloud

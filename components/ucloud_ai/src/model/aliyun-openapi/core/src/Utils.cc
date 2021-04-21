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

#include <algorithm>
#include <alibabacloud/core/Utils.h>
#include <sstream>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#elif defined(USE_CRYPTO_MBEDTLS)
#include "mbedtls/md.h"
#include "mbedtls/compat-1.3.h"
#include "mbedtls/sha1.h"
#include "mbedtls/base64.h"
#include "mbedtls/md5.h"
#else
#include <openssl/hmac.h>
#include <openssl/md5.h>
#include <uuid/uuid.h>
#endif
#include <curl/curl.h>
#include <json/json.h>
#include <time.h>

std::string AlibabaCloud::GenerateUuid() {
#ifdef _WIN32
  char *data;
  UUID uuidhandle;
  UuidCreate(&uuidhandle);
  UuidToString(&uuidhandle, reinterpret_cast<RPC_CSTR *>(&data));
  std::string uuid(data);
  RpcStringFree(reinterpret_cast<RPC_CSTR *>(&data));
  return uuid;
#else
#if 0
  uuid_t uu;
  uuid_generate(uu);
  char buf[36];
  uuid_unparse(uu, buf);
  return buf;
#else
  char buf[36];
  unsigned int seed = (unsigned int)aos_now();

  srand(seed);
  sprintf(buf,"680c83c1-fa94-4b90-a364-%d",rand());
  std::string uuid(buf);
  return uuid;
#endif
#endif
}

std::string AlibabaCloud::UrlEncode(const std::string &src) {
  CURL *curl = curl_easy_init();
  char *output = curl_easy_escape(curl, src.c_str(), src.size());
  std::string result(output);
  curl_free(output);
  curl_easy_cleanup(curl);
  return result;
}

std::string AlibabaCloud::UrlDecode(const std::string &src) {
  CURL *curl = curl_easy_init();
  int outlength = 0;
  char *output = curl_easy_unescape(curl, src.c_str(), src.size(), &outlength);
  std::string result(output, outlength);
  curl_free(output);
  curl_easy_cleanup(curl);
  return result;
}

std::string AlibabaCloud::ComputeContentMD5(const char *data, size_t size) {
#ifdef _WIN32
  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;
  BYTE pbHash[16];
  DWORD dwDataLen = 16;

  CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
  CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
  CryptHashData(hHash, (BYTE *)(data), size, 0);
  CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwDataLen, 0);

  CryptDestroyHash(hHash);
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
  unsigned char md_data[16];
  unsigned int mdLen = 16;
  mbedtls_md5_context ctx;
	unsigned int olen = 0;

	mbedtls_md5_ret((const unsigned char*)data, size, md_data);

  char encodedData[100];

  mbedtls_base64_encode((unsigned char*)encodedData, sizeof(encodedData), &olen, md_data, mdLen);

  return encodedData;
#else
  unsigned char md[MD5_DIGEST_LENGTH];
  MD5(reinterpret_cast<const unsigned char *>(data), size,
      (unsigned char *)&md);

  char encodedData[100];
  EVP_EncodeBlock(reinterpret_cast<unsigned char *>(encodedData), md,
                  MD5_DIGEST_LENGTH);
  return encodedData;
#endif
}

void AlibabaCloud::StringReplace(std::string &src, const std::string &s1,
                                 const std::string &s2) {
  std::string::size_type pos = 0;
  while ((pos = src.find(s1, pos)) != std::string::npos) {
    src.replace(pos, s1.length(), s2);
    pos += s2.length();
  }
}

std::string AlibabaCloud::HttpMethodToString(HttpRequest::Method method) {
  switch (method) {
  case HttpRequest::Method::Head:
    return "HEAD";
    break;
  case HttpRequest::Method::Post:
    return "POST";
    break;
  case HttpRequest::Method::Put:
    return "PUT";
    break;
  case HttpRequest::Method::Delete:
    return "DELETE";
    break;
  case HttpRequest::Method::Connect:
    return "CONNECT";
    break;
  case HttpRequest::Method::Options:
    return "OPTIONS";
    break;
  case HttpRequest::Method::Patch:
    return "PATCH";
    break;
  case HttpRequest::Method::Trace:
    return "TRACE";
    break;
  case HttpRequest::Method::Get:
  default:
    return "GET";
    break;
  }
}

std::string AlibabaCloud::canonicalizedQuery(
    const std::map<std::string, std::string> &params) {
  if (params.empty())
    return std::string();

  std::stringstream ss;
  for (const auto &p : params) {
    std::string key = UrlEncode(p.first);
    StringReplace(key, "+", "%20");
    StringReplace(key, "*", "%2A");
    StringReplace(key, "%7E", "~");
    std::string value = UrlEncode(p.second);
    StringReplace(value, "+", "%20");
    StringReplace(value, "*", "%2A");
    StringReplace(value, "%7E", "~");
    ss << "&" << key << "=" << value;
  }
  return ss.str().substr(1);
}

std::string AlibabaCloud::canonicalizedHeaders(
    const HttpMessage::HeaderCollection &headers) {
  std::map<std::string, std::string> materials;
  for (const auto &p : headers) {
    std::string key = p.first;
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    if (key.find("x-acs-") != 0)
      continue;

    std::string value = p.second;
    StringReplace(value, "\t", " ");
    StringReplace(value, "\n", " ");
    StringReplace(value, "\r", " ");
    StringReplace(value, "\f", " ");
    materials[key] = value;
  }

  if (materials.empty())
    return std::string();
  std::stringstream ss;
  for (const auto &p : materials)
    ss << p.first << ":" << p.second << "\n";

  return ss.str();
}

std::string AlibabaCloud::GetEnv(const std::string env) {
#ifdef _WIN32
  char *buf = nullptr;
  size_t sz = 0;
  if (_dupenv_s(&buf, &sz, env.c_str()) == 0 && buf != nullptr) {
    std::string var(buf);
    free(buf);
    return var;
  } else {
    if (buf) {
      free(buf);
    }
    return std::string();
  }
#else
  char *var = getenv(env.c_str());
  if (var) {
    return std::string(var);
  }
  return std::string();
#endif
}

std::string
AlibabaCloud::MapToJson(const std::map<std::string, std::string> &maps) {
  Json::Value jsonObject;
  Json::FastWriter writer;
  for (std::map<std::string, std::string>::const_iterator iter = maps.begin();
       iter != maps.end(); ++iter) {
    jsonObject[iter->first] = iter->second;
  }
  std::string unformat_str = writer.write(jsonObject);
  return unformat_str.substr(0, unformat_str.length() - 1);
}

std::map<std::string, std::string>
AlibabaCloud::JsonToMap(const std::string &json) {
  Json::Reader reader;
  Json::Value value;
  std::map<std::string, std::string> maps;

  if (json.length() > 0) {
    if (reader.parse(json, value)) {
      Json::Value::Members members = value.getMemberNames();
      for (Json::Value::Members::iterator it = members.begin();
           it != members.end(); ++it) {
        Json::ValueType vt = value[*it].type();
        switch (vt) {
        case Json::stringValue: {
          maps.insert(
              std::pair<std::string, std::string>(*it, value[*it].asString()));
          break;
        }
        case Json::intValue: {
          int inttmp = value[*it].asInt();
          maps.insert(
              std::pair<std::string, std::string>(*it, std::to_string(inttmp)));
          break;
        }
        case Json::arrayValue: {
          std::string strid;
          for (unsigned int i = 0; i < value[*it].size(); i++) {
            strid += value[*it][i].asString();
            strid += ",";
          }
          if (!strid.empty()) {
            strid = strid.substr(0, strid.size() - 1);
          }
          maps.insert(std::pair<std::string, std::string>(*it, strid));
          break;
        }
        default: {
          break;
        }
        }
      }
    }
  }

  return maps;
}
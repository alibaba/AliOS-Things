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

#include "Utils.h"

#ifdef USE_CRYPTO_MBEDTLS
#include "mbedtls/md.h"
#include "mbedtls/compat-1.3.h"
#include "mbedtls/sha1.h"
#include "mbedtls/base64.h"
#include "mbedtls/md5.h"

#else

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/md5.h>

#ifdef OPENSSL_IS_BORINGSSL 
#include <openssl/base64.h>
#endif

#endif

#include <algorithm>
#include <cstring>
#include <iostream> 
#include <sstream>
#include <map>
#include <regex>
#include <iomanip>
#include <alibabacloud/oss/Const.h>
#include <alibabacloud/oss/http/HttpType.h>
#include <alibabacloud/oss/http/Url.h>
#include "../external/json/json.h"

using namespace AlibabaCloud::OSS;

#if defined(__GNUG__) && __GNUC__ < 5

#else
static const std::regex ipPattern("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
static const std::regex bucketNamePattern("^[a-z0-9][a-z0-9\\-]{1,61}[a-z0-9]$");
static const std::regex loggingPrefixPattern("^[a-zA-Z][a-zA-Z0-9\\-]{0,31}$");
#endif

std::string AlibabaCloud::OSS::GenerateUuid()
{
    return "";
}

std::string AlibabaCloud::OSS::UrlEncode(const std::string & src)
{
    std::stringstream dest;
    static const char *hex = "0123456789ABCDEF";
    unsigned char c;

    for (size_t i = 0; i < src.size(); i++) {
        c = src[i];
        if (isalnum(c) || (c == '-') || (c == '_') || (c == '.') || (c == '~')) {
            dest << c;
        } else if (c == ' ') {
            dest << "%20";
        } else {
            dest << '%' << hex[c >> 4] << hex[c & 15];
        }
    }

    return dest.str();
}

std::string AlibabaCloud::OSS::UrlDecode(const std::string & src)
{
    std::stringstream unescaped;
    unescaped.fill('0');
    unescaped << std::hex;

    size_t safeLength = src.size();
    const char *safe = src.c_str();
    for (auto i = safe, n = safe + safeLength; i != n; ++i)
    {
        char c = *i;
        if(c == '%')
        {
            char hex[3];
            hex[0] = *(i + 1);
            hex[1] = *(i + 2);
            hex[2] = 0;
            i += 2;
            auto hexAsInteger = strtol(hex, nullptr, 16);
            unescaped << (char)hexAsInteger;
        }
        else
        {
            unescaped << *i;
        }
    }

    return unescaped.str();
}

std::string AlibabaCloud::OSS::Base64Encode(const std::string &src)
{
    return AlibabaCloud::OSS::Base64Encode(src.c_str(), static_cast<int>(src.size()));
}

std::string AlibabaCloud::OSS::Base64Encode(const ByteBuffer& buffer)
{
    return AlibabaCloud::OSS::Base64Encode(reinterpret_cast<const char*>(buffer.data()), static_cast<int>(buffer.size()));
}

std::string AlibabaCloud::OSS::Base64Encode(const char *src, int len)
{
    if (!src || len == 0) {
        return "";
    }

    static const char *ENC = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    auto in = reinterpret_cast<const unsigned char *>(src);
    auto inLen = len;
    std::stringstream ss;
    while (inLen) {
        // first 6 bits of char 1
        ss << ENC[*in >> 2];
        if (!--inLen) {
            // last 2 bits of char 1, 4 bits of 0
            ss << ENC[(*in & 0x3) << 4];
            ss << '=';
            ss << '=';
            break;
        }
        // last 2 bits of char 1, first 4 bits of char 2
        ss << ENC[((*in & 0x3) << 4) | (*(in + 1) >> 4)];
        in++;
        if (!--inLen) {
            // last 4 bits of char 2, 2 bits of 0
            ss << ENC[(*in & 0xF) << 2];
            ss << '=';
            break;
        }
        // last 4 bits of char 2, first 2 bits of char 3
        ss << ENC[((*in & 0xF) << 2) | (*(in + 1) >> 6)];
        in++;
        // last 6 bits of char 3
        ss << ENC[*in & 0x3F];
        in++, inLen--;
    }
    return ss.str();
}

std::string AlibabaCloud::OSS::Base64EncodeUrlSafe(const std::string &src)
{
    return AlibabaCloud::OSS::Base64EncodeUrlSafe(src.c_str(), static_cast<int>(src.size()));
}

std::string AlibabaCloud::OSS::Base64EncodeUrlSafe(const char *src, int len)
{
    std::string out = AlibabaCloud::OSS::Base64Encode(src, len);

    while (out.size() > 0 && *out.rbegin() == '=')
        out.pop_back();

    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { 
        if (c == '+') return '-';
        if (c == '/') return '_';
        return (char)c;
    });
    return out;
}

std::string AlibabaCloud::OSS::XmlEscape(const std::string& value)
{
    struct Entity {
        const char* pattern;
        char value;
    };

    static const Entity entities[] = {
        { "&quot;", '\"' },
        { "&amp;",  '&'  },
        { "&apos;", '\'' },
        { "&lt;",	'<'  },
        { "&gt;",	'>'  },
        { "&#13;",	'\r' }
    };

    if (value.empty()) {
        return value;
    }

    std::stringstream ss;
    for (size_t i = 0; i < value.size(); i++) {
        bool flag = false;
        for (size_t j = 0; j < (sizeof(entities)/sizeof(entities[0])); j++) {
            if (value[i] == entities[j].value) {
                flag = true;
                ss << entities[j].pattern;
                break;
            }
        }

        if (!flag) {
            ss << value[i];
        }
    }

    return ss.str();
}
ByteBuffer AlibabaCloud::OSS::Base64Decode(const char *data, int len)
{
    int in_len = len;
    int i = 0;
    int in_ = 0;
    unsigned char part4[4];

    const int max_len = (len * 3 / 4);
    ByteBuffer ret(max_len);
    int idx = 0;

    while (in_len-- && (data[in_] != '=')) {
        unsigned char ch = data[in_++];
        if ('A' <= ch && ch <= 'Z')  ch = ch - 'A';           // A - Z
        else if ('a' <= ch && ch <= 'z') ch = ch - 'a' + 26;  // a - z
        else if ('0' <= ch && ch <= '9') ch = ch - '0' + 52;  // 0 - 9
        else if ('+' == ch) ch = 62;                          // +
        else if ('/' == ch) ch = 63;                          // /
        else if ('=' == ch) ch = 64;                          // =
        else ch = 0xff;                                       // something wrong
        part4[i++] = ch;
        if (i == 4) {
            ret[idx++] = (part4[0] << 2) + ((part4[1] & 0x30) >> 4);
            ret[idx++] = ((part4[1] & 0xf) << 4) + ((part4[2] & 0x3c) >> 2);
            ret[idx++] = ((part4[2] & 0x3) << 6) + part4[3];
            i = 0;
        }
    }

    if (i) {
        for (int j = i; j < 4; j++)
            part4[j] = 0xFF;
        ret[idx++] = (part4[0] << 2) + ((part4[1] & 0x30) >> 4);
        if (part4[2] != 0xFF) {
            ret[idx++] = ((part4[1] & 0xf) << 4) + ((part4[2] & 0x3c) >> 2);
            if (part4[3] != 0xFF) {
                ret[idx++] = ((part4[2] & 0x3) << 6) + part4[3];
            }
        }
    }

    ret.resize(idx);
    return ret;
}

ByteBuffer AlibabaCloud::OSS::Base64Decode(const std::string &src)
{
    return Base64Decode(src.c_str(), src.size());
}


std::string AlibabaCloud::OSS::ComputeContentMD5(const std::string& data) 
{
    return ComputeContentMD5(data.c_str(), data.size());
}

std::string AlibabaCloud::OSS::ComputeContentMD5(const char * data, size_t size)
{
#if defined(USE_CRYPTO_MBEDTLS)
    unsigned char md_data[16];
    unsigned int mdLen = 16;
    mbedtls_md5_context ctx;
	unsigned int olen = 0;
#if 0
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);
    mbedtls_md5_update(&ctx, (const unsigned char *)data, size);
    mbedtls_md5_finish(&ctx, md_data);
    mbedtls_md5_free(&ctx);
#endif

	mbedtls_md5_ret((const unsigned char*)data, size, md_data);

    char encodedData[100];

    mbedtls_base64_encode((unsigned char*)encodedData, sizeof(encodedData), &olen, md_data, mdLen);

    return encodedData;
    //TODO: ethan: not used for the moment
#elif 0//defined(USE_CRYPTO_MBEDTLS)
	unsigned char md_data[16];
    unsigned int mdLen = 16;
    unsigned int olen = 0;

	mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_MD5), data, size, md_data);

	char encodedData[100];

    mbedtls_base64_encode((unsigned char*)encodedData, sizeof(encodedData), &olen, md_data, mdLen);

    return encodedData;
#else

    unsigned char md[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(data), size, (unsigned char*)&md);
     
    char encodedData[100];

    EVP_EncodeBlock(reinterpret_cast<unsigned char*>(encodedData), md, MD5_DIGEST_LENGTH);

    return encodedData;
#endif
}

std::string AlibabaCloud::OSS::ComputeContentMD5(std::istream& stream) 
{
#ifdef USE_CRYPTO_MBEDTLS
	unsigned char md_value[16];
    unsigned int md_len = 16;
    unsigned int olen = 0;
    mbedtls_md5_context ctx;

    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts_ret(&ctx);

    auto currentPos = stream.tellg();
    if (currentPos == static_cast<std::streampos>(-1)) {
        currentPos = 0;
        stream.clear();
    }
    stream.seekg(0, stream.beg);

	// TODO: ethan, need to shrink the buffer, or stack overflow may happen?

    char streamBuffer[2048];
    while (stream.good())
    {
        stream.read(streamBuffer, 2048);
        auto bytesRead = stream.gcount();

        if (bytesRead > 0)
        {
            mbedtls_md5_update_ret(&ctx, (const unsigned char*)streamBuffer, static_cast<size_t>(bytesRead));
        }
    }

    mbedtls_md5_finish_ret(&ctx, md_value);
    mbedtls_md5_free(&ctx);

    stream.clear();
    stream.seekg(currentPos, stream.beg);

    //Based64
    char encodedData[100];

    mbedtls_base64_encode((unsigned char*)encodedData, sizeof(encodedData), &olen, md_value, md_len);

    return encodedData;
#else
    auto ctx = EVP_MD_CTX_create();

    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len = 0;

    EVP_MD_CTX_init(ctx);
#ifndef OPENSSL_IS_BORINGSSL 
    EVP_MD_CTX_set_flags(ctx, EVP_MD_CTX_FLAG_NON_FIPS_ALLOW);
#endif
    EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);

    auto currentPos = stream.tellg();
    if (currentPos == static_cast<std::streampos>(-1)) {
        currentPos = 0;
        stream.clear();
    }
    stream.seekg(0, stream.beg);

    char streamBuffer[2048];
    while (stream.good())
    {
        stream.read(streamBuffer, 2048);
        auto bytesRead = stream.gcount();

        if (bytesRead > 0)
        {
            EVP_DigestUpdate(ctx, streamBuffer, static_cast<size_t>(bytesRead));
        }
    }

    EVP_DigestFinal_ex(ctx, md_value, &md_len);
    EVP_MD_CTX_destroy(ctx);
    stream.clear();
    stream.seekg(currentPos, stream.beg);

    //Based64
    char encodedData[100];
    EVP_EncodeBlock(reinterpret_cast<unsigned char*>(encodedData), md_value, md_len);
	return encodedData;
#endif
}
static std::string HexToString(const unsigned char *data, size_t size)
{ 
    static char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    std::stringstream ss;
    for (size_t i = 0; i < size; i++)
        ss << hex[(data[i] >> 4)] << hex[(data[i] & 0x0F)];
    return ss.str();
}

std::string AlibabaCloud::OSS::ComputeContentETag(const std::string& data)
{
    return ComputeContentETag(data.c_str(), data.size());
}

std::string AlibabaCloud::OSS::ComputeContentETag(const char * data, size_t size)
{
    if (!data) {
        return "";
    }
#if defined(USE_CRYPTO_MBEDTLS)
    unsigned char md_data[16];

#if 0
    mbedtls_md5_context ctx;

    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts_ret(&ctx);
    mbedtls_md5_update_ret(&ctx, (const unsigned char*)data, size);
    mbedtls_md5_finish_ret(&ctx, md_data);
    mbedtls_md5_free(&ctx);
#endif

    mbedtls_md5_ret((const unsigned char*)data, size, md_data);

    return HexToString(md_data, 16);

#elif 0//defined(USE_CRYPTO_MBEDTLS)
	unsigned char md_data[16];

	mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_MD5), data, size, md_data);

	return HexToString(md_data, 16);
#else
    unsigned char md[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(data), size, (unsigned char*)&md);
	return HexToString(md, MD5_DIGEST_LENGTH);
#endif
}

std::string AlibabaCloud::OSS::ComputeContentETag(std::istream& stream)
{
#ifdef USE_CRYPTO_MBEDTLS
	unsigned char md_value[16];
    unsigned int md_len = 16;

	mbedtls_md5_context ctx;

    mbedtls_md5_init( &ctx);
    mbedtls_md5_starts_ret(&ctx);

    auto currentPos = stream.tellg();
    if (currentPos == static_cast<std::streampos>(-1)) {
        currentPos = 0;
        stream.clear();
    }
    stream.seekg(0, stream.beg);
	// TODO: ethan, need to shrink the buffer, or stack overflow may happen?
    char streamBuffer[2048];
    while (stream.good())
    {
        stream.read(streamBuffer, 2048);
        auto bytesRead = stream.gcount();

        if (bytesRead > 0)
        {
            mbedtls_md5_update_ret(&ctx, (const unsigned char*)streamBuffer, static_cast<size_t>(bytesRead));
        }
    }

    mbedtls_md5_finish_ret(&ctx, md_value);
	mbedtls_md5_free(&ctx);

    stream.clear();
    stream.seekg(currentPos, stream.beg);
    return HexToString(md_value, md_len);
#else
    auto ctx = EVP_MD_CTX_create();

    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len = 0;

    EVP_MD_CTX_init(ctx);
#ifndef OPENSSL_IS_BORINGSSL 
    EVP_MD_CTX_set_flags(ctx, EVP_MD_CTX_FLAG_NON_FIPS_ALLOW);
#endif

    EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);

    auto currentPos = stream.tellg();
    if (currentPos == static_cast<std::streampos>(-1)) {
        currentPos = 0;
        stream.clear();
    }
    stream.seekg(0, stream.beg);

    char streamBuffer[2048];
    while (stream.good())
    {
        stream.read(streamBuffer, 2048);
        auto bytesRead = stream.gcount();

        if (bytesRead > 0)
        {
            EVP_DigestUpdate(ctx, streamBuffer, static_cast<size_t>(bytesRead));
        }
    }

    EVP_DigestFinal_ex(ctx, md_value, &md_len);
    EVP_MD_CTX_destroy(ctx);
    stream.clear();
    stream.seekg(currentPos, stream.beg);
    return HexToString(md_value, md_len);
#endif
}


void AlibabaCloud::OSS::StringReplace(std::string & src, const std::string & s1, const std::string & s2)
{
    std::string::size_type pos =0;
    while ((pos = src.find(s1, pos)) != std::string::npos) 
    {
        src.replace(pos, s1.length(), s2);
        pos += s2.length(); 
    }
}

std::string AlibabaCloud::OSS::LeftTrim(const char* source)
{
    std::string copy(source);
    copy.erase(copy.begin(), std::find_if(copy.begin(), copy.end(), [](unsigned char ch) { return !::isspace(ch); }));
    return copy;
}

std::string AlibabaCloud::OSS::RightTrim(const char* source)
{
    std::string copy(source);
    copy.erase(std::find_if(copy.rbegin(), copy.rend(), [](unsigned char ch) { return !::isspace(ch); }).base(), copy.end());
    return copy;
}

std::string AlibabaCloud::OSS::Trim(const char* source)
{
    return LeftTrim(RightTrim(source).c_str());
}

std::string AlibabaCloud::OSS::LeftTrimQuotes(const char* source)
{
    std::string copy(source);
    copy.erase(copy.begin(), std::find_if(copy.begin(), copy.end(), [](int ch) { return !(ch == '"'); }));
    return copy;
}

std::string AlibabaCloud::OSS::RightTrimQuotes(const char* source)
{
    std::string copy(source);
    copy.erase(std::find_if(copy.rbegin(), copy.rend(), [](int ch) { return !(ch == '"'); }).base(), copy.end());
    return copy;
}

std::string AlibabaCloud::OSS::TrimQuotes(const char* source)
{
    return LeftTrimQuotes(RightTrimQuotes(source).c_str());
}

std::string AlibabaCloud::OSS::ToLower(const char* source)
{
    std::string copy;
    if (source) {
        size_t srcLength = strlen(source);
        copy.resize(srcLength);
        std::transform(source, source + srcLength, copy.begin(), [](unsigned char c) { return (char)::tolower(c); });
    }
    return copy;
}

std::string AlibabaCloud::OSS::ToUpper(const char* source)
{
    std::string copy;
    if (source) {
        size_t srcLength = strlen(source);
        copy.resize(srcLength);
        std::transform(source, source + srcLength, copy.begin(), [](unsigned char c) { return (char)::toupper(c); });
    }
    return copy;
}

bool AlibabaCloud::OSS::IsIp(const std::string &host)
{
#if defined(__GNUG__) && __GNUC__ < 5
    int n[4];
    char c[4];
    const char *p = host.c_str();
    if (sscanf(p, "%d%c%d%c%d%c%d%c", &n[0], &c[0], &n[1], &c[1], &n[2], &c[2], &n[3], &c[3]) == 7)
    {
        int i;
        for (i = 0; i < 3; ++i)
            if (c[i] != '.')
                return false;
        for (i = 0; i < 4; ++i)
            if (n[i] > 255 || n[i] < 0)
                return false;
        return true;
    }
    return false;
#else
    return std::regex_match(host, ipPattern);
#endif
}

std::string AlibabaCloud::OSS::ToGmtTime(std::time_t &t)
{
    std::stringstream date;
    std::tm tm;
#ifdef _WIN32
    ::gmtime_s(&tm, &t);
#else
    ::gmtime_r(&t, &tm);
#endif

#if defined(__GNUG__) && __GNUC__ < 5
    static const char wday_name[][4] = {
      "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    static const char mon_name[][4] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    char tmbuff[26];
    snprintf(tmbuff, sizeof(tmbuff), "%.3s, %.2d %.3s %d %.2d:%.2d:%.2d",
        wday_name[tm.tm_wday], tm.tm_mday, mon_name[tm.tm_mon],
        1900 + tm.tm_year,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
    date << tmbuff << " GMT";
#else
    date.imbue(std::locale::classic());
    date << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
#endif
    return date.str();    
}

std::string AlibabaCloud::OSS::ToUtcTime(std::time_t &t)
{
    std::stringstream date;
    std::tm tm;
#ifdef _WIN32
    ::gmtime_s(&tm, &t);
#else
    ::gmtime_r(&t, &tm);
#endif
#if defined(__GNUG__) && __GNUC__ < 5
    char tmbuff[26];
    strftime(tmbuff, 26, "%Y-%m-%dT%H:%M:%S.000Z", &tm);
    date << tmbuff;
#else
    date.imbue(std::locale::classic());
    date << std::put_time(&tm, "%Y-%m-%dT%X.000Z");
#endif
    return date.str();
}

std::time_t AlibabaCloud::OSS::UtcToUnixTime(const std::string &t)
{
    const char* date = t.c_str();
    std::tm tm;
    std::time_t tt = -1;
    int ms;
    auto result = sscanf(date, "%4d-%2d-%2dT%2d:%2d:%2d.%dZ",
        &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec, &ms);

    if (result == 7) {
        tm.tm_year = tm.tm_year - 1900;
        tm.tm_mon = tm.tm_mon - 1;
#ifdef _WIN32
        tt = _mkgmtime64(&tm);
#else
#if 0
        tt = timegm(&tm);
#endif
#endif // _WIN32
    }
    return tt < 0 ? -1 : tt;
}

bool AlibabaCloud::OSS::IsValidBucketName(const std::string &bucketName)
{
#if defined(__GNUG__) && __GNUC__ < 5
    if (bucketName.size() < 3 || bucketName.size() > 63)
        return false;
    for (auto it = bucketName.begin(); it != bucketName.end(); it++) {
        if (!((*it >= 'a' && *it <= 'z') || (*it >= '0' && *it <= '9') || *it == '-'))
            return false;
    }
    if (*bucketName.begin() == '-' || *bucketName.rbegin() == '-')
        return false;
    return true;
#else
    if (bucketName.empty())
        return false;
    return std::regex_match(bucketName, bucketNamePattern);
#endif
}

 bool AlibabaCloud::OSS::IsValidObjectKey(const std::string & key)
{
    if (key.empty() || !key.compare(0, 1, "\\", 1))
         return false;

    return key.size() <= ObjectNameLengthLimit;
}

 bool AlibabaCloud::OSS::IsValidLoggingPrefix(const std::string &prefix)
 {
     if (prefix.empty())
         return true;
#if defined(__GNUG__) && __GNUC__ < 5
     if (prefix.size() > 32)
         return false;

     auto ch = static_cast<int>(*prefix.begin());
     if (isalpha(ch) == 0)
         return false;

     for (auto it = prefix.begin(); it != prefix.end(); it++) {
         ch = static_cast<int>(*it);
         if (!(::isalpha(ch) || ::isdigit(ch) || *it == '-'))
             return false;
     }
     return true;
#else
     return std::regex_match(prefix, loggingPrefixPattern);
#endif
 }

 bool AlibabaCloud::OSS::IsValidChannelName(const std::string &channelName)
 {
     if(channelName.empty() ||
        std::string::npos != channelName.find('/') || 
        channelName.size() > MaxLiveChannelNameLength)
        return false;
    
    return true;
 }

 bool AlibabaCloud::OSS::IsValidPlayListName(const std::string &playlistName)
 {
     if(playlistName.empty())
    {
        return false;
    }else{
        if(!IsValidObjectKey(playlistName))
        {
            return false;
        }
        if(playlistName.size() < MinLiveChannelPlayListLength || 
            playlistName.size() > MaxLiveChannelPlayListLength)
        {
            return false;
        }
        std::size_t lastPos = playlistName.find_last_of('.');
        std::size_t slashPos = playlistName.find('/');
        if(lastPos == std::string::npos || 
            slashPos != std::string::npos ||
            0 == lastPos || '.' == playlistName[lastPos-1])
        {
            return false;
        }else{
            std::string suffix = playlistName.substr(lastPos);
            if(suffix.size() < 5)
            {
                return false;
            }
            if(ToLower(suffix.c_str()) != ".m3u8")
            {
                return false;
            }
            return true;
        }
    }
 }

 bool AlibabaCloud::OSS::IsValidTagKey(const std::string &key)
 {
     if (key.empty() || key.size() > TagKeyLengthLimit)
         return false;

     return true;
 }

 bool AlibabaCloud::OSS::IsValidTagValue(const std::string &value)
 {
     return value.size() <= TagValueLengthLimit;
 }

const std::string& AlibabaCloud::OSS::LookupMimeType(const std::string &name)
{
    const static std::map<std::string, std::string> mimeType = {
        {"html", "text/html"},
        {"htm", "text/html"},
        {"shtml", "text/html"},
        {"css", "text/css"},
        {"xml", "text/xml"},
        {"gif", "image/gif"},
        {"jpeg", "image/jpeg"},
        {"jpg", "image/jpeg"},
        {"js", "application/x-javascript"},
        {"atom", "application/atom+xml"},
        {"rss", "application/rss+xml"},
        {"mml", "text/mathml"},
        {"txt", "text/plain"},
        {"jad", "text/vnd.sun.j2me.app-descriptor"},
        {"wml", "text/vnd.wap.wml"},
        {"htc", "text/x-component"},
        {"png", "image/png"},
        {"tif", "image/tiff"},
        {"tiff", "image/tiff"},
        {"wbmp", "image/vnd.wap.wbmp"},
        {"ico", "image/x-icon"},
        {"jng", "image/x-jng"},
        {"bmp", "image/x-ms-bmp"},
        {"svg", "image/svg+xml"},
        {"svgz", "image/svg+xml"},
        {"webp", "image/webp"},
        {"jar", "application/java-archive"},
        {"war", "application/java-archive"},
        {"ear", "application/java-archive"},
        {"hqx", "application/mac-binhex40"},
        {"doc ", "application/msword"},
        {"pdf", "application/pdf"},
        {"ps", "application/postscript"},
        {"eps", "application/postscript"},
        {"ai", "application/postscript"},
        {"rtf", "application/rtf"},
        {"xls", "application/vnd.ms-excel"},
        {"ppt", "application/vnd.ms-powerpoint"},
        {"wmlc", "application/vnd.wap.wmlc"},
        {"kml", "application/vnd.google-earth.kml+xml"},
        {"kmz", "application/vnd.google-earth.kmz"},
        {"7z", "application/x-7z-compressed"},
        {"cco", "application/x-cocoa"},
        {"jardiff", "application/x-java-archive-diff"},
        {"jnlp", "application/x-java-jnlp-file"},
        {"run", "application/x-makeself"},
        {"pl", "application/x-perl"},
        {"pm", "application/x-perl"},
        {"prc", "application/x-pilot"},
        {"pdb", "application/x-pilot"},
        {"rar", "application/x-rar-compressed"},
        {"rpm", "application/x-redhat-package-manager"},
        {"sea", "application/x-sea"},
        {"swf", "application/x-shockwave-flash"},
        {"sit", "application/x-stuffit"},
        {"tcl", "application/x-tcl"},
        {"tk", "application/x-tcl"},
        {"der", "application/x-x509-ca-cert"},
        {"pem", "application/x-x509-ca-cert"},
        {"crt", "application/x-x509-ca-cert"},
        {"xpi", "application/x-xpinstall"},
        {"xhtml", "application/xhtml+xml"},
        {"zip", "application/zip"},
        {"wgz", "application/x-nokia-widget"},
        {"bin", "application/octet-stream"},
        {"exe", "application/octet-stream"},
        {"dll", "application/octet-stream"},
        {"deb", "application/octet-stream"},
        {"dmg", "application/octet-stream"},
        {"eot", "application/octet-stream"},
        {"iso", "application/octet-stream"},
        {"img", "application/octet-stream"},
        {"msi", "application/octet-stream"},
        {"msp", "application/octet-stream"},
        {"msm", "application/octet-stream"},
        {"mid", "audio/midi"},
        {"midi", "audio/midi"},
        {"kar", "audio/midi"},
        {"mp3", "audio/mpeg"},
        {"ogg", "audio/ogg"},
        {"m4a", "audio/x-m4a"},
        {"ra", "audio/x-realaudio"},
        {"3gpp", "video/3gpp"},
        {"3gp", "video/3gpp"},
        {"mp4", "video/mp4"},
        {"mpeg", "video/mpeg"},
        {"mpg", "video/mpeg"},
        {"mov", "video/quicktime"},
        {"webm", "video/webm"},
        {"flv", "video/x-flv"},
        {"m4v", "video/x-m4v"},
        {"mng", "video/x-mng"},
        {"asx", "video/x-ms-asf"},
        {"asf", "video/x-ms-asf"},
        {"wmv", "video/x-ms-wmv"},
        {"avi", "video/x-msvideo"},
        {"ts", "video/MP2T"},
        {"m3u8", "application/x-mpegURL"},
        {"apk", "application/vnd.android.package-archive"}
        };

    const static std::string defaultType("application/octet-stream");
    std::string::size_type last_pos  = name.find_last_of('.');
    std::string::size_type first_pos = name.find_first_of('.');
    std::string prefix, ext, ext2;

    if (last_pos == std::string::npos) {
        return defaultType;
    }

    // extract the last extension
    if (last_pos != std::string::npos) {
        ext = name.substr(1 + last_pos, std::string::npos);
    }
    if (last_pos != std::string::npos) {
        if (first_pos != std::string::npos && first_pos < last_pos) {
            prefix = name.substr(0, last_pos);
            // Now get the second to last file extension
            std::string::size_type next_pos = prefix.find_last_of('.');
            if (next_pos != std::string::npos) {
                ext2 = prefix.substr(1 + next_pos, std::string::npos);
            }
        }
    }

    ext = ToLower(ext.c_str());
    auto iter = mimeType.find(ext);
    if (iter != mimeType.end()) {
        return (*iter).second; 
    }

    if (first_pos == last_pos) {
        return defaultType;
    }

    ext2 = ToLower(ext2.c_str());
    iter = mimeType.find(ext2);
    if (iter != mimeType.end()) {
        return (*iter).second;
    }

    return defaultType;
}

std::string AlibabaCloud::OSS::CombineHostString(
    const std::string &endpoint, 
    const std::string &bucket, 
    bool isCname)
{
    Url url(endpoint);
    if (url.scheme().empty()) {
        url.setScheme(Http::SchemeToString(Http::HTTP));
    }

    if (!bucket.empty() && !isCname && !IsIp(url.host())) {
        std::string host(bucket);
        host.append(".").append(url.host());
        url.setHost(host);
    }

    std::ostringstream out;
    out << url.scheme() << "://" << url.authority();
    return out.str();
}

std::string AlibabaCloud::OSS::CombinePathString(
    const std::string &endpoint,
    const std::string &bucket,
    const std::string &key)
{
    Url url(endpoint);
    std::string path;
    path = "/";
    if (IsIp(url.host())) {
        path.append(bucket).append("/");
    }
    path.append(UrlEncode(key));
    return path;
}

std::string AlibabaCloud::OSS::CombineRTMPString(
    const std::string &endpoint, 
    const std::string &bucket,
    bool isCname)
{
    Url url(endpoint);
    if (!bucket.empty() && !isCname && !IsIp(url.host())) {
        std::string host(bucket);
        host.append(".").append(url.host());
        url.setHost(host);
    }

    std::ostringstream out;
    out << "rtmp://" << url.authority();
    return out.str();
}

std::string AlibabaCloud::OSS::CombineQueryString(const ParameterCollection &parameters)
{
    std::stringstream ss;
    if (!parameters.empty()) {
        for (const auto &p : parameters)
        {
            if (p.second.empty())
                ss << "&" << UrlEncode(p.first);
            else
                ss << "&" << UrlEncode(p.first) << "=" << UrlEncode(p.second);
        }
    }
    return ss.str().substr(1);
}

std::streampos AlibabaCloud::OSS::GetIOStreamLength(std::iostream &stream)
{
    auto currentPos = stream.tellg();
    if (currentPos == static_cast<std::streampos>(-1)) {
        currentPos = 0;
        stream.clear();
    }
    stream.seekg(0, stream.end);
    auto streamSize = stream.tellg();
    stream.seekg(currentPos, stream.beg);
    return streamSize;
}

const char *AlibabaCloud::OSS::ToStorageClassName(StorageClass storageClass)
{
    static const char *StorageClassName[] = { "Standard", "IA", "Archive", "ColdArchive" };
    return StorageClassName[storageClass - StorageClass::Standard];
}

StorageClass AlibabaCloud::OSS::ToStorageClassType(const char *name)
{
    std::string storageName = ToLower(name);
    if(!storageName.compare("ia")) return StorageClass::IA;
    else if (!storageName.compare("archive")) return StorageClass::Archive;
    else if (!storageName.compare("coldarchive")) return StorageClass::ColdArchive;
    else return StorageClass::Standard;
}

const char *AlibabaCloud::OSS::ToAclName(CannedAccessControlList acl)
{
    static const char *AclName[] = { "private", "public-read", "public-read-write", "default"};
    return AclName[acl - CannedAccessControlList::Private];
}

CannedAccessControlList AlibabaCloud::OSS::ToAclType(const char *name)
{
    std::string aclName = ToLower(name);
    if (!aclName.compare("private")) return CannedAccessControlList::Private;
    else if (!aclName.compare("public-read")) return CannedAccessControlList::PublicRead;
    else if (!aclName.compare("public-read-write")) return CannedAccessControlList::PublicReadWrite;
    else return CannedAccessControlList::Default;
}

const char *AlibabaCloud::OSS::ToCopyActionName(CopyActionList action)
{
    static const char *ActionName[] = { "COPY", "REPLACE"};
    return ActionName[action - CopyActionList::Copy];
}

const char *AlibabaCloud::OSS::ToRuleStatusName(RuleStatus status)
{
    static const char *StatusName[] = { "Enabled", "Disabled" };
    return StatusName[status - RuleStatus::Enabled];
}

RuleStatus AlibabaCloud::OSS::ToRuleStatusType(const char *name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("enabled")) return RuleStatus::Enabled;
    else return RuleStatus::Disabled;
}

const char *AlibabaCloud::OSS::ToLiveChannelStatusName(LiveChannelStatus status)
{
    if(status > LiveChannelStatus::LiveStatus)
        return "";

    static const char *StatusName[] = { "enabled", "disabled", "idle", "live" };
    return StatusName[status - LiveChannelStatus::EnabledStatus];
}

LiveChannelStatus AlibabaCloud::OSS::ToLiveChannelStatusType(const char *name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("enabled")) return LiveChannelStatus::EnabledStatus;
    else if (!statusName.compare("disabled")) return LiveChannelStatus::DisabledStatus;
    else if (!statusName.compare("idle")) return LiveChannelStatus::IdleStatus;
    else if (!statusName.compare("live")) return LiveChannelStatus::LiveStatus;
    else return LiveChannelStatus::UnknownStatus;
}

const char* AlibabaCloud::OSS::ToRequestPayerName(RequestPayer payer)
{
    static const char* PayerName[] = { "NotSet", "BucketOwner", "Requester"};
    return PayerName[static_cast<int>(payer) - static_cast<int>(RequestPayer::NotSet)];
}

RequestPayer AlibabaCloud::OSS::ToRequestPayer(const char* name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("bucketowner")) return RequestPayer::BucketOwner;
    else if (!statusName.compare("requester")) return RequestPayer::Requester;
    else return RequestPayer::NotSet;
}

const char* AlibabaCloud::OSS::ToSSEAlgorithmName(SSEAlgorithm sse)
{
    static const char* StatusName[] = { "NotSet", "KMS", "AES256" };
    return StatusName[static_cast<int>(sse) - static_cast<int>(SSEAlgorithm::NotSet)];
}

SSEAlgorithm AlibabaCloud::OSS::ToSSEAlgorithm(const char* name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("kms")) return SSEAlgorithm::KMS;
    else if (!statusName.compare("aes256")) return SSEAlgorithm::AES256;
    else return SSEAlgorithm::NotSet;
}

DataRedundancyType AlibabaCloud::OSS::ToDataRedundancyType(const char* name)
{
    std::string storageName = ToLower(name);
    if (!storageName.compare("lrs")) return DataRedundancyType::LRS;
    else if (!storageName.compare("zrs")) return DataRedundancyType::ZRS;
    else return DataRedundancyType::NotSet;
}

const char* AlibabaCloud::OSS::ToDataRedundancyTypeName(DataRedundancyType type)
{
    static const char* typeName[] = { "NotSet", "LRS", "ZRS" };
    return typeName[static_cast<int>(type) - static_cast<int>(DataRedundancyType::NotSet)];
}

const char * AlibabaCloud::OSS::ToVersioningStatusName(VersioningStatus status)
{
    static const char *StatusName[] = { "NotSet", "Enabled", "Suspended" };
    return StatusName[static_cast<int>(status) - static_cast<int>(VersioningStatus::NotSet)];
}

VersioningStatus AlibabaCloud::OSS::ToVersioningStatusType(const char *name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("enabled")) return VersioningStatus::Enabled;
    else if (!statusName.compare("suspended")) return VersioningStatus::Suspended;
    else return VersioningStatus::NotSet;
}

const char* AlibabaCloud::OSS::ToInventoryFormatName(InventoryFormat status)
{
    static const char* StatusName[] = { "NotSet", "CSV"};
    return StatusName[static_cast<int>(status) - static_cast<int>(InventoryFormat::NotSet)];
}

InventoryFormat AlibabaCloud::OSS::ToInventoryFormatType(const char* name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("csv")) return InventoryFormat::CSV;
    else return InventoryFormat::NotSet;
}

const char* AlibabaCloud::OSS::ToInventoryFrequencyName(InventoryFrequency status)
{
    static const char* StatusName[] = { "NotSet", "Daily", "Weekly" };
    return StatusName[static_cast<int>(status) - static_cast<int>(InventoryFrequency::NotSet)];
}

InventoryFrequency AlibabaCloud::OSS::ToInventoryFrequencyType(const char* name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("daily")) return InventoryFrequency::Daily;
    else if (!statusName.compare("weekly")) return InventoryFrequency::Weekly;
    else return InventoryFrequency::NotSet;
}

const char* AlibabaCloud::OSS::ToInventoryOptionalFieldName(InventoryOptionalField status)
{
    static const char* StatusName[] = { "NotSet", "Size", "LastModifiedDate", "ETag", "StorageClass", "IsMultipartUploaded", "EncryptionStatus" };
    return StatusName[static_cast<int>(status) - static_cast<int>(InventoryOptionalField::NotSet)];
}

InventoryOptionalField AlibabaCloud::OSS::ToInventoryOptionalFieldType(const char* name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("size")) return InventoryOptionalField::Size;
    else if (!statusName.compare("lastmodifieddate")) return InventoryOptionalField::LastModifiedDate;
    else if (!statusName.compare("etag")) return InventoryOptionalField::ETag;
    else if (!statusName.compare("storageclass")) return InventoryOptionalField::StorageClass;
    else if (!statusName.compare("ismultipartuploaded")) return InventoryOptionalField::IsMultipartUploaded;
    else if (!statusName.compare("encryptionstatus")) return InventoryOptionalField::EncryptionStatus;
    else return InventoryOptionalField::NotSet;
}

const char* AlibabaCloud::OSS::ToInventoryIncludedObjectVersionsName(InventoryIncludedObjectVersions status)
{
    static const char* StatusName[] = { "NotSet", "All", "Current" };
    return StatusName[static_cast<int>(status) - static_cast<int>(InventoryIncludedObjectVersions::NotSet)];
}

InventoryIncludedObjectVersions AlibabaCloud::OSS::ToInventoryIncludedObjectVersionsType(const char* name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("all")) return InventoryIncludedObjectVersions::All;
    else if (!statusName.compare("current")) return InventoryIncludedObjectVersions::Current;
    else return InventoryIncludedObjectVersions::NotSet;
}

std::string AlibabaCloud::OSS::ToInventoryBucketFullName(const std::string& name)
{
    std::string fullName = "acs:oss:::";
    return fullName.append(name);
}

std::string AlibabaCloud::OSS::ToInventoryBucketShortName(const char* name)
{
    std::string name_ = ToLower(name);
    std::string shortName;

    if (!name_.compare(0, 10, "acs:oss:::")) {
        shortName.append(name + 10);
    }
    return shortName;
}

const char * AlibabaCloud::OSS::ToTierTypeName(TierType status)
{
    static const char *StatusName[] = { "Expedited", "Standard", "Bulk" };
    return StatusName[static_cast<int>(status) - static_cast<int>(TierType::Expedited)];
}

TierType AlibabaCloud::OSS::ToTierType(const char *name)
{
    std::string statusName = ToLower(name);
    if (!statusName.compare("expedited")) return TierType::Expedited;
    else if (!statusName.compare("bulk")) return TierType::Bulk;
    else return TierType::Standard;
}

#if !defined(OSS_DISABLE_RESUAMABLE) || !defined(OSS_DISABLE_ENCRYPTION)
std::map<std::string, std::string> AlibabaCloud::OSS::JsonStringToMap(const std::string& jsonStr)
{
    std::map<std::string, std::string> valueMap;
    Json::Value root;
    Json::CharReaderBuilder rbuilder;
    std::stringstream input(jsonStr);
    std::string errMsg;

    if (Json::parseFromStream(rbuilder, input, &root, &errMsg)) {

        for (auto it = root.begin(); it != root.end(); ++it)
        {
            valueMap[it.key().asString()] = (*it).asString();
        }
    }

    return valueMap;
}

std::string AlibabaCloud::OSS::MapToJsonString(const std::map<std::string, std::string>& map)
{
    if (map.empty()) {
        return "";
    }
    Json::Value root;
    for (const auto& it : map) {
        root[it.first] = it.second;
    }

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, root);
}
#endif

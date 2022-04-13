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

#include <alibabacloud/oss/model/CompleteMultipartUploadResult.h>
#include <external/tinyxml2/tinyxml2.h>
#include "utils/Utils.h"
#include <alibabacloud/oss/http/HttpType.h>

using namespace AlibabaCloud::OSS;
using namespace tinyxml2;


CompleteMultipartUploadResult::CompleteMultipartUploadResult() :
    OssObjectResult(),
    crc64_(0),
    content_(nullptr)
{
}

CompleteMultipartUploadResult::CompleteMultipartUploadResult(const std::string& result) :
    CompleteMultipartUploadResult()
{
    *this = result;
}

CompleteMultipartUploadResult::CompleteMultipartUploadResult(const std::shared_ptr<std::iostream>& result,
    const HeaderCollection& headers) :
    OssObjectResult(headers),
    crc64_(0),
    content_(nullptr)
{
    std::string contentType;
    if (headers.find(Http::CONTENT_TYPE) != headers.end()) {
        contentType = ToLower(headers.at(Http::CONTENT_TYPE).c_str());
    }

    if (contentType.compare("application/json") != 0) {
        std::istreambuf_iterator<char> isb(*result.get()), end;
        std::string str(isb, end);
        *this = str;
    }
    else {
        content_ = result;
        parseDone_ = true;
    }

    if (headers.find("x-oss-hash-crc64ecma") != headers.end()) {
        crc64_ =  std::strtoull(headers.at("x-oss-hash-crc64ecma").c_str(), nullptr, 10);
    }

    if (eTag_.empty() && headers.find(Http::ETAG) != headers.end()) {
        eTag_ = TrimQuotes(headers.at(Http::ETAG).c_str());
    }
}

CompleteMultipartUploadResult& CompleteMultipartUploadResult::operator =(const std::string& result)
{
    if (result.empty()) {
        parseDone_ = true;
        return *this;
    }

    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("CompleteMultipartUploadResult", root->Name(), 29)) {
            XMLElement *node;
            node = root->FirstChildElement("EncodingType");
            if (node && node->GetText())
            {
                encodingType_ = node->GetText();
            }
            bool bEncode = false;
            bEncode = !ToLower(encodingType_.c_str()).compare(0, 3, "url", 3);

            node = root->FirstChildElement("Bucket");
            if (node && node->GetText())
            {
                bucket_ = node->GetText();
            }

            node = root->FirstChildElement("Location");
            if(node && node->GetText())
            {
                if (bEncode)
                {
                    location_ = UrlDecode(node->GetText());
                }
                else {
                    location_ = node->GetText();
                }
            }

            node = root->FirstChildElement("Key");
            if(node && node->GetText())
            {
                if(bEncode)
                {
                    key_ = UrlDecode(node->GetText());
                }else{
                    key_ = node->GetText();
                }
            }

            node = root->FirstChildElement("ETag");
            if(node && node->GetText())
            {
                eTag_ = node->GetText();
            }
            parseDone_ = true;
        }
    }
    return *this;
}

const std::string& CompleteMultipartUploadResult::Location() const
{
    return location_;
}

const std::string& CompleteMultipartUploadResult::Key() const
{
    return key_;
}

const std::string& CompleteMultipartUploadResult::Bucket() const
{
    return bucket_;
}

const std::string& CompleteMultipartUploadResult::ETag() const
{
    return eTag_;
}

const std::string& CompleteMultipartUploadResult::EncodingType() const
{
    return encodingType_;
}

uint64_t CompleteMultipartUploadResult::CRC64() const
{
    return crc64_;
}

const std::shared_ptr<std::iostream>& CompleteMultipartUploadResult::Content() const
{
    return content_;
}

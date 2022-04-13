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

#include <sstream>
#include <alibabacloud/oss/model/ListPartsResult.h>
#include <external/tinyxml2/tinyxml2.h>
#include "utils/Utils.h"

using namespace AlibabaCloud::OSS;
using namespace tinyxml2;
using std::stringstream;

ListPartsResult::ListPartsResult():
    OssResult(),
    maxParts_(0),
    partNumberMarker_(0),
    nextPartNumberMarker_(0),
    isTruncated_(false)
{
}

ListPartsResult::ListPartsResult(const std::string& result):
    ListPartsResult()
{
    *this = result;
}

ListPartsResult::ListPartsResult(const std::shared_ptr<std::iostream>& result):
    ListPartsResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

ListPartsResult& ListPartsResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("ListPartsResult", root->Name(), 15)) {
            XMLElement *node;
            node = root->FirstChildElement("EncodingType");
            if (node && node->GetText()) encodingType_ = node->GetText();

            //Detect encode type
            bool useUrlDecode = !ToLower(encodingType_.c_str()).compare(0, 3, "url", 3);

            node = root->FirstChildElement("Bucket");
            if (node && node->GetText()) bucket_ = node->GetText();

            node = root->FirstChildElement("Key");
            if (node && node->GetText()) key_ = useUrlDecode ? UrlDecode(node->GetText()) : node->GetText();

            node = root->FirstChildElement("UploadId");
            if (node && node->GetText()) uploadId_ = node->GetText();

            node = root->FirstChildElement("PartNumberMarker");
            if(node && node->GetText())
            {
                partNumberMarker_ = std::strtoul(node->GetText(), nullptr, 10);
            }

            node = root->FirstChildElement("NextPartNumberMarker");
            if(node && node->GetText())
            {
                nextPartNumberMarker_ = std::strtoul(node->GetText(), nullptr, 10);
            }

            node = root->FirstChildElement("MaxParts");
            if (node && node->GetText())
            {
                maxParts_ = std::strtoul(node->GetText(), nullptr, 10);
            }

            node = root->FirstChildElement("IsTruncated");
            if (node && node->GetText()) isTruncated_ = node->BoolText();

            XMLElement * partNode = root->FirstChildElement("Part");
            for( ; partNode ; partNode = partNode->NextSiblingElement("Part"))
            {
                Part part;
                node = partNode->FirstChildElement("PartNumber");
                if(node && node->GetText())
                {
                    part.partNumber_ = std::atoi(node->GetText());
                }

                node = partNode->FirstChildElement("LastModified");
                if(node && node->GetText()) part.lastModified_ = node->GetText();

                node = partNode->FirstChildElement("ETag");
                if (node && node->GetText()) part.eTag_ = TrimQuotes(node->GetText());

                node = partNode->FirstChildElement("Size");
                if(node && node->GetText())
                {
                    part.size_ = std::strtoll(node->GetText(), nullptr, 10);
                }

                node = partNode->FirstChildElement("HashCrc64ecma");
                if(node && node->GetText())
                {
                    part.cRC64_ = std::strtoull(node->GetText(), nullptr, 10);
                }
                partList_.push_back(part);
            }
        }
        //TODO check the result and the parse flag;
        parseDone_ = true;
    }
    return *this;
}

const std::string& ListPartsResult::UploadId() const
{
    return uploadId_;
}

const std::string& ListPartsResult::Key() const
{
    return key_;
}
const std::string& ListPartsResult::Bucket() const
{
    return bucket_;
}
const std::string& ListPartsResult::EncodingType() const
{
    return encodingType_;
}

uint32_t ListPartsResult::MaxParts() const
{
    return maxParts_;
}

uint32_t ListPartsResult::PartNumberMarker() const
{
    return partNumberMarker_;
}

uint32_t ListPartsResult::NextPartNumberMarker() const
{
    return nextPartNumberMarker_;
}

const PartList& ListPartsResult::PartList() const
{
    return partList_;
}

bool ListPartsResult::IsTruncated() const
{
    return isTruncated_;
}

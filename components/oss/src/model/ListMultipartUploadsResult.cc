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
#include <alibabacloud/oss/model/ListMultipartUploadsResult.h>
#include <alibabacloud/oss/model/Owner.h>
#include <external/tinyxml2/tinyxml2.h>
#include "utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;
using std::stringstream;

ListMultipartUploadsResult::ListMultipartUploadsResult() :
    OssResult(),
    maxUploads_(0),
    isTruncated_(false)
{
}

ListMultipartUploadsResult::ListMultipartUploadsResult(
    const std::string& result):
    ListMultipartUploadsResult()
{
    *this = result;
}

ListMultipartUploadsResult::ListMultipartUploadsResult(
    const std::shared_ptr<std::iostream>& result):
    ListMultipartUploadsResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

ListMultipartUploadsResult& ListMultipartUploadsResult::operator =(
    const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("ListMultipartUploadsResult", root->Name(), 26)) {
            XMLElement *node;

            node = root->FirstChildElement("Bucket");
            if (node && node->GetText()) {
                bucket_ = node->GetText();
            }

            node = root->FirstChildElement("EncodingType");
            bool isUrlEncode = false;
            if (node && node->GetText()) {
                encodingType_ = node->GetText();
                isUrlEncode = !ToLower(encodingType_.c_str()).compare(0, 3, "url", 3);
            }

            node  = root->FirstChildElement("KeyMarker");
            if(node && node->GetText())
            {
                keyMarker_ = isUrlEncode ? UrlDecode(node->GetText()) : node->GetText();
            }

            node = root->FirstChildElement("UploadIdMarker");
            if(node && node->GetText())
            {
                uploadIdMarker_ = node->GetText();
            }

            node  = root->FirstChildElement("NextKeyMarker");
            if(node && node->GetText())
            {
                nextKeyMarker_ = isUrlEncode ? 
                    UrlDecode(node->GetText()) : node->GetText();
            }

            node = root->FirstChildElement("NextUploadIdMarker");
            if(node && node->GetText())
            {
                nextUploadIdMarker_ = node->GetText();
            }

            node = root->FirstChildElement("MaxUploads");
            if(node && node->GetText())
            {
                maxUploads_ = std::strtoul(node->GetText(), nullptr, 10);
            }

            //CommonPrefixes
            node = root->FirstChildElement("CommonPrefixes");
            for (; node; node = node->NextSiblingElement("CommonPrefixes")) {
                XMLElement *prefix_node = node->FirstChildElement("Prefix");
                if (prefix_node && prefix_node->GetText()) commonPrefixes_.push_back(
                    (isUrlEncode ? UrlDecode(prefix_node->GetText()) : prefix_node->GetText()));
            }

            node = root->FirstChildElement("IsTruncated");
            if (node && node->GetText()) {
                isTruncated_ = node->BoolText();
            }

            XMLElement * uploadNode = root->FirstChildElement("Upload");
            for( ; uploadNode ; uploadNode = uploadNode->NextSiblingElement("Upload"))
            {
                MultipartUpload rec;
                node = uploadNode->FirstChildElement("Key");
                if(node && node->GetText())
                {
                    rec.Key = isUrlEncode ? UrlDecode(node->GetText()) : node->GetText();
                }

                node = uploadNode->FirstChildElement("UploadId");
                if(node && node->GetText())
                {
                    rec.UploadId = node->GetText();
                }

                node = uploadNode->FirstChildElement("Initiated");
                if(node && node->GetText())
                {
                    rec.Initiated = node->GetText();
                }
                multipartUploadList_.push_back(rec);
            }
            parseDone_ = true;
		}
    }
    return *this;
}

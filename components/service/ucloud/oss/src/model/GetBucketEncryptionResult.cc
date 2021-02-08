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


#include <alibabacloud/oss/model/GetBucketEncryptionResult.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;


GetBucketEncryptionResult::GetBucketEncryptionResult() :
    OssResult()
{
}

GetBucketEncryptionResult::GetBucketEncryptionResult(const std::string& result) :
    GetBucketEncryptionResult()
{
    *this = result;
}

GetBucketEncryptionResult::GetBucketEncryptionResult(const std::shared_ptr<std::iostream>& result) :
    GetBucketEncryptionResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketEncryptionResult& GetBucketEncryptionResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("ServerSideEncryptionRule", root->Name(), 24)) {
            XMLElement* node;

            node = root->FirstChildElement("ApplyServerSideEncryptionByDefault");
            if (node) {
                XMLElement* sub_node;
                sub_node = node->FirstChildElement("SSEAlgorithm");
                if (sub_node && sub_node->GetText()) SSEAlgorithm_ = ToSSEAlgorithm(sub_node->GetText());

                sub_node = node->FirstChildElement("KMSMasterKeyID");
                if (sub_node && sub_node->GetText()) KMSMasterKeyID_ = sub_node->GetText();
            }
            parseDone_ = true;
        }
    }
    return *this;
}

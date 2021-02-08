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


#include <alibabacloud/oss/model/GetBucketAclResult.h>
#include <alibabacloud/oss/model/Owner.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

GetBucketAclResult::GetBucketAclResult() :
    OssResult()
{
}

GetBucketAclResult::GetBucketAclResult(const std::string& result):
    GetBucketAclResult()
{
    *this = result;
}

GetBucketAclResult::GetBucketAclResult(const std::shared_ptr<std::iostream>& result):
    GetBucketAclResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketAclResult& GetBucketAclResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("AccessControlPolicy", root->Name(), 19)) {
            XMLElement *node;

            node = root->FirstChildElement("Owner");
            std::string owner_ID, owner_DisplayName;
            if (node) {
                XMLElement *sub_node;
                sub_node = node->FirstChildElement("ID");
                if (sub_node && sub_node->GetText()) owner_ID = sub_node->GetText();

                sub_node = node->FirstChildElement("DisplayName");
                if (sub_node && sub_node->GetText()) owner_DisplayName = sub_node->GetText();
            }

            node = root->FirstChildElement("AccessControlList");
            if (node) {
                XMLElement *sub_node;
                sub_node = node->FirstChildElement("Grant");
                if (sub_node && sub_node->GetText()) acl_ = ToAclType(sub_node->GetText());
            }

            owner_ = AlibabaCloud::OSS::Owner(owner_ID, owner_DisplayName);

            //TODO check the result and the parse flag;
            parseDone_ = true;
        }
    }
    return *this;
}

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


#include <alibabacloud/oss/model/GetBucketCorsResult.h>
#include <alibabacloud/oss/model/Owner.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

GetBucketCorsResult::GetBucketCorsResult() :
    OssResult()
{
}

GetBucketCorsResult::GetBucketCorsResult(const std::string& result):
    GetBucketCorsResult()
{
    *this = result;
}

GetBucketCorsResult::GetBucketCorsResult(const std::shared_ptr<std::iostream>& result):
    GetBucketCorsResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketCorsResult& GetBucketCorsResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("CORSConfiguration", root->Name(), 17)) {
            XMLElement *rule_node = root->FirstChildElement("CORSRule");
            for (; rule_node; rule_node = rule_node->NextSiblingElement("CORSRule")) {
                XMLElement *node = rule_node->FirstChildElement();
                CORSRule rule;
                for (; node; node = node->NextSiblingElement()) {

                    if (!strncmp(node->Name(), "AllowedOrigin", 13))
                        if (node->GetText()) rule.addAllowedOrigin(node->GetText());

                    if (!strncmp(node->Name(), "AllowedMethod", 13))
                        if (node->GetText()) rule.addAllowedMethod(node->GetText());

                    if (!strncmp(node->Name(), "AllowedHeader", 13))
                        if (node->GetText()) rule.addAllowedHeader(node->GetText());

                    if (!strncmp(node->Name(), "ExposeHeader", 12))
                        if (node->GetText()) rule.addExposeHeader(node->GetText());

                    if (!strncmp(node->Name(), "MaxAgeSeconds", 13))
                        if (node->GetText()) rule.setMaxAgeSeconds(std::atoi(node->GetText()));
                }
                ruleList_.push_back(rule);
            }
            parseDone_ = true;
        }
    }
    return *this;
}

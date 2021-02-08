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


#include <alibabacloud/oss/model/GetBucketRefererResult.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;


GetBucketRefererResult::GetBucketRefererResult() :
    OssResult()
{
}

GetBucketRefererResult::GetBucketRefererResult(const std::string& result):
    GetBucketRefererResult()
{
    *this = result;
}

GetBucketRefererResult::GetBucketRefererResult(const std::shared_ptr<std::iostream>& result):
    GetBucketRefererResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketRefererResult& GetBucketRefererResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("RefererConfiguration", root->Name(), 20)) {
            XMLElement *node;
            node = root->FirstChildElement("AllowEmptyReferer");
            if (node && node->GetText()) allowEmptyReferer_ = (std::strncmp(node->GetText(), "true", 4)? false:true);

            node = root->FirstChildElement("RefererList");
            if (node) {
                XMLElement *referer_node = node->FirstChildElement("Referer");
                for (; referer_node; referer_node = referer_node->NextSiblingElement()) {
                    if (referer_node->GetText())
                        refererList_.push_back(referer_node->GetText());
                }
            }
		    parseDone_ = true;
		}
    }
    return *this;
}

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


#include <alibabacloud/oss/model/GetBucketTaggingResult.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

GetBucketTaggingResult::GetBucketTaggingResult() :
    OssResult()
{
}

GetBucketTaggingResult::GetBucketTaggingResult(const std::string& result) :
    GetBucketTaggingResult()
{
    *this = result;
}

GetBucketTaggingResult::GetBucketTaggingResult(const std::shared_ptr<std::iostream>& result) :
    GetBucketTaggingResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketTaggingResult& GetBucketTaggingResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("Tagging", root->Name(), 7)) {
            XMLElement* tagSet_node = root->FirstChildElement("TagSet");
            if (tagSet_node) {
                XMLElement* tag_node = tagSet_node->FirstChildElement("Tag");
                for (; tag_node; tag_node = tag_node->NextSiblingElement("Tag")) {
                    XMLElement* subNode;
                    Tag tag;
                    //Key
                    subNode = tag_node->FirstChildElement("Key");
                    if (subNode && subNode->GetText()) {
                        tag.setKey(subNode->GetText());
                    }
                    //Value
                    subNode = tag_node->FirstChildElement("Value");
                    if (subNode && subNode->GetText()) {
                        tag.setValue(subNode->GetText());
                    }
                    tagging_.addTag(tag);
                }
            }
            //TODO check the result and the parse flag;
            parseDone_ = true;
        }
    }
    return *this;
}


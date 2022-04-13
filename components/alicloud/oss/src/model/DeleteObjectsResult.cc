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

#include <alibabacloud/oss/model/DeleteObjectsResult.h>
#include <external/tinyxml2/tinyxml2.h>
#include "utils/Utils.h"

using namespace AlibabaCloud::OSS;
using namespace tinyxml2;


DeleteObjectsResult::DeleteObjectsResult() :
    OssResult(),
    quiet_(false),
    keyList_()
{
}

DeleteObjectsResult::DeleteObjectsResult(const std::string& result) :
    DeleteObjectsResult()
{
    *this = result;
}

DeleteObjectsResult::DeleteObjectsResult(const std::shared_ptr<std::iostream>& result) :
    DeleteObjectsResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

DeleteObjectsResult& DeleteObjectsResult::operator =(const std::string& result)
{
    if (result.empty()) {
        quiet_ = true;
        parseDone_ = true;
        return *this;
    }

    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("DeleteResult", root->Name(), 12)) {
            XMLElement *node;
            std::string encodeType;
            node = root->FirstChildElement("EncodingType");
            if (node && node->GetText()) encodeType = node->GetText();

            bool useUrlDecode = !ToLower(encodeType.c_str()).compare(0, 3, "url", 3);

            //Deleted
            node = root->FirstChildElement("Deleted");
            for (; node; node = node->NextSiblingElement("Deleted")) {
                XMLElement *sub_node;
                sub_node = node->FirstChildElement("Key");
                if (sub_node && sub_node->GetText()) keyList_.push_back(useUrlDecode?UrlDecode(sub_node->GetText()):sub_node->GetText());
            }
        }
        parseDone_ = true;
    }
    return *this;
}

bool DeleteObjectsResult::Quiet() const
{
    return quiet_;
}

const std::list<std::string>& DeleteObjectsResult::keyList() const
{
    return keyList_;
}


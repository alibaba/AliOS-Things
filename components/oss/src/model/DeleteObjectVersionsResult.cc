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

#include <alibabacloud/oss/model/DeleteObjectVersionsResult.h>
#include <external/tinyxml2/tinyxml2.h>
#include "utils/Utils.h"

using namespace AlibabaCloud::OSS;
using namespace tinyxml2;


DeleteObjectVersionsResult::DeleteObjectVersionsResult() :
    OssResult(),
    quiet_(false),
    deletedObjects_()
{
}

DeleteObjectVersionsResult::DeleteObjectVersionsResult(const std::string& result) :
    DeleteObjectVersionsResult()
{
    *this = result;
}

DeleteObjectVersionsResult::DeleteObjectVersionsResult(const std::shared_ptr<std::iostream>& result) :
    DeleteObjectVersionsResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

DeleteObjectVersionsResult& DeleteObjectVersionsResult::operator =(const std::string& result)
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
                DeletedObject object;

                sub_node = node->FirstChildElement("Key");
                if (sub_node && sub_node->GetText()) {
                    object.setKey(useUrlDecode ? UrlDecode(sub_node->GetText()) : sub_node->GetText());
                }

                sub_node = node->FirstChildElement("VersionId");
                if (sub_node && sub_node->GetText()) {
                    object.setVersionId(sub_node->GetText());
                }

                sub_node = node->FirstChildElement("DeleteMarker");
                if (sub_node && sub_node->GetText()) {
                    object.setDeleteMarker(!std::strncmp("true", sub_node->GetText(), 4));
                }

                sub_node = node->FirstChildElement("DeleteMarkerVersionId");
                if (sub_node && sub_node->GetText()) {
                    object.setDeleteMarkerVersionId(sub_node->GetText());
                }
                deletedObjects_.push_back(object);
            }
        }
        parseDone_ = true;
    }
    return *this;
}

bool DeleteObjectVersionsResult::Quiet() const
{
    return quiet_;
}

const DeletedObjectList& DeleteObjectVersionsResult::DeletedObjects() const 
{
    return deletedObjects_; 
}




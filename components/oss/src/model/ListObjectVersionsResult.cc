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


#include <alibabacloud/oss/model/ListObjectVersionsResult.h>
#include <external/tinyxml2/tinyxml2.h>
#include <alibabacloud/oss/model/Owner.h>
#include "utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;


ListObjectVersionsResult::ListObjectVersionsResult() :
    OssResult(),
    name_(),
    prefix_(),
    keyMarker_(),
    nextKeyMarker_(),
    versionIdMarker_(),
    nextVersionIdMarker_(),
    delimiter_(),
    encodingType_(),
    isTruncated_(false),
    maxKeys_(),
    commonPrefixes_(),
    objectVersionSummarys_(),
    deleteMarkerSummarys_()
{
}

ListObjectVersionsResult::ListObjectVersionsResult(const std::string& result):
    ListObjectVersionsResult()
{
    *this = result;
}

ListObjectVersionsResult::ListObjectVersionsResult(const std::shared_ptr<std::iostream>& result):
    ListObjectVersionsResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

ListObjectVersionsResult& ListObjectVersionsResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("ListVersionsResult", root->Name(), 18)) {
            XMLElement *node;
            node = root->FirstChildElement("Name");
            if (node && node->GetText()) name_ = node->GetText();

            node = root->FirstChildElement("Prefix");
            if (node && node->GetText()) prefix_ = node->GetText();

            node = root->FirstChildElement("KeyMarker");
            if (node && node->GetText()) keyMarker_ = node->GetText();

            node = root->FirstChildElement("NextKeyMarker");
            if (node && node->GetText()) nextKeyMarker_ = node->GetText();

            node = root->FirstChildElement("VersionIdMarker");
            if (node && node->GetText()) versionIdMarker_ = node->GetText();

            node = root->FirstChildElement("NextVersionIdMarker");
            if (node && node->GetText()) nextVersionIdMarker_ = node->GetText();

            node = root->FirstChildElement("Delimiter");
            if (node && node->GetText()) delimiter_ = node->GetText();

            node = root->FirstChildElement("MaxKeys");
            if (node && node->GetText()) maxKeys_ = atoi(node->GetText());

            node = root->FirstChildElement("IsTruncated");
            if (node && node->GetText()) isTruncated_ = !std::strncmp("true", node->GetText(), 4);

            node = root->FirstChildElement("EncodingType");
            if (node && node->GetText()) encodingType_ = node->GetText();

            //Detect encode type
            bool useUrlDecode = !ToLower(encodingType_.c_str()).compare(0, 3, "url", 3);

            //CommonPrefixes
            node = root->FirstChildElement("CommonPrefixes");
            for (; node; node = node->NextSiblingElement("CommonPrefixes")) {
                XMLElement *prefix_node = node->FirstChildElement("Prefix");
                if (prefix_node && prefix_node->GetText()) commonPrefixes_.push_back(
                    (useUrlDecode ? UrlDecode(prefix_node->GetText()) : prefix_node->GetText()));
            }

            //Version
            XMLElement *contents_node = root->FirstChildElement("Version");
            for (; contents_node; contents_node = contents_node->NextSiblingElement("Version")) {
                ObjectVersionSummary content;
                node = contents_node->FirstChildElement("Key");
                if (node && node->GetText()) content.key_ = useUrlDecode ? UrlDecode(node->GetText()) : node->GetText();

                node = contents_node->FirstChildElement("VersionId");
                if (node && node->GetText()) content.versionid_ = node->GetText();

                node = contents_node->FirstChildElement("IsLatest");
                if (node && node->GetText()) content.isLatest_ = !std::strncmp("true", node->GetText(), 4);

                node = contents_node->FirstChildElement("LastModified");
                if (node && node->GetText()) content.lastModified_ = node->GetText();

                node = contents_node->FirstChildElement("ETag");
                if (node && node->GetText()) content.eTag_ = TrimQuotes(node->GetText());

                node = contents_node->FirstChildElement("Size");
                if (node && node->GetText()) content.size_ = std::atoll(node->GetText());

                node = contents_node->FirstChildElement("StorageClass");
                if (node && node->GetText()) content.storageClass_ = ToStorageClassType(node->GetText());

                node = contents_node->FirstChildElement("Type");
                if (node && node->GetText()) content.type_ = node->GetText();

                node = contents_node->FirstChildElement("Owner");
                std::string owner_ID, owner_DisplayName;
                if (node) {
                    XMLElement *sub_node;
                    sub_node = node->FirstChildElement("ID");
                    if (sub_node && sub_node->GetText()) owner_ID = sub_node->GetText();

                    sub_node = node->FirstChildElement("DisplayName");
                    if (sub_node && sub_node->GetText()) owner_DisplayName = sub_node->GetText();
                }

                content.owner_ = Owner(owner_ID, owner_DisplayName);
                objectVersionSummarys_.push_back(content);
            }

            //DeleteMarker
            contents_node = root->FirstChildElement("DeleteMarker");
            for (; contents_node; contents_node = contents_node->NextSiblingElement("DeleteMarker")) {
                DeleteMarkerSummary content;
                node = contents_node->FirstChildElement("Key");
                if (node && node->GetText()) content.key_ = useUrlDecode ? UrlDecode(node->GetText()) : node->GetText();

                node = contents_node->FirstChildElement("VersionId");
                if (node && node->GetText()) content.versionid_ = node->GetText();

                node = contents_node->FirstChildElement("IsLatest");
                if (node && node->GetText()) content.isLatest_ = !std::strncmp("true", node->GetText(), 4);

                node = contents_node->FirstChildElement("LastModified");
                if (node && node->GetText()) content.lastModified_ = node->GetText();

                node = contents_node->FirstChildElement("Owner");
                std::string owner_ID, owner_DisplayName;
                if (node) {
                    XMLElement *sub_node;
                    sub_node = node->FirstChildElement("ID");
                    if (sub_node && sub_node->GetText()) owner_ID = sub_node->GetText();

                    sub_node = node->FirstChildElement("DisplayName");
                    if (sub_node && sub_node->GetText()) owner_DisplayName = sub_node->GetText();
                }

                content.owner_ = Owner(owner_ID, owner_DisplayName);
                deleteMarkerSummarys_.push_back(content);
            }

            //EncodingType
            if (useUrlDecode) {
                delimiter_     = UrlDecode(delimiter_);
                keyMarker_     = UrlDecode(keyMarker_);
                nextKeyMarker_ = UrlDecode(nextKeyMarker_);
                prefix_        = UrlDecode(prefix_);
            }
        }

        //TODO check the result and the parse flag;
        parseDone_ = true;
    }

    return *this;
}


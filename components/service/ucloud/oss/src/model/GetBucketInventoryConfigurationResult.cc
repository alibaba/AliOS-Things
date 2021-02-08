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


#include <alibabacloud/oss/model/GetBucketInventoryConfigurationResult.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

GetBucketInventoryConfigurationResult::GetBucketInventoryConfigurationResult() :
    OssResult()
{
}

GetBucketInventoryConfigurationResult::GetBucketInventoryConfigurationResult(const std::string& result) :
    GetBucketInventoryConfigurationResult()
{
    *this = result;
}

GetBucketInventoryConfigurationResult::GetBucketInventoryConfigurationResult(const std::shared_ptr<std::iostream>& result) :
    GetBucketInventoryConfigurationResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketInventoryConfigurationResult& GetBucketInventoryConfigurationResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("InventoryConfiguration", root->Name(), 22)) {
            XMLElement* node;

            node = root->FirstChildElement("Id");
            if (node && node->GetText()) inventoryConfiguration_.setId(node->GetText());

            node = root->FirstChildElement("IsEnabled");
            if (node && node->GetText()) inventoryConfiguration_.setIsEnabled((std::strncmp(node->GetText(), "true", 4) ? false : true));

            node = root->FirstChildElement("Filter");
            if (node) {
                InventoryFilter filter;
                XMLElement* prefix_node = node->FirstChildElement("Prefix");
                if (prefix_node && prefix_node->GetText()) filter.setPrefix(prefix_node->GetText());
                inventoryConfiguration_.setFilter(filter);
            }
            node = root->FirstChildElement("Destination");
            if (node) {
                XMLElement* next_node;
                next_node = node->FirstChildElement("OSSBucketDestination");
                if (next_node) {
                    XMLElement* sub_node;
                    InventoryOSSBucketDestination dest;
                    sub_node = next_node->FirstChildElement("Format");
                    if (sub_node && sub_node->GetText()) dest.setFormat(ToInventoryFormatType(sub_node->GetText()));
                    sub_node = next_node->FirstChildElement("AccountId");
                    if (sub_node && sub_node->GetText()) dest.setAccountId(sub_node->GetText());
                    sub_node = next_node->FirstChildElement("RoleArn");
                    if (sub_node && sub_node->GetText()) dest.setRoleArn(sub_node->GetText());
                    sub_node = next_node->FirstChildElement("Bucket");
                    if (sub_node && sub_node->GetText()) dest.setBucket(ToInventoryBucketShortName(sub_node->GetText()));
                    sub_node = next_node->FirstChildElement("Prefix");
                    if (sub_node && sub_node->GetText()) dest.setPrefix(sub_node->GetText());
                    sub_node = next_node->FirstChildElement("Encryption");
                    if (sub_node) {
                        InventoryEncryption encryption;
                        XMLElement* sse_node;
                        sse_node = sub_node->FirstChildElement("SSE-KMS");
                        if (sse_node) {
                            InventorySSEKMS ssekms;
                            XMLElement* key_node;
                            key_node = sse_node->FirstChildElement("KeyId");
                            if (key_node && key_node->GetText()) ssekms.setKeyId(key_node->GetText());
                            encryption.setSSEKMS(ssekms);
                        }

                        sse_node = sub_node->FirstChildElement("SSE-OSS");
                        if (sse_node) {
                            encryption.setSSEOSS(InventorySSEOSS());
                        }
                        dest.setEncryption(encryption);
                    }
                    inventoryConfiguration_.setDestination(InventoryDestination(dest));
                }
            }

            node = root->FirstChildElement("Schedule");
            if (node) {
                XMLElement* freq_node = node->FirstChildElement("Frequency");
                if (freq_node && freq_node->GetText()) inventoryConfiguration_.setSchedule(ToInventoryFrequencyType(freq_node->GetText()));
            }

            node = root->FirstChildElement("IncludedObjectVersions");
            if (node && node->GetText()) inventoryConfiguration_.setIncludedObjectVersions(ToInventoryIncludedObjectVersionsType(node->GetText()));

            node = root->FirstChildElement("OptionalFields");
            if (node) {
                InventoryOptionalFields field;
                XMLElement* field_node = node->FirstChildElement("Field");
                for (; field_node; field_node = field_node->NextSiblingElement()) {
                    if (field_node->GetText())
                        field.push_back(ToInventoryOptionalFieldType(field_node->GetText()));
                }
                inventoryConfiguration_.setOptionalFields(field);
            }
            parseDone_ = true;
        }
    }
    return *this;
}

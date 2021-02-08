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


#include <alibabacloud/oss/model/ListBucketInventoryConfigurationsResult.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

ListBucketInventoryConfigurationsResult::ListBucketInventoryConfigurationsResult() :
    OssResult()
{
}

ListBucketInventoryConfigurationsResult::ListBucketInventoryConfigurationsResult(const std::string& result) :
    ListBucketInventoryConfigurationsResult()
{
    *this = result;
}

ListBucketInventoryConfigurationsResult::ListBucketInventoryConfigurationsResult(const std::shared_ptr<std::iostream>& result) :
    ListBucketInventoryConfigurationsResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

ListBucketInventoryConfigurationsResult& ListBucketInventoryConfigurationsResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("ListInventoryConfigurationsResult", root->Name(), 33)) {
            XMLElement* node;

            node = root->FirstChildElement("InventoryConfiguration");

            for (; node; node = node->NextSiblingElement("InventoryConfiguration")) {
                XMLElement* conf_node;

                InventoryConfiguration inventoryConfiguration;

                conf_node = node->FirstChildElement("Id");
                if (conf_node && conf_node->GetText()) inventoryConfiguration.setId(conf_node->GetText());

                conf_node = node->FirstChildElement("IsEnabled");
                if (conf_node && conf_node->GetText()) inventoryConfiguration.setIsEnabled((std::strncmp(conf_node->GetText(), "true", 4) ? false : true));

                conf_node = node->FirstChildElement("Filter");
                if (conf_node) {
                    InventoryFilter filter;
                    XMLElement* prefix_node = conf_node->FirstChildElement("Prefix");
                    if (prefix_node && prefix_node->GetText()) filter.setPrefix(prefix_node->GetText());
                    inventoryConfiguration.setFilter(filter);
                }
                conf_node = node->FirstChildElement("Destination");
                if (conf_node) {
                    XMLElement* next_node;
                    next_node = conf_node->FirstChildElement("OSSBucketDestination");
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
                        inventoryConfiguration.setDestination(InventoryDestination(dest));
                    }
                }

                conf_node = node->FirstChildElement("Schedule");
                if (conf_node) {
                    XMLElement* freq_node = conf_node->FirstChildElement("Frequency");
                    if (freq_node && freq_node->GetText()) inventoryConfiguration.setSchedule(ToInventoryFrequencyType(freq_node->GetText()));
                }

                conf_node = node->FirstChildElement("IncludedObjectVersions");
                if (conf_node && conf_node->GetText()) inventoryConfiguration.setIncludedObjectVersions(ToInventoryIncludedObjectVersionsType(conf_node->GetText()));

                conf_node = node->FirstChildElement("OptionalFields");
                if (conf_node) {
                    InventoryOptionalFields field;
                    XMLElement* field_node = conf_node->FirstChildElement("Field");
                    for (; field_node; field_node = field_node->NextSiblingElement()) {
                        if (field_node->GetText())
                            field.push_back(ToInventoryOptionalFieldType(field_node->GetText()));
                    }
                    inventoryConfiguration.setOptionalFields(field);
                }

                inventoryConfigurationList_.push_back(inventoryConfiguration);
            }

            node = root->FirstChildElement("IsTruncated");
            if (node && node->GetText()) isTruncated_ = (std::strncmp(node->GetText(), "true", 4) ? false : true);

            node = root->FirstChildElement("NextContinuationToken");
            if (node && node->GetText()) nextContinuationToken_ = node->GetText();

            parseDone_ = true;
        }
    }
    return *this;
}

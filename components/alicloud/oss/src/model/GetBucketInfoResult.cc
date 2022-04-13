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


#include <alibabacloud/oss/model/GetBucketInfoResult.h>
#include <external/tinyxml2/tinyxml2.h>
#include "utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;


GetBucketInfoResult::GetBucketInfoResult() :
    OssResult(),
    dataRedundancyType_(AlibabaCloud::OSS::DataRedundancyType::NotSet),
    sseAlgorithm_(AlibabaCloud::OSS::SSEAlgorithm::NotSet),
    versioningStatus_(AlibabaCloud::OSS::VersioningStatus::NotSet)
{
}

GetBucketInfoResult::GetBucketInfoResult(const std::string& result):
    GetBucketInfoResult()
{
    *this = result;
}

GetBucketInfoResult::GetBucketInfoResult(const std::shared_ptr<std::iostream>& result):
    GetBucketInfoResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketInfoResult& GetBucketInfoResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("BucketInfo", root->Name(), 10)) {
            XMLElement *node;
            XMLElement *bucket_node;
            bucket_node = root->FirstChildElement("Bucket");
            if (bucket_node) {
                node = bucket_node->FirstChildElement("CreationDate");
                if (node && node->GetText()) creationDate_ = node->GetText();

                node = bucket_node->FirstChildElement("ExtranetEndpoint");
                if (node && node->GetText()) extranetEndpoint_ = node->GetText();

                node = bucket_node->FirstChildElement("IntranetEndpoint");
                if (node && node->GetText()) intranetEndpoint_ = node->GetText();

                node = bucket_node->FirstChildElement("Location");
                if (node && node->GetText()) location_ = node->GetText();

                node = bucket_node->FirstChildElement("Name");
                if (node && node->GetText()) name_ = node->GetText();

                node = bucket_node->FirstChildElement("StorageClass");
                if (node && node->GetText()) storageClass_ = ToStorageClassType(node->GetText());

                node = bucket_node->FirstChildElement("Owner");
                std::string owner_ID, owner_DisplayName;
                if (node) {
                    XMLElement *sub_node;
                    sub_node = node->FirstChildElement("ID");
                    if (sub_node && sub_node->GetText()) owner_ID = sub_node->GetText();

                    sub_node = node->FirstChildElement("DisplayName");
                    if (sub_node && sub_node->GetText()) owner_DisplayName = sub_node->GetText();
                }
                owner_ = AlibabaCloud::OSS::Owner(owner_ID, owner_DisplayName);

                node = bucket_node->FirstChildElement("AccessControlList");
                if (node) {
                    XMLElement *sub_node;
                    sub_node = node->FirstChildElement("Grant");
                    if (sub_node && sub_node->GetText()) acl_ = ToAclType(sub_node->GetText());
                }

                node = bucket_node->FirstChildElement("DataRedundancyType");
                if (node && node->GetText()) dataRedundancyType_ = ToDataRedundancyType(node->GetText());

                node = bucket_node->FirstChildElement("Comment");
                if (node && node->GetText()) comment_ = node->GetText();

                node = bucket_node->FirstChildElement("ServerSideEncryptionRule");
                if (node) {
                    XMLElement *sub_node;
                    sub_node = node->FirstChildElement("SSEAlgorithm");
                    if (sub_node && sub_node->GetText()) sseAlgorithm_ = ToSSEAlgorithm(sub_node->GetText());

                    sub_node = node->FirstChildElement("KMSMasterKeyID");
                    if (sub_node && sub_node->GetText()) kmsMasterKeyID_ = sub_node->GetText();
                }

                node = bucket_node->FirstChildElement("Versioning");
                if (node && node->GetText()) versioningStatus_ = ToVersioningStatusType(node->GetText());
            }
            //TODO check the result and the parse flag;
            parseDone_ = true;
        }
    }
    return *this;
}

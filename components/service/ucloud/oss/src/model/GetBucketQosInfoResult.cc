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


#include <alibabacloud/oss/model/GetBucketQosInfoResult.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

GetBucketQosInfoResult::GetBucketQosInfoResult() :
    OssResult()
{
}

GetBucketQosInfoResult::GetBucketQosInfoResult(const std::string& result) :
    GetBucketQosInfoResult()
{
    *this = result;
}

GetBucketQosInfoResult::GetBucketQosInfoResult(const std::shared_ptr<std::iostream>& result) :
    GetBucketQosInfoResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketQosInfoResult& GetBucketQosInfoResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("QoSConfiguration", root->Name(), 16)) {
            XMLElement* node;
            node = root->FirstChildElement("TotalUploadBandwidth");
            if (node && node->GetText()) {
                qosInfo_.setTotalUploadBandwidth(std::strtoll(node->GetText(), nullptr, 10));
            }
            node = root->FirstChildElement("IntranetUploadBandwidth");
            if (node && node->GetText()) {
                qosInfo_.setIntranetUploadBandwidth(std::strtoll(node->GetText(), nullptr, 10));
            }
            node = root->FirstChildElement("ExtranetUploadBandwidth");
            if (node && node->GetText()) {
                qosInfo_.setExtranetUploadBandwidth(std::strtoll(node->GetText(), nullptr, 10));
            }
            node = root->FirstChildElement("TotalDownloadBandwidth");
            if (node && node->GetText()) {
                qosInfo_.setTotalDownloadBandwidth(std::strtoll(node->GetText(), nullptr, 10));
            }
            node = root->FirstChildElement("IntranetDownloadBandwidth");
            if (node && node->GetText()) {
                qosInfo_.setIntranetDownloadBandwidth(std::strtoll(node->GetText(), nullptr, 10));
            }
            node = root->FirstChildElement("ExtranetDownloadBandwidth");
            if (node && node->GetText()) {
                qosInfo_.setExtranetDownloadBandwidth(std::strtoll(node->GetText(), nullptr, 10));
            }
            node = root->FirstChildElement("TotalQps");
            if (node && node->GetText()) {
                qosInfo_.setTotalQps(std::strtoll(node->GetText(), nullptr, 10));
            }
            node = root->FirstChildElement("IntranetQps");
            if (node && node->GetText()) {
                qosInfo_.setIntranetQps(std::strtoll(node->GetText(), nullptr, 10));
            }
            node = root->FirstChildElement("ExtranetQps");
            if (node && node->GetText()) {
                qosInfo_.setExtranetQps(std::strtoll(node->GetText(), nullptr, 10));
            }
            parseDone_ = true;
        }
    }
    return *this;
}

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


#include <alibabacloud/oss/model/GetBucketStatResult.h>
#include <external/tinyxml2/tinyxml2.h>
#include "utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;


GetBucketStatResult::GetBucketStatResult() :
    OssResult(),
    storage_(0),
    objectCount_(0),
    multipartUploadCount_(0)
{
}

GetBucketStatResult::GetBucketStatResult(const std::string& result):
    GetBucketStatResult()
{
    *this = result;
}

GetBucketStatResult::GetBucketStatResult(const std::shared_ptr<std::iostream>& result):
    GetBucketStatResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketStatResult& GetBucketStatResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("BucketStat", root->Name(), 10)) {
            XMLElement *node;
            node = root->FirstChildElement("Storage");
            if (node && node->GetText()) storage_ = atoll(node->GetText());

            node = root->FirstChildElement("ObjectCount");
            if (node && node->GetText()) objectCount_ = atoll(node->GetText());

            node = root->FirstChildElement("MultipartUploadCount");
            if (node && node->GetText()) multipartUploadCount_ = atoll(node->GetText());

            parseDone_ = true;
		}
    }
    return *this;
}


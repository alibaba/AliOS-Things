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


#include <alibabacloud/oss/model/GetBucketLoggingResult.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;


GetBucketLoggingResult::GetBucketLoggingResult() :
    OssResult()
{
}

GetBucketLoggingResult::GetBucketLoggingResult(const std::string& result):
    GetBucketLoggingResult()
{
    *this = result;
}

GetBucketLoggingResult::GetBucketLoggingResult(const std::shared_ptr<std::iostream>& result):
    GetBucketLoggingResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketLoggingResult& GetBucketLoggingResult::operator=(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("BucketLoggingStatus", root->Name(), 19)) {
            XMLElement *log_node;
            log_node = root->FirstChildElement("LoggingEnabled");
            if (log_node) {
                XMLElement *node;
                node = log_node->FirstChildElement("TargetBucket");
                if (node && node->GetText()) targetBucket_ = node->GetText();

                node = log_node->FirstChildElement("TargetPrefix");
                if (node && node->GetText()) targetPrefix_ = node->GetText();
            }
            parseDone_ = true;
		}
    }
    return *this;
}

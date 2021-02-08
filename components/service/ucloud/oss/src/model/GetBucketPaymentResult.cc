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


#include <alibabacloud/oss/model/GetBucketPaymentResult.h>
#include <tinyxml2/tinyxml2.h>
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

GetBucketPaymentResult::GetBucketPaymentResult() :
    OssResult()
{
}

GetBucketPaymentResult::GetBucketPaymentResult(const std::string& result) :
    GetBucketPaymentResult()
{
    *this = result;
}

GetBucketPaymentResult::GetBucketPaymentResult(const std::shared_ptr<std::iostream>& result) :
    GetBucketPaymentResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetBucketPaymentResult& GetBucketPaymentResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("RequestPaymentConfiguration", root->Name(), 27)) {
            XMLElement* node;
            node = root->FirstChildElement("Payer");
            if (node && node->GetText()) payer_ = ToRequestPayer(node->GetText());
            parseDone_ = true;
        }
    }
    return *this;
}

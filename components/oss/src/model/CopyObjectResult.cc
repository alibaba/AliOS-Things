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


#include <alibabacloud/oss/model/CopyObjectResult.h>
#include <external/tinyxml2/tinyxml2.h>
#include "utils/Utils.h"

using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

CopyObjectResult::CopyObjectResult() :
    OssObjectResult()
{
}

CopyObjectResult::CopyObjectResult(const std::string& data):
    CopyObjectResult()
{
    *this = data;
}

CopyObjectResult::CopyObjectResult(const std::shared_ptr<std::iostream>& data):
    CopyObjectResult()
{
    std::istreambuf_iterator<char> isb(*data.get()), end;
    std::string str(isb, end);
    *this = str;
}

CopyObjectResult::CopyObjectResult(const HeaderCollection& headers, const std::shared_ptr<std::iostream>& data):
    OssObjectResult(headers)
{
    if (headers.find("x-oss-copy-source-version-id") != headers.end()) {
        sourceVersionId_ = headers.at("x-oss-copy-source-version-id");
    }

    std::istreambuf_iterator<char> isb(*data.get()), end;
    std::string str(isb, end);
    *this = str;
}

CopyObjectResult& CopyObjectResult::operator =(const std::string& data)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(data.c_str(), data.size())) == XML_SUCCESS) {
        XMLElement* root = doc.RootElement();
        if (root && !std::strncmp("CopyObjectResult", root->Name(), strlen("CopyObjectResult"))) {
            XMLElement *node;
            node = root->FirstChildElement("LastModified");
            if (node && node->GetText()) {
                lastModified_ = node->GetText();
            }

            node = root->FirstChildElement("ETag");
            if (node && node->GetText()) {
                etag_ = TrimQuotes(node->GetText());
            }

            //TODO check the result and the parse flag;
            parseDone_ = true;
        }
    }
    return *this;
}


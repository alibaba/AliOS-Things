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

#include <alibabacloud/oss/model/SetBucketWebsiteRequest.h>
#include <sstream>
#include <utils/Utils.h>
#include "ModelError.h"

using namespace AlibabaCloud::OSS;

SetBucketWebsiteRequest::SetBucketWebsiteRequest(const std::string& bucket) :
    OssBucketRequest(bucket),
    indexDocumentIsSet_(false),
    errorDocumentIsSet_(false)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

std::string SetBucketWebsiteRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<WebsiteConfiguration>" << std::endl;
    ss << "  <IndexDocument>" << std::endl;
    ss << "    <Suffix>" << indexDocument_ << "</Suffix>" << std::endl;
    ss << "  </IndexDocument>" << std::endl;
    if (errorDocumentIsSet_) {
        ss << "  <ErrorDocument>" << std::endl;
        ss << "    <Key>" << errorDocument_ << "</Key>" << std::endl;
        ss << "  </ErrorDocument>" << std::endl;
    }
    ss << "</WebsiteConfiguration>" << std::endl;
    return ss.str();
}

ParameterCollection SetBucketWebsiteRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["website"] = "";
    return parameters;
}

static bool IsValidWebpage(const std::string &webpage)
{
    const std::string pageSuffix = ".html";
    return (webpage.size() > pageSuffix.size()) &&
        (webpage.substr(webpage.size() - 5).compare(".html") == 0);
}

int SetBucketWebsiteRequest::validate() const
{
    int ret = OssBucketRequest::validate();
    if (ret != 0) 
        return ret;

    if (indexDocument_.empty())
        return ARG_ERROR_WEBSITE_INDEX_DOCCUMENT_EMPTY;

    if (!IsValidWebpage(indexDocument_))
        return ARG_ERROR_WEBSITE_INDEX_DOCCUMENT_NAME_INVALID;

    if (errorDocumentIsSet_ && !IsValidWebpage(errorDocument_))
        return ARG_ERROR_WEBSITE_ERROR_DOCCUMENT_NAME_INVALID;

    return 0;
}


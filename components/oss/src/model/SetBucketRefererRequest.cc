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

#include <alibabacloud/oss/model/SetBucketRefererRequest.h>
#include <sstream>

using namespace AlibabaCloud::OSS;

SetBucketRefererRequest::SetBucketRefererRequest(const std::string& bucket) :
    OssBucketRequest(bucket),
    allowEmptyReferer_(true)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

SetBucketRefererRequest::SetBucketRefererRequest(const std::string& bucket, const RefererList& refererList) :
    SetBucketRefererRequest(bucket, refererList, true)
{
}

SetBucketRefererRequest::SetBucketRefererRequest(const std::string& bucket, const RefererList& refererList,
    bool allowEmptyReferer) :
    OssBucketRequest(bucket),
    allowEmptyReferer_(allowEmptyReferer),
    refererList_(refererList)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

std::string SetBucketRefererRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<RefererConfiguration>" << std::endl;
    ss << "  <AllowEmptyReferer>" << (allowEmptyReferer_ ? "true" : "false") << "</AllowEmptyReferer>" << std::endl;
    ss << "  <RefererList>" << std::endl;
    for (const auto &referer : refererList_) {
        ss << "    <Referer>" << referer << "</Referer>" << std::endl;
    }
    ss << "  </RefererList>" << std::endl;
    ss << "</RefererConfiguration>" << std::endl;
    return ss.str();
}
ParameterCollection SetBucketRefererRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["referer"] = "";
    return parameters;
}

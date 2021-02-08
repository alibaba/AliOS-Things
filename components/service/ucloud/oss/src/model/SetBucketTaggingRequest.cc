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


#include <alibabacloud/oss/model/SetBucketTaggingRequest.h>
#include <alibabacloud/oss/Const.h>
#include "../utils/Utils.h"
#include "ModelError.h"
#include <sstream>
using namespace AlibabaCloud::OSS;

SetBucketTaggingRequest::SetBucketTaggingRequest(const std::string& bucket) :
    SetBucketTaggingRequest(bucket, Tagging())
{
}

SetBucketTaggingRequest::SetBucketTaggingRequest(const std::string& bucket, const Tagging& tagging) :
    OssBucketRequest(bucket),
    tagging_(tagging)
{
    setFlags(Flags() | REQUEST_FLAG_CONTENTMD5);
}

void SetBucketTaggingRequest::setTagging(const Tagging& tagging)
{
    tagging_ = tagging;
}


std::string SetBucketTaggingRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<Tagging>" << std::endl;
    ss << "  <TagSet>" << std::endl;
    for (const auto& tag : tagging_.Tags()) {
        ss << "    <Tag><Key>" << tag.Key() << "</Key><Value>" << tag.Value() << "</Value></Tag>" << std::endl;
    }
    ss << "  </TagSet>" << std::endl;
    ss << "</Tagging>" << std::endl;
    return ss.str();
}

ParameterCollection SetBucketTaggingRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["tagging"] = "";
    return parameters;
}

int SetBucketTaggingRequest::validate() const
{
    int ret;
    if ((ret = OssBucketRequest::validate()) != 0) {
        return ret;
    }

    if (tagging_.Tags().empty() || tagging_.Tags().size() > MaxTagSize) {
        return ARG_ERROR_TAGGING_TAGS_LIMIT;
    }

    for (const auto& tag : tagging_.Tags()) {

        if (!IsValidTagKey(tag.Key()))
            return ARG_ERROR_TAGGING_TAG_KEY_LIMIT;

        if (!IsValidTagValue(tag.Value()))
            return ARG_ERROR_TAGGING_TAG_VALUE_LIMIT;
    }

    return 0;
}
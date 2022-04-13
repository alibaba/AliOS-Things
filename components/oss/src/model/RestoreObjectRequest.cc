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


#include <alibabacloud/oss/model/RestoreObjectRequest.h>
#include <utils/Utils.h>
#include <sstream>

using namespace AlibabaCloud::OSS;

RestoreObjectRequest::RestoreObjectRequest(const std::string &bucket, const std::string &key)
    :OssObjectRequest(bucket,key),
    days_(1U),
    tierTypeIsSet_(false)
{
}

void RestoreObjectRequest::setDays(uint32_t days)
{
    days_ = days;
}

void RestoreObjectRequest::setTierType(TierType type)
{
    tierType_ = type;
    tierTypeIsSet_ = true;
}

std::string RestoreObjectRequest::payload() const
{
    std::stringstream ss;
    if (tierTypeIsSet_) {
        ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        ss << "<RestoreRequest>" << std::endl;
        ss << "<Days>" << std::to_string(days_) << "</Days>" << std::endl;
        ss << "<JobParameters><Tier>" << ToTierTypeName(tierType_) << "</Tier></JobParameters>" << std::endl;
        ss << "</RestoreRequest>" << std::endl;
    }
    return ss.str();
}

ParameterCollection RestoreObjectRequest::specialParameters() const
{
    auto parameters = OssObjectRequest::specialParameters();
    parameters["restore"]="";
    return parameters;
}


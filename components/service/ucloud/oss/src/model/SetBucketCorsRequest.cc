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

#include <alibabacloud/oss/model/SetBucketCorsRequest.h>
#include "../utils/Utils.h"
#include "ModelError.h"
#include <sstream>
#include <set>
#include <alibabacloud/oss/Const.h>

using namespace AlibabaCloud::OSS;

SetBucketCorsRequest::SetBucketCorsRequest(const std::string &bucket) :
    OssBucketRequest(bucket)
{
}

void SetBucketCorsRequest::addCORSRule(const CORSRule &rule)
{
    ruleList_.push_back(rule);
}

void SetBucketCorsRequest::setCORSRules(const CORSRuleList &rules)
{
    ruleList_ = rules;
}

std::string SetBucketCorsRequest::payload() const
{
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<CORSConfiguration>" << std::endl;
    for (const auto &rule : ruleList_)
    {
        ss << "  <CORSRule>" << std::endl;
        for (const auto &origin : rule.AllowedOrigins())
            ss << "    <AllowedOrigin>" << origin << "</AllowedOrigin>" << std::endl;

        for (const auto &method : rule.AllowedMethods())
            ss << "    <AllowedMethod>" << method << "</AllowedMethod>" << std::endl;

        for (const auto &header : rule.AllowedHeaders())
            ss << "    <AllowedHeader>" << header << "</AllowedHeader>" << std::endl;

        for (const auto &header : rule.ExposeHeaders())
            ss << "    <ExposeHeader>" << header << "</ExposeHeader>" << std::endl;

        if (rule.MaxAgeSeconds() > 0)
            ss << "    <MaxAgeSeconds>" << std::to_string(rule.MaxAgeSeconds()) << "</MaxAgeSeconds>" << std::endl;
        ss << "  </CORSRule>" << std::endl;
    }
    ss << "</CORSConfiguration>" << std::endl;
    return ss.str();
}

ParameterCollection SetBucketCorsRequest::specialParameters() const
{
    ParameterCollection parameters;
    parameters["cors"] = "";
    return parameters;
}

static int CountOfAsterisk(const CORSAllowedList &items)
{
    int count = 0;
    for (auto const&item : items) {
        count += item.compare("*") ? 0 : 1;
    }
    return count;
}

static bool InAllowedMethods(const CORSAllowedList &items)
{
    static std::set<std::string> allowedMethods =
    {
        "GET" , "PUT" , "DELETE", "POST", "HEAD"
    };

    //if (items.empty())
    //    return false;

    for (auto const&item : items) {
        if (allowedMethods.find(Trim(item.c_str())) == allowedMethods.end()) {
            return false;
        }
    }
    return true;
}

int SetBucketCorsRequest::validate() const
{
    int ret = OssBucketRequest::validate();
    if (ret) return ret;

    if (ruleList_.size() > BucketCorsRuleLimit)
        return ARG_ERROR_CORS_RULE_LIMIT;

    for (auto const &rule : ruleList_) {

        if (rule.AllowedOrigins().empty())
            return ARG_ERROR_CORS_ALLOWEDORIGINS_EMPTY;

        if (CountOfAsterisk(rule.AllowedOrigins()) > 1)
            return ARG_ERROR_CORS_ALLOWEDORIGINS_ASTERISK_COUNT;

        if (rule.AllowedMethods().empty())
            return ARG_ERROR_CORS_ALLOWEDMETHODS_EMPTY;

        if (!InAllowedMethods(rule.AllowedMethods()))
            return ARG_ERROR_CORS_ALLOWEDMETHODS_VALUE;

        if (CountOfAsterisk(rule.AllowedHeaders()) > 1)
            return ARG_ERROR_CORS_ALLOWEDHEADERS_ASTERISK_COUNT;

        if (CountOfAsterisk(rule.ExposeHeaders()) > 0)
            return ARG_ERROR_CORS_EXPOSEHEADERS_ASTERISK_COUNT;

        if ((rule.MaxAgeSeconds() != CORSRule::UNSET_AGE_SEC) &&
            (rule.MaxAgeSeconds() < 0 || rule.MaxAgeSeconds() > 999999999)) {
            return ARG_ERROR_CORS_MAXAGESECONDS_RANGE;
        }
    }

    return 0;
}


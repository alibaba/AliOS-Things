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


#include <alibabacloud/oss/model/GetObjectRequest.h>
#include <alibabacloud/oss/http/HttpType.h>
#include "utils/Utils.h"
#include "ModelError.h"
#include <set>
#include <sstream>
using namespace AlibabaCloud::OSS;

GetObjectRequest::GetObjectRequest(const std::string &bucket, const std::string &key):
    GetObjectRequest(bucket, key, "")
{
}

GetObjectRequest::GetObjectRequest(const std::string &bucket, const std::string &key, const std::string &process) :
    OssObjectRequest(bucket, key),
    rangeIsSet_(false),
    process_(process),
    trafficLimit_(0),
    rangeIsStandardMode_(false),
    userAgent_()

{
    setFlags(Flags() | REQUEST_FLAG_CHECK_CRC64);
}

GetObjectRequest::GetObjectRequest(const std::string& bucket, const std::string& key,
    const std::string &modifiedSince, const std::string &unmodifiedSince,
    const std::vector<std::string> &matchingETags, const std::vector<std::string> &nonmatchingETags,
    const std::map<std::string, std::string> &responseHeaderParameters) : 
    OssObjectRequest(bucket, key),
    rangeIsSet_(false), 
    modifiedSince_(modifiedSince), 
    unmodifiedSince_(unmodifiedSince), 
    matchingETags_(matchingETags),
    nonmatchingETags_(nonmatchingETags), 
    process_(""),
    responseHeaderParameters_(responseHeaderParameters),
    trafficLimit_(0),
    rangeIsStandardMode_(false),
    userAgent_()

{
}

void GetObjectRequest::setRange(int64_t start, int64_t end)
{
    range_[0] = start;
    range_[1] = end;
    rangeIsSet_ = true;
    rangeIsStandardMode_ = false;
}

void GetObjectRequest::setRange(int64_t start, int64_t end, bool standard)
{
    range_[0] = start;
    range_[1] = end;
    rangeIsSet_ = true;
    rangeIsStandardMode_ = standard;
}

void GetObjectRequest::setModifiedSinceConstraint(const std::string &gmt)
{
    modifiedSince_ = gmt;
}

void GetObjectRequest::setUnmodifiedSinceConstraint(const std::string &gmt)
{
    unmodifiedSince_ = gmt;
}

void GetObjectRequest::setMatchingETagConstraints(const std::vector<std::string> &match)
{
    matchingETags_ = match;
}

void GetObjectRequest::addMatchingETagConstraint(const std::string &match)
{
    matchingETags_.push_back(match);
}

void GetObjectRequest::setNonmatchingETagConstraints(const std::vector<std::string> &match)
{
    nonmatchingETags_ = match;
}

void GetObjectRequest::addNonmatchingETagConstraint(const std::string &match)
{
    nonmatchingETags_.push_back(match);
}

void GetObjectRequest::setProcess(const std::string &process)
{
    process_ = process;
}

void GetObjectRequest::addResponseHeaders(RequestResponseHeader header, const std::string &value)
{
    static const char *ResponseHeader[] = {
        "response-content-type", "response-content-language", 
        "response-expires", "response-cache-control", 
        "response-content-disposition", "response-content-encoding"};
    responseHeaderParameters_[ResponseHeader[header - RequestResponseHeader::ContentType]] = value;
}

void GetObjectRequest::setTrafficLimit(uint64_t value)
{
    trafficLimit_ = value;
}

void GetObjectRequest::setUserAgent(const std::string& ua)
{
    userAgent_ = ua;
}

std::pair<int64_t, int64_t> GetObjectRequest::Range() const
{
    int64_t begin = -1;
    int64_t end = -1;
    if (rangeIsSet_) {
        begin = range_[0];
        end = range_[1];
    }

    return std::pair<int64_t, int64_t>(begin, end);
}

int GetObjectRequest::validate() const
{
    int ret = OssObjectRequest::validate();
    if (ret != 0)
        return ret;

    if (rangeIsSet_ && (range_[0] < 0 || range_[1] < -1 || (range_[1] > -1 && range_[1] < range_[0]) ))
        return ARG_ERROR_OBJECT_RANGE_INVALID;

    return 0;
}

HeaderCollection GetObjectRequest::specialHeaders() const
{
    auto headers = OssObjectRequest::specialHeaders();
    if (rangeIsSet_) {
        std::stringstream ss;
        ss << "bytes=" << range_[0] << "-";
        if (range_[1] != -1) ss << range_[1];
        headers[Http::RANGE] = ss.str();

        if (rangeIsStandardMode_) {
            headers["x-oss-range-behavior"] = "standard";
        }
    }

    if (!modifiedSince_.empty())
    {
        headers["If-Modified-Since"] = modifiedSince_;
    }

    if (!unmodifiedSince_.empty())
    {
        headers["If-Unmodified-Since"] = unmodifiedSince_;
    }

    if (matchingETags_.size() > 0) {
        std::stringstream ss;
        bool first = true;
        for (auto const& str : matchingETags_) {
            if (!first) {
                ss << ",";
            }
            ss << str;
            first = false;
        }
        headers["If-Match"] = ss.str();
    }

    if (nonmatchingETags_.size() > 0) {
        std::stringstream ss;
        bool first = true;
        for (auto const& str : nonmatchingETags_) {
            if (!first) {
                ss << ",";
            }
            ss << str;
            first = false;
        }
        headers["If-None-Match"] = ss.str();
    }

    if (trafficLimit_ != 0) {
        headers["x-oss-traffic-limit"] = std::to_string(trafficLimit_);
    }

    if (!userAgent_.empty()) {
        headers[Http::USER_AGENT] = userAgent_;
    }
    return headers;
}

ParameterCollection GetObjectRequest::specialParameters() const
{
    auto parameters = OssObjectRequest::specialParameters();
    for (auto const& param : responseHeaderParameters_) {
        parameters[param.first] = param.second;
    }

    if (!process_.empty()) {
        parameters["x-oss-process"] = process_;
    }

    return parameters;
}


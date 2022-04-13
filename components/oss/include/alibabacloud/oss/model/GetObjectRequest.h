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

#pragma once
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/OssRequest.h>
#include <alibabacloud/oss/Types.h>
#include <vector>
#include <map>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT GetObjectRequest: public OssObjectRequest
    {
    public:
        GetObjectRequest(const std::string& bucket, const std::string& key);
        GetObjectRequest(const std::string& bucket, const std::string& key, 
            const std::string& process);
        GetObjectRequest(const std::string &bucket, const std::string &key, 
            const std::string &modifiedSince, const std::string &unmodifiedSince, 
            const std::vector<std::string> &matchingETags, const std::vector<std::string> &nonmatchingETags, 
            const std::map<std::string, std::string> &responseHeaderParameters_);
        void setRange(int64_t start, int64_t end);
        void setRange(int64_t start, int64_t end, bool standard);
        void setModifiedSinceConstraint(const std::string& gmt);
        void setUnmodifiedSinceConstraint(const std::string& gmt);
        void setMatchingETagConstraints(const std::vector<std::string>& match);
        void addMatchingETagConstraint(const std::string& match);
        void setNonmatchingETagConstraints(const std::vector<std::string>& match);
        void addNonmatchingETagConstraint(const std::string& match);
        void setProcess(const std::string& process);
        void addResponseHeaders(RequestResponseHeader header, const std::string& value);
        void setTrafficLimit(uint64_t value);
        void setUserAgent(const std::string& ua);

        std::pair<int64_t, int64_t> Range() const;
    protected:
        virtual HeaderCollection specialHeaders() const ;
        virtual ParameterCollection specialParameters() const;
        virtual int validate() const;
    private:
        int64_t range_[2];
        bool rangeIsSet_;
        std::string modifiedSince_;
        std::string unmodifiedSince_;
        std::vector<std::string> matchingETags_;
        std::vector<std::string> nonmatchingETags_;
        std::string process_;
        std::map<std::string, std::string> responseHeaderParameters_;
        uint64_t trafficLimit_;
        bool rangeIsStandardMode_;
        std::string userAgent_;
    };
} 
}

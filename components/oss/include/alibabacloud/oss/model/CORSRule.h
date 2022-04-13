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
#include <string>
#include <list>

namespace AlibabaCloud
{
namespace OSS
{
    using CORSAllowedList = std::list<std::string>;
    class ALIBABACLOUD_OSS_EXPORT CORSRule
    {
    public:
        static const int UNSET_AGE_SEC = -1;
    public:
        CORSRule() : maxAgeSeconds_(UNSET_AGE_SEC) {}
        const CORSAllowedList& AllowedOrigins() const { return allowedOrigins_; }
        const CORSAllowedList& AllowedMethods() const { return allowedMethods_; }
        const CORSAllowedList& AllowedHeaders() const { return allowedHeaders_; }
        const CORSAllowedList& ExposeHeaders() const { return exposeHeaders_; }
        int MaxAgeSeconds() const { return maxAgeSeconds_; }
        void addAllowedOrigin(const std::string& origin) { allowedOrigins_.push_back(origin); }
        void addAllowedMethod(const std::string& method) { allowedMethods_.push_back(method); }
        void addAllowedHeader(const std::string& header) { allowedHeaders_.push_back(header); }
        void addExposeHeader(const std::string& header) { exposeHeaders_.push_back(header); }
        void setMaxAgeSeconds(int value) { maxAgeSeconds_ = value; }
        void clear()
        {
            allowedOrigins_.clear();
            allowedMethods_.clear();
            allowedHeaders_.clear();
            exposeHeaders_.clear();
            maxAgeSeconds_ = UNSET_AGE_SEC;
        }
    private:
        CORSAllowedList allowedOrigins_;
        CORSAllowedList allowedMethods_;
        CORSAllowedList allowedHeaders_;
        CORSAllowedList exposeHeaders_;
        int maxAgeSeconds_;
    };

    using CORSRuleList = std::list<CORSRule>;
} 
}

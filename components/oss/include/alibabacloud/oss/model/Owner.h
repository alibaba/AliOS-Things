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
#include <string>
#include <alibabacloud/oss/Export.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT Owner
    {
    public:
        Owner() = default;
        Owner(const std::string& id, const std::string& name) :
            id_(id),
            displayName_(name)
        {}
        Owner(const Owner& rhs) :
            id_(rhs.id_),
            displayName_(rhs.displayName_)
        {}
        Owner(Owner&& lhs) :
            id_(std::move(lhs.id_)),
            displayName_(std::move(lhs.displayName_))
        {}
        Owner& operator=(const Owner& rhs)
        {
            id_ = rhs.id_;
            displayName_ = rhs.displayName_;
            return *this;
        }
        Owner& operator=(Owner&& lhs)
        {
            id_ = std::move(lhs.id_);
            displayName_ = std::move(lhs.displayName_);
            return *this;
        }
        const std::string& Id() const { return id_; }
        const std::string& DisplayName() const { return displayName_; };
    private:
        std::string id_;
        std::string displayName_;
    };
} 
}

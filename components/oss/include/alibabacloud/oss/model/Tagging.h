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
#include <alibabacloud/oss/Types.h>
namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT Tag
    {
    public:
        Tag() {};
        Tag(const std::string& key, const std::string& value) :
            key_(key), value_(value) 
        {}
        void setKey(const std::string& key) { key_ = key; }
        void setValue(const std::string& value) { value_ = value; }
        const std::string& Key() const { return key_; }
        const std::string& Value() const { return value_; }
    private:
        std::string key_;
        std::string value_;
    };

    using TagSet = std::vector<Tag>;

    class ALIBABACLOUD_OSS_EXPORT Tagging
    {
    public:
        Tagging() {};
        Tagging(const TagSet& tags) { tagSet_ = tags;}

        const TagSet& Tags() const { return tagSet_; }
        void setTags(const TagSet& tags) { tagSet_ = tags; }
        void setTags(TagSet&& tags) { tagSet_ = std::move(tags); }
        void addTag(const Tag& tag)  { tagSet_.push_back(tag) ; }
        void addTag(Tag&& tag) { tagSet_.push_back(std::move(tag)); }
        void clear() { tagSet_.clear();}
        std::string toQueryParameters();
    private:
        TagSet tagSet_;
    };
}
}

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
#include <vector>
#include <alibabacloud/oss/Export.h>
#include <alibabacloud/oss/Types.h>
#include <alibabacloud/oss/model/Tagging.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT LifeCycleExpiration
    {
    public:
        LifeCycleExpiration();
        LifeCycleExpiration(uint32_t days);
        LifeCycleExpiration(const std::string& createdBeforeDate);
        void setDays(uint32_t days);
        void setCreatedBeforeDate(const std::string& date);
        uint32_t Days() const { return days_; }
        const std::string& CreatedBeforeDate() const { return createdBeforeDate_; }
    private:
        uint32_t days_;
        std::string createdBeforeDate_;
    };

    class ALIBABACLOUD_OSS_EXPORT LifeCycleTransition
    {
    public:
        LifeCycleTransition() = default;
        LifeCycleTransition(const LifeCycleExpiration& expiration, AlibabaCloud::OSS::StorageClass storageClass);
        void setExpiration(const LifeCycleExpiration& expiration);
        void setStorageClass(AlibabaCloud::OSS::StorageClass storageClass);
        const LifeCycleExpiration& Expiration() const { return expiration_; }
        LifeCycleExpiration& Expiration() { return expiration_; }
        AlibabaCloud::OSS::StorageClass StorageClass() const { return storageClass_; }
    private:
        LifeCycleExpiration expiration_;
        AlibabaCloud::OSS::StorageClass storageClass_;
    };

    using LifeCycleTransitionList = std::vector<LifeCycleTransition>;

    class ALIBABACLOUD_OSS_EXPORT LifecycleRule
    {
    public:
        LifecycleRule();
        const std::string& ID() const { return id_; }
        const std::string& Prefix() const { return prefix_; }
        RuleStatus Status() const {  return status_;}
        const LifeCycleExpiration& Expiration() const { return expiration_; }
        const LifeCycleTransitionList& TransitionList() const { return transitionList_; }
        const LifeCycleExpiration& AbortMultipartUpload() const { return abortMultipartUpload_; }
        LifeCycleExpiration& Expiration() { return expiration_; }
        LifeCycleTransitionList& TransitionList() { return transitionList_; }
        LifeCycleExpiration& AbortMultipartUpload() { return abortMultipartUpload_; }

        const TagSet& Tags() const { return tagSet_; }
        TagSet& Tags() { return tagSet_; }

        void setID(const std::string& id) { id_ = id; }
        void setPrefix(const std::string& prefix) { prefix_ = prefix; }
        void setStatus(RuleStatus status) { status_ = status; }
        void setExpiration(const LifeCycleExpiration& expiration) { expiration_ = expiration; }
        void addTransition(const LifeCycleTransition&transition) { transitionList_.push_back(transition); }
        void setTransitionList(const LifeCycleTransitionList& transitionList) { transitionList_ = transitionList; }
        void setAbortMultipartUpload(const LifeCycleExpiration& expiration) { abortMultipartUpload_ = expiration; }
        void addTag(const Tag& tag) { tagSet_.push_back(tag); }
        void setTags(const TagSet& tags) { tagSet_ = tags; }
        bool hasExpiration() const;
        bool hasTransitionList() const;
        bool hasAbortMultipartUpload() const;
        bool operator==(const LifecycleRule& right) const;

        bool ExpiredObjectDeleteMarker() const { return expiredObjectDeleteMarker_; };
        const LifeCycleExpiration& NoncurrentVersionExpiration() const { return noncurrentVersionExpiration_; }
        const LifeCycleTransitionList& NoncurrentVersionTransitionList() const { return noncurrentVersionTransitionList_; }
        LifeCycleExpiration& NoncurrentVersionExpiration() { return noncurrentVersionExpiration_; }
        LifeCycleTransitionList& NoncurrentVersionTransitionList() { return noncurrentVersionTransitionList_; }
        void setExpiredObjectDeleteMarker(bool value) { expiredObjectDeleteMarker_ = value; };
        void setNoncurrentVersionExpiration(const LifeCycleExpiration& expiration) { noncurrentVersionExpiration_ = expiration; }
        void addNoncurrentVersionTransition(const LifeCycleTransition&transition) { noncurrentVersionTransitionList_.push_back(transition); }
        void setNoncurrentVersionTransitionList(const LifeCycleTransitionList& transitionList) { noncurrentVersionTransitionList_ = transitionList; }
        bool hasNoncurrentVersionExpiration() const;
        bool hasNoncurrentVersionTransitionList() const;

    private:
        std::string id_;
        std::string prefix_;
        RuleStatus status_;
        LifeCycleExpiration expiration_;
        LifeCycleTransitionList transitionList_;
        LifeCycleExpiration abortMultipartUpload_;
        TagSet tagSet_;

        bool expiredObjectDeleteMarker_;
        LifeCycleExpiration noncurrentVersionExpiration_;
        LifeCycleTransitionList noncurrentVersionTransitionList_;
    };
    using LifecycleRuleList = std::vector<LifecycleRule>;
} 
}

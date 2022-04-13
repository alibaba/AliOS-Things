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
    class ALIBABACLOUD_OSS_EXPORT QosConfiguration
    {
    public:
        QosConfiguration():
            totalUploadBandwidth_(-1), intranetUploadBandwidth_(-1),
            extranetUploadBandwidth_(-1), totalDownloadBandwidth_(-1),
            intranetDownloadBandwidth_(-1), extranetDownloadBandwidth_(-1),
            totalQps_(-1), intranetQps_(-1), extranetQps_(-1)
        {
        }
        int64_t TotalUploadBandwidth() const { return totalUploadBandwidth_; }
        int64_t IntranetUploadBandwidth() const { return intranetUploadBandwidth_; }
        int64_t ExtranetUploadBandwidth() const { return extranetUploadBandwidth_; }
        int64_t TotalDownloadBandwidth() const { return totalDownloadBandwidth_; }
        int64_t IntranetDownloadBandwidth() const { return intranetDownloadBandwidth_; }
        int64_t ExtranetDownloadBandwidth() const { return extranetDownloadBandwidth_; }
        int64_t TotalQps() const { return totalQps_; }
        int64_t IntranetQps() const { return intranetQps_; }
        int64_t ExtranetQps() const { return extranetQps_; }

        void setTotalUploadBandwidth(int64_t value) { totalUploadBandwidth_ = value; }
        void setIntranetUploadBandwidth(int64_t value) { intranetUploadBandwidth_ = value; }
        void setExtranetUploadBandwidth(int64_t value) { extranetUploadBandwidth_ = value; }
        void setTotalDownloadBandwidth(int64_t value) { totalDownloadBandwidth_ = value; }
        void setIntranetDownloadBandwidth(int64_t value) { intranetDownloadBandwidth_ = value; }
        void setExtranetDownloadBandwidth(int64_t value) { extranetDownloadBandwidth_ = value; }
        void setTotalQps(int64_t value) { totalQps_ = value; }
        void setIntranetQps(int64_t value) { intranetQps_ = value; }
        void setExtranetQps(int64_t value) { extranetQps_ = value; }

    private:
        int64_t totalUploadBandwidth_;
        int64_t intranetUploadBandwidth_;
        int64_t extranetUploadBandwidth_;
        int64_t totalDownloadBandwidth_;
        int64_t intranetDownloadBandwidth_;
        int64_t extranetDownloadBandwidth_;
        int64_t totalQps_;
        int64_t intranetQps_;
        int64_t extranetQps_;
    };
}
}
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
#include <alibabacloud/oss/OssResult.h>
#include <alibabacloud/oss/model/Owner.h>
#include <alibabacloud/oss/ServiceRequest.h>

namespace AlibabaCloud
{
namespace OSS
{
    class ALIBABACLOUD_OSS_EXPORT GetLiveChannelStatResult : public OssResult
    {
    public:
        GetLiveChannelStatResult();
        GetLiveChannelStatResult(const std::string& data);
        GetLiveChannelStatResult(const std::shared_ptr<std::iostream>& data);
        GetLiveChannelStatResult& operator=(const std::string& data);

        LiveChannelStatus Status() const;
        const std::string& ConnectedTime() const;
        const std::string& RemoteAddr() const;
        uint32_t Width() const;
        uint32_t Height() const;
        uint64_t FrameRate() const;
        uint64_t VideoBandWidth() const;
        const std::string& VideoCodec() const;
        uint64_t SampleRate() const;
        uint64_t AudioBandWidth() const;
        const std::string& AudioCodec() const;
    private:
        std::string connectedTime_;
        LiveChannelStatus status_;
        std::string remoteAddr_;
        uint32_t width_;
        uint32_t height_;
        uint64_t frameRate_;
        uint64_t videoBandWidth_;
        std::string videoCodec_;
        uint64_t sampleRate_;
        uint64_t audioBandWidth_;
        std::string audioCodec_;
    };
} 
}


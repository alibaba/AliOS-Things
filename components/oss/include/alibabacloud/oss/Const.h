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
#include <cstdint>

namespace AlibabaCloud
{
namespace OSS
{
    const int64_t MaxFileSize = 5LL * 1024LL * 1024LL * 1024LL;
    const int32_t MaxPrefixStringSize = 1024;
    const int32_t MaxMarkerStringSize = 1024;
    const int32_t MaxDelimiterStringSize = 1024;
    const int32_t MaxReturnedKeys = 1000;
    const int32_t MaxUploads = 1000;
    const int32_t DeleteObjectsUpperLimit = 1000;
    const int32_t BucketCorsRuleLimit = 10;
    const int32_t LifecycleRuleLimit = 1000;
    const int32_t ObjectNameLengthLimit = 1023;
    const int32_t PartNumberUpperLimit = 10000;
    const int32_t DefaultPartSize = 8 * 1024 * 1024;
    const int32_t PartSizeLowerLimit = 100 * 1024;
    const int32_t MaxPathLength = 124;
    const int32_t MinPathLength = 4;
    const int32_t DefaultResumableThreadNum = 3;
    const uint32_t MaxLiveChannelNameLength = 1023;
    const uint32_t MaxLiveChannelDescriptionLength = 128;
    const uint32_t MinLiveChannelFragCount = 1;
    const uint32_t MaxLiveChannelFragCount = 100;
    const uint32_t MinLiveChannelFragDuration = 1;
    const uint32_t MaxLiveChannelFragDuration = 100;
    const uint32_t MinLiveChannelPlayListLength = 6;
    const uint32_t MaxLiveChannelPlayListLength = 128;
    const uint32_t MinLiveChannelInterval = 1;
    const uint32_t MaxLiveChannelInterval = 100;
    const uint64_t SecondsOfDay = 24*60*60;
    const uint32_t MaxListLiveChannelKeys = 1000;
    const uint32_t TagKeyLengthLimit = 128;
    const uint32_t TagValueLengthLimit = 256;
    const uint32_t MaxTagSize = 10;

#ifdef _WIN32
    const char PATH_DELIMITER = '\\';
    const wchar_t WPATH_DELIMITER = L'\\';
#else
    const char PATH_DELIMITER = '/';
    const wchar_t WPATH_DELIMITER = L'/';
#endif
}
}

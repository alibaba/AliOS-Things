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


#include <alibabacloud/oss/model/GenerateRTMPSignedUrlRequest.h>
#include <sstream>
#include "../utils/Utils.h"

using namespace AlibabaCloud::OSS;

GenerateRTMPSignedUrlRequest::GenerateRTMPSignedUrlRequest(
    const std::string& bucket, 
    const std::string& channelName, const std::string& playlist,
    uint64_t expires):
    LiveChannelRequest(bucket, channelName),
    playList_(playlist),
    expires_(expires)
{

}

ParameterCollection GenerateRTMPSignedUrlRequest::Parameters() const
{
    ParameterCollection collection;
    if(!playList_.empty())
    {
        collection["playlistName"] = playList_;
    }
    return collection;
}

void GenerateRTMPSignedUrlRequest::setPlayList(const std::string &playList)
{
    playList_ = playList;
}

void GenerateRTMPSignedUrlRequest::setExpires(uint64_t expires)
{
    expires_ = expires;
}

const std::string& GenerateRTMPSignedUrlRequest::PlayList() const
{
    return playList_;
}

uint64_t GenerateRTMPSignedUrlRequest::Expires() const
{
    return expires_;
}


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

#include <alibabacloud/oss/model/ListLiveChannelRequest.h>
#include <sstream>
#include "ModelError.h"
#include "Const.h"

using namespace AlibabaCloud::OSS;

ListLiveChannelRequest::ListLiveChannelRequest(const std::string &bucket):
    OssBucketRequest(bucket), 
    maxKeys_(100)
{
    
}

ParameterCollection ListLiveChannelRequest::specialParameters() const
{
    ParameterCollection colletion;
    colletion["live"] = "";
    if(!marker_.empty())
    {
        colletion["marker"] = marker_;
    }
    if(!prefix_.empty())
    {
        colletion["prefix"] = prefix_;
    }
    if(0 != maxKeys_)
    {
        colletion["max-keys"] = std::to_string(maxKeys_);
    }
    return colletion;
}

int ListLiveChannelRequest::validate() const
{
    if(0 == maxKeys_ || MaxListLiveChannelKeys < maxKeys_)
    {
        return ARG_ERROR_LIVECHANNEL_BAD_MAXKEY_PARAM;
    }
    return OssBucketRequest::validate();
}

void ListLiveChannelRequest::setMarker(const std::string &marker)
{
    marker_ = marker;
}

void ListLiveChannelRequest::setPrefix(const std::string &prefix)
{
    prefix_ = prefix;
}

void ListLiveChannelRequest::setMaxKeys(uint32_t maxKeys)
{
    maxKeys_ = maxKeys;
}


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


#include <alibabacloud/oss/model/GetVodPlaylistResult.h>
#include <tinyxml2/tinyxml2.h>
#include <alibabacloud/oss/model/Bucket.h>
#include <alibabacloud/oss/model/Owner.h>
#include <sstream>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;



GetVodPlaylistResult::GetVodPlaylistResult():
    OssResult()
{
}

GetVodPlaylistResult::GetVodPlaylistResult(const std::string& result):
    GetVodPlaylistResult()
{
    *this = result;
}

GetVodPlaylistResult::GetVodPlaylistResult(const std::shared_ptr<std::iostream>& result):
    GetVodPlaylistResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetVodPlaylistResult& GetVodPlaylistResult::operator =(const std::string& result)
{
    playListContent_ = result;
    parseDone_ = true;
    return *this;
}

const std::string& GetVodPlaylistResult::PlaylistContent() const
{
    return playListContent_;
}

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


#include <alibabacloud/oss/model/GetLiveChannelInfoResult.h>
#include <alibabacloud/oss/model/Owner.h>
#include <external/tinyxml2/tinyxml2.h>
#include <sstream>
#include "utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

GetLiveChannelInfoResult::GetLiveChannelInfoResult() :
    OssResult(), 
    fragDuration_(0), 
    fragCount_(0)
{
    
}

GetLiveChannelInfoResult::GetLiveChannelInfoResult(const std::string& result):
    GetLiveChannelInfoResult()
{
    *this = result;
}

GetLiveChannelInfoResult::GetLiveChannelInfoResult(const std::shared_ptr<std::iostream>& result):
    GetLiveChannelInfoResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetLiveChannelInfoResult& GetLiveChannelInfoResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("LiveChannelConfiguration", root->Name(), 24)) {
            XMLElement *node;

            node  = root->FirstChildElement("Description");
            if(node && node->GetText())
            {
                description_ = node->GetText();
            }

            node  = root->FirstChildElement("Status");
            if(node && node->GetText())
            {
                status_ = ToLiveChannelStatusType(node->GetText());
            }
            
            XMLElement *targetNode = root->FirstChildElement("Target");
            if(targetNode)
            {
                node  = targetNode->FirstChildElement("Type");
                if(node && node->GetText())
                {
                    channelType_ = node->GetText();
                }

                node  = targetNode->FirstChildElement("FragDuration");
                if(node && node->GetText())
                {
                    fragDuration_ = std::strtoull(node->GetText(), nullptr, 10);
                }

                node  = targetNode->FirstChildElement("FragCount");
                if(node && node->GetText())
                {
                    fragCount_ = std::strtoull(node->GetText(), nullptr, 10);
                }

                node  = targetNode->FirstChildElement("PlaylistName");
                if(node && node->GetText())
                {
                    playListName_ = node->GetText();
                }
            }
            parseDone_ = true;
        }
    }
    return *this;
}

const std::string& GetLiveChannelInfoResult::Description() const
{
    return description_;
}

LiveChannelStatus GetLiveChannelInfoResult::Status() const
{
    return status_;
}

const std::string& GetLiveChannelInfoResult::Type() const
{
    return channelType_;
}
uint64_t GetLiveChannelInfoResult::FragDuration() const
{
    return fragDuration_;
}

uint64_t GetLiveChannelInfoResult::FragCount() const
{
    return fragCount_;
}

const std::string& GetLiveChannelInfoResult::PlaylistName() const
{
    return playListName_;
}

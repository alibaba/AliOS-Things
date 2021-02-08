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


#include <alibabacloud/oss/model/GetLiveChannelHistoryResult.h>
#include <alibabacloud/oss/model/Owner.h>
#include <tinyxml2/tinyxml2.h>
#include <sstream>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

GetLiveChannelHistoryResult::GetLiveChannelHistoryResult() :
    OssResult()
{
    
}

GetLiveChannelHistoryResult::GetLiveChannelHistoryResult(const std::string& result):
    GetLiveChannelHistoryResult()
{
    *this = result;
}

GetLiveChannelHistoryResult::GetLiveChannelHistoryResult(const std::shared_ptr<std::iostream>& result):
    GetLiveChannelHistoryResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetLiveChannelHistoryResult& GetLiveChannelHistoryResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("LiveChannelHistory", root->Name(), 18)) {
            XMLElement *node;
            XMLElement *recordNode = root->FirstChildElement("LiveRecord");
            for(; recordNode; recordNode = recordNode->NextSiblingElement("LiveRecord"))
            {
                LiveRecord rec;
                node = recordNode->FirstChildElement("StartTime");
                if(node && node->GetText())
                {
                    rec.startTime = node->GetText();
                }
                node = recordNode->FirstChildElement("EndTime");
                if(node && node->GetText())
                {
                    rec.endTime = node->GetText();
                }
                node = recordNode->FirstChildElement("RemoteAddr");
                if(node && node->GetText())
                {
                    rec.remoteAddr = node->GetText();
                }
                recordList_.push_back(rec);
            }
            parseDone_ = true;
        }
    }
    return *this;
}

const LiveRecordVec& GetLiveChannelHistoryResult::LiveRecordList() const
{
    return recordList_;
}

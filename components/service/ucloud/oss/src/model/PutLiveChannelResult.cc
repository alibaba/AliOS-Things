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


#include <alibabacloud/oss/model/PutLiveChannelResult.h>
#include <tinyxml2/tinyxml2.h>
#include <alibabacloud/oss/model/Bucket.h>
#include <alibabacloud/oss/model/Owner.h>
#include "../utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

PutLiveChannelResult::PutLiveChannelResult():
    OssResult()
{
}

PutLiveChannelResult::PutLiveChannelResult(const std::string& result):
    PutLiveChannelResult()
{
    *this = result;
}

PutLiveChannelResult::PutLiveChannelResult(const std::shared_ptr<std::iostream>& result):
    PutLiveChannelResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

PutLiveChannelResult& PutLiveChannelResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("CreateLiveChannelResult", root->Name(), 23)) {
            XMLElement *node;
            
            XMLElement* publishNode = root->FirstChildElement("PublishUrls");
            if(publishNode && (node = publishNode->FirstChildElement("Url"))){
                publishUrl_ = node->GetText();
            }

            XMLElement* playNode = root->FirstChildElement("PlayUrls");
            if(playNode && (node = playNode->FirstChildElement("Url"))){
                playUrl_ = node->GetText();
            }
        }
        parseDone_ = true;
    }
    return *this;
}

const std::string& PutLiveChannelResult::PublishUrl() const
{
    return publishUrl_;
}

const std::string& PutLiveChannelResult::PlayUrl() const
{
    return playUrl_;
}


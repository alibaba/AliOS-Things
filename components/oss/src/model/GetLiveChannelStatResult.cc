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


#include <alibabacloud/oss/model/GetLiveChannelStatResult.h>
#include <alibabacloud/oss/model/Owner.h>
#include <external/tinyxml2/tinyxml2.h>
#include <sstream>
#include "utils/Utils.h"
using namespace AlibabaCloud::OSS;
using namespace tinyxml2;

GetLiveChannelStatResult::GetLiveChannelStatResult() :
    OssResult(), 
    width_(0), 
    height_(0), 
    frameRate_(0),
    videoBandWidth_(0), 
    sampleRate_(0), 
    audioBandWidth_(0)
{
    
}

GetLiveChannelStatResult::GetLiveChannelStatResult(const std::string& result):
    GetLiveChannelStatResult()
{
    *this = result;
}

GetLiveChannelStatResult::GetLiveChannelStatResult(const std::shared_ptr<std::iostream>& result):
    GetLiveChannelStatResult()
{
    std::istreambuf_iterator<char> isb(*result.get()), end;
    std::string str(isb, end);
    *this = str;
}

GetLiveChannelStatResult& GetLiveChannelStatResult::operator =(const std::string& result)
{
    XMLDocument doc;
    XMLError xml_err;
    if ((xml_err = doc.Parse(result.c_str(), result.size())) == XML_SUCCESS) {
        XMLElement* root =doc.RootElement();
        if (root && !std::strncmp("LiveChannelStat", root->Name(), 15)) {
            XMLElement *node;

            node  = root->FirstChildElement("Status");
            if(node && node->GetText())
            {
                status_ = ToLiveChannelStatusType(node->GetText());
            }
            
            node = root->FirstChildElement("ConnectedTime");
            if(node && node->GetText())
            {
                connectedTime_ = node->GetText();
            }

            node = root->FirstChildElement("RemoteAddr");
            if(node && node->GetText())
            {
                remoteAddr_ = node->GetText();
            }
            XMLElement *videoRoot = root->FirstChildElement("Video");
            if(videoRoot)
            {
                node = videoRoot->FirstChildElement("Width");
                if(node && node->GetText())
                {
                    width_ = std::strtoul(node->GetText(), nullptr, 10);
                }
                node = videoRoot->FirstChildElement("Height");
                if(node && node->GetText())
                {
                    height_ = std::strtoul(node->GetText(), nullptr, 10);
                }
                node = videoRoot->FirstChildElement("FrameRate");
                if(node && node->GetText())
                {
                    frameRate_ = std::strtoull(node->GetText(), nullptr, 10);
                }
                node = videoRoot->FirstChildElement("Bandwidth");
                if(node && node->GetText())
                {
                    videoBandWidth_ = std::strtoull(node->GetText(), nullptr, 10);
                }
                node = videoRoot->FirstChildElement("Codec");
                if(node && node->GetText())
                {
                    videoCodec_ = node->GetText();
                }
            }
            XMLElement *audioRoot = root->FirstChildElement("Audio");
            if(audioRoot)
            {
                node = audioRoot->FirstChildElement("Bandwidth");
                if(node && node->GetText())
                {
                    audioBandWidth_ = std::strtoull(node->GetText(), nullptr, 10);
                }
                node = audioRoot->FirstChildElement("SampleRate");
                if(node && node->GetText())
                {
                    sampleRate_ = std::strtoull(node->GetText(), nullptr, 10);
                }
                node = audioRoot->FirstChildElement("Codec");
                if(node && node->GetText())
                {
                    audioCodec_ = node->GetText();
                }
            }
            parseDone_ = true;
        }
    }
    return *this;
}

LiveChannelStatus GetLiveChannelStatResult::Status() const
{
    return status_;
}

const std::string& GetLiveChannelStatResult::ConnectedTime() const
{
    return connectedTime_;
}

const std::string& GetLiveChannelStatResult::RemoteAddr() const
{
    return remoteAddr_;
}

uint32_t GetLiveChannelStatResult::Width() const
{
    return width_;
}

uint32_t GetLiveChannelStatResult::Height() const
{
    return height_;
}

uint64_t GetLiveChannelStatResult::FrameRate() const
{
    return frameRate_;
}

uint64_t GetLiveChannelStatResult::VideoBandWidth() const
{
    return videoBandWidth_;
}

const std::string& GetLiveChannelStatResult::VideoCodec() const
{
    return videoCodec_;
}

uint64_t GetLiveChannelStatResult::SampleRate() const
{
    return sampleRate_;
}

uint64_t GetLiveChannelStatResult::AudioBandWidth() const
{
    return audioBandWidth_;
}

const std::string& GetLiveChannelStatResult::AudioCodec() const
{
    return audioCodec_;
}


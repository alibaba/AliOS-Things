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

#include <alibabacloud/live/model/DescribeLiveStreamDelayConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamDelayConfigResult::DescribeLiveStreamDelayConfigResult() :
	ServiceResult()
{}

DescribeLiveStreamDelayConfigResult::DescribeLiveStreamDelayConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamDelayConfigResult::~DescribeLiveStreamDelayConfigResult()
{}

void DescribeLiveStreamDelayConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto liveStreamHlsDelayConfigNode = value["LiveStreamHlsDelayConfig"];
	if(!liveStreamHlsDelayConfigNode["Level"].isNull())
		liveStreamHlsDelayConfig_.level = liveStreamHlsDelayConfigNode["Level"].asString();
	if(!liveStreamHlsDelayConfigNode["Delay"].isNull())
		liveStreamHlsDelayConfig_.delay = std::stoi(liveStreamHlsDelayConfigNode["Delay"].asString());
	auto liveStreamFlvDelayConfigNode = value["LiveStreamFlvDelayConfig"];
	if(!liveStreamFlvDelayConfigNode["Level"].isNull())
		liveStreamFlvDelayConfig_.level = liveStreamFlvDelayConfigNode["Level"].asString();
	if(!liveStreamFlvDelayConfigNode["Delay"].isNull())
		liveStreamFlvDelayConfig_.delay = std::stoi(liveStreamFlvDelayConfigNode["Delay"].asString());
	auto liveStreamRtmpDelayConfigNode = value["LiveStreamRtmpDelayConfig"];
	if(!liveStreamRtmpDelayConfigNode["Level"].isNull())
		liveStreamRtmpDelayConfig_.level = liveStreamRtmpDelayConfigNode["Level"].asString();
	if(!liveStreamRtmpDelayConfigNode["Delay"].isNull())
		liveStreamRtmpDelayConfig_.delay = std::stoi(liveStreamRtmpDelayConfigNode["Delay"].asString());

}

DescribeLiveStreamDelayConfigResult::LiveStreamFlvDelayConfig DescribeLiveStreamDelayConfigResult::getLiveStreamFlvDelayConfig()const
{
	return liveStreamFlvDelayConfig_;
}

DescribeLiveStreamDelayConfigResult::LiveStreamRtmpDelayConfig DescribeLiveStreamDelayConfigResult::getLiveStreamRtmpDelayConfig()const
{
	return liveStreamRtmpDelayConfig_;
}

DescribeLiveStreamDelayConfigResult::LiveStreamHlsDelayConfig DescribeLiveStreamDelayConfigResult::getLiveStreamHlsDelayConfig()const
{
	return liveStreamHlsDelayConfig_;
}


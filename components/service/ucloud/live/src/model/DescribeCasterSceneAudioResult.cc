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

#include <alibabacloud/live/model/DescribeCasterSceneAudioResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCasterSceneAudioResult::DescribeCasterSceneAudioResult() :
	ServiceResult()
{}

DescribeCasterSceneAudioResult::DescribeCasterSceneAudioResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCasterSceneAudioResult::~DescribeCasterSceneAudioResult()
{}

void DescribeCasterSceneAudioResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allAudioLayersNode = value["AudioLayers"]["AudioLayer"];
	for (auto valueAudioLayersAudioLayer : allAudioLayersNode)
	{
		AudioLayer audioLayersObject;
		if(!valueAudioLayersAudioLayer["VolumeRate"].isNull())
			audioLayersObject.volumeRate = std::stof(valueAudioLayersAudioLayer["VolumeRate"].asString());
		if(!valueAudioLayersAudioLayer["ValidChannel"].isNull())
			audioLayersObject.validChannel = valueAudioLayersAudioLayer["ValidChannel"].asString();
		if(!valueAudioLayersAudioLayer["FixedDelayDuration"].isNull())
			audioLayersObject.fixedDelayDuration = std::stoi(valueAudioLayersAudioLayer["FixedDelayDuration"].asString());
		audioLayers_.push_back(audioLayersObject);
	}
	auto allMixList = value["MixList"]["LocationId"];
	for (const auto &item : allMixList)
		mixList_.push_back(item.asString());
	if(!value["CasterId"].isNull())
		casterId_ = value["CasterId"].asString();
	if(!value["FollowEnable"].isNull())
		followEnable_ = std::stoi(value["FollowEnable"].asString());

}

std::vector<std::string> DescribeCasterSceneAudioResult::getMixList()const
{
	return mixList_;
}

std::vector<DescribeCasterSceneAudioResult::AudioLayer> DescribeCasterSceneAudioResult::getAudioLayers()const
{
	return audioLayers_;
}

std::string DescribeCasterSceneAudioResult::getCasterId()const
{
	return casterId_;
}

int DescribeCasterSceneAudioResult::getFollowEnable()const
{
	return followEnable_;
}


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

#include <alibabacloud/live/model/DescribeCasterLayoutsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCasterLayoutsResult::DescribeCasterLayoutsResult() :
	ServiceResult()
{}

DescribeCasterLayoutsResult::DescribeCasterLayoutsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCasterLayoutsResult::~DescribeCasterLayoutsResult()
{}

void DescribeCasterLayoutsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLayoutsNode = value["Layouts"]["Layout"];
	for (auto valueLayoutsLayout : allLayoutsNode)
	{
		Layout layoutsObject;
		if(!valueLayoutsLayout["LayoutId"].isNull())
			layoutsObject.layoutId = valueLayoutsLayout["LayoutId"].asString();
		auto allVideoLayersNode = allLayoutsNode["VideoLayers"]["VideoLayer"];
		for (auto allLayoutsNodeVideoLayersVideoLayer : allVideoLayersNode)
		{
			Layout::VideoLayer videoLayersObject;
			if(!allLayoutsNodeVideoLayersVideoLayer["FillMode"].isNull())
				videoLayersObject.fillMode = allLayoutsNodeVideoLayersVideoLayer["FillMode"].asString();
			if(!allLayoutsNodeVideoLayersVideoLayer["HeightNormalized"].isNull())
				videoLayersObject.heightNormalized = std::stof(allLayoutsNodeVideoLayersVideoLayer["HeightNormalized"].asString());
			if(!allLayoutsNodeVideoLayersVideoLayer["WidthNormalized"].isNull())
				videoLayersObject.widthNormalized = std::stof(allLayoutsNodeVideoLayersVideoLayer["WidthNormalized"].asString());
			if(!allLayoutsNodeVideoLayersVideoLayer["PositionRefer"].isNull())
				videoLayersObject.positionRefer = allLayoutsNodeVideoLayersVideoLayer["PositionRefer"].asString();
			if(!allLayoutsNodeVideoLayersVideoLayer["FixedDelayDuration"].isNull())
				videoLayersObject.fixedDelayDuration = std::stoi(allLayoutsNodeVideoLayersVideoLayer["FixedDelayDuration"].asString());
			auto allPositionNormalizeds = value["PositionNormalizeds"]["Position"];
			for (auto value : allPositionNormalizeds)
				videoLayersObject.positionNormalizeds.push_back(value.asString());
			layoutsObject.videoLayers.push_back(videoLayersObject);
		}
		auto allAudioLayersNode = allLayoutsNode["AudioLayers"]["AudioLayer"];
		for (auto allLayoutsNodeAudioLayersAudioLayer : allAudioLayersNode)
		{
			Layout::AudioLayer audioLayersObject;
			if(!allLayoutsNodeAudioLayersAudioLayer["VolumeRate"].isNull())
				audioLayersObject.volumeRate = std::stof(allLayoutsNodeAudioLayersAudioLayer["VolumeRate"].asString());
			if(!allLayoutsNodeAudioLayersAudioLayer["ValidChannel"].isNull())
				audioLayersObject.validChannel = allLayoutsNodeAudioLayersAudioLayer["ValidChannel"].asString();
			if(!allLayoutsNodeAudioLayersAudioLayer["FixedDelayDuration"].isNull())
				audioLayersObject.fixedDelayDuration = std::stoi(allLayoutsNodeAudioLayersAudioLayer["FixedDelayDuration"].asString());
			layoutsObject.audioLayers.push_back(audioLayersObject);
		}
		auto allBlendList = value["BlendList"]["LocationId"];
		for (auto value : allBlendList)
			layoutsObject.blendList.push_back(value.asString());
		auto allMixList = value["MixList"]["LocationId"];
		for (auto value : allMixList)
			layoutsObject.mixList.push_back(value.asString());
		layouts_.push_back(layoutsObject);
	}
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

std::vector<DescribeCasterLayoutsResult::Layout> DescribeCasterLayoutsResult::getLayouts()const
{
	return layouts_;
}

int DescribeCasterLayoutsResult::getTotal()const
{
	return total_;
}


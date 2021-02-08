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

#include <alibabacloud/live/model/DescribeCasterStreamUrlResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCasterStreamUrlResult::DescribeCasterStreamUrlResult() :
	ServiceResult()
{}

DescribeCasterStreamUrlResult::DescribeCasterStreamUrlResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCasterStreamUrlResult::~DescribeCasterStreamUrlResult()
{}

void DescribeCasterStreamUrlResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allCasterStreamsNode = value["CasterStreams"]["CasterStream"];
	for (auto valueCasterStreamsCasterStream : allCasterStreamsNode)
	{
		CasterStream casterStreamsObject;
		if(!valueCasterStreamsCasterStream["SceneId"].isNull())
			casterStreamsObject.sceneId = valueCasterStreamsCasterStream["SceneId"].asString();
		if(!valueCasterStreamsCasterStream["StreamUrl"].isNull())
			casterStreamsObject.streamUrl = valueCasterStreamsCasterStream["StreamUrl"].asString();
		if(!valueCasterStreamsCasterStream["RtmpUrl"].isNull())
			casterStreamsObject.rtmpUrl = valueCasterStreamsCasterStream["RtmpUrl"].asString();
		if(!valueCasterStreamsCasterStream["OutputType"].isNull())
			casterStreamsObject.outputType = std::stoi(valueCasterStreamsCasterStream["OutputType"].asString());
		auto allStreamInfosNode = allCasterStreamsNode["StreamInfos"]["StreamInfo"];
		for (auto allCasterStreamsNodeStreamInfosStreamInfo : allStreamInfosNode)
		{
			CasterStream::StreamInfo streamInfosObject;
			if(!allCasterStreamsNodeStreamInfosStreamInfo["TranscodeConfig"].isNull())
				streamInfosObject.transcodeConfig = allCasterStreamsNodeStreamInfosStreamInfo["TranscodeConfig"].asString();
			if(!allCasterStreamsNodeStreamInfosStreamInfo["VideoFormat"].isNull())
				streamInfosObject.videoFormat = allCasterStreamsNodeStreamInfosStreamInfo["VideoFormat"].asString();
			if(!allCasterStreamsNodeStreamInfosStreamInfo["OutputStreamUrl"].isNull())
				streamInfosObject.outputStreamUrl = allCasterStreamsNodeStreamInfosStreamInfo["OutputStreamUrl"].asString();
			casterStreamsObject.streamInfos.push_back(streamInfosObject);
		}
		casterStreams_.push_back(casterStreamsObject);
	}
	if(!value["CasterId"].isNull())
		casterId_ = value["CasterId"].asString();
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

std::vector<DescribeCasterStreamUrlResult::CasterStream> DescribeCasterStreamUrlResult::getCasterStreams()const
{
	return casterStreams_;
}

int DescribeCasterStreamUrlResult::getTotal()const
{
	return total_;
}

std::string DescribeCasterStreamUrlResult::getCasterId()const
{
	return casterId_;
}


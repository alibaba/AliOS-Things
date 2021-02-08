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

#include <alibabacloud/live/model/DescribeLiveDomainFrameRateAndBitRateDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainFrameRateAndBitRateDataResult::DescribeLiveDomainFrameRateAndBitRateDataResult() :
	ServiceResult()
{}

DescribeLiveDomainFrameRateAndBitRateDataResult::DescribeLiveDomainFrameRateAndBitRateDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainFrameRateAndBitRateDataResult::~DescribeLiveDomainFrameRateAndBitRateDataResult()
{}

void DescribeLiveDomainFrameRateAndBitRateDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFrameRateAndBitRateInfosNode = value["FrameRateAndBitRateInfos"]["FrameRateAndBitRateInfo"];
	for (auto valueFrameRateAndBitRateInfosFrameRateAndBitRateInfo : allFrameRateAndBitRateInfosNode)
	{
		FrameRateAndBitRateInfo frameRateAndBitRateInfosObject;
		if(!valueFrameRateAndBitRateInfosFrameRateAndBitRateInfo["AudioFrameRate"].isNull())
			frameRateAndBitRateInfosObject.audioFrameRate = std::stof(valueFrameRateAndBitRateInfosFrameRateAndBitRateInfo["AudioFrameRate"].asString());
		if(!valueFrameRateAndBitRateInfosFrameRateAndBitRateInfo["BitRate"].isNull())
			frameRateAndBitRateInfosObject.bitRate = std::stof(valueFrameRateAndBitRateInfosFrameRateAndBitRateInfo["BitRate"].asString());
		if(!valueFrameRateAndBitRateInfosFrameRateAndBitRateInfo["VideoFrameRate"].isNull())
			frameRateAndBitRateInfosObject.videoFrameRate = std::stof(valueFrameRateAndBitRateInfosFrameRateAndBitRateInfo["VideoFrameRate"].asString());
		if(!valueFrameRateAndBitRateInfosFrameRateAndBitRateInfo["StreamUrl"].isNull())
			frameRateAndBitRateInfosObject.streamUrl = valueFrameRateAndBitRateInfosFrameRateAndBitRateInfo["StreamUrl"].asString();
		frameRateAndBitRateInfos_.push_back(frameRateAndBitRateInfosObject);
	}

}

std::vector<DescribeLiveDomainFrameRateAndBitRateDataResult::FrameRateAndBitRateInfo> DescribeLiveDomainFrameRateAndBitRateDataResult::getFrameRateAndBitRateInfos()const
{
	return frameRateAndBitRateInfos_;
}


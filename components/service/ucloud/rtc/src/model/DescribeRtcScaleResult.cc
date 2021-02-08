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

#include <alibabacloud/rtc/model/DescribeRtcScaleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcScaleResult::DescribeRtcScaleResult() :
	ServiceResult()
{}

DescribeRtcScaleResult::DescribeRtcScaleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcScaleResult::~DescribeRtcScaleResult()
{}

void DescribeRtcScaleResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allScaleNode = value["Scale"]["ScaleItem"];
	for (auto valueScaleScaleItem : allScaleNode)
	{
		ScaleItem scaleObject;
		if(!valueScaleScaleItem["Time"].isNull())
			scaleObject.time = valueScaleScaleItem["Time"].asString();
		if(!valueScaleScaleItem["ChannelCount"].isNull())
			scaleObject.channelCount = std::stol(valueScaleScaleItem["ChannelCount"].asString());
		if(!valueScaleScaleItem["UserCount"].isNull())
			scaleObject.userCount = std::stol(valueScaleScaleItem["UserCount"].asString());
		if(!valueScaleScaleItem["SessionCount"].isNull())
			scaleObject.sessionCount = std::stol(valueScaleScaleItem["SessionCount"].asString());
		if(!valueScaleScaleItem["AudioDuration"].isNull())
			scaleObject.audioDuration = std::stol(valueScaleScaleItem["AudioDuration"].asString());
		if(!valueScaleScaleItem["VideoDuration"].isNull())
			scaleObject.videoDuration = std::stol(valueScaleScaleItem["VideoDuration"].asString());
		scale_.push_back(scaleObject);
	}

}

std::vector<DescribeRtcScaleResult::ScaleItem> DescribeRtcScaleResult::getScale()const
{
	return scale_;
}


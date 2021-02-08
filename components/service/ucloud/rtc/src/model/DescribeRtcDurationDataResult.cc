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

#include <alibabacloud/rtc/model/DescribeRtcDurationDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcDurationDataResult::DescribeRtcDurationDataResult() :
	ServiceResult()
{}

DescribeRtcDurationDataResult::DescribeRtcDurationDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcDurationDataResult::~DescribeRtcDurationDataResult()
{}

void DescribeRtcDurationDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDurationDataPerIntervalNode = value["DurationDataPerInterval"]["DurationModule"];
	for (auto valueDurationDataPerIntervalDurationModule : allDurationDataPerIntervalNode)
	{
		DurationModule durationDataPerIntervalObject;
		if(!valueDurationDataPerIntervalDurationModule["TimeStamp"].isNull())
			durationDataPerIntervalObject.timeStamp = valueDurationDataPerIntervalDurationModule["TimeStamp"].asString();
		if(!valueDurationDataPerIntervalDurationModule["TotalDuration"].isNull())
			durationDataPerIntervalObject.totalDuration = std::stol(valueDurationDataPerIntervalDurationModule["TotalDuration"].asString());
		if(!valueDurationDataPerIntervalDurationModule["AudioDuration"].isNull())
			durationDataPerIntervalObject.audioDuration = std::stol(valueDurationDataPerIntervalDurationModule["AudioDuration"].asString());
		if(!valueDurationDataPerIntervalDurationModule["V360Duration"].isNull())
			durationDataPerIntervalObject.v360Duration = std::stol(valueDurationDataPerIntervalDurationModule["V360Duration"].asString());
		if(!valueDurationDataPerIntervalDurationModule["V720Duration"].isNull())
			durationDataPerIntervalObject.v720Duration = std::stol(valueDurationDataPerIntervalDurationModule["V720Duration"].asString());
		if(!valueDurationDataPerIntervalDurationModule["V1080Duration"].isNull())
			durationDataPerIntervalObject.v1080Duration = std::stol(valueDurationDataPerIntervalDurationModule["V1080Duration"].asString());
		if(!valueDurationDataPerIntervalDurationModule["ContentDuration"].isNull())
			durationDataPerIntervalObject.contentDuration = std::stol(valueDurationDataPerIntervalDurationModule["ContentDuration"].asString());
		durationDataPerInterval_.push_back(durationDataPerIntervalObject);
	}

}

std::vector<DescribeRtcDurationDataResult::DurationModule> DescribeRtcDurationDataResult::getDurationDataPerInterval()const
{
	return durationDataPerInterval_;
}


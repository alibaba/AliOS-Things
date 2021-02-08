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

#include <alibabacloud/rtc/model/DescribeRtcScaleDetailResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcScaleDetailResult::DescribeRtcScaleDetailResult() :
	ServiceResult()
{}

DescribeRtcScaleDetailResult::DescribeRtcScaleDetailResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcScaleDetailResult::~DescribeRtcScaleDetailResult()
{}

void DescribeRtcScaleDetailResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allScaleNode = value["Scale"]["ScaleItem"];
	for (auto valueScaleScaleItem : allScaleNode)
	{
		ScaleItem scaleObject;
		if(!valueScaleScaleItem["TS"].isNull())
			scaleObject.tS = valueScaleScaleItem["TS"].asString();
		if(!valueScaleScaleItem["CC"].isNull())
			scaleObject.cC = std::stol(valueScaleScaleItem["CC"].asString());
		if(!valueScaleScaleItem["UC"].isNull())
			scaleObject.uC = std::stol(valueScaleScaleItem["UC"].asString());
		scale_.push_back(scaleObject);
	}

}

std::vector<DescribeRtcScaleDetailResult::ScaleItem> DescribeRtcScaleDetailResult::getScale()const
{
	return scale_;
}


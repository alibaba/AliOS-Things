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

#include <alibabacloud/live/model/DescribeUpBpsPeakDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeUpBpsPeakDataResult::DescribeUpBpsPeakDataResult() :
	ServiceResult()
{}

DescribeUpBpsPeakDataResult::DescribeUpBpsPeakDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeUpBpsPeakDataResult::~DescribeUpBpsPeakDataResult()
{}

void DescribeUpBpsPeakDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDescribeUpPeakTrafficsNode = value["DescribeUpPeakTraffics"]["DescribeUpPeakTraffic"];
	for (auto valueDescribeUpPeakTrafficsDescribeUpPeakTraffic : allDescribeUpPeakTrafficsNode)
	{
		DescribeUpPeakTraffic describeUpPeakTrafficsObject;
		if(!valueDescribeUpPeakTrafficsDescribeUpPeakTraffic["PeakTime"].isNull())
			describeUpPeakTrafficsObject.peakTime = valueDescribeUpPeakTrafficsDescribeUpPeakTraffic["PeakTime"].asString();
		if(!valueDescribeUpPeakTrafficsDescribeUpPeakTraffic["QueryTime"].isNull())
			describeUpPeakTrafficsObject.queryTime = valueDescribeUpPeakTrafficsDescribeUpPeakTraffic["QueryTime"].asString();
		if(!valueDescribeUpPeakTrafficsDescribeUpPeakTraffic["StatName"].isNull())
			describeUpPeakTrafficsObject.statName = valueDescribeUpPeakTrafficsDescribeUpPeakTraffic["StatName"].asString();
		if(!valueDescribeUpPeakTrafficsDescribeUpPeakTraffic["BandWidth"].isNull())
			describeUpPeakTrafficsObject.bandWidth = valueDescribeUpPeakTrafficsDescribeUpPeakTraffic["BandWidth"].asString();
		describeUpPeakTraffics_.push_back(describeUpPeakTrafficsObject);
	}

}

std::vector<DescribeUpBpsPeakDataResult::DescribeUpPeakTraffic> DescribeUpBpsPeakDataResult::getDescribeUpPeakTraffics()const
{
	return describeUpPeakTraffics_;
}


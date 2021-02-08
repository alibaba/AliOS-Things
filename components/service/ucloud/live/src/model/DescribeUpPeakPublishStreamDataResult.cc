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

#include <alibabacloud/live/model/DescribeUpPeakPublishStreamDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeUpPeakPublishStreamDataResult::DescribeUpPeakPublishStreamDataResult() :
	ServiceResult()
{}

DescribeUpPeakPublishStreamDataResult::DescribeUpPeakPublishStreamDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeUpPeakPublishStreamDataResult::~DescribeUpPeakPublishStreamDataResult()
{}

void DescribeUpPeakPublishStreamDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDescribeUpPeakPublishStreamDatasNode = value["DescribeUpPeakPublishStreamDatas"]["DescribeUpPeakPublishStreamData"];
	for (auto valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData : allDescribeUpPeakPublishStreamDatasNode)
	{
		DescribeUpPeakPublishStreamData describeUpPeakPublishStreamDatasObject;
		if(!valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["PublishStreamNum"].isNull())
			describeUpPeakPublishStreamDatasObject.publishStreamNum = std::stoi(valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["PublishStreamNum"].asString());
		if(!valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["PeakTime"].isNull())
			describeUpPeakPublishStreamDatasObject.peakTime = valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["PeakTime"].asString();
		if(!valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["QueryTime"].isNull())
			describeUpPeakPublishStreamDatasObject.queryTime = valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["QueryTime"].asString();
		if(!valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["StatName"].isNull())
			describeUpPeakPublishStreamDatasObject.statName = valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["StatName"].asString();
		if(!valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["BandWidth"].isNull())
			describeUpPeakPublishStreamDatasObject.bandWidth = valueDescribeUpPeakPublishStreamDatasDescribeUpPeakPublishStreamData["BandWidth"].asString();
		describeUpPeakPublishStreamDatas_.push_back(describeUpPeakPublishStreamDatasObject);
	}

}

std::vector<DescribeUpPeakPublishStreamDataResult::DescribeUpPeakPublishStreamData> DescribeUpPeakPublishStreamDataResult::getDescribeUpPeakPublishStreamDatas()const
{
	return describeUpPeakPublishStreamDatas_;
}


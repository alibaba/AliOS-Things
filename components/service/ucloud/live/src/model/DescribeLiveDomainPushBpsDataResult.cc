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

#include <alibabacloud/live/model/DescribeLiveDomainPushBpsDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainPushBpsDataResult::DescribeLiveDomainPushBpsDataResult() :
	ServiceResult()
{}

DescribeLiveDomainPushBpsDataResult::DescribeLiveDomainPushBpsDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainPushBpsDataResult::~DescribeLiveDomainPushBpsDataResult()
{}

void DescribeLiveDomainPushBpsDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allBpsDataPerIntervalNode = value["BpsDataPerInterval"]["DataModule"];
	for (auto valueBpsDataPerIntervalDataModule : allBpsDataPerIntervalNode)
	{
		DataModule bpsDataPerIntervalObject;
		if(!valueBpsDataPerIntervalDataModule["TimeStamp"].isNull())
			bpsDataPerIntervalObject.timeStamp = valueBpsDataPerIntervalDataModule["TimeStamp"].asString();
		if(!valueBpsDataPerIntervalDataModule["BpsValue"].isNull())
			bpsDataPerIntervalObject.bpsValue = valueBpsDataPerIntervalDataModule["BpsValue"].asString();
		bpsDataPerInterval_.push_back(bpsDataPerIntervalObject);
	}
	if(!value["DomainName"].isNull())
		domainName_ = value["DomainName"].asString();
	if(!value["StartTime"].isNull())
		startTime_ = value["StartTime"].asString();
	if(!value["EndTime"].isNull())
		endTime_ = value["EndTime"].asString();
	if(!value["DataInterval"].isNull())
		dataInterval_ = value["DataInterval"].asString();

}

std::string DescribeLiveDomainPushBpsDataResult::getEndTime()const
{
	return endTime_;
}

std::string DescribeLiveDomainPushBpsDataResult::getDomainName()const
{
	return domainName_;
}

std::string DescribeLiveDomainPushBpsDataResult::getStartTime()const
{
	return startTime_;
}

std::string DescribeLiveDomainPushBpsDataResult::getDataInterval()const
{
	return dataInterval_;
}

std::vector<DescribeLiveDomainPushBpsDataResult::DataModule> DescribeLiveDomainPushBpsDataResult::getBpsDataPerInterval()const
{
	return bpsDataPerInterval_;
}


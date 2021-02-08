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

#include <alibabacloud/live/model/DescribeLiveDomainRealTimeBpsDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainRealTimeBpsDataResult::DescribeLiveDomainRealTimeBpsDataResult() :
	ServiceResult()
{}

DescribeLiveDomainRealTimeBpsDataResult::DescribeLiveDomainRealTimeBpsDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainRealTimeBpsDataResult::~DescribeLiveDomainRealTimeBpsDataResult()
{}

void DescribeLiveDomainRealTimeBpsDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRealTimeBpsDataPerIntervalNode = value["RealTimeBpsDataPerInterval"]["DataModule"];
	for (auto valueRealTimeBpsDataPerIntervalDataModule : allRealTimeBpsDataPerIntervalNode)
	{
		DataModule realTimeBpsDataPerIntervalObject;
		if(!valueRealTimeBpsDataPerIntervalDataModule["TimeStamp"].isNull())
			realTimeBpsDataPerIntervalObject.timeStamp = valueRealTimeBpsDataPerIntervalDataModule["TimeStamp"].asString();
		if(!valueRealTimeBpsDataPerIntervalDataModule["Value"].isNull())
			realTimeBpsDataPerIntervalObject.value = valueRealTimeBpsDataPerIntervalDataModule["Value"].asString();
		realTimeBpsDataPerInterval_.push_back(realTimeBpsDataPerIntervalObject);
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

std::string DescribeLiveDomainRealTimeBpsDataResult::getEndTime()const
{
	return endTime_;
}

std::string DescribeLiveDomainRealTimeBpsDataResult::getDomainName()const
{
	return domainName_;
}

std::string DescribeLiveDomainRealTimeBpsDataResult::getStartTime()const
{
	return startTime_;
}

std::string DescribeLiveDomainRealTimeBpsDataResult::getDataInterval()const
{
	return dataInterval_;
}

std::vector<DescribeLiveDomainRealTimeBpsDataResult::DataModule> DescribeLiveDomainRealTimeBpsDataResult::getRealTimeBpsDataPerInterval()const
{
	return realTimeBpsDataPerInterval_;
}


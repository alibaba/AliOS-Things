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

#include <alibabacloud/live/model/DescribeLiveDomainRealTimeHttpCodeDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainRealTimeHttpCodeDataResult::DescribeLiveDomainRealTimeHttpCodeDataResult() :
	ServiceResult()
{}

DescribeLiveDomainRealTimeHttpCodeDataResult::DescribeLiveDomainRealTimeHttpCodeDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainRealTimeHttpCodeDataResult::~DescribeLiveDomainRealTimeHttpCodeDataResult()
{}

void DescribeLiveDomainRealTimeHttpCodeDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRealTimeHttpCodeDataNode = value["RealTimeHttpCodeData"]["UsageData"];
	for (auto valueRealTimeHttpCodeDataUsageData : allRealTimeHttpCodeDataNode)
	{
		UsageData realTimeHttpCodeDataObject;
		if(!valueRealTimeHttpCodeDataUsageData["TimeStamp"].isNull())
			realTimeHttpCodeDataObject.timeStamp = valueRealTimeHttpCodeDataUsageData["TimeStamp"].asString();
		auto allValueNode = allRealTimeHttpCodeDataNode["Value"]["RealTimeCodeProportionData"];
		for (auto allRealTimeHttpCodeDataNodeValueRealTimeCodeProportionData : allValueNode)
		{
			UsageData::RealTimeCodeProportionData valueObject;
			if(!allRealTimeHttpCodeDataNodeValueRealTimeCodeProportionData["Code"].isNull())
				valueObject.code = allRealTimeHttpCodeDataNodeValueRealTimeCodeProportionData["Code"].asString();
			if(!allRealTimeHttpCodeDataNodeValueRealTimeCodeProportionData["Proportion"].isNull())
				valueObject.proportion = allRealTimeHttpCodeDataNodeValueRealTimeCodeProportionData["Proportion"].asString();
			if(!allRealTimeHttpCodeDataNodeValueRealTimeCodeProportionData["Count"].isNull())
				valueObject.count = allRealTimeHttpCodeDataNodeValueRealTimeCodeProportionData["Count"].asString();
			realTimeHttpCodeDataObject.value.push_back(valueObject);
		}
		realTimeHttpCodeData_.push_back(realTimeHttpCodeDataObject);
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

std::string DescribeLiveDomainRealTimeHttpCodeDataResult::getEndTime()const
{
	return endTime_;
}

std::string DescribeLiveDomainRealTimeHttpCodeDataResult::getDomainName()const
{
	return domainName_;
}

std::string DescribeLiveDomainRealTimeHttpCodeDataResult::getStartTime()const
{
	return startTime_;
}

std::string DescribeLiveDomainRealTimeHttpCodeDataResult::getDataInterval()const
{
	return dataInterval_;
}

std::vector<DescribeLiveDomainRealTimeHttpCodeDataResult::UsageData> DescribeLiveDomainRealTimeHttpCodeDataResult::getRealTimeHttpCodeData()const
{
	return realTimeHttpCodeData_;
}


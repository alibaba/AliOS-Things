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

#include <alibabacloud/live/model/DescribeLiveDomainTimeShiftDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainTimeShiftDataResult::DescribeLiveDomainTimeShiftDataResult() :
	ServiceResult()
{}

DescribeLiveDomainTimeShiftDataResult::DescribeLiveDomainTimeShiftDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainTimeShiftDataResult::~DescribeLiveDomainTimeShiftDataResult()
{}

void DescribeLiveDomainTimeShiftDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allTimeShiftDataNode = value["TimeShiftData"]["DataModule"];
	for (auto valueTimeShiftDataDataModule : allTimeShiftDataNode)
	{
		DataModule timeShiftDataObject;
		if(!valueTimeShiftDataDataModule["TimeStamp"].isNull())
			timeShiftDataObject.timeStamp = valueTimeShiftDataDataModule["TimeStamp"].asString();
		if(!valueTimeShiftDataDataModule["Size"].isNull())
			timeShiftDataObject.size = valueTimeShiftDataDataModule["Size"].asString();
		if(!valueTimeShiftDataDataModule["Type"].isNull())
			timeShiftDataObject.type = valueTimeShiftDataDataModule["Type"].asString();
		timeShiftData_.push_back(timeShiftDataObject);
	}

}

std::vector<DescribeLiveDomainTimeShiftDataResult::DataModule> DescribeLiveDomainTimeShiftDataResult::getTimeShiftData()const
{
	return timeShiftData_;
}


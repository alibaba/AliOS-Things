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

#include <alibabacloud/live/model/DescribeLiveDetectPornDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDetectPornDataResult::DescribeLiveDetectPornDataResult() :
	ServiceResult()
{}

DescribeLiveDetectPornDataResult::DescribeLiveDetectPornDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDetectPornDataResult::~DescribeLiveDetectPornDataResult()
{}

void DescribeLiveDetectPornDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDetectPornDataNode = value["DetectPornData"]["DataModule"];
	for (auto valueDetectPornDataDataModule : allDetectPornDataNode)
	{
		DataModule detectPornDataObject;
		if(!valueDetectPornDataDataModule["TimeStamp"].isNull())
			detectPornDataObject.timeStamp = valueDetectPornDataDataModule["TimeStamp"].asString();
		if(!valueDetectPornDataDataModule["App"].isNull())
			detectPornDataObject.app = valueDetectPornDataDataModule["App"].asString();
		if(!valueDetectPornDataDataModule["Domain"].isNull())
			detectPornDataObject.domain = valueDetectPornDataDataModule["Domain"].asString();
		if(!valueDetectPornDataDataModule["Stream"].isNull())
			detectPornDataObject.stream = valueDetectPornDataDataModule["Stream"].asString();
		if(!valueDetectPornDataDataModule["Fee"].isNull())
			detectPornDataObject.fee = valueDetectPornDataDataModule["Fee"].asString();
		if(!valueDetectPornDataDataModule["Scene"].isNull())
			detectPornDataObject.scene = valueDetectPornDataDataModule["Scene"].asString();
		if(!valueDetectPornDataDataModule["Region"].isNull())
			detectPornDataObject.region = valueDetectPornDataDataModule["Region"].asString();
		if(!valueDetectPornDataDataModule["Count"].isNull())
			detectPornDataObject.count = std::stol(valueDetectPornDataDataModule["Count"].asString());
		detectPornData_.push_back(detectPornDataObject);
	}

}

std::vector<DescribeLiveDetectPornDataResult::DataModule> DescribeLiveDetectPornDataResult::getDetectPornData()const
{
	return detectPornData_;
}


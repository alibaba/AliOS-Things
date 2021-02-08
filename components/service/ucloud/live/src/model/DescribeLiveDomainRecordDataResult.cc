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

#include <alibabacloud/live/model/DescribeLiveDomainRecordDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainRecordDataResult::DescribeLiveDomainRecordDataResult() :
	ServiceResult()
{}

DescribeLiveDomainRecordDataResult::DescribeLiveDomainRecordDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainRecordDataResult::~DescribeLiveDomainRecordDataResult()
{}

void DescribeLiveDomainRecordDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRecordDataInfosNode = value["RecordDataInfos"]["RecordDataInfo"];
	for (auto valueRecordDataInfosRecordDataInfo : allRecordDataInfosNode)
	{
		RecordDataInfo recordDataInfosObject;
		if(!valueRecordDataInfosRecordDataInfo["Date"].isNull())
			recordDataInfosObject.date = valueRecordDataInfosRecordDataInfo["Date"].asString();
		if(!valueRecordDataInfosRecordDataInfo["Total"].isNull())
			recordDataInfosObject.total = std::stoi(valueRecordDataInfosRecordDataInfo["Total"].asString());
		auto detailNode = value["Detail"];
		if(!detailNode["MP4"].isNull())
			recordDataInfosObject.detail.mP4 = std::stoi(detailNode["MP4"].asString());
		if(!detailNode["FLV"].isNull())
			recordDataInfosObject.detail.fLV = std::stoi(detailNode["FLV"].asString());
		if(!detailNode["TS"].isNull())
			recordDataInfosObject.detail.tS = std::stoi(detailNode["TS"].asString());
		recordDataInfos_.push_back(recordDataInfosObject);
	}

}

std::vector<DescribeLiveDomainRecordDataResult::RecordDataInfo> DescribeLiveDomainRecordDataResult::getRecordDataInfos()const
{
	return recordDataInfos_;
}


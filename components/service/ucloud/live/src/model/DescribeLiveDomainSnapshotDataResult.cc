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

#include <alibabacloud/live/model/DescribeLiveDomainSnapshotDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainSnapshotDataResult::DescribeLiveDomainSnapshotDataResult() :
	ServiceResult()
{}

DescribeLiveDomainSnapshotDataResult::DescribeLiveDomainSnapshotDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainSnapshotDataResult::~DescribeLiveDomainSnapshotDataResult()
{}

void DescribeLiveDomainSnapshotDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allSnapshotDataInfosNode = value["SnapshotDataInfos"]["SnapshotDataInfo"];
	for (auto valueSnapshotDataInfosSnapshotDataInfo : allSnapshotDataInfosNode)
	{
		SnapshotDataInfo snapshotDataInfosObject;
		if(!valueSnapshotDataInfosSnapshotDataInfo["Date"].isNull())
			snapshotDataInfosObject.date = valueSnapshotDataInfosSnapshotDataInfo["Date"].asString();
		if(!valueSnapshotDataInfosSnapshotDataInfo["Total"].isNull())
			snapshotDataInfosObject.total = std::stoi(valueSnapshotDataInfosSnapshotDataInfo["Total"].asString());
		snapshotDataInfos_.push_back(snapshotDataInfosObject);
	}

}

std::vector<DescribeLiveDomainSnapshotDataResult::SnapshotDataInfo> DescribeLiveDomainSnapshotDataResult::getSnapshotDataInfos()const
{
	return snapshotDataInfos_;
}


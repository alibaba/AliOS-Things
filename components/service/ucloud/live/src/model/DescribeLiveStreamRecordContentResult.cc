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

#include <alibabacloud/live/model/DescribeLiveStreamRecordContentResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamRecordContentResult::DescribeLiveStreamRecordContentResult() :
	ServiceResult()
{}

DescribeLiveStreamRecordContentResult::DescribeLiveStreamRecordContentResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamRecordContentResult::~DescribeLiveStreamRecordContentResult()
{}

void DescribeLiveStreamRecordContentResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRecordContentInfoListNode = value["RecordContentInfoList"]["RecordContentInfo"];
	for (auto valueRecordContentInfoListRecordContentInfo : allRecordContentInfoListNode)
	{
		RecordContentInfo recordContentInfoListObject;
		if(!valueRecordContentInfoListRecordContentInfo["OssEndpoint"].isNull())
			recordContentInfoListObject.ossEndpoint = valueRecordContentInfoListRecordContentInfo["OssEndpoint"].asString();
		if(!valueRecordContentInfoListRecordContentInfo["OssBucket"].isNull())
			recordContentInfoListObject.ossBucket = valueRecordContentInfoListRecordContentInfo["OssBucket"].asString();
		if(!valueRecordContentInfoListRecordContentInfo["OssObjectPrefix"].isNull())
			recordContentInfoListObject.ossObjectPrefix = valueRecordContentInfoListRecordContentInfo["OssObjectPrefix"].asString();
		if(!valueRecordContentInfoListRecordContentInfo["StartTime"].isNull())
			recordContentInfoListObject.startTime = valueRecordContentInfoListRecordContentInfo["StartTime"].asString();
		if(!valueRecordContentInfoListRecordContentInfo["EndTime"].isNull())
			recordContentInfoListObject.endTime = valueRecordContentInfoListRecordContentInfo["EndTime"].asString();
		if(!valueRecordContentInfoListRecordContentInfo["Duration"].isNull())
			recordContentInfoListObject.duration = std::stof(valueRecordContentInfoListRecordContentInfo["Duration"].asString());
		recordContentInfoList_.push_back(recordContentInfoListObject);
	}

}

std::vector<DescribeLiveStreamRecordContentResult::RecordContentInfo> DescribeLiveStreamRecordContentResult::getRecordContentInfoList()const
{
	return recordContentInfoList_;
}


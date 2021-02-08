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

#include <alibabacloud/live/model/DescribeRecordsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeRecordsResult::DescribeRecordsResult() :
	ServiceResult()
{}

DescribeRecordsResult::DescribeRecordsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRecordsResult::~DescribeRecordsResult()
{}

void DescribeRecordsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRecordsNode = value["Records"]["Record"];
	for (auto valueRecordsRecord : allRecordsNode)
	{
		Record recordsObject;
		if(!valueRecordsRecord["RecordId"].isNull())
			recordsObject.recordId = valueRecordsRecord["RecordId"].asString();
		if(!valueRecordsRecord["AppId"].isNull())
			recordsObject.appId = valueRecordsRecord["AppId"].asString();
		if(!valueRecordsRecord["BoardId"].isNull())
			recordsObject.boardId = std::stoi(valueRecordsRecord["BoardId"].asString());
		if(!valueRecordsRecord["RecordStartTime"].isNull())
			recordsObject.recordStartTime = std::stol(valueRecordsRecord["RecordStartTime"].asString());
		if(!valueRecordsRecord["StartTime"].isNull())
			recordsObject.startTime = std::stol(valueRecordsRecord["StartTime"].asString());
		if(!valueRecordsRecord["EndTime"].isNull())
			recordsObject.endTime = std::stol(valueRecordsRecord["EndTime"].asString());
		if(!valueRecordsRecord["State"].isNull())
			recordsObject.state = std::stoi(valueRecordsRecord["State"].asString());
		if(!valueRecordsRecord["OssPath"].isNull())
			recordsObject.ossPath = valueRecordsRecord["OssPath"].asString();
		if(!valueRecordsRecord["OssBucket"].isNull())
			recordsObject.ossBucket = valueRecordsRecord["OssBucket"].asString();
		if(!valueRecordsRecord["OssEndpoint"].isNull())
			recordsObject.ossEndpoint = valueRecordsRecord["OssEndpoint"].asString();
		records_.push_back(recordsObject);
	}

}

std::vector<DescribeRecordsResult::Record> DescribeRecordsResult::getRecords()const
{
	return records_;
}


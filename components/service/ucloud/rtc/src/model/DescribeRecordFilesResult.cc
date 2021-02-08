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

#include <alibabacloud/rtc/model/DescribeRecordFilesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRecordFilesResult::DescribeRecordFilesResult() :
	ServiceResult()
{}

DescribeRecordFilesResult::DescribeRecordFilesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRecordFilesResult::~DescribeRecordFilesResult()
{}

void DescribeRecordFilesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRecordFilesNode = value["RecordFiles"]["RecordFile"];
	for (auto valueRecordFilesRecordFile : allRecordFilesNode)
	{
		RecordFile recordFilesObject;
		if(!valueRecordFilesRecordFile["CreateTime"].isNull())
			recordFilesObject.createTime = valueRecordFilesRecordFile["CreateTime"].asString();
		if(!valueRecordFilesRecordFile["AppId"].isNull())
			recordFilesObject.appId = valueRecordFilesRecordFile["AppId"].asString();
		if(!valueRecordFilesRecordFile["ChannelId"].isNull())
			recordFilesObject.channelId = valueRecordFilesRecordFile["ChannelId"].asString();
		if(!valueRecordFilesRecordFile["TaskId"].isNull())
			recordFilesObject.taskId = valueRecordFilesRecordFile["TaskId"].asString();
		if(!valueRecordFilesRecordFile["StartTime"].isNull())
			recordFilesObject.startTime = valueRecordFilesRecordFile["StartTime"].asString();
		if(!valueRecordFilesRecordFile["StopTime"].isNull())
			recordFilesObject.stopTime = valueRecordFilesRecordFile["StopTime"].asString();
		if(!valueRecordFilesRecordFile["Url"].isNull())
			recordFilesObject.url = valueRecordFilesRecordFile["Url"].asString();
		if(!valueRecordFilesRecordFile["Duration"].isNull())
			recordFilesObject.duration = std::stof(valueRecordFilesRecordFile["Duration"].asString());
		recordFiles_.push_back(recordFilesObject);
	}
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stol(value["TotalNum"].asString());
	if(!value["TotalPage"].isNull())
		totalPage_ = std::stol(value["TotalPage"].asString());

}

long DescribeRecordFilesResult::getTotalNum()const
{
	return totalNum_;
}

long DescribeRecordFilesResult::getTotalPage()const
{
	return totalPage_;
}

std::vector<DescribeRecordFilesResult::RecordFile> DescribeRecordFilesResult::getRecordFiles()const
{
	return recordFiles_;
}


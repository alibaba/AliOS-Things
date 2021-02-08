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

#include <alibabacloud/live/model/DescribeRecordResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeRecordResult::DescribeRecordResult() :
	ServiceResult()
{}

DescribeRecordResult::DescribeRecordResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRecordResult::~DescribeRecordResult()
{}

void DescribeRecordResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["RecordId"].isNull())
		recordId_ = value["RecordId"].asString();
	if(!value["AppId"].isNull())
		appId_ = value["AppId"].asString();
	if(!value["BoardId"].isNull())
		boardId_ = std::stoi(value["BoardId"].asString());
	if(!value["RecordStartTime"].isNull())
		recordStartTime_ = std::stol(value["RecordStartTime"].asString());
	if(!value["StartTime"].isNull())
		startTime_ = std::stol(value["StartTime"].asString());
	if(!value["EndTime"].isNull())
		endTime_ = std::stol(value["EndTime"].asString());
	if(!value["State"].isNull())
		state_ = std::stoi(value["State"].asString());
	if(!value["OssPath"].isNull())
		ossPath_ = value["OssPath"].asString();
	if(!value["OssBucket"].isNull())
		ossBucket_ = value["OssBucket"].asString();
	if(!value["OssEndpoint"].isNull())
		ossEndpoint_ = value["OssEndpoint"].asString();

}

int DescribeRecordResult::getBoardId()const
{
	return boardId_;
}

std::string DescribeRecordResult::getAppId()const
{
	return appId_;
}

long DescribeRecordResult::getRecordStartTime()const
{
	return recordStartTime_;
}

long DescribeRecordResult::getEndTime()const
{
	return endTime_;
}

std::string DescribeRecordResult::getOssEndpoint()const
{
	return ossEndpoint_;
}

int DescribeRecordResult::getState()const
{
	return state_;
}

std::string DescribeRecordResult::getOssPath()const
{
	return ossPath_;
}

long DescribeRecordResult::getStartTime()const
{
	return startTime_;
}

std::string DescribeRecordResult::getOssBucket()const
{
	return ossBucket_;
}

std::string DescribeRecordResult::getRecordId()const
{
	return recordId_;
}


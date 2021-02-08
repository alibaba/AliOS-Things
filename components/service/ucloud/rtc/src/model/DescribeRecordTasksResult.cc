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

#include <alibabacloud/rtc/model/DescribeRecordTasksResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRecordTasksResult::DescribeRecordTasksResult() :
	ServiceResult()
{}

DescribeRecordTasksResult::DescribeRecordTasksResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRecordTasksResult::~DescribeRecordTasksResult()
{}

void DescribeRecordTasksResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRecordTasksNode = value["RecordTasks"]["RecordTask"];
	for (auto valueRecordTasksRecordTask : allRecordTasksNode)
	{
		RecordTask recordTasksObject;
		if(!valueRecordTasksRecordTask["CreateTime"].isNull())
			recordTasksObject.createTime = valueRecordTasksRecordTask["CreateTime"].asString();
		if(!valueRecordTasksRecordTask["AppId"].isNull())
			recordTasksObject.appId = valueRecordTasksRecordTask["AppId"].asString();
		if(!valueRecordTasksRecordTask["ChannelId"].isNull())
			recordTasksObject.channelId = valueRecordTasksRecordTask["ChannelId"].asString();
		if(!valueRecordTasksRecordTask["TaskId"].isNull())
			recordTasksObject.taskId = valueRecordTasksRecordTask["TaskId"].asString();
		if(!valueRecordTasksRecordTask["TemplateId"].isNull())
			recordTasksObject.templateId = valueRecordTasksRecordTask["TemplateId"].asString();
		if(!valueRecordTasksRecordTask["Status"].isNull())
			recordTasksObject.status = std::stoi(valueRecordTasksRecordTask["Status"].asString());
		auto allUserPanesNode = valueRecordTasksRecordTask["UserPanes"]["UserPanesItem"];
		for (auto valueRecordTasksRecordTaskUserPanesUserPanesItem : allUserPanesNode)
		{
			RecordTask::UserPanesItem userPanesObject;
			if(!valueRecordTasksRecordTaskUserPanesUserPanesItem["PaneId"].isNull())
				userPanesObject.paneId = std::stoi(valueRecordTasksRecordTaskUserPanesUserPanesItem["PaneId"].asString());
			if(!valueRecordTasksRecordTaskUserPanesUserPanesItem["UserId"].isNull())
				userPanesObject.userId = valueRecordTasksRecordTaskUserPanesUserPanesItem["UserId"].asString();
			if(!valueRecordTasksRecordTaskUserPanesUserPanesItem["Source"].isNull())
				userPanesObject.source = valueRecordTasksRecordTaskUserPanesUserPanesItem["Source"].asString();
			recordTasksObject.userPanes.push_back(userPanesObject);
		}
		auto allSubSpecUsers = value["SubSpecUsers"]["SubSpecUser"];
		for (auto value : allSubSpecUsers)
			recordTasksObject.subSpecUsers.push_back(value.asString());
		recordTasks_.push_back(recordTasksObject);
	}
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stol(value["TotalNum"].asString());
	if(!value["TotalPage"].isNull())
		totalPage_ = std::stol(value["TotalPage"].asString());

}

std::vector<DescribeRecordTasksResult::RecordTask> DescribeRecordTasksResult::getRecordTasks()const
{
	return recordTasks_;
}

long DescribeRecordTasksResult::getTotalNum()const
{
	return totalNum_;
}

long DescribeRecordTasksResult::getTotalPage()const
{
	return totalPage_;
}


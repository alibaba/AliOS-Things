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

#include <alibabacloud/imm/model/ListOfficeConversionTaskResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

ListOfficeConversionTaskResult::ListOfficeConversionTaskResult() :
	ServiceResult()
{}

ListOfficeConversionTaskResult::ListOfficeConversionTaskResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListOfficeConversionTaskResult::~ListOfficeConversionTaskResult()
{}

void ListOfficeConversionTaskResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allTasksNode = value["Tasks"]["TasksItem"];
	for (auto valueTasksTasksItem : allTasksNode)
	{
		TasksItem tasksObject;
		if(!valueTasksTasksItem["TaskId"].isNull())
			tasksObject.taskId = valueTasksTasksItem["TaskId"].asString();
		if(!valueTasksTasksItem["Status"].isNull())
			tasksObject.status = valueTasksTasksItem["Status"].asString();
		if(!valueTasksTasksItem["Percent"].isNull())
			tasksObject.percent = std::stoi(valueTasksTasksItem["Percent"].asString());
		if(!valueTasksTasksItem["PageCount"].isNull())
			tasksObject.pageCount = std::stoi(valueTasksTasksItem["PageCount"].asString());
		if(!valueTasksTasksItem["SrcUri"].isNull())
			tasksObject.srcUri = valueTasksTasksItem["SrcUri"].asString();
		if(!valueTasksTasksItem["TgtType"].isNull())
			tasksObject.tgtType = valueTasksTasksItem["TgtType"].asString();
		if(!valueTasksTasksItem["TgtUri"].isNull())
			tasksObject.tgtUri = valueTasksTasksItem["TgtUri"].asString();
		if(!valueTasksTasksItem["ImageSpec"].isNull())
			tasksObject.imageSpec = valueTasksTasksItem["ImageSpec"].asString();
		if(!valueTasksTasksItem["NotifyTopicName"].isNull())
			tasksObject.notifyTopicName = valueTasksTasksItem["NotifyTopicName"].asString();
		if(!valueTasksTasksItem["NotifyEndpoint"].isNull())
			tasksObject.notifyEndpoint = valueTasksTasksItem["NotifyEndpoint"].asString();
		if(!valueTasksTasksItem["ExternalID"].isNull())
			tasksObject.externalID = valueTasksTasksItem["ExternalID"].asString();
		if(!valueTasksTasksItem["CreateTime"].isNull())
			tasksObject.createTime = valueTasksTasksItem["CreateTime"].asString();
		if(!valueTasksTasksItem["FinishTime"].isNull())
			tasksObject.finishTime = valueTasksTasksItem["FinishTime"].asString();
		tasks_.push_back(tasksObject);
	}
	if(!value["NextMarker"].isNull())
		nextMarker_ = value["NextMarker"].asString();

}

std::vector<ListOfficeConversionTaskResult::TasksItem> ListOfficeConversionTaskResult::getTasks()const
{
	return tasks_;
}

std::string ListOfficeConversionTaskResult::getNextMarker()const
{
	return nextMarker_;
}


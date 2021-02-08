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

#include <alibabacloud/imm/model/ListVideoTasksResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

ListVideoTasksResult::ListVideoTasksResult() :
	ServiceResult()
{}

ListVideoTasksResult::ListVideoTasksResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListVideoTasksResult::~ListVideoTasksResult()
{}

void ListVideoTasksResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allTasksNode = value["Tasks"]["TasksItem"];
	for (auto valueTasksTasksItem : allTasksNode)
	{
		TasksItem tasksObject;
		if(!valueTasksTasksItem["Status"].isNull())
			tasksObject.status = valueTasksTasksItem["Status"].asString();
		if(!valueTasksTasksItem["TaskId"].isNull())
			tasksObject.taskId = valueTasksTasksItem["TaskId"].asString();
		if(!valueTasksTasksItem["TaskType"].isNull())
			tasksObject.taskType = valueTasksTasksItem["TaskType"].asString();
		if(!valueTasksTasksItem["Parameters"].isNull())
			tasksObject.parameters = valueTasksTasksItem["Parameters"].asString();
		if(!valueTasksTasksItem["Result"].isNull())
			tasksObject.result = valueTasksTasksItem["Result"].asString();
		if(!valueTasksTasksItem["StartTime"].isNull())
			tasksObject.startTime = valueTasksTasksItem["StartTime"].asString();
		if(!valueTasksTasksItem["EndTime"].isNull())
			tasksObject.endTime = valueTasksTasksItem["EndTime"].asString();
		if(!valueTasksTasksItem["ErrorMessage"].isNull())
			tasksObject.errorMessage = valueTasksTasksItem["ErrorMessage"].asString();
		if(!valueTasksTasksItem["NotifyEndpoint"].isNull())
			tasksObject.notifyEndpoint = valueTasksTasksItem["NotifyEndpoint"].asString();
		if(!valueTasksTasksItem["NotifyTopicName"].isNull())
			tasksObject.notifyTopicName = valueTasksTasksItem["NotifyTopicName"].asString();
		if(!valueTasksTasksItem["Progress"].isNull())
			tasksObject.progress = std::stoi(valueTasksTasksItem["Progress"].asString());
		tasks_.push_back(tasksObject);
	}
	if(!value["NextMarker"].isNull())
		nextMarker_ = value["NextMarker"].asString();

}

std::vector<ListVideoTasksResult::TasksItem> ListVideoTasksResult::getTasks()const
{
	return tasks_;
}

std::string ListVideoTasksResult::getNextMarker()const
{
	return nextMarker_;
}


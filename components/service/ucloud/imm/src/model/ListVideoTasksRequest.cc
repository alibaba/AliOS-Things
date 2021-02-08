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

#include <alibabacloud/imm/model/ListVideoTasksRequest.h>

using AlibabaCloud::Imm::Model::ListVideoTasksRequest;

ListVideoTasksRequest::ListVideoTasksRequest() :
	RpcServiceRequest("imm", "2017-09-06", "ListVideoTasks")
{
	setMethod(HttpRequest::Method::Post);
}

ListVideoTasksRequest::~ListVideoTasksRequest()
{}

int ListVideoTasksRequest::getMaxKeys()const
{
	return maxKeys_;
}

void ListVideoTasksRequest::setMaxKeys(int maxKeys)
{
	maxKeys_ = maxKeys;
	setParameter("MaxKeys", std::to_string(maxKeys));
}

std::string ListVideoTasksRequest::getProject()const
{
	return project_;
}

void ListVideoTasksRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string ListVideoTasksRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ListVideoTasksRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ListVideoTasksRequest::getTaskType()const
{
	return taskType_;
}

void ListVideoTasksRequest::setTaskType(const std::string& taskType)
{
	taskType_ = taskType;
	setParameter("TaskType", taskType);
}

std::string ListVideoTasksRequest::getMarker()const
{
	return marker_;
}

void ListVideoTasksRequest::setMarker(const std::string& marker)
{
	marker_ = marker;
	setParameter("Marker", marker);
}


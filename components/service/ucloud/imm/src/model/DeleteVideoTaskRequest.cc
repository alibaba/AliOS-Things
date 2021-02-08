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

#include <alibabacloud/imm/model/DeleteVideoTaskRequest.h>

using AlibabaCloud::Imm::Model::DeleteVideoTaskRequest;

DeleteVideoTaskRequest::DeleteVideoTaskRequest() :
	RpcServiceRequest("imm", "2017-09-06", "DeleteVideoTask")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteVideoTaskRequest::~DeleteVideoTaskRequest()
{}

std::string DeleteVideoTaskRequest::getProject()const
{
	return project_;
}

void DeleteVideoTaskRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string DeleteVideoTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteVideoTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteVideoTaskRequest::getTaskId()const
{
	return taskId_;
}

void DeleteVideoTaskRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

std::string DeleteVideoTaskRequest::getTaskType()const
{
	return taskType_;
}

void DeleteVideoTaskRequest::setTaskType(const std::string& taskType)
{
	taskType_ = taskType;
	setParameter("TaskType", taskType);
}


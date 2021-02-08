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

#include <alibabacloud/imm/model/GetVideoTaskRequest.h>

using AlibabaCloud::Imm::Model::GetVideoTaskRequest;

GetVideoTaskRequest::GetVideoTaskRequest() :
	RpcServiceRequest("imm", "2017-09-06", "GetVideoTask")
{
	setMethod(HttpRequest::Method::Post);
}

GetVideoTaskRequest::~GetVideoTaskRequest()
{}

std::string GetVideoTaskRequest::getProject()const
{
	return project_;
}

void GetVideoTaskRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string GetVideoTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetVideoTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetVideoTaskRequest::getTaskId()const
{
	return taskId_;
}

void GetVideoTaskRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

std::string GetVideoTaskRequest::getTaskType()const
{
	return taskType_;
}

void GetVideoTaskRequest::setTaskType(const std::string& taskType)
{
	taskType_ = taskType;
	setParameter("TaskType", taskType);
}


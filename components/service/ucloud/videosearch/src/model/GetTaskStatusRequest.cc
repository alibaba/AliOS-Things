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

#include <alibabacloud/videosearch/model/GetTaskStatusRequest.h>

using AlibabaCloud::Videosearch::Model::GetTaskStatusRequest;

GetTaskStatusRequest::GetTaskStatusRequest() :
	RpcServiceRequest("videosearch", "2020-02-25", "GetTaskStatus")
{
	setMethod(HttpRequest::Method::Post);
}

GetTaskStatusRequest::~GetTaskStatusRequest()
{}

std::string GetTaskStatusRequest::getClientToken()const
{
	return clientToken_;
}

void GetTaskStatusRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setParameter("ClientToken", clientToken);
}

std::string GetTaskStatusRequest::getInstanceId()const
{
	return instanceId_;
}

void GetTaskStatusRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setBodyParameter("InstanceId", instanceId);
}

std::string GetTaskStatusRequest::getTaskId()const
{
	return taskId_;
}

void GetTaskStatusRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setBodyParameter("TaskId", taskId);
}


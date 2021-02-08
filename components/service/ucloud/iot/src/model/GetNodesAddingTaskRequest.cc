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

#include <alibabacloud/iot/model/GetNodesAddingTaskRequest.h>

using AlibabaCloud::Iot::Model::GetNodesAddingTaskRequest;

GetNodesAddingTaskRequest::GetNodesAddingTaskRequest() :
	RpcServiceRequest("iot", "2018-01-20", "GetNodesAddingTask")
{
	setMethod(HttpRequest::Method::Post);
}

GetNodesAddingTaskRequest::~GetNodesAddingTaskRequest()
{}

std::string GetNodesAddingTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetNodesAddingTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetNodesAddingTaskRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void GetNodesAddingTaskRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string GetNodesAddingTaskRequest::getTaskId()const
{
	return taskId_;
}

void GetNodesAddingTaskRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

std::string GetNodesAddingTaskRequest::getApiProduct()const
{
	return apiProduct_;
}

void GetNodesAddingTaskRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string GetNodesAddingTaskRequest::getApiRevision()const
{
	return apiRevision_;
}

void GetNodesAddingTaskRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


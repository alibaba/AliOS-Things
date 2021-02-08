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

#include <alibabacloud/iot/model/GetLoraNodesTaskRequest.h>

using AlibabaCloud::Iot::Model::GetLoraNodesTaskRequest;

GetLoraNodesTaskRequest::GetLoraNodesTaskRequest() :
	RpcServiceRequest("iot", "2018-01-20", "GetLoraNodesTask")
{
	setMethod(HttpRequest::Method::Post);
}

GetLoraNodesTaskRequest::~GetLoraNodesTaskRequest()
{}

std::string GetLoraNodesTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetLoraNodesTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetLoraNodesTaskRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void GetLoraNodesTaskRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string GetLoraNodesTaskRequest::getTaskId()const
{
	return taskId_;
}

void GetLoraNodesTaskRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

std::string GetLoraNodesTaskRequest::getApiProduct()const
{
	return apiProduct_;
}

void GetLoraNodesTaskRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string GetLoraNodesTaskRequest::getApiRevision()const
{
	return apiRevision_;
}

void GetLoraNodesTaskRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


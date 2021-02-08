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

#include <alibabacloud/iot/model/UpdateConsumerGroupRequest.h>

using AlibabaCloud::Iot::Model::UpdateConsumerGroupRequest;

UpdateConsumerGroupRequest::UpdateConsumerGroupRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateConsumerGroup")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateConsumerGroupRequest::~UpdateConsumerGroupRequest()
{}

std::string UpdateConsumerGroupRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateConsumerGroupRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UpdateConsumerGroupRequest::getGroupId()const
{
	return groupId_;
}

void UpdateConsumerGroupRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string UpdateConsumerGroupRequest::getNewGroupName()const
{
	return newGroupName_;
}

void UpdateConsumerGroupRequest::setNewGroupName(const std::string& newGroupName)
{
	newGroupName_ = newGroupName;
	setParameter("NewGroupName", newGroupName);
}

std::string UpdateConsumerGroupRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateConsumerGroupRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateConsumerGroupRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateConsumerGroupRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


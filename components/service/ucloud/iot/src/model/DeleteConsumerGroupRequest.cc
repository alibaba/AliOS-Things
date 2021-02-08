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

#include <alibabacloud/iot/model/DeleteConsumerGroupRequest.h>

using AlibabaCloud::Iot::Model::DeleteConsumerGroupRequest;

DeleteConsumerGroupRequest::DeleteConsumerGroupRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteConsumerGroup")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteConsumerGroupRequest::~DeleteConsumerGroupRequest()
{}

std::string DeleteConsumerGroupRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteConsumerGroupRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DeleteConsumerGroupRequest::getGroupId()const
{
	return groupId_;
}

void DeleteConsumerGroupRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DeleteConsumerGroupRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteConsumerGroupRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteConsumerGroupRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteConsumerGroupRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


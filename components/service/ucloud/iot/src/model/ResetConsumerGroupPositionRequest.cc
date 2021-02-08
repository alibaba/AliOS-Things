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

#include <alibabacloud/iot/model/ResetConsumerGroupPositionRequest.h>

using AlibabaCloud::Iot::Model::ResetConsumerGroupPositionRequest;

ResetConsumerGroupPositionRequest::ResetConsumerGroupPositionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ResetConsumerGroupPosition")
{
	setMethod(HttpRequest::Method::Post);
}

ResetConsumerGroupPositionRequest::~ResetConsumerGroupPositionRequest()
{}

std::string ResetConsumerGroupPositionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ResetConsumerGroupPositionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string ResetConsumerGroupPositionRequest::getGroupId()const
{
	return groupId_;
}

void ResetConsumerGroupPositionRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string ResetConsumerGroupPositionRequest::getApiProduct()const
{
	return apiProduct_;
}

void ResetConsumerGroupPositionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ResetConsumerGroupPositionRequest::getApiRevision()const
{
	return apiRevision_;
}

void ResetConsumerGroupPositionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


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

#include <alibabacloud/iot/model/CreateConsumerGroupSubscribeRelationRequest.h>

using AlibabaCloud::Iot::Model::CreateConsumerGroupSubscribeRelationRequest;

CreateConsumerGroupSubscribeRelationRequest::CreateConsumerGroupSubscribeRelationRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateConsumerGroupSubscribeRelation")
{
	setMethod(HttpRequest::Method::Post);
}

CreateConsumerGroupSubscribeRelationRequest::~CreateConsumerGroupSubscribeRelationRequest()
{}

std::string CreateConsumerGroupSubscribeRelationRequest::getConsumerGroupId()const
{
	return consumerGroupId_;
}

void CreateConsumerGroupSubscribeRelationRequest::setConsumerGroupId(const std::string& consumerGroupId)
{
	consumerGroupId_ = consumerGroupId;
	setParameter("ConsumerGroupId", consumerGroupId);
}

std::string CreateConsumerGroupSubscribeRelationRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateConsumerGroupSubscribeRelationRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateConsumerGroupSubscribeRelationRequest::getProductKey()const
{
	return productKey_;
}

void CreateConsumerGroupSubscribeRelationRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string CreateConsumerGroupSubscribeRelationRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateConsumerGroupSubscribeRelationRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateConsumerGroupSubscribeRelationRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateConsumerGroupSubscribeRelationRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


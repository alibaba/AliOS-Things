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

#include <alibabacloud/iot/model/DeleteConsumerGroupSubscribeRelationRequest.h>

using AlibabaCloud::Iot::Model::DeleteConsumerGroupSubscribeRelationRequest;

DeleteConsumerGroupSubscribeRelationRequest::DeleteConsumerGroupSubscribeRelationRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteConsumerGroupSubscribeRelation")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteConsumerGroupSubscribeRelationRequest::~DeleteConsumerGroupSubscribeRelationRequest()
{}

std::string DeleteConsumerGroupSubscribeRelationRequest::getConsumerGroupId()const
{
	return consumerGroupId_;
}

void DeleteConsumerGroupSubscribeRelationRequest::setConsumerGroupId(const std::string& consumerGroupId)
{
	consumerGroupId_ = consumerGroupId;
	setParameter("ConsumerGroupId", consumerGroupId);
}

std::string DeleteConsumerGroupSubscribeRelationRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteConsumerGroupSubscribeRelationRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DeleteConsumerGroupSubscribeRelationRequest::getProductKey()const
{
	return productKey_;
}

void DeleteConsumerGroupSubscribeRelationRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string DeleteConsumerGroupSubscribeRelationRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteConsumerGroupSubscribeRelationRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteConsumerGroupSubscribeRelationRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteConsumerGroupSubscribeRelationRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


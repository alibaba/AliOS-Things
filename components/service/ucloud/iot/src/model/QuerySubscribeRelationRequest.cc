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

#include <alibabacloud/iot/model/QuerySubscribeRelationRequest.h>

using AlibabaCloud::Iot::Model::QuerySubscribeRelationRequest;

QuerySubscribeRelationRequest::QuerySubscribeRelationRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QuerySubscribeRelation")
{
	setMethod(HttpRequest::Method::Post);
}

QuerySubscribeRelationRequest::~QuerySubscribeRelationRequest()
{}

std::string QuerySubscribeRelationRequest::getType()const
{
	return type_;
}

void QuerySubscribeRelationRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

std::string QuerySubscribeRelationRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QuerySubscribeRelationRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QuerySubscribeRelationRequest::getProductKey()const
{
	return productKey_;
}

void QuerySubscribeRelationRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string QuerySubscribeRelationRequest::getApiProduct()const
{
	return apiProduct_;
}

void QuerySubscribeRelationRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QuerySubscribeRelationRequest::getApiRevision()const
{
	return apiRevision_;
}

void QuerySubscribeRelationRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


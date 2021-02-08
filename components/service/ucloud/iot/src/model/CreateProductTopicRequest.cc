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

#include <alibabacloud/iot/model/CreateProductTopicRequest.h>

using AlibabaCloud::Iot::Model::CreateProductTopicRequest;

CreateProductTopicRequest::CreateProductTopicRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateProductTopic")
{
	setMethod(HttpRequest::Method::Post);
}

CreateProductTopicRequest::~CreateProductTopicRequest()
{}

std::string CreateProductTopicRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateProductTopicRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateProductTopicRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateProductTopicRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateProductTopicRequest::getTopicShortName()const
{
	return topicShortName_;
}

void CreateProductTopicRequest::setTopicShortName(const std::string& topicShortName)
{
	topicShortName_ = topicShortName;
	setParameter("TopicShortName", topicShortName);
}

std::string CreateProductTopicRequest::getProductKey()const
{
	return productKey_;
}

void CreateProductTopicRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string CreateProductTopicRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateProductTopicRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateProductTopicRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateProductTopicRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string CreateProductTopicRequest::getOperation()const
{
	return operation_;
}

void CreateProductTopicRequest::setOperation(const std::string& operation)
{
	operation_ = operation;
	setParameter("Operation", operation);
}

std::string CreateProductTopicRequest::getDesc()const
{
	return desc_;
}

void CreateProductTopicRequest::setDesc(const std::string& desc)
{
	desc_ = desc;
	setParameter("Desc", desc);
}


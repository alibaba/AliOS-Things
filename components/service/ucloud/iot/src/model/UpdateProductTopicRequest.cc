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

#include <alibabacloud/iot/model/UpdateProductTopicRequest.h>

using AlibabaCloud::Iot::Model::UpdateProductTopicRequest;

UpdateProductTopicRequest::UpdateProductTopicRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateProductTopic")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateProductTopicRequest::~UpdateProductTopicRequest()
{}

std::string UpdateProductTopicRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UpdateProductTopicRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UpdateProductTopicRequest::getTopicId()const
{
	return topicId_;
}

void UpdateProductTopicRequest::setTopicId(const std::string& topicId)
{
	topicId_ = topicId;
	setParameter("TopicId", topicId);
}

std::string UpdateProductTopicRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateProductTopicRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UpdateProductTopicRequest::getTopicShortName()const
{
	return topicShortName_;
}

void UpdateProductTopicRequest::setTopicShortName(const std::string& topicShortName)
{
	topicShortName_ = topicShortName;
	setParameter("TopicShortName", topicShortName);
}

std::string UpdateProductTopicRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateProductTopicRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateProductTopicRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateProductTopicRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string UpdateProductTopicRequest::getOperation()const
{
	return operation_;
}

void UpdateProductTopicRequest::setOperation(const std::string& operation)
{
	operation_ = operation;
	setParameter("Operation", operation);
}

std::string UpdateProductTopicRequest::getDesc()const
{
	return desc_;
}

void UpdateProductTopicRequest::setDesc(const std::string& desc)
{
	desc_ = desc;
	setParameter("Desc", desc);
}


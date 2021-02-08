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

#include <alibabacloud/iot/model/PubBroadcastRequest.h>

using AlibabaCloud::Iot::Model::PubBroadcastRequest;

PubBroadcastRequest::PubBroadcastRequest() :
	RpcServiceRequest("iot", "2018-01-20", "PubBroadcast")
{
	setMethod(HttpRequest::Method::Post);
}

PubBroadcastRequest::~PubBroadcastRequest()
{}

std::string PubBroadcastRequest::getMessageContent()const
{
	return messageContent_;
}

void PubBroadcastRequest::setMessageContent(const std::string& messageContent)
{
	messageContent_ = messageContent;
	setParameter("MessageContent", messageContent);
}

std::string PubBroadcastRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void PubBroadcastRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string PubBroadcastRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void PubBroadcastRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string PubBroadcastRequest::getTopicFullName()const
{
	return topicFullName_;
}

void PubBroadcastRequest::setTopicFullName(const std::string& topicFullName)
{
	topicFullName_ = topicFullName;
	setParameter("TopicFullName", topicFullName);
}

std::string PubBroadcastRequest::getProductKey()const
{
	return productKey_;
}

void PubBroadcastRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string PubBroadcastRequest::getApiProduct()const
{
	return apiProduct_;
}

void PubBroadcastRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string PubBroadcastRequest::getApiRevision()const
{
	return apiRevision_;
}

void PubBroadcastRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


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

#include <alibabacloud/iot/model/PubRequest.h>

using AlibabaCloud::Iot::Model::PubRequest;

PubRequest::PubRequest() :
	RpcServiceRequest("iot", "2018-01-20", "Pub")
{
	setMethod(HttpRequest::Method::Post);
}

PubRequest::~PubRequest()
{}

std::string PubRequest::getMessageContent()const
{
	return messageContent_;
}

void PubRequest::setMessageContent(const std::string& messageContent)
{
	messageContent_ = messageContent;
	setParameter("MessageContent", messageContent);
}

std::string PubRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void PubRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

int PubRequest::getQos()const
{
	return qos_;
}

void PubRequest::setQos(int qos)
{
	qos_ = qos;
	setParameter("Qos", std::to_string(qos));
}

std::string PubRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void PubRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string PubRequest::getTopicFullName()const
{
	return topicFullName_;
}

void PubRequest::setTopicFullName(const std::string& topicFullName)
{
	topicFullName_ = topicFullName;
	setParameter("TopicFullName", topicFullName);
}

std::string PubRequest::getProductKey()const
{
	return productKey_;
}

void PubRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string PubRequest::getApiProduct()const
{
	return apiProduct_;
}

void PubRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string PubRequest::getApiRevision()const
{
	return apiRevision_;
}

void PubRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


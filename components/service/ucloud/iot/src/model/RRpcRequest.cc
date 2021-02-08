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

#include <alibabacloud/iot/model/RRpcRequest.h>

using AlibabaCloud::Iot::Model::RRpcRequest;

RRpcRequest::RRpcRequest() :
	RpcServiceRequest("iot", "2018-01-20", "RRpc")
{
	setMethod(HttpRequest::Method::Post);
}

RRpcRequest::~RRpcRequest()
{}

int RRpcRequest::getTimeout()const
{
	return timeout_;
}

void RRpcRequest::setTimeout(int timeout)
{
	timeout_ = timeout;
	setParameter("Timeout", std::to_string(timeout));
}

std::string RRpcRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void RRpcRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string RRpcRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void RRpcRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string RRpcRequest::getRequestBase64Byte()const
{
	return requestBase64Byte_;
}

void RRpcRequest::setRequestBase64Byte(const std::string& requestBase64Byte)
{
	requestBase64Byte_ = requestBase64Byte;
	setParameter("RequestBase64Byte", requestBase64Byte);
}

std::string RRpcRequest::getProductKey()const
{
	return productKey_;
}

void RRpcRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string RRpcRequest::getApiProduct()const
{
	return apiProduct_;
}

void RRpcRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string RRpcRequest::getTopic()const
{
	return topic_;
}

void RRpcRequest::setTopic(const std::string& topic)
{
	topic_ = topic;
	setParameter("Topic", topic);
}

std::string RRpcRequest::getApiRevision()const
{
	return apiRevision_;
}

void RRpcRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string RRpcRequest::getDeviceName()const
{
	return deviceName_;
}

void RRpcRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}


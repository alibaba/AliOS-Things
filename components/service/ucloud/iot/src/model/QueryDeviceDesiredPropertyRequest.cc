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

#include <alibabacloud/iot/model/QueryDeviceDesiredPropertyRequest.h>

using AlibabaCloud::Iot::Model::QueryDeviceDesiredPropertyRequest;

QueryDeviceDesiredPropertyRequest::QueryDeviceDesiredPropertyRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDeviceDesiredProperty")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDeviceDesiredPropertyRequest::~QueryDeviceDesiredPropertyRequest()
{}

std::string QueryDeviceDesiredPropertyRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDeviceDesiredPropertyRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDeviceDesiredPropertyRequest::getIotId()const
{
	return iotId_;
}

void QueryDeviceDesiredPropertyRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string QueryDeviceDesiredPropertyRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDeviceDesiredPropertyRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::vector<std::string> QueryDeviceDesiredPropertyRequest::getIdentifier()const
{
	return identifier_;
}

void QueryDeviceDesiredPropertyRequest::setIdentifier(const std::vector<std::string>& identifier)
{
	identifier_ = identifier;
	for(int dep1 = 0; dep1!= identifier.size(); dep1++) {
		setParameter("Identifier."+ std::to_string(dep1), identifier.at(dep1));
	}
}

std::string QueryDeviceDesiredPropertyRequest::getProductKey()const
{
	return productKey_;
}

void QueryDeviceDesiredPropertyRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string QueryDeviceDesiredPropertyRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDeviceDesiredPropertyRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDeviceDesiredPropertyRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDeviceDesiredPropertyRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryDeviceDesiredPropertyRequest::getDeviceName()const
{
	return deviceName_;
}

void QueryDeviceDesiredPropertyRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}


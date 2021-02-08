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

#include <alibabacloud/iot/model/QueryDeviceCertRequest.h>

using AlibabaCloud::Iot::Model::QueryDeviceCertRequest;

QueryDeviceCertRequest::QueryDeviceCertRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDeviceCert")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDeviceCertRequest::~QueryDeviceCertRequest()
{}

std::string QueryDeviceCertRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDeviceCertRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDeviceCertRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDeviceCertRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryDeviceCertRequest::getProductKey()const
{
	return productKey_;
}

void QueryDeviceCertRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string QueryDeviceCertRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDeviceCertRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDeviceCertRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDeviceCertRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryDeviceCertRequest::getDeviceName()const
{
	return deviceName_;
}

void QueryDeviceCertRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}


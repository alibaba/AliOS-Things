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

#include <alibabacloud/iot/model/QueryDeviceServiceDataRequest.h>

using AlibabaCloud::Iot::Model::QueryDeviceServiceDataRequest;

QueryDeviceServiceDataRequest::QueryDeviceServiceDataRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDeviceServiceData")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDeviceServiceDataRequest::~QueryDeviceServiceDataRequest()
{}

long QueryDeviceServiceDataRequest::getStartTime()const
{
	return startTime_;
}

void QueryDeviceServiceDataRequest::setStartTime(long startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", std::to_string(startTime));
}

std::string QueryDeviceServiceDataRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDeviceServiceDataRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDeviceServiceDataRequest::getIotId()const
{
	return iotId_;
}

void QueryDeviceServiceDataRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string QueryDeviceServiceDataRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDeviceServiceDataRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryDeviceServiceDataRequest::getPageSize()const
{
	return pageSize_;
}

void QueryDeviceServiceDataRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string QueryDeviceServiceDataRequest::getIdentifier()const
{
	return identifier_;
}

void QueryDeviceServiceDataRequest::setIdentifier(const std::string& identifier)
{
	identifier_ = identifier;
	setParameter("Identifier", identifier);
}

long QueryDeviceServiceDataRequest::getEndTime()const
{
	return endTime_;
}

void QueryDeviceServiceDataRequest::setEndTime(long endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", std::to_string(endTime));
}

std::string QueryDeviceServiceDataRequest::getProductKey()const
{
	return productKey_;
}

void QueryDeviceServiceDataRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

int QueryDeviceServiceDataRequest::getAsc()const
{
	return asc_;
}

void QueryDeviceServiceDataRequest::setAsc(int asc)
{
	asc_ = asc;
	setParameter("Asc", std::to_string(asc));
}

std::string QueryDeviceServiceDataRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDeviceServiceDataRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDeviceServiceDataRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDeviceServiceDataRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryDeviceServiceDataRequest::getDeviceName()const
{
	return deviceName_;
}

void QueryDeviceServiceDataRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}


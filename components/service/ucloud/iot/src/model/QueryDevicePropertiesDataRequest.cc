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

#include <alibabacloud/iot/model/QueryDevicePropertiesDataRequest.h>

using AlibabaCloud::Iot::Model::QueryDevicePropertiesDataRequest;

QueryDevicePropertiesDataRequest::QueryDevicePropertiesDataRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDevicePropertiesData")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDevicePropertiesDataRequest::~QueryDevicePropertiesDataRequest()
{}

long QueryDevicePropertiesDataRequest::getStartTime()const
{
	return startTime_;
}

void QueryDevicePropertiesDataRequest::setStartTime(long startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", std::to_string(startTime));
}

std::string QueryDevicePropertiesDataRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDevicePropertiesDataRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDevicePropertiesDataRequest::getIotId()const
{
	return iotId_;
}

void QueryDevicePropertiesDataRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string QueryDevicePropertiesDataRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDevicePropertiesDataRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryDevicePropertiesDataRequest::getPageSize()const
{
	return pageSize_;
}

void QueryDevicePropertiesDataRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::vector<std::string> QueryDevicePropertiesDataRequest::getIdentifier()const
{
	return identifier_;
}

void QueryDevicePropertiesDataRequest::setIdentifier(const std::vector<std::string>& identifier)
{
	identifier_ = identifier;
	for(int dep1 = 0; dep1!= identifier.size(); dep1++) {
		setParameter("Identifier."+ std::to_string(dep1), identifier.at(dep1));
	}
}

long QueryDevicePropertiesDataRequest::getEndTime()const
{
	return endTime_;
}

void QueryDevicePropertiesDataRequest::setEndTime(long endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", std::to_string(endTime));
}

std::string QueryDevicePropertiesDataRequest::getProductKey()const
{
	return productKey_;
}

void QueryDevicePropertiesDataRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

int QueryDevicePropertiesDataRequest::getAsc()const
{
	return asc_;
}

void QueryDevicePropertiesDataRequest::setAsc(int asc)
{
	asc_ = asc;
	setParameter("Asc", std::to_string(asc));
}

std::string QueryDevicePropertiesDataRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDevicePropertiesDataRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDevicePropertiesDataRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDevicePropertiesDataRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryDevicePropertiesDataRequest::getDeviceName()const
{
	return deviceName_;
}

void QueryDevicePropertiesDataRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}


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

#include <alibabacloud/iot/model/GetThingTopoRequest.h>

using AlibabaCloud::Iot::Model::GetThingTopoRequest;

GetThingTopoRequest::GetThingTopoRequest() :
	RpcServiceRequest("iot", "2018-01-20", "GetThingTopo")
{
	setMethod(HttpRequest::Method::Post);
}

GetThingTopoRequest::~GetThingTopoRequest()
{}

std::string GetThingTopoRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetThingTopoRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetThingTopoRequest::getIotId()const
{
	return iotId_;
}

void GetThingTopoRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string GetThingTopoRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void GetThingTopoRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int GetThingTopoRequest::getPageSize()const
{
	return pageSize_;
}

void GetThingTopoRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string GetThingTopoRequest::getProductKey()const
{
	return productKey_;
}

void GetThingTopoRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

int GetThingTopoRequest::getPageNo()const
{
	return pageNo_;
}

void GetThingTopoRequest::setPageNo(int pageNo)
{
	pageNo_ = pageNo;
	setParameter("PageNo", std::to_string(pageNo));
}

std::string GetThingTopoRequest::getApiProduct()const
{
	return apiProduct_;
}

void GetThingTopoRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string GetThingTopoRequest::getApiRevision()const
{
	return apiRevision_;
}

void GetThingTopoRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string GetThingTopoRequest::getDeviceName()const
{
	return deviceName_;
}

void GetThingTopoRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}


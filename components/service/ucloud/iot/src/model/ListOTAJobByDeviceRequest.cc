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

#include <alibabacloud/iot/model/ListOTAJobByDeviceRequest.h>

using AlibabaCloud::Iot::Model::ListOTAJobByDeviceRequest;

ListOTAJobByDeviceRequest::ListOTAJobByDeviceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ListOTAJobByDevice")
{
	setMethod(HttpRequest::Method::Post);
}

ListOTAJobByDeviceRequest::~ListOTAJobByDeviceRequest()
{}

std::string ListOTAJobByDeviceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ListOTAJobByDeviceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int ListOTAJobByDeviceRequest::getPageSize()const
{
	return pageSize_;
}

void ListOTAJobByDeviceRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string ListOTAJobByDeviceRequest::getFirmwareId()const
{
	return firmwareId_;
}

void ListOTAJobByDeviceRequest::setFirmwareId(const std::string& firmwareId)
{
	firmwareId_ = firmwareId;
	setParameter("FirmwareId", firmwareId);
}

int ListOTAJobByDeviceRequest::getCurrentPage()const
{
	return currentPage_;
}

void ListOTAJobByDeviceRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string ListOTAJobByDeviceRequest::getProductKey()const
{
	return productKey_;
}

void ListOTAJobByDeviceRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string ListOTAJobByDeviceRequest::getApiProduct()const
{
	return apiProduct_;
}

void ListOTAJobByDeviceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ListOTAJobByDeviceRequest::getApiRevision()const
{
	return apiRevision_;
}

void ListOTAJobByDeviceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string ListOTAJobByDeviceRequest::getDeviceName()const
{
	return deviceName_;
}

void ListOTAJobByDeviceRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}


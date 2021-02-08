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

#include <alibabacloud/iot/model/ListOTAJobByFirmwareRequest.h>

using AlibabaCloud::Iot::Model::ListOTAJobByFirmwareRequest;

ListOTAJobByFirmwareRequest::ListOTAJobByFirmwareRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ListOTAJobByFirmware")
{
	setMethod(HttpRequest::Method::Post);
}

ListOTAJobByFirmwareRequest::~ListOTAJobByFirmwareRequest()
{}

std::string ListOTAJobByFirmwareRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ListOTAJobByFirmwareRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ListOTAJobByFirmwareRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ListOTAJobByFirmwareRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int ListOTAJobByFirmwareRequest::getPageSize()const
{
	return pageSize_;
}

void ListOTAJobByFirmwareRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string ListOTAJobByFirmwareRequest::getFirmwareId()const
{
	return firmwareId_;
}

void ListOTAJobByFirmwareRequest::setFirmwareId(const std::string& firmwareId)
{
	firmwareId_ = firmwareId;
	setParameter("FirmwareId", firmwareId);
}

int ListOTAJobByFirmwareRequest::getCurrentPage()const
{
	return currentPage_;
}

void ListOTAJobByFirmwareRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string ListOTAJobByFirmwareRequest::getApiProduct()const
{
	return apiProduct_;
}

void ListOTAJobByFirmwareRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ListOTAJobByFirmwareRequest::getApiRevision()const
{
	return apiRevision_;
}

void ListOTAJobByFirmwareRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


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

#include <alibabacloud/iot/model/ListOTAFirmwareRequest.h>

using AlibabaCloud::Iot::Model::ListOTAFirmwareRequest;

ListOTAFirmwareRequest::ListOTAFirmwareRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ListOTAFirmware")
{
	setMethod(HttpRequest::Method::Post);
}

ListOTAFirmwareRequest::~ListOTAFirmwareRequest()
{}

std::string ListOTAFirmwareRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ListOTAFirmwareRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ListOTAFirmwareRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ListOTAFirmwareRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int ListOTAFirmwareRequest::getPageSize()const
{
	return pageSize_;
}

void ListOTAFirmwareRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int ListOTAFirmwareRequest::getCurrentPage()const
{
	return currentPage_;
}

void ListOTAFirmwareRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string ListOTAFirmwareRequest::getProductKey()const
{
	return productKey_;
}

void ListOTAFirmwareRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string ListOTAFirmwareRequest::getApiProduct()const
{
	return apiProduct_;
}

void ListOTAFirmwareRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ListOTAFirmwareRequest::getApiRevision()const
{
	return apiRevision_;
}

void ListOTAFirmwareRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string ListOTAFirmwareRequest::getDestVersion()const
{
	return destVersion_;
}

void ListOTAFirmwareRequest::setDestVersion(const std::string& destVersion)
{
	destVersion_ = destVersion;
	setParameter("DestVersion", destVersion);
}


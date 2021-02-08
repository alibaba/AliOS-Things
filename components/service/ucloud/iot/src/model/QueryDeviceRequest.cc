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

#include <alibabacloud/iot/model/QueryDeviceRequest.h>

using AlibabaCloud::Iot::Model::QueryDeviceRequest;

QueryDeviceRequest::QueryDeviceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDevice")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDeviceRequest::~QueryDeviceRequest()
{}

std::string QueryDeviceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDeviceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDeviceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDeviceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryDeviceRequest::getPageSize()const
{
	return pageSize_;
}

void QueryDeviceRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryDeviceRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryDeviceRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryDeviceRequest::getProductKey()const
{
	return productKey_;
}

void QueryDeviceRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string QueryDeviceRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDeviceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDeviceRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDeviceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


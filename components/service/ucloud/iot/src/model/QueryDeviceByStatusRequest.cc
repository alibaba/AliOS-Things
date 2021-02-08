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

#include <alibabacloud/iot/model/QueryDeviceByStatusRequest.h>

using AlibabaCloud::Iot::Model::QueryDeviceByStatusRequest;

QueryDeviceByStatusRequest::QueryDeviceByStatusRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDeviceByStatus")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDeviceByStatusRequest::~QueryDeviceByStatusRequest()
{}

std::string QueryDeviceByStatusRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDeviceByStatusRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDeviceByStatusRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void QueryDeviceByStatusRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string QueryDeviceByStatusRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDeviceByStatusRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryDeviceByStatusRequest::getPageSize()const
{
	return pageSize_;
}

void QueryDeviceByStatusRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryDeviceByStatusRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryDeviceByStatusRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryDeviceByStatusRequest::getProductKey()const
{
	return productKey_;
}

void QueryDeviceByStatusRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string QueryDeviceByStatusRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDeviceByStatusRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDeviceByStatusRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDeviceByStatusRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryDeviceByStatusRequest::getBizTenantId()const
{
	return bizTenantId_;
}

void QueryDeviceByStatusRequest::setBizTenantId(const std::string& bizTenantId)
{
	bizTenantId_ = bizTenantId;
	setParameter("BizTenantId", bizTenantId);
}

int QueryDeviceByStatusRequest::getStatus()const
{
	return status_;
}

void QueryDeviceByStatusRequest::setStatus(int status)
{
	status_ = status;
	setParameter("Status", std::to_string(status));
}


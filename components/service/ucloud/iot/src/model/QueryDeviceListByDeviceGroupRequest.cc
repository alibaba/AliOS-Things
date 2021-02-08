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

#include <alibabacloud/iot/model/QueryDeviceListByDeviceGroupRequest.h>

using AlibabaCloud::Iot::Model::QueryDeviceListByDeviceGroupRequest;

QueryDeviceListByDeviceGroupRequest::QueryDeviceListByDeviceGroupRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryDeviceListByDeviceGroup")
{
	setMethod(HttpRequest::Method::Post);
}

QueryDeviceListByDeviceGroupRequest::~QueryDeviceListByDeviceGroupRequest()
{}

std::string QueryDeviceListByDeviceGroupRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryDeviceListByDeviceGroupRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryDeviceListByDeviceGroupRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryDeviceListByDeviceGroupRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryDeviceListByDeviceGroupRequest::getPageSize()const
{
	return pageSize_;
}

void QueryDeviceListByDeviceGroupRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string QueryDeviceListByDeviceGroupRequest::getGroupId()const
{
	return groupId_;
}

void QueryDeviceListByDeviceGroupRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

int QueryDeviceListByDeviceGroupRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryDeviceListByDeviceGroupRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryDeviceListByDeviceGroupRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryDeviceListByDeviceGroupRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryDeviceListByDeviceGroupRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryDeviceListByDeviceGroupRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


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

#include <alibabacloud/iot/model/QueryProductListRequest.h>

using AlibabaCloud::Iot::Model::QueryProductListRequest;

QueryProductListRequest::QueryProductListRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryProductList")
{
	setMethod(HttpRequest::Method::Post);
}

QueryProductListRequest::~QueryProductListRequest()
{}

std::string QueryProductListRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryProductListRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryProductListRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void QueryProductListRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string QueryProductListRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryProductListRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryProductListRequest::getPageSize()const
{
	return pageSize_;
}

void QueryProductListRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string QueryProductListRequest::getAliyunCommodityCode()const
{
	return aliyunCommodityCode_;
}

void QueryProductListRequest::setAliyunCommodityCode(const std::string& aliyunCommodityCode)
{
	aliyunCommodityCode_ = aliyunCommodityCode;
	setParameter("AliyunCommodityCode", aliyunCommodityCode);
}

int QueryProductListRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryProductListRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryProductListRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryProductListRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryProductListRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryProductListRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


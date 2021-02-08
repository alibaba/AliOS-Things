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

#include <alibabacloud/linkface/model/QueryAuthenticationRequest.h>

using AlibabaCloud::LinkFace::Model::QueryAuthenticationRequest;

QueryAuthenticationRequest::QueryAuthenticationRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "QueryAuthentication")
{
	setMethod(HttpRequest::Method::Post);
}

QueryAuthenticationRequest::~QueryAuthenticationRequest()
{}

std::string QueryAuthenticationRequest::getIotId()const
{
	return iotId_;
}

void QueryAuthenticationRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setBodyParameter("IotId", iotId);
}

int QueryAuthenticationRequest::getPageSize()const
{
	return pageSize_;
}

void QueryAuthenticationRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setBodyParameter("PageSize", std::to_string(pageSize));
}

int QueryAuthenticationRequest::getLicenseType()const
{
	return licenseType_;
}

void QueryAuthenticationRequest::setLicenseType(int licenseType)
{
	licenseType_ = licenseType;
	setBodyParameter("LicenseType", std::to_string(licenseType));
}

int QueryAuthenticationRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryAuthenticationRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setBodyParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryAuthenticationRequest::getProductKey()const
{
	return productKey_;
}

void QueryAuthenticationRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setBodyParameter("ProductKey", productKey);
}

std::string QueryAuthenticationRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryAuthenticationRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryAuthenticationRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryAuthenticationRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryAuthenticationRequest::getDeviceName()const
{
	return deviceName_;
}

void QueryAuthenticationRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setBodyParameter("DeviceName", deviceName);
}


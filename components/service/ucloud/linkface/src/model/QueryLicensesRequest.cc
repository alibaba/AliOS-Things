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

#include <alibabacloud/linkface/model/QueryLicensesRequest.h>

using AlibabaCloud::LinkFace::Model::QueryLicensesRequest;

QueryLicensesRequest::QueryLicensesRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "QueryLicenses")
{
	setMethod(HttpRequest::Method::Post);
}

QueryLicensesRequest::~QueryLicensesRequest()
{}

int QueryLicensesRequest::getPageSize()const
{
	return pageSize_;
}

void QueryLicensesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setBodyParameter("PageSize", std::to_string(pageSize));
}

int QueryLicensesRequest::getLicenseType()const
{
	return licenseType_;
}

void QueryLicensesRequest::setLicenseType(int licenseType)
{
	licenseType_ = licenseType;
	setBodyParameter("LicenseType", std::to_string(licenseType));
}

int QueryLicensesRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryLicensesRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setBodyParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryLicensesRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryLicensesRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryLicensesRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryLicensesRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


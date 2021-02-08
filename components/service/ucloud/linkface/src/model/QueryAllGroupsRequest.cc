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

#include <alibabacloud/linkface/model/QueryAllGroupsRequest.h>

using AlibabaCloud::LinkFace::Model::QueryAllGroupsRequest;

QueryAllGroupsRequest::QueryAllGroupsRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "QueryAllGroups")
{
	setMethod(HttpRequest::Method::Post);
}

QueryAllGroupsRequest::~QueryAllGroupsRequest()
{}

int QueryAllGroupsRequest::getPageSize()const
{
	return pageSize_;
}

void QueryAllGroupsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setBodyParameter("PageSize", std::to_string(pageSize));
}

int QueryAllGroupsRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryAllGroupsRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setBodyParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryAllGroupsRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryAllGroupsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryAllGroupsRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryAllGroupsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


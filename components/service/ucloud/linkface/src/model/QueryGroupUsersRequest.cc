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

#include <alibabacloud/linkface/model/QueryGroupUsersRequest.h>

using AlibabaCloud::LinkFace::Model::QueryGroupUsersRequest;

QueryGroupUsersRequest::QueryGroupUsersRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "QueryGroupUsers")
{
	setMethod(HttpRequest::Method::Post);
}

QueryGroupUsersRequest::~QueryGroupUsersRequest()
{}

int QueryGroupUsersRequest::getPageSize()const
{
	return pageSize_;
}

void QueryGroupUsersRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setBodyParameter("PageSize", std::to_string(pageSize));
}

std::string QueryGroupUsersRequest::getGroupId()const
{
	return groupId_;
}

void QueryGroupUsersRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setBodyParameter("GroupId", groupId);
}

int QueryGroupUsersRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryGroupUsersRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setBodyParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryGroupUsersRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryGroupUsersRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryGroupUsersRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryGroupUsersRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


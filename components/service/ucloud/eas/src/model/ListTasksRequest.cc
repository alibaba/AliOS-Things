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

#include <alibabacloud/eas/model/ListTasksRequest.h>

using AlibabaCloud::Eas::Model::ListTasksRequest;

ListTasksRequest::ListTasksRequest() :
	RoaServiceRequest("eas", "2018-05-22")
{
	setResourcePath("/api/tasks");
	setMethod(HttpRequest::Method::Get);
}

ListTasksRequest::~ListTasksRequest()
{}

std::string ListTasksRequest::getFilter()const
{
	return filter_;
}

void ListTasksRequest::setFilter(const std::string& filter)
{
	filter_ = filter;
	setParameter("Filter", filter);
}

std::string ListTasksRequest::getPageSize()const
{
	return pageSize_;
}

void ListTasksRequest::setPageSize(const std::string& pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", pageSize);
}

std::string ListTasksRequest::getSort()const
{
	return sort_;
}

void ListTasksRequest::setSort(const std::string& sort)
{
	sort_ = sort;
	setParameter("Sort", sort);
}

std::string ListTasksRequest::getPageNum()const
{
	return pageNum_;
}

void ListTasksRequest::setPageNum(const std::string& pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", pageNum);
}


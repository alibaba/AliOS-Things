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

#include <alibabacloud/cloudphoto/model/FetchPhotosRequest.h>

using AlibabaCloud::CloudPhoto::Model::FetchPhotosRequest;

FetchPhotosRequest::FetchPhotosRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "FetchPhotos")
{
	setMethod(HttpRequest::Method::Post);
}

FetchPhotosRequest::~FetchPhotosRequest()
{}

std::string FetchPhotosRequest::getLibraryId()const
{
	return libraryId_;
}

void FetchPhotosRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string FetchPhotosRequest::getOrderBy()const
{
	return orderBy_;
}

void FetchPhotosRequest::setOrderBy(const std::string& orderBy)
{
	orderBy_ = orderBy;
	setParameter("OrderBy", orderBy);
}

std::string FetchPhotosRequest::getStoreName()const
{
	return storeName_;
}

void FetchPhotosRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

int FetchPhotosRequest::getSize()const
{
	return size_;
}

void FetchPhotosRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

std::string FetchPhotosRequest::getState()const
{
	return state_;
}

void FetchPhotosRequest::setState(const std::string& state)
{
	state_ = state;
	setParameter("State", state);
}

int FetchPhotosRequest::getPage()const
{
	return page_;
}

void FetchPhotosRequest::setPage(int page)
{
	page_ = page;
	setParameter("Page", std::to_string(page));
}

std::string FetchPhotosRequest::getOrder()const
{
	return order_;
}

void FetchPhotosRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}


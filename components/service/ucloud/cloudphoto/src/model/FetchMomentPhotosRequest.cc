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

#include <alibabacloud/cloudphoto/model/FetchMomentPhotosRequest.h>

using AlibabaCloud::CloudPhoto::Model::FetchMomentPhotosRequest;

FetchMomentPhotosRequest::FetchMomentPhotosRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "FetchMomentPhotos")
{
	setMethod(HttpRequest::Method::Post);
}

FetchMomentPhotosRequest::~FetchMomentPhotosRequest()
{}

std::string FetchMomentPhotosRequest::getLibraryId()const
{
	return libraryId_;
}

void FetchMomentPhotosRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string FetchMomentPhotosRequest::getOrderBy()const
{
	return orderBy_;
}

void FetchMomentPhotosRequest::setOrderBy(const std::string& orderBy)
{
	orderBy_ = orderBy;
	setParameter("OrderBy", orderBy);
}

std::string FetchMomentPhotosRequest::getStoreName()const
{
	return storeName_;
}

void FetchMomentPhotosRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

long FetchMomentPhotosRequest::getMomentId()const
{
	return momentId_;
}

void FetchMomentPhotosRequest::setMomentId(long momentId)
{
	momentId_ = momentId;
	setParameter("MomentId", std::to_string(momentId));
}

int FetchMomentPhotosRequest::getSize()const
{
	return size_;
}

void FetchMomentPhotosRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

int FetchMomentPhotosRequest::getPage()const
{
	return page_;
}

void FetchMomentPhotosRequest::setPage(int page)
{
	page_ = page;
	setParameter("Page", std::to_string(page));
}

std::string FetchMomentPhotosRequest::getOrder()const
{
	return order_;
}

void FetchMomentPhotosRequest::setOrder(const std::string& order)
{
	order_ = order;
	setParameter("Order", order);
}


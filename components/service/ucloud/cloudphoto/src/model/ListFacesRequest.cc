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

#include <alibabacloud/cloudphoto/model/ListFacesRequest.h>

using AlibabaCloud::CloudPhoto::Model::ListFacesRequest;

ListFacesRequest::ListFacesRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "ListFaces")
{
	setMethod(HttpRequest::Method::Post);
}

ListFacesRequest::~ListFacesRequest()
{}

std::string ListFacesRequest::getCursor()const
{
	return cursor_;
}

void ListFacesRequest::setCursor(const std::string& cursor)
{
	cursor_ = cursor;
	setParameter("Cursor", cursor);
}

std::string ListFacesRequest::getHasFaceName()const
{
	return hasFaceName_;
}

void ListFacesRequest::setHasFaceName(const std::string& hasFaceName)
{
	hasFaceName_ = hasFaceName;
	setParameter("HasFaceName", hasFaceName);
}

std::string ListFacesRequest::getLibraryId()const
{
	return libraryId_;
}

void ListFacesRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string ListFacesRequest::getStoreName()const
{
	return storeName_;
}

void ListFacesRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

int ListFacesRequest::getSize()const
{
	return size_;
}

void ListFacesRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

std::string ListFacesRequest::getState()const
{
	return state_;
}

void ListFacesRequest::setState(const std::string& state)
{
	state_ = state;
	setParameter("State", state);
}

std::string ListFacesRequest::getDirection()const
{
	return direction_;
}

void ListFacesRequest::setDirection(const std::string& direction)
{
	direction_ = direction;
	setParameter("Direction", direction);
}


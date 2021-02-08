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

#include <alibabacloud/cloudphoto/model/ListAlbumsRequest.h>

using AlibabaCloud::CloudPhoto::Model::ListAlbumsRequest;

ListAlbumsRequest::ListAlbumsRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "ListAlbums")
{
	setMethod(HttpRequest::Method::Post);
}

ListAlbumsRequest::~ListAlbumsRequest()
{}

std::string ListAlbumsRequest::getCursor()const
{
	return cursor_;
}

void ListAlbumsRequest::setCursor(const std::string& cursor)
{
	cursor_ = cursor;
	setParameter("Cursor", cursor);
}

std::string ListAlbumsRequest::getLibraryId()const
{
	return libraryId_;
}

void ListAlbumsRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string ListAlbumsRequest::getStoreName()const
{
	return storeName_;
}

void ListAlbumsRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

int ListAlbumsRequest::getSize()const
{
	return size_;
}

void ListAlbumsRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

std::string ListAlbumsRequest::getState()const
{
	return state_;
}

void ListAlbumsRequest::setState(const std::string& state)
{
	state_ = state;
	setParameter("State", state);
}

std::string ListAlbumsRequest::getDirection()const
{
	return direction_;
}

void ListAlbumsRequest::setDirection(const std::string& direction)
{
	direction_ = direction;
	setParameter("Direction", direction);
}


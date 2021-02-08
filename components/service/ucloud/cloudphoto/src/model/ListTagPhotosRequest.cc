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

#include <alibabacloud/cloudphoto/model/ListTagPhotosRequest.h>

using AlibabaCloud::CloudPhoto::Model::ListTagPhotosRequest;

ListTagPhotosRequest::ListTagPhotosRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "ListTagPhotos")
{
	setMethod(HttpRequest::Method::Post);
}

ListTagPhotosRequest::~ListTagPhotosRequest()
{}

std::string ListTagPhotosRequest::getCursor()const
{
	return cursor_;
}

void ListTagPhotosRequest::setCursor(const std::string& cursor)
{
	cursor_ = cursor;
	setParameter("Cursor", cursor);
}

long ListTagPhotosRequest::getTagId()const
{
	return tagId_;
}

void ListTagPhotosRequest::setTagId(long tagId)
{
	tagId_ = tagId;
	setParameter("TagId", std::to_string(tagId));
}

std::string ListTagPhotosRequest::getLibraryId()const
{
	return libraryId_;
}

void ListTagPhotosRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string ListTagPhotosRequest::getStoreName()const
{
	return storeName_;
}

void ListTagPhotosRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

int ListTagPhotosRequest::getSize()const
{
	return size_;
}

void ListTagPhotosRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

std::string ListTagPhotosRequest::getState()const
{
	return state_;
}

void ListTagPhotosRequest::setState(const std::string& state)
{
	state_ = state;
	setParameter("State", state);
}

std::string ListTagPhotosRequest::getDirection()const
{
	return direction_;
}

void ListTagPhotosRequest::setDirection(const std::string& direction)
{
	direction_ = direction;
	setParameter("Direction", direction);
}


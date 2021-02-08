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

#include <alibabacloud/cloudphoto/model/ListAlbumPhotosRequest.h>

using AlibabaCloud::CloudPhoto::Model::ListAlbumPhotosRequest;

ListAlbumPhotosRequest::ListAlbumPhotosRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "ListAlbumPhotos")
{
	setMethod(HttpRequest::Method::Post);
}

ListAlbumPhotosRequest::~ListAlbumPhotosRequest()
{}

std::string ListAlbumPhotosRequest::getCursor()const
{
	return cursor_;
}

void ListAlbumPhotosRequest::setCursor(const std::string& cursor)
{
	cursor_ = cursor;
	setParameter("Cursor", cursor);
}

std::string ListAlbumPhotosRequest::getLibraryId()const
{
	return libraryId_;
}

void ListAlbumPhotosRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

long ListAlbumPhotosRequest::getAlbumId()const
{
	return albumId_;
}

void ListAlbumPhotosRequest::setAlbumId(long albumId)
{
	albumId_ = albumId;
	setParameter("AlbumId", std::to_string(albumId));
}

std::string ListAlbumPhotosRequest::getStoreName()const
{
	return storeName_;
}

void ListAlbumPhotosRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

int ListAlbumPhotosRequest::getSize()const
{
	return size_;
}

void ListAlbumPhotosRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

std::string ListAlbumPhotosRequest::getState()const
{
	return state_;
}

void ListAlbumPhotosRequest::setState(const std::string& state)
{
	state_ = state;
	setParameter("State", state);
}

std::string ListAlbumPhotosRequest::getDirection()const
{
	return direction_;
}

void ListAlbumPhotosRequest::setDirection(const std::string& direction)
{
	direction_ = direction;
	setParameter("Direction", direction);
}


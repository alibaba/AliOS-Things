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

#include <alibabacloud/cloudphoto/model/FetchAlbumTagPhotosRequest.h>

using AlibabaCloud::CloudPhoto::Model::FetchAlbumTagPhotosRequest;

FetchAlbumTagPhotosRequest::FetchAlbumTagPhotosRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "FetchAlbumTagPhotos")
{
	setMethod(HttpRequest::Method::Post);
}

FetchAlbumTagPhotosRequest::~FetchAlbumTagPhotosRequest()
{}

long FetchAlbumTagPhotosRequest::getTagId()const
{
	return tagId_;
}

void FetchAlbumTagPhotosRequest::setTagId(long tagId)
{
	tagId_ = tagId;
	setParameter("TagId", std::to_string(tagId));
}

std::string FetchAlbumTagPhotosRequest::getLibraryId()const
{
	return libraryId_;
}

void FetchAlbumTagPhotosRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

long FetchAlbumTagPhotosRequest::getAlbumId()const
{
	return albumId_;
}

void FetchAlbumTagPhotosRequest::setAlbumId(long albumId)
{
	albumId_ = albumId;
	setParameter("AlbumId", std::to_string(albumId));
}

std::string FetchAlbumTagPhotosRequest::getStoreName()const
{
	return storeName_;
}

void FetchAlbumTagPhotosRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

int FetchAlbumTagPhotosRequest::getSize()const
{
	return size_;
}

void FetchAlbumTagPhotosRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

int FetchAlbumTagPhotosRequest::getPage()const
{
	return page_;
}

void FetchAlbumTagPhotosRequest::setPage(int page)
{
	page_ = page;
	setParameter("Page", std::to_string(page));
}


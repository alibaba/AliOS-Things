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

#include <alibabacloud/cloudphoto/model/GetVideoCoverRequest.h>

using AlibabaCloud::CloudPhoto::Model::GetVideoCoverRequest;

GetVideoCoverRequest::GetVideoCoverRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "GetVideoCover")
{
	setMethod(HttpRequest::Method::Post);
}

GetVideoCoverRequest::~GetVideoCoverRequest()
{}

std::string GetVideoCoverRequest::getLibraryId()const
{
	return libraryId_;
}

void GetVideoCoverRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

long GetVideoCoverRequest::getPhotoId()const
{
	return photoId_;
}

void GetVideoCoverRequest::setPhotoId(long photoId)
{
	photoId_ = photoId;
	setParameter("PhotoId", std::to_string(photoId));
}

std::string GetVideoCoverRequest::getStoreName()const
{
	return storeName_;
}

void GetVideoCoverRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string GetVideoCoverRequest::getZoomType()const
{
	return zoomType_;
}

void GetVideoCoverRequest::setZoomType(const std::string& zoomType)
{
	zoomType_ = zoomType;
	setParameter("ZoomType", zoomType);
}


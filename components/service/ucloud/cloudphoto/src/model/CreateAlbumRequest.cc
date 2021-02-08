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

#include <alibabacloud/cloudphoto/model/CreateAlbumRequest.h>

using AlibabaCloud::CloudPhoto::Model::CreateAlbumRequest;

CreateAlbumRequest::CreateAlbumRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "CreateAlbum")
{
	setMethod(HttpRequest::Method::Post);
}

CreateAlbumRequest::~CreateAlbumRequest()
{}

std::string CreateAlbumRequest::getLibraryId()const
{
	return libraryId_;
}

void CreateAlbumRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string CreateAlbumRequest::getStoreName()const
{
	return storeName_;
}

void CreateAlbumRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string CreateAlbumRequest::getRemark()const
{
	return remark_;
}

void CreateAlbumRequest::setRemark(const std::string& remark)
{
	remark_ = remark;
	setParameter("Remark", remark);
}

std::string CreateAlbumRequest::getAlbumName()const
{
	return albumName_;
}

void CreateAlbumRequest::setAlbumName(const std::string& albumName)
{
	albumName_ = albumName;
	setParameter("AlbumName", albumName);
}


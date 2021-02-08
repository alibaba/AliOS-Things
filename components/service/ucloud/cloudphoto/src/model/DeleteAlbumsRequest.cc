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

#include <alibabacloud/cloudphoto/model/DeleteAlbumsRequest.h>

using AlibabaCloud::CloudPhoto::Model::DeleteAlbumsRequest;

DeleteAlbumsRequest::DeleteAlbumsRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "DeleteAlbums")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteAlbumsRequest::~DeleteAlbumsRequest()
{}

std::string DeleteAlbumsRequest::getLibraryId()const
{
	return libraryId_;
}

void DeleteAlbumsRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::vector<long> DeleteAlbumsRequest::getAlbumId()const
{
	return albumId_;
}

void DeleteAlbumsRequest::setAlbumId(const std::vector<long>& albumId)
{
	albumId_ = albumId;
	for(int dep1 = 0; dep1!= albumId.size(); dep1++) {
		setParameter("AlbumId."+ std::to_string(dep1), std::to_string(albumId.at(dep1)));
	}
}

std::string DeleteAlbumsRequest::getStoreName()const
{
	return storeName_;
}

void DeleteAlbumsRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}


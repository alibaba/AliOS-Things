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

#include <alibabacloud/cloudphoto/model/EditPhotosRequest.h>

using AlibabaCloud::CloudPhoto::Model::EditPhotosRequest;

EditPhotosRequest::EditPhotosRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "EditPhotos")
{
	setMethod(HttpRequest::Method::Post);
}

EditPhotosRequest::~EditPhotosRequest()
{}

std::string EditPhotosRequest::getLibraryId()const
{
	return libraryId_;
}

void EditPhotosRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::vector<long> EditPhotosRequest::getPhotoId()const
{
	return photoId_;
}

void EditPhotosRequest::setPhotoId(const std::vector<long>& photoId)
{
	photoId_ = photoId;
	for(int dep1 = 0; dep1!= photoId.size(); dep1++) {
		setParameter("PhotoId."+ std::to_string(dep1), std::to_string(photoId.at(dep1)));
	}
}

std::string EditPhotosRequest::getStoreName()const
{
	return storeName_;
}

void EditPhotosRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string EditPhotosRequest::getRemark()const
{
	return remark_;
}

void EditPhotosRequest::setRemark(const std::string& remark)
{
	remark_ = remark;
	setParameter("Remark", remark);
}

std::string EditPhotosRequest::getTitle()const
{
	return title_;
}

void EditPhotosRequest::setTitle(const std::string& title)
{
	title_ = title;
	setParameter("Title", title);
}

long EditPhotosRequest::getTakenAt()const
{
	return takenAt_;
}

void EditPhotosRequest::setTakenAt(long takenAt)
{
	takenAt_ = takenAt;
	setParameter("TakenAt", std::to_string(takenAt));
}

long EditPhotosRequest::getShareExpireTime()const
{
	return shareExpireTime_;
}

void EditPhotosRequest::setShareExpireTime(long shareExpireTime)
{
	shareExpireTime_ = shareExpireTime;
	setParameter("ShareExpireTime", std::to_string(shareExpireTime));
}


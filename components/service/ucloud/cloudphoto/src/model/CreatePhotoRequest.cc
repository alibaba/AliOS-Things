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

#include <alibabacloud/cloudphoto/model/CreatePhotoRequest.h>

using AlibabaCloud::CloudPhoto::Model::CreatePhotoRequest;

CreatePhotoRequest::CreatePhotoRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "CreatePhoto")
{
	setMethod(HttpRequest::Method::Post);
}

CreatePhotoRequest::~CreatePhotoRequest()
{}

std::string CreatePhotoRequest::getPhotoTitle()const
{
	return photoTitle_;
}

void CreatePhotoRequest::setPhotoTitle(const std::string& photoTitle)
{
	photoTitle_ = photoTitle;
	setParameter("PhotoTitle", photoTitle);
}

std::string CreatePhotoRequest::getStoreName()const
{
	return storeName_;
}

void CreatePhotoRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string CreatePhotoRequest::getRemark()const
{
	return remark_;
}

void CreatePhotoRequest::setRemark(const std::string& remark)
{
	remark_ = remark;
	setParameter("Remark", remark);
}

std::string CreatePhotoRequest::getSessionId()const
{
	return sessionId_;
}

void CreatePhotoRequest::setSessionId(const std::string& sessionId)
{
	sessionId_ = sessionId;
	setParameter("SessionId", sessionId);
}

long CreatePhotoRequest::getTakenAt()const
{
	return takenAt_;
}

void CreatePhotoRequest::setTakenAt(long takenAt)
{
	takenAt_ = takenAt;
	setParameter("TakenAt", std::to_string(takenAt));
}

std::string CreatePhotoRequest::getStaging()const
{
	return staging_;
}

void CreatePhotoRequest::setStaging(const std::string& staging)
{
	staging_ = staging;
	setParameter("Staging", staging);
}

std::string CreatePhotoRequest::getFileId()const
{
	return fileId_;
}

void CreatePhotoRequest::setFileId(const std::string& fileId)
{
	fileId_ = fileId;
	setParameter("FileId", fileId);
}

std::string CreatePhotoRequest::getLibraryId()const
{
	return libraryId_;
}

void CreatePhotoRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string CreatePhotoRequest::getUploadType()const
{
	return uploadType_;
}

void CreatePhotoRequest::setUploadType(const std::string& uploadType)
{
	uploadType_ = uploadType;
	setParameter("UploadType", uploadType);
}

long CreatePhotoRequest::getShareExpireTime()const
{
	return shareExpireTime_;
}

void CreatePhotoRequest::setShareExpireTime(long shareExpireTime)
{
	shareExpireTime_ = shareExpireTime;
	setParameter("ShareExpireTime", std::to_string(shareExpireTime));
}


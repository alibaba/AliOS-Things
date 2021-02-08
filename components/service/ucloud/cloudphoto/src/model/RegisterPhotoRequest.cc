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

#include <alibabacloud/cloudphoto/model/RegisterPhotoRequest.h>

using AlibabaCloud::CloudPhoto::Model::RegisterPhotoRequest;

RegisterPhotoRequest::RegisterPhotoRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "RegisterPhoto")
{
	setMethod(HttpRequest::Method::Post);
}

RegisterPhotoRequest::~RegisterPhotoRequest()
{}

float RegisterPhotoRequest::getLatitude()const
{
	return latitude_;
}

void RegisterPhotoRequest::setLatitude(float latitude)
{
	latitude_ = latitude;
	setParameter("Latitude", std::to_string(latitude));
}

std::string RegisterPhotoRequest::getPhotoTitle()const
{
	return photoTitle_;
}

void RegisterPhotoRequest::setPhotoTitle(const std::string& photoTitle)
{
	photoTitle_ = photoTitle;
	setParameter("PhotoTitle", photoTitle);
}

std::string RegisterPhotoRequest::getStoreName()const
{
	return storeName_;
}

void RegisterPhotoRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string RegisterPhotoRequest::getIsVideo()const
{
	return isVideo_;
}

void RegisterPhotoRequest::setIsVideo(const std::string& isVideo)
{
	isVideo_ = isVideo;
	setParameter("IsVideo", isVideo);
}

std::string RegisterPhotoRequest::getRemark()const
{
	return remark_;
}

void RegisterPhotoRequest::setRemark(const std::string& remark)
{
	remark_ = remark;
	setParameter("Remark", remark);
}

long RegisterPhotoRequest::getTakenAt()const
{
	return takenAt_;
}

void RegisterPhotoRequest::setTakenAt(long takenAt)
{
	takenAt_ = takenAt;
	setParameter("TakenAt", std::to_string(takenAt));
}

float RegisterPhotoRequest::getLongitude()const
{
	return longitude_;
}

void RegisterPhotoRequest::setLongitude(float longitude)
{
	longitude_ = longitude;
	setParameter("Longitude", std::to_string(longitude));
}

int RegisterPhotoRequest::getHeight()const
{
	return height_;
}

void RegisterPhotoRequest::setHeight(int height)
{
	height_ = height;
	setParameter("Height", std::to_string(height));
}

std::string RegisterPhotoRequest::getLibraryId()const
{
	return libraryId_;
}

void RegisterPhotoRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

long RegisterPhotoRequest::getSize()const
{
	return size_;
}

void RegisterPhotoRequest::setSize(long size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

int RegisterPhotoRequest::getWidth()const
{
	return width_;
}

void RegisterPhotoRequest::setWidth(int width)
{
	width_ = width;
	setParameter("Width", std::to_string(width));
}

std::string RegisterPhotoRequest::getLocation()const
{
	return location_;
}

void RegisterPhotoRequest::setLocation(const std::string& location)
{
	location_ = location;
	setParameter("Location", location);
}

std::string RegisterPhotoRequest::getMd5()const
{
	return md5_;
}

void RegisterPhotoRequest::setMd5(const std::string& md5)
{
	md5_ = md5;
	setParameter("Md5", md5);
}


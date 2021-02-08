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

#include <alibabacloud/cloudphoto/model/ListFacePhotosRequest.h>

using AlibabaCloud::CloudPhoto::Model::ListFacePhotosRequest;

ListFacePhotosRequest::ListFacePhotosRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "ListFacePhotos")
{
	setMethod(HttpRequest::Method::Post);
}

ListFacePhotosRequest::~ListFacePhotosRequest()
{}

std::string ListFacePhotosRequest::getCursor()const
{
	return cursor_;
}

void ListFacePhotosRequest::setCursor(const std::string& cursor)
{
	cursor_ = cursor;
	setParameter("Cursor", cursor);
}

std::string ListFacePhotosRequest::getLibraryId()const
{
	return libraryId_;
}

void ListFacePhotosRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string ListFacePhotosRequest::getStoreName()const
{
	return storeName_;
}

void ListFacePhotosRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

long ListFacePhotosRequest::getFaceId()const
{
	return faceId_;
}

void ListFacePhotosRequest::setFaceId(long faceId)
{
	faceId_ = faceId;
	setParameter("FaceId", std::to_string(faceId));
}

int ListFacePhotosRequest::getSize()const
{
	return size_;
}

void ListFacePhotosRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

std::string ListFacePhotosRequest::getState()const
{
	return state_;
}

void ListFacePhotosRequest::setState(const std::string& state)
{
	state_ = state;
	setParameter("State", state);
}

std::string ListFacePhotosRequest::getDirection()const
{
	return direction_;
}

void ListFacePhotosRequest::setDirection(const std::string& direction)
{
	direction_ = direction;
	setParameter("Direction", direction);
}


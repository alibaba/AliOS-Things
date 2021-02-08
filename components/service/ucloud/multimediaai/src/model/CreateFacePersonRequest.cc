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

#include <alibabacloud/multimediaai/model/CreateFacePersonRequest.h>

using AlibabaCloud::Multimediaai::Model::CreateFacePersonRequest;

CreateFacePersonRequest::CreateFacePersonRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "CreateFacePerson")
{
	setMethod(HttpRequest::Method::Post);
}

CreateFacePersonRequest::~CreateFacePersonRequest()
{}

long CreateFacePersonRequest::getFaceGroupId()const
{
	return faceGroupId_;
}

void CreateFacePersonRequest::setFaceGroupId(long faceGroupId)
{
	faceGroupId_ = faceGroupId;
	setParameter("FaceGroupId", std::to_string(faceGroupId));
}

std::string CreateFacePersonRequest::getImageUrls()const
{
	return imageUrls_;
}

void CreateFacePersonRequest::setImageUrls(const std::string& imageUrls)
{
	imageUrls_ = imageUrls;
	setParameter("ImageUrls", imageUrls);
}

std::string CreateFacePersonRequest::getFacePersonName()const
{
	return facePersonName_;
}

void CreateFacePersonRequest::setFacePersonName(const std::string& facePersonName)
{
	facePersonName_ = facePersonName;
	setParameter("FacePersonName", facePersonName);
}


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

#include <alibabacloud/multimediaai/model/RegisterFaceImageRequest.h>

using AlibabaCloud::Multimediaai::Model::RegisterFaceImageRequest;

RegisterFaceImageRequest::RegisterFaceImageRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "RegisterFaceImage")
{
	setMethod(HttpRequest::Method::Post);
}

RegisterFaceImageRequest::~RegisterFaceImageRequest()
{}

long RegisterFaceImageRequest::getFaceGroupId()const
{
	return faceGroupId_;
}

void RegisterFaceImageRequest::setFaceGroupId(long faceGroupId)
{
	faceGroupId_ = faceGroupId;
	setParameter("FaceGroupId", std::to_string(faceGroupId));
}

long RegisterFaceImageRequest::getFacePersonId()const
{
	return facePersonId_;
}

void RegisterFaceImageRequest::setFacePersonId(long facePersonId)
{
	facePersonId_ = facePersonId;
	setParameter("FacePersonId", std::to_string(facePersonId));
}

std::string RegisterFaceImageRequest::getImageUrl()const
{
	return imageUrl_;
}

void RegisterFaceImageRequest::setImageUrl(const std::string& imageUrl)
{
	imageUrl_ = imageUrl;
	setParameter("ImageUrl", imageUrl);
}


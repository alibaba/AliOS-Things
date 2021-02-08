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

#include <alibabacloud/linkface/model/UpdateFaceRequest.h>

using AlibabaCloud::LinkFace::Model::UpdateFaceRequest;

UpdateFaceRequest::UpdateFaceRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "UpdateFace")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateFaceRequest::~UpdateFaceRequest()
{}

std::string UpdateFaceRequest::getUserId()const
{
	return userId_;
}

void UpdateFaceRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

std::string UpdateFaceRequest::getUserInfo()const
{
	return userInfo_;
}

void UpdateFaceRequest::setUserInfo(const std::string& userInfo)
{
	userInfo_ = userInfo;
	setBodyParameter("UserInfo", userInfo);
}

std::string UpdateFaceRequest::getImage()const
{
	return image_;
}

void UpdateFaceRequest::setImage(const std::string& image)
{
	image_ = image;
	setBodyParameter("Image", image);
}

std::string UpdateFaceRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateFaceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateFaceRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateFaceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


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

#include <alibabacloud/facebody/model/FaceMakeupRequest.h>

using AlibabaCloud::Facebody::Model::FaceMakeupRequest;

FaceMakeupRequest::FaceMakeupRequest() :
	RpcServiceRequest("facebody", "2019-12-30", "FaceMakeup")
{
	setMethod(HttpRequest::Method::Post);
}

FaceMakeupRequest::~FaceMakeupRequest()
{}

float FaceMakeupRequest::getStrength()const
{
	return strength_;
}

void FaceMakeupRequest::setStrength(float strength)
{
	strength_ = strength;
	setBodyParameter("Strength", std::to_string(strength));
}

std::string FaceMakeupRequest::getMakeupType()const
{
	return makeupType_;
}

void FaceMakeupRequest::setMakeupType(const std::string& makeupType)
{
	makeupType_ = makeupType;
	setBodyParameter("MakeupType", makeupType);
}

std::string FaceMakeupRequest::getResourceType()const
{
	return resourceType_;
}

void FaceMakeupRequest::setResourceType(const std::string& resourceType)
{
	resourceType_ = resourceType;
	setBodyParameter("ResourceType", resourceType);
}

std::string FaceMakeupRequest::getImageURL()const
{
	return imageURL_;
}

void FaceMakeupRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}


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

#include <alibabacloud/facebody/model/FaceTidyupRequest.h>

using AlibabaCloud::Facebody::Model::FaceTidyupRequest;

FaceTidyupRequest::FaceTidyupRequest() :
	RpcServiceRequest("facebody", "2019-12-30", "FaceTidyup")
{
	setMethod(HttpRequest::Method::Post);
}

FaceTidyupRequest::~FaceTidyupRequest()
{}

int FaceTidyupRequest::getShapeType()const
{
	return shapeType_;
}

void FaceTidyupRequest::setShapeType(int shapeType)
{
	shapeType_ = shapeType;
	setBodyParameter("ShapeType", std::to_string(shapeType));
}

float FaceTidyupRequest::getStrength()const
{
	return strength_;
}

void FaceTidyupRequest::setStrength(float strength)
{
	strength_ = strength;
	setBodyParameter("Strength", std::to_string(strength));
}

std::string FaceTidyupRequest::getImageURL()const
{
	return imageURL_;
}

void FaceTidyupRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}


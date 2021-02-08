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

#include <alibabacloud/facebody/model/CompareFaceRequest.h>

using AlibabaCloud::Facebody::Model::CompareFaceRequest;

CompareFaceRequest::CompareFaceRequest() :
	RpcServiceRequest("facebody", "2019-12-30", "CompareFace")
{
	setMethod(HttpRequest::Method::Post);
}

CompareFaceRequest::~CompareFaceRequest()
{}

int CompareFaceRequest::getImageType()const
{
	return imageType_;
}

void CompareFaceRequest::setImageType(int imageType)
{
	imageType_ = imageType;
	setBodyParameter("ImageType", std::to_string(imageType));
}

std::string CompareFaceRequest::getImageURLB()const
{
	return imageURLB_;
}

void CompareFaceRequest::setImageURLB(const std::string& imageURLB)
{
	imageURLB_ = imageURLB;
	setBodyParameter("ImageURLB", imageURLB);
}

std::string CompareFaceRequest::getImageURLA()const
{
	return imageURLA_;
}

void CompareFaceRequest::setImageURLA(const std::string& imageURLA)
{
	imageURLA_ = imageURLA;
	setBodyParameter("ImageURLA", imageURLA);
}


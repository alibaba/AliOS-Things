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

#include <alibabacloud/ocr/model/RecognizeDriverLicenseRequest.h>

using AlibabaCloud::Ocr::Model::RecognizeDriverLicenseRequest;

RecognizeDriverLicenseRequest::RecognizeDriverLicenseRequest() :
	RpcServiceRequest("ocr", "2019-12-30", "RecognizeDriverLicense")
{
	setMethod(HttpRequest::Method::Post);
}

RecognizeDriverLicenseRequest::~RecognizeDriverLicenseRequest()
{}

int RecognizeDriverLicenseRequest::getImageType()const
{
	return imageType_;
}

void RecognizeDriverLicenseRequest::setImageType(int imageType)
{
	imageType_ = imageType;
	setBodyParameter("ImageType", std::to_string(imageType));
}

std::string RecognizeDriverLicenseRequest::getSide()const
{
	return side_;
}

void RecognizeDriverLicenseRequest::setSide(const std::string& side)
{
	side_ = side;
	setBodyParameter("Side", side);
}

std::string RecognizeDriverLicenseRequest::getImageURL()const
{
	return imageURL_;
}

void RecognizeDriverLicenseRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}


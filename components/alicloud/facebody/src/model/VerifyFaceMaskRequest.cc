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

#include <alibabacloud/facebody/model/VerifyFaceMaskRequest.h>

using AlibabaCloud::Facebody::Model::VerifyFaceMaskRequest;

VerifyFaceMaskRequest::VerifyFaceMaskRequest() :
	RpcServiceRequest("facebody", "2019-12-30", "VerifyFaceMask")
{
	setMethod(HttpRequest::Method::Post);
}

VerifyFaceMaskRequest::~VerifyFaceMaskRequest()
{}

std::string VerifyFaceMaskRequest::getImageURL()const
{
	return imageURL_;
}

void VerifyFaceMaskRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}

std::string VerifyFaceMaskRequest::getRefUrl()const
{
	return refUrl_;
}

void VerifyFaceMaskRequest::setRefUrl(const std::string& refUrl)
{
	refUrl_ = refUrl;
	setBodyParameter("RefUrl", refUrl);
}


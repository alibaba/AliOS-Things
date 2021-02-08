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

#include <alibabacloud/imageseg/model/SegmentHeadRequest.h>

using AlibabaCloud::Imageseg::Model::SegmentHeadRequest;

SegmentHeadRequest::SegmentHeadRequest() :
	RpcServiceRequest("imageseg", "2019-12-30", "SegmentHead")
{
	setMethod(HttpRequest::Method::Post);
}

SegmentHeadRequest::~SegmentHeadRequest()
{}

std::string SegmentHeadRequest::getReturnForm()const
{
	return returnForm_;
}

void SegmentHeadRequest::setReturnForm(const std::string& returnForm)
{
	returnForm_ = returnForm;
	setParameter("ReturnForm", returnForm);
}

std::string SegmentHeadRequest::getImageURL()const
{
	return imageURL_;
}

void SegmentHeadRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setParameter("ImageURL", imageURL);
}


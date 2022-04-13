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

#include <alibabacloud/imageenhan/model/ColorizeImageRequest.h>

using AlibabaCloud::Imageenhan::Model::ColorizeImageRequest;

ColorizeImageRequest::ColorizeImageRequest() :
	RpcServiceRequest("imageenhan", "2019-09-30", "ColorizeImage")
{
	setMethod(HttpRequest::Method::Post);
}

ColorizeImageRequest::~ColorizeImageRequest()
{}

std::string ColorizeImageRequest::getImageURL()const
{
	return imageURL_;
}

void ColorizeImageRequest::setImageURL(const std::string& imageURL)
{
	imageURL_ = imageURL;
	setBodyParameter("ImageURL", imageURL);
}


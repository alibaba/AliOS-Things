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

#include <alibabacloud/imageenhan/model/GenerateDynamicImageRequest.h>

using AlibabaCloud::Imageenhan::Model::GenerateDynamicImageRequest;

GenerateDynamicImageRequest::GenerateDynamicImageRequest() :
	RpcServiceRequest("imageenhan", "2019-09-30", "GenerateDynamicImage")
{
	setMethod(HttpRequest::Method::Post);
}

GenerateDynamicImageRequest::~GenerateDynamicImageRequest()
{}

std::string GenerateDynamicImageRequest::getUrl()const
{
	return url_;
}

void GenerateDynamicImageRequest::setUrl(const std::string& url)
{
	url_ = url;
	setBodyParameter("Url", url);
}

std::string GenerateDynamicImageRequest::getOperation()const
{
	return operation_;
}

void GenerateDynamicImageRequest::setOperation(const std::string& operation)
{
	operation_ = operation;
	setBodyParameter("Operation", operation);
}


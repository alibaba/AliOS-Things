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

#include <alibabacloud/imageprocess/model/ClassifyFNFRequest.h>

using AlibabaCloud::Imageprocess::Model::ClassifyFNFRequest;

ClassifyFNFRequest::ClassifyFNFRequest() :
	RpcServiceRequest("imageprocess", "2020-03-20", "ClassifyFNF")
{
	setMethod(HttpRequest::Method::Post);
}

ClassifyFNFRequest::~ClassifyFNFRequest()
{}

std::string ClassifyFNFRequest::getDataFormat()const
{
	return dataFormat_;
}

void ClassifyFNFRequest::setDataFormat(const std::string& dataFormat)
{
	dataFormat_ = dataFormat;
	setBodyParameter("DataFormat", dataFormat);
}

std::string ClassifyFNFRequest::getOrgId()const
{
	return orgId_;
}

void ClassifyFNFRequest::setOrgId(const std::string& orgId)
{
	orgId_ = orgId;
	setBodyParameter("OrgId", orgId);
}

std::string ClassifyFNFRequest::getOrgName()const
{
	return orgName_;
}

void ClassifyFNFRequest::setOrgName(const std::string& orgName)
{
	orgName_ = orgName;
	setBodyParameter("OrgName", orgName);
}

std::string ClassifyFNFRequest::getImageUrl()const
{
	return imageUrl_;
}

void ClassifyFNFRequest::setImageUrl(const std::string& imageUrl)
{
	imageUrl_ = imageUrl;
	setBodyParameter("ImageUrl", imageUrl);
}

std::string ClassifyFNFRequest::getTracerId()const
{
	return tracerId_;
}

void ClassifyFNFRequest::setTracerId(const std::string& tracerId)
{
	tracerId_ = tracerId;
	setBodyParameter("TracerId", tracerId);
}


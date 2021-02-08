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

#include <alibabacloud/imageprocess/model/DetectHipKeypointXRayRequest.h>

using AlibabaCloud::Imageprocess::Model::DetectHipKeypointXRayRequest;

DetectHipKeypointXRayRequest::DetectHipKeypointXRayRequest() :
	RpcServiceRequest("imageprocess", "2020-03-20", "DetectHipKeypointXRay")
{
	setMethod(HttpRequest::Method::Post);
}

DetectHipKeypointXRayRequest::~DetectHipKeypointXRayRequest()
{}

std::string DetectHipKeypointXRayRequest::getDataFormat()const
{
	return dataFormat_;
}

void DetectHipKeypointXRayRequest::setDataFormat(const std::string& dataFormat)
{
	dataFormat_ = dataFormat;
	setBodyParameter("DataFormat", dataFormat);
}

std::string DetectHipKeypointXRayRequest::getOrgId()const
{
	return orgId_;
}

void DetectHipKeypointXRayRequest::setOrgId(const std::string& orgId)
{
	orgId_ = orgId;
	setBodyParameter("OrgId", orgId);
}

std::string DetectHipKeypointXRayRequest::getOrgName()const
{
	return orgName_;
}

void DetectHipKeypointXRayRequest::setOrgName(const std::string& orgName)
{
	orgName_ = orgName;
	setBodyParameter("OrgName", orgName);
}

std::string DetectHipKeypointXRayRequest::getImageUrl()const
{
	return imageUrl_;
}

void DetectHipKeypointXRayRequest::setImageUrl(const std::string& imageUrl)
{
	imageUrl_ = imageUrl;
	setBodyParameter("ImageUrl", imageUrl);
}

std::string DetectHipKeypointXRayRequest::getTracerId()const
{
	return tracerId_;
}

void DetectHipKeypointXRayRequest::setTracerId(const std::string& tracerId)
{
	tracerId_ = tracerId;
	setBodyParameter("TracerId", tracerId);
}


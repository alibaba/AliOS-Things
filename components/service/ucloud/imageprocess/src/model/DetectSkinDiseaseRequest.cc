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

#include <alibabacloud/imageprocess/model/DetectSkinDiseaseRequest.h>

using AlibabaCloud::Imageprocess::Model::DetectSkinDiseaseRequest;

DetectSkinDiseaseRequest::DetectSkinDiseaseRequest() :
	RpcServiceRequest("imageprocess", "2020-03-20", "DetectSkinDisease")
{
	setMethod(HttpRequest::Method::Post);
}

DetectSkinDiseaseRequest::~DetectSkinDiseaseRequest()
{}

std::string DetectSkinDiseaseRequest::getUrl()const
{
	return url_;
}

void DetectSkinDiseaseRequest::setUrl(const std::string& url)
{
	url_ = url;
	setBodyParameter("Url", url);
}

std::string DetectSkinDiseaseRequest::getOrgId()const
{
	return orgId_;
}

void DetectSkinDiseaseRequest::setOrgId(const std::string& orgId)
{
	orgId_ = orgId;
	setBodyParameter("OrgId", orgId);
}

std::string DetectSkinDiseaseRequest::getOrgName()const
{
	return orgName_;
}

void DetectSkinDiseaseRequest::setOrgName(const std::string& orgName)
{
	orgName_ = orgName;
	setBodyParameter("OrgName", orgName);
}


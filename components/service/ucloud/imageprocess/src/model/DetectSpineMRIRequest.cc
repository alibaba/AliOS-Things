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

#include <alibabacloud/imageprocess/model/DetectSpineMRIRequest.h>

using AlibabaCloud::Imageprocess::Model::DetectSpineMRIRequest;

DetectSpineMRIRequest::DetectSpineMRIRequest() :
	RpcServiceRequest("imageprocess", "2020-03-20", "DetectSpineMRI")
{
	setMethod(HttpRequest::Method::Post);
}

DetectSpineMRIRequest::~DetectSpineMRIRequest()
{}

std::string DetectSpineMRIRequest::getDataFormat()const
{
	return dataFormat_;
}

void DetectSpineMRIRequest::setDataFormat(const std::string& dataFormat)
{
	dataFormat_ = dataFormat;
	setBodyParameter("DataFormat", dataFormat);
}

std::vector<DetectSpineMRIRequest::URLList> DetectSpineMRIRequest::getURLList()const
{
	return uRLList_;
}

void DetectSpineMRIRequest::setURLList(const std::vector<URLList>& uRLList)
{
	uRLList_ = uRLList;
	for(int dep1 = 0; dep1!= uRLList.size(); dep1++) {
		auto uRLListObj = uRLList.at(dep1);
		std::string uRLListObjStr = "URLList." + std::to_string(dep1 + 1);
		setParameter(uRLListObjStr + ".URL", uRLListObj.uRL);
	}
}

std::string DetectSpineMRIRequest::getOrgId()const
{
	return orgId_;
}

void DetectSpineMRIRequest::setOrgId(const std::string& orgId)
{
	orgId_ = orgId;
	setBodyParameter("OrgId", orgId);
}

std::string DetectSpineMRIRequest::getOrgName()const
{
	return orgName_;
}

void DetectSpineMRIRequest::setOrgName(const std::string& orgName)
{
	orgName_ = orgName;
	setBodyParameter("OrgName", orgName);
}


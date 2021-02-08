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

#include <alibabacloud/imageprocess/model/RunCTRegistrationRequest.h>

using AlibabaCloud::Imageprocess::Model::RunCTRegistrationRequest;

RunCTRegistrationRequest::RunCTRegistrationRequest() :
	RpcServiceRequest("imageprocess", "2020-03-20", "RunCTRegistration")
{
	setMethod(HttpRequest::Method::Post);
}

RunCTRegistrationRequest::~RunCTRegistrationRequest()
{}

std::string RunCTRegistrationRequest::getDataSourceType()const
{
	return dataSourceType_;
}

void RunCTRegistrationRequest::setDataSourceType(const std::string& dataSourceType)
{
	dataSourceType_ = dataSourceType;
	setBodyParameter("DataSourceType", dataSourceType);
}

std::string RunCTRegistrationRequest::getOrgName()const
{
	return orgName_;
}

void RunCTRegistrationRequest::setOrgName(const std::string& orgName)
{
	orgName_ = orgName;
	setBodyParameter("OrgName", orgName);
}

std::vector<RunCTRegistrationRequest::ReferenceList> RunCTRegistrationRequest::getReferenceList()const
{
	return referenceList_;
}

void RunCTRegistrationRequest::setReferenceList(const std::vector<ReferenceList>& referenceList)
{
	referenceList_ = referenceList;
	for(int dep1 = 0; dep1!= referenceList.size(); dep1++) {
		auto referenceListObj = referenceList.at(dep1);
		std::string referenceListObjStr = "ReferenceList." + std::to_string(dep1 + 1);
		setParameter(referenceListObjStr + ".ReferenceURL", referenceListObj.referenceURL);
	}
}

std::string RunCTRegistrationRequest::getDataFormat()const
{
	return dataFormat_;
}

void RunCTRegistrationRequest::setDataFormat(const std::string& dataFormat)
{
	dataFormat_ = dataFormat;
	setBodyParameter("DataFormat", dataFormat);
}

std::string RunCTRegistrationRequest::getOrgId()const
{
	return orgId_;
}

void RunCTRegistrationRequest::setOrgId(const std::string& orgId)
{
	orgId_ = orgId;
	setBodyParameter("OrgId", orgId);
}

bool RunCTRegistrationRequest::getAsync()const
{
	return async_;
}

void RunCTRegistrationRequest::setAsync(bool async)
{
	async_ = async;
	setBodyParameter("Async", async ? "true" : "false");
}

std::vector<RunCTRegistrationRequest::FloatingList> RunCTRegistrationRequest::getFloatingList()const
{
	return floatingList_;
}

void RunCTRegistrationRequest::setFloatingList(const std::vector<FloatingList>& floatingList)
{
	floatingList_ = floatingList;
	for(int dep1 = 0; dep1!= floatingList.size(); dep1++) {
		auto floatingListObj = floatingList.at(dep1);
		std::string floatingListObjStr = "FloatingList." + std::to_string(dep1 + 1);
		setParameter(floatingListObjStr + ".FloatingURL", floatingListObj.floatingURL);
	}
}


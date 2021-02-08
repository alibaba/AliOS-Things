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

#include <alibabacloud/cloudapi/model/CreateIpControlRequest.h>

using AlibabaCloud::CloudAPI::Model::CreateIpControlRequest;

CreateIpControlRequest::CreateIpControlRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "CreateIpControl")
{
	setMethod(HttpRequest::Method::Post);
}

CreateIpControlRequest::~CreateIpControlRequest()
{}

std::string CreateIpControlRequest::getIpControlName()const
{
	return ipControlName_;
}

void CreateIpControlRequest::setIpControlName(const std::string& ipControlName)
{
	ipControlName_ = ipControlName;
	setParameter("IpControlName", ipControlName);
}

std::vector<CreateIpControlRequest::IpControlPolicys> CreateIpControlRequest::getIpControlPolicys()const
{
	return ipControlPolicys_;
}

void CreateIpControlRequest::setIpControlPolicys(const std::vector<IpControlPolicys>& ipControlPolicys)
{
	ipControlPolicys_ = ipControlPolicys;
	for(int dep1 = 0; dep1!= ipControlPolicys.size(); dep1++) {
		auto ipControlPolicysObj = ipControlPolicys.at(dep1);
		std::string ipControlPolicysObjStr = "IpControlPolicys." + std::to_string(dep1 + 1);
		setParameter(ipControlPolicysObjStr + ".AppId", ipControlPolicysObj.appId);
		setParameter(ipControlPolicysObjStr + ".CidrIp", ipControlPolicysObj.cidrIp);
	}
}

std::string CreateIpControlRequest::getDescription()const
{
	return description_;
}

void CreateIpControlRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string CreateIpControlRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateIpControlRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateIpControlRequest::getSecurityToken()const
{
	return securityToken_;
}

void CreateIpControlRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string CreateIpControlRequest::getIpControlType()const
{
	return ipControlType_;
}

void CreateIpControlRequest::setIpControlType(const std::string& ipControlType)
{
	ipControlType_ = ipControlType;
	setParameter("IpControlType", ipControlType);
}


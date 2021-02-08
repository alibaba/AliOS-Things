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

#include <alibabacloud/cloudapi/model/SetVpcAccessRequest.h>

using AlibabaCloud::CloudAPI::Model::SetVpcAccessRequest;

SetVpcAccessRequest::SetVpcAccessRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "SetVpcAccess")
{
	setMethod(HttpRequest::Method::Post);
}

SetVpcAccessRequest::~SetVpcAccessRequest()
{}

std::string SetVpcAccessRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SetVpcAccessRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SetVpcAccessRequest::getInstanceId()const
{
	return instanceId_;
}

void SetVpcAccessRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string SetVpcAccessRequest::getSecurityToken()const
{
	return securityToken_;
}

void SetVpcAccessRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int SetVpcAccessRequest::getPort()const
{
	return port_;
}

void SetVpcAccessRequest::setPort(int port)
{
	port_ = port;
	setParameter("Port", std::to_string(port));
}

std::string SetVpcAccessRequest::getVpcId()const
{
	return vpcId_;
}

void SetVpcAccessRequest::setVpcId(const std::string& vpcId)
{
	vpcId_ = vpcId;
	setParameter("VpcId", vpcId);
}

std::string SetVpcAccessRequest::getName()const
{
	return name_;
}

void SetVpcAccessRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}


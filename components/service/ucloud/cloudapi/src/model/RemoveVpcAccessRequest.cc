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

#include <alibabacloud/cloudapi/model/RemoveVpcAccessRequest.h>

using AlibabaCloud::CloudAPI::Model::RemoveVpcAccessRequest;

RemoveVpcAccessRequest::RemoveVpcAccessRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "RemoveVpcAccess")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveVpcAccessRequest::~RemoveVpcAccessRequest()
{}

std::string RemoveVpcAccessRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void RemoveVpcAccessRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string RemoveVpcAccessRequest::getInstanceId()const
{
	return instanceId_;
}

void RemoveVpcAccessRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string RemoveVpcAccessRequest::getSecurityToken()const
{
	return securityToken_;
}

void RemoveVpcAccessRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int RemoveVpcAccessRequest::getPort()const
{
	return port_;
}

void RemoveVpcAccessRequest::setPort(int port)
{
	port_ = port;
	setParameter("Port", std::to_string(port));
}

std::string RemoveVpcAccessRequest::getVpcId()const
{
	return vpcId_;
}

void RemoveVpcAccessRequest::setVpcId(const std::string& vpcId)
{
	vpcId_ = vpcId;
	setParameter("VpcId", vpcId);
}


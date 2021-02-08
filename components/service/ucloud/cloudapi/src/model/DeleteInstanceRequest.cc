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

#include <alibabacloud/cloudapi/model/DeleteInstanceRequest.h>

using AlibabaCloud::CloudAPI::Model::DeleteInstanceRequest;

DeleteInstanceRequest::DeleteInstanceRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DeleteInstance")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteInstanceRequest::~DeleteInstanceRequest()
{}

std::string DeleteInstanceRequest::getToken()const
{
	return token_;
}

void DeleteInstanceRequest::setToken(const std::string& token)
{
	token_ = token;
	setParameter("Token", token);
}

std::string DeleteInstanceRequest::getInstanceId()const
{
	return instanceId_;
}

void DeleteInstanceRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::vector<DeleteInstanceRequest::Tag> DeleteInstanceRequest::getTag()const
{
	return tag_;
}

void DeleteInstanceRequest::setTag(const std::vector<Tag>& tag)
{
	tag_ = tag;
	for(int dep1 = 0; dep1!= tag.size(); dep1++) {
		auto tagObj = tag.at(dep1);
		std::string tagObjStr = "Tag." + std::to_string(dep1 + 1);
		setParameter(tagObjStr + ".Value", tagObj.value);
		setParameter(tagObjStr + ".Key", tagObj.key);
	}
}


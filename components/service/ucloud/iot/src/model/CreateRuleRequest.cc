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

#include <alibabacloud/iot/model/CreateRuleRequest.h>

using AlibabaCloud::Iot::Model::CreateRuleRequest;

CreateRuleRequest::CreateRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateRule")
{
	setMethod(HttpRequest::Method::Post);
}

CreateRuleRequest::~CreateRuleRequest()
{}

std::string CreateRuleRequest::getSelect()const
{
	return select_;
}

void CreateRuleRequest::setSelect(const std::string& select)
{
	select_ = select;
	setParameter("Select", select);
}

std::string CreateRuleRequest::getRuleDesc()const
{
	return ruleDesc_;
}

void CreateRuleRequest::setRuleDesc(const std::string& ruleDesc)
{
	ruleDesc_ = ruleDesc;
	setParameter("RuleDesc", ruleDesc);
}

std::string CreateRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateRuleRequest::getShortTopic()const
{
	return shortTopic_;
}

void CreateRuleRequest::setShortTopic(const std::string& shortTopic)
{
	shortTopic_ = shortTopic;
	setParameter("ShortTopic", shortTopic);
}

std::string CreateRuleRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void CreateRuleRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string CreateRuleRequest::getDataType()const
{
	return dataType_;
}

void CreateRuleRequest::setDataType(const std::string& dataType)
{
	dataType_ = dataType;
	setParameter("DataType", dataType);
}

std::string CreateRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateRuleRequest::getWhere()const
{
	return where_;
}

void CreateRuleRequest::setWhere(const std::string& where)
{
	where_ = where;
	setParameter("Where", where);
}

int CreateRuleRequest::getTopicType()const
{
	return topicType_;
}

void CreateRuleRequest::setTopicType(int topicType)
{
	topicType_ = topicType;
	setParameter("TopicType", std::to_string(topicType));
}

std::string CreateRuleRequest::getProductKey()const
{
	return productKey_;
}

void CreateRuleRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string CreateRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateRuleRequest::getName()const
{
	return name_;
}

void CreateRuleRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}

std::string CreateRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


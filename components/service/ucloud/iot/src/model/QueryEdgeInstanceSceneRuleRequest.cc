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

#include <alibabacloud/iot/model/QueryEdgeInstanceSceneRuleRequest.h>

using AlibabaCloud::Iot::Model::QueryEdgeInstanceSceneRuleRequest;

QueryEdgeInstanceSceneRuleRequest::QueryEdgeInstanceSceneRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryEdgeInstanceSceneRule")
{
	setMethod(HttpRequest::Method::Post);
}

QueryEdgeInstanceSceneRuleRequest::~QueryEdgeInstanceSceneRuleRequest()
{}

std::string QueryEdgeInstanceSceneRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryEdgeInstanceSceneRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryEdgeInstanceSceneRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryEdgeInstanceSceneRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryEdgeInstanceSceneRuleRequest::getPageSize()const
{
	return pageSize_;
}

void QueryEdgeInstanceSceneRuleRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryEdgeInstanceSceneRuleRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryEdgeInstanceSceneRuleRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryEdgeInstanceSceneRuleRequest::getInstanceId()const
{
	return instanceId_;
}

void QueryEdgeInstanceSceneRuleRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string QueryEdgeInstanceSceneRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryEdgeInstanceSceneRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryEdgeInstanceSceneRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryEdgeInstanceSceneRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


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

#include <alibabacloud/iot/model/QueryPageByApplyIdRequest.h>

using AlibabaCloud::Iot::Model::QueryPageByApplyIdRequest;

QueryPageByApplyIdRequest::QueryPageByApplyIdRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryPageByApplyId")
{
	setMethod(HttpRequest::Method::Post);
}

QueryPageByApplyIdRequest::~QueryPageByApplyIdRequest()
{}

std::string QueryPageByApplyIdRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryPageByApplyIdRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryPageByApplyIdRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryPageByApplyIdRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryPageByApplyIdRequest::getPageSize()const
{
	return pageSize_;
}

void QueryPageByApplyIdRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryPageByApplyIdRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryPageByApplyIdRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

long QueryPageByApplyIdRequest::getApplyId()const
{
	return applyId_;
}

void QueryPageByApplyIdRequest::setApplyId(long applyId)
{
	applyId_ = applyId;
	setParameter("ApplyId", std::to_string(applyId));
}

std::string QueryPageByApplyIdRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryPageByApplyIdRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryPageByApplyIdRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryPageByApplyIdRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


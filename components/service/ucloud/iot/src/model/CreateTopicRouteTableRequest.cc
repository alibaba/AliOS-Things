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

#include <alibabacloud/iot/model/CreateTopicRouteTableRequest.h>

using AlibabaCloud::Iot::Model::CreateTopicRouteTableRequest;

CreateTopicRouteTableRequest::CreateTopicRouteTableRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateTopicRouteTable")
{
	setMethod(HttpRequest::Method::Post);
}

CreateTopicRouteTableRequest::~CreateTopicRouteTableRequest()
{}

std::string CreateTopicRouteTableRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateTopicRouteTableRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateTopicRouteTableRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateTopicRouteTableRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::vector<std::string> CreateTopicRouteTableRequest::getDstTopic()const
{
	return dstTopic_;
}

void CreateTopicRouteTableRequest::setDstTopic(const std::vector<std::string>& dstTopic)
{
	dstTopic_ = dstTopic;
	for(int dep1 = 0; dep1!= dstTopic.size(); dep1++) {
		setParameter("DstTopic."+ std::to_string(dep1), dstTopic.at(dep1));
	}
}

std::string CreateTopicRouteTableRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateTopicRouteTableRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateTopicRouteTableRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateTopicRouteTableRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string CreateTopicRouteTableRequest::getSrcTopic()const
{
	return srcTopic_;
}

void CreateTopicRouteTableRequest::setSrcTopic(const std::string& srcTopic)
{
	srcTopic_ = srcTopic;
	setParameter("SrcTopic", srcTopic);
}


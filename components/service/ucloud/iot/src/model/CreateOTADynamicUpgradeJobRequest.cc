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

#include <alibabacloud/iot/model/CreateOTADynamicUpgradeJobRequest.h>

using AlibabaCloud::Iot::Model::CreateOTADynamicUpgradeJobRequest;

CreateOTADynamicUpgradeJobRequest::CreateOTADynamicUpgradeJobRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateOTADynamicUpgradeJob")
{
	setMethod(HttpRequest::Method::Post);
}

CreateOTADynamicUpgradeJobRequest::~CreateOTADynamicUpgradeJobRequest()
{}

int CreateOTADynamicUpgradeJobRequest::getRetryCount()const
{
	return retryCount_;
}

void CreateOTADynamicUpgradeJobRequest::setRetryCount(int retryCount)
{
	retryCount_ = retryCount;
	setParameter("RetryCount", std::to_string(retryCount));
}

int CreateOTADynamicUpgradeJobRequest::getTimeoutInMinutes()const
{
	return timeoutInMinutes_;
}

void CreateOTADynamicUpgradeJobRequest::setTimeoutInMinutes(int timeoutInMinutes)
{
	timeoutInMinutes_ = timeoutInMinutes;
	setParameter("TimeoutInMinutes", std::to_string(timeoutInMinutes));
}

std::string CreateOTADynamicUpgradeJobRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateOTADynamicUpgradeJobRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateOTADynamicUpgradeJobRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateOTADynamicUpgradeJobRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateOTADynamicUpgradeJobRequest::getFirmwareId()const
{
	return firmwareId_;
}

void CreateOTADynamicUpgradeJobRequest::setFirmwareId(const std::string& firmwareId)
{
	firmwareId_ = firmwareId;
	setParameter("FirmwareId", firmwareId);
}

std::string CreateOTADynamicUpgradeJobRequest::getProductKey()const
{
	return productKey_;
}

void CreateOTADynamicUpgradeJobRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

int CreateOTADynamicUpgradeJobRequest::getRetryInterval()const
{
	return retryInterval_;
}

void CreateOTADynamicUpgradeJobRequest::setRetryInterval(int retryInterval)
{
	retryInterval_ = retryInterval;
	setParameter("RetryInterval", std::to_string(retryInterval));
}

std::vector<std::string> CreateOTADynamicUpgradeJobRequest::getSrcVersion()const
{
	return srcVersion_;
}

void CreateOTADynamicUpgradeJobRequest::setSrcVersion(const std::vector<std::string>& srcVersion)
{
	srcVersion_ = srcVersion;
	for(int dep1 = 0; dep1!= srcVersion.size(); dep1++) {
		setParameter("SrcVersion."+ std::to_string(dep1), srcVersion.at(dep1));
	}
}

std::string CreateOTADynamicUpgradeJobRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateOTADynamicUpgradeJobRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateOTADynamicUpgradeJobRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateOTADynamicUpgradeJobRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

int CreateOTADynamicUpgradeJobRequest::getMaximumPerMinute()const
{
	return maximumPerMinute_;
}

void CreateOTADynamicUpgradeJobRequest::setMaximumPerMinute(int maximumPerMinute)
{
	maximumPerMinute_ = maximumPerMinute;
	setParameter("MaximumPerMinute", std::to_string(maximumPerMinute));
}


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

#include <alibabacloud/iot/model/CreateOTAStaticUpgradeJobRequest.h>

using AlibabaCloud::Iot::Model::CreateOTAStaticUpgradeJobRequest;

CreateOTAStaticUpgradeJobRequest::CreateOTAStaticUpgradeJobRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateOTAStaticUpgradeJob")
{
	setMethod(HttpRequest::Method::Post);
}

CreateOTAStaticUpgradeJobRequest::~CreateOTAStaticUpgradeJobRequest()
{}

int CreateOTAStaticUpgradeJobRequest::getRetryCount()const
{
	return retryCount_;
}

void CreateOTAStaticUpgradeJobRequest::setRetryCount(int retryCount)
{
	retryCount_ = retryCount;
	setParameter("RetryCount", std::to_string(retryCount));
}

int CreateOTAStaticUpgradeJobRequest::getTimeoutInMinutes()const
{
	return timeoutInMinutes_;
}

void CreateOTAStaticUpgradeJobRequest::setTimeoutInMinutes(int timeoutInMinutes)
{
	timeoutInMinutes_ = timeoutInMinutes;
	setParameter("TimeoutInMinutes", std::to_string(timeoutInMinutes));
}

std::string CreateOTAStaticUpgradeJobRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateOTAStaticUpgradeJobRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateOTAStaticUpgradeJobRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateOTAStaticUpgradeJobRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateOTAStaticUpgradeJobRequest::getTargetSelection()const
{
	return targetSelection_;
}

void CreateOTAStaticUpgradeJobRequest::setTargetSelection(const std::string& targetSelection)
{
	targetSelection_ = targetSelection;
	setParameter("TargetSelection", targetSelection);
}

std::string CreateOTAStaticUpgradeJobRequest::getGrayPercent()const
{
	return grayPercent_;
}

void CreateOTAStaticUpgradeJobRequest::setGrayPercent(const std::string& grayPercent)
{
	grayPercent_ = grayPercent;
	setParameter("GrayPercent", grayPercent);
}

std::string CreateOTAStaticUpgradeJobRequest::getFirmwareId()const
{
	return firmwareId_;
}

void CreateOTAStaticUpgradeJobRequest::setFirmwareId(const std::string& firmwareId)
{
	firmwareId_ = firmwareId;
	setParameter("FirmwareId", firmwareId);
}

std::string CreateOTAStaticUpgradeJobRequest::getProductKey()const
{
	return productKey_;
}

void CreateOTAStaticUpgradeJobRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

int CreateOTAStaticUpgradeJobRequest::getRetryInterval()const
{
	return retryInterval_;
}

void CreateOTAStaticUpgradeJobRequest::setRetryInterval(int retryInterval)
{
	retryInterval_ = retryInterval;
	setParameter("RetryInterval", std::to_string(retryInterval));
}

std::vector<std::string> CreateOTAStaticUpgradeJobRequest::getSrcVersion()const
{
	return srcVersion_;
}

void CreateOTAStaticUpgradeJobRequest::setSrcVersion(const std::vector<std::string>& srcVersion)
{
	srcVersion_ = srcVersion;
	for(int dep1 = 0; dep1!= srcVersion.size(); dep1++) {
		setParameter("SrcVersion."+ std::to_string(dep1), srcVersion.at(dep1));
	}
}

long CreateOTAStaticUpgradeJobRequest::getScheduleTime()const
{
	return scheduleTime_;
}

void CreateOTAStaticUpgradeJobRequest::setScheduleTime(long scheduleTime)
{
	scheduleTime_ = scheduleTime;
	setParameter("ScheduleTime", std::to_string(scheduleTime));
}

std::string CreateOTAStaticUpgradeJobRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateOTAStaticUpgradeJobRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateOTAStaticUpgradeJobRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateOTAStaticUpgradeJobRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

int CreateOTAStaticUpgradeJobRequest::getMaximumPerMinute()const
{
	return maximumPerMinute_;
}

void CreateOTAStaticUpgradeJobRequest::setMaximumPerMinute(int maximumPerMinute)
{
	maximumPerMinute_ = maximumPerMinute;
	setParameter("MaximumPerMinute", std::to_string(maximumPerMinute));
}

std::vector<std::string> CreateOTAStaticUpgradeJobRequest::getTargetDeviceName()const
{
	return targetDeviceName_;
}

void CreateOTAStaticUpgradeJobRequest::setTargetDeviceName(const std::vector<std::string>& targetDeviceName)
{
	targetDeviceName_ = targetDeviceName;
	for(int dep1 = 0; dep1!= targetDeviceName.size(); dep1++) {
		setParameter("TargetDeviceName."+ std::to_string(dep1), targetDeviceName.at(dep1));
	}
}


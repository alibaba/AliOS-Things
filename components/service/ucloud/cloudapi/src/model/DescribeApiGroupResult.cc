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

#include <alibabacloud/cloudapi/model/DescribeApiGroupResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApiGroupResult::DescribeApiGroupResult() :
	ServiceResult()
{}

DescribeApiGroupResult::DescribeApiGroupResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApiGroupResult::~DescribeApiGroupResult()
{}

void DescribeApiGroupResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allCustomDomainsNode = value["CustomDomains"]["DomainItem"];
	for (auto valueCustomDomainsDomainItem : allCustomDomainsNode)
	{
		DomainItem customDomainsObject;
		if(!valueCustomDomainsDomainItem["DomainName"].isNull())
			customDomainsObject.domainName = valueCustomDomainsDomainItem["DomainName"].asString();
		if(!valueCustomDomainsDomainItem["CertificateId"].isNull())
			customDomainsObject.certificateId = valueCustomDomainsDomainItem["CertificateId"].asString();
		if(!valueCustomDomainsDomainItem["CertificateName"].isNull())
			customDomainsObject.certificateName = valueCustomDomainsDomainItem["CertificateName"].asString();
		if(!valueCustomDomainsDomainItem["DomainCNAMEStatus"].isNull())
			customDomainsObject.domainCNAMEStatus = valueCustomDomainsDomainItem["DomainCNAMEStatus"].asString();
		if(!valueCustomDomainsDomainItem["DomainBindingStatus"].isNull())
			customDomainsObject.domainBindingStatus = valueCustomDomainsDomainItem["DomainBindingStatus"].asString();
		if(!valueCustomDomainsDomainItem["BindStageName"].isNull())
			customDomainsObject.bindStageName = valueCustomDomainsDomainItem["BindStageName"].asString();
		if(!valueCustomDomainsDomainItem["DomainLegalStatus"].isNull())
			customDomainsObject.domainLegalStatus = valueCustomDomainsDomainItem["DomainLegalStatus"].asString();
		if(!valueCustomDomainsDomainItem["DomainWebSocketStatus"].isNull())
			customDomainsObject.domainWebSocketStatus = valueCustomDomainsDomainItem["DomainWebSocketStatus"].asString();
		if(!valueCustomDomainsDomainItem["DomainRemark"].isNull())
			customDomainsObject.domainRemark = valueCustomDomainsDomainItem["DomainRemark"].asString();
		customDomains_.push_back(customDomainsObject);
	}
	auto allStageItemsNode = value["StageItems"]["StageInfo"];
	for (auto valueStageItemsStageInfo : allStageItemsNode)
	{
		StageInfo stageItemsObject;
		if(!valueStageItemsStageInfo["StageId"].isNull())
			stageItemsObject.stageId = valueStageItemsStageInfo["StageId"].asString();
		if(!valueStageItemsStageInfo["StageName"].isNull())
			stageItemsObject.stageName = valueStageItemsStageInfo["StageName"].asString();
		if(!valueStageItemsStageInfo["Description"].isNull())
			stageItemsObject.description = valueStageItemsStageInfo["Description"].asString();
		stageItems_.push_back(stageItemsObject);
	}
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();
	if(!value["GroupName"].isNull())
		groupName_ = value["GroupName"].asString();
	if(!value["SubDomain"].isNull())
		subDomain_ = value["SubDomain"].asString();
	if(!value["Description"].isNull())
		description_ = value["Description"].asString();
	if(!value["CreatedTime"].isNull())
		createdTime_ = value["CreatedTime"].asString();
	if(!value["ModifiedTime"].isNull())
		modifiedTime_ = value["ModifiedTime"].asString();
	if(!value["RegionId"].isNull())
		regionId_ = value["RegionId"].asString();
	if(!value["Status"].isNull())
		status_ = value["Status"].asString();
	if(!value["BillingStatus"].isNull())
		billingStatus_ = value["BillingStatus"].asString();
	if(!value["IllegalStatus"].isNull())
		illegalStatus_ = value["IllegalStatus"].asString();
	if(!value["TrafficLimit"].isNull())
		trafficLimit_ = std::stoi(value["TrafficLimit"].asString());
	if(!value["VpcDomain"].isNull())
		vpcDomain_ = value["VpcDomain"].asString();
	if(!value["InstanceType"].isNull())
		instanceType_ = value["InstanceType"].asString();
	if(!value["InstanceId"].isNull())
		instanceId_ = value["InstanceId"].asString();
	if(!value["HttpsPolicy"].isNull())
		httpsPolicy_ = value["HttpsPolicy"].asString();
	if(!value["UserLogConfig"].isNull())
		userLogConfig_ = value["UserLogConfig"].asString();
	if(!value["CustomTraceConfig"].isNull())
		customTraceConfig_ = value["CustomTraceConfig"].asString();
	if(!value["RpcPattern"].isNull())
		rpcPattern_ = value["RpcPattern"].asString();
	if(!value["CompatibleFlags"].isNull())
		compatibleFlags_ = value["CompatibleFlags"].asString();

}

std::string DescribeApiGroupResult::getGroupName()const
{
	return groupName_;
}

std::string DescribeApiGroupResult::getStatus()const
{
	return status_;
}

std::string DescribeApiGroupResult::getDescription()const
{
	return description_;
}

std::string DescribeApiGroupResult::getCreatedTime()const
{
	return createdTime_;
}

std::string DescribeApiGroupResult::getHttpsPolicy()const
{
	return httpsPolicy_;
}

std::string DescribeApiGroupResult::getInstanceId()const
{
	return instanceId_;
}

std::string DescribeApiGroupResult::getCompatibleFlags()const
{
	return compatibleFlags_;
}

std::string DescribeApiGroupResult::getCustomTraceConfig()const
{
	return customTraceConfig_;
}

std::string DescribeApiGroupResult::getUserLogConfig()const
{
	return userLogConfig_;
}

std::string DescribeApiGroupResult::getBillingStatus()const
{
	return billingStatus_;
}

int DescribeApiGroupResult::getTrafficLimit()const
{
	return trafficLimit_;
}

std::string DescribeApiGroupResult::getGroupId()const
{
	return groupId_;
}

std::string DescribeApiGroupResult::getVpcDomain()const
{
	return vpcDomain_;
}

std::string DescribeApiGroupResult::getSubDomain()const
{
	return subDomain_;
}

std::vector<DescribeApiGroupResult::DomainItem> DescribeApiGroupResult::getCustomDomains()const
{
	return customDomains_;
}

std::string DescribeApiGroupResult::getModifiedTime()const
{
	return modifiedTime_;
}

std::string DescribeApiGroupResult::getRpcPattern()const
{
	return rpcPattern_;
}

std::vector<DescribeApiGroupResult::StageInfo> DescribeApiGroupResult::getStageItems()const
{
	return stageItems_;
}

std::string DescribeApiGroupResult::getIllegalStatus()const
{
	return illegalStatus_;
}

std::string DescribeApiGroupResult::getRegionId()const
{
	return regionId_;
}

std::string DescribeApiGroupResult::getInstanceType()const
{
	return instanceType_;
}


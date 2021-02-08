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

#include <alibabacloud/servicemesh/model/GetVmMetaResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Servicemesh;
using namespace AlibabaCloud::Servicemesh::Model;

GetVmMetaResult::GetVmMetaResult() :
	ServiceResult()
{}

GetVmMetaResult::GetVmMetaResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetVmMetaResult::~GetVmMetaResult()
{}

void GetVmMetaResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto vmMetaInfoNode = value["VmMetaInfo"];
	if(!vmMetaInfoNode["RootCertPath"].isNull())
		vmMetaInfo_.rootCertPath = vmMetaInfoNode["RootCertPath"].asString();
	if(!vmMetaInfoNode["RootCertContent"].isNull())
		vmMetaInfo_.rootCertContent = vmMetaInfoNode["RootCertContent"].asString();
	if(!vmMetaInfoNode["KeyPath"].isNull())
		vmMetaInfo_.keyPath = vmMetaInfoNode["KeyPath"].asString();
	if(!vmMetaInfoNode["KeyContent"].isNull())
		vmMetaInfo_.keyContent = vmMetaInfoNode["KeyContent"].asString();
	if(!vmMetaInfoNode["CertChainPath"].isNull())
		vmMetaInfo_.certChainPath = vmMetaInfoNode["CertChainPath"].asString();
	if(!vmMetaInfoNode["CertChainContent"].isNull())
		vmMetaInfo_.certChainContent = vmMetaInfoNode["CertChainContent"].asString();
	if(!vmMetaInfoNode["EnvoyEnvPath"].isNull())
		vmMetaInfo_.envoyEnvPath = vmMetaInfoNode["EnvoyEnvPath"].asString();
	if(!vmMetaInfoNode["EnvoyEnvContent"].isNull())
		vmMetaInfo_.envoyEnvContent = vmMetaInfoNode["EnvoyEnvContent"].asString();
	if(!vmMetaInfoNode["HostsPath"].isNull())
		vmMetaInfo_.hostsPath = vmMetaInfoNode["HostsPath"].asString();
	if(!vmMetaInfoNode["HostsContent"].isNull())
		vmMetaInfo_.hostsContent = vmMetaInfoNode["HostsContent"].asString();
	if(!vmMetaInfoNode["TokenPath"].isNull())
		vmMetaInfo_.tokenPath = vmMetaInfoNode["TokenPath"].asString();
	if(!vmMetaInfoNode["TokenContent"].isNull())
		vmMetaInfo_.tokenContent = vmMetaInfoNode["TokenContent"].asString();

}

GetVmMetaResult::VmMetaInfo GetVmMetaResult::getVmMetaInfo()const
{
	return vmMetaInfo_;
}


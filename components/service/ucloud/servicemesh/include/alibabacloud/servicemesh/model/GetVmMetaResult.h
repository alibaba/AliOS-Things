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

#ifndef ALIBABACLOUD_SERVICEMESH_MODEL_GETVMMETARESULT_H_
#define ALIBABACLOUD_SERVICEMESH_MODEL_GETVMMETARESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/servicemesh/ServicemeshExport.h>

namespace AlibabaCloud
{
	namespace Servicemesh
	{
		namespace Model
		{
			class ALIBABACLOUD_SERVICEMESH_EXPORT GetVmMetaResult : public ServiceResult
			{
			public:
				struct VmMetaInfo
				{
					std::string envoyEnvContent;
					std::string tokenPath;
					std::string certChainPath;
					std::string certChainContent;
					std::string rootCertPath;
					std::string rootCertContent;
					std::string tokenContent;
					std::string envoyEnvPath;
					std::string hostsContent;
					std::string hostsPath;
					std::string keyContent;
					std::string keyPath;
				};


				GetVmMetaResult();
				explicit GetVmMetaResult(const std::string &payload);
				~GetVmMetaResult();
				VmMetaInfo getVmMetaInfo()const;

			protected:
				void parse(const std::string &payload);
			private:
				VmMetaInfo vmMetaInfo_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SERVICEMESH_MODEL_GETVMMETARESULT_H_
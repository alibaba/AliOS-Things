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

#ifndef ALIBABACLOUD_SERVICEMESH_MODEL_GETVMMETAREQUEST_H_
#define ALIBABACLOUD_SERVICEMESH_MODEL_GETVMMETAREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/servicemesh/ServicemeshExport.h>

namespace AlibabaCloud
{
	namespace Servicemesh
	{
		namespace Model
		{
			class ALIBABACLOUD_SERVICEMESH_EXPORT GetVmMetaRequest : public RpcServiceRequest
			{

			public:
				GetVmMetaRequest();
				~GetVmMetaRequest();

				std::string getServiceAccount()const;
				void setServiceAccount(const std::string& serviceAccount);
				std::string getTrustDomain()const;
				void setTrustDomain(const std::string& trustDomain);
				std::string get_Namespace()const;
				void set_Namespace(const std::string& _namespace);
				std::string getServiceMeshId()const;
				void setServiceMeshId(const std::string& serviceMeshId);

            private:
				std::string serviceAccount_;
				std::string trustDomain_;
				std::string _namespace_;
				std::string serviceMeshId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SERVICEMESH_MODEL_GETVMMETAREQUEST_H_
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

#ifndef ALIBABACLOUD_SERVICEMESH_MODEL_ADDVMAPPTOMESHREQUEST_H_
#define ALIBABACLOUD_SERVICEMESH_MODEL_ADDVMAPPTOMESHREQUEST_H_

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
			class ALIBABACLOUD_SERVICEMESH_EXPORT AddVmAppToMeshRequest : public RpcServiceRequest
			{

			public:
				AddVmAppToMeshRequest();
				~AddVmAppToMeshRequest();

				std::string getServiceAccount()const;
				void setServiceAccount(const std::string& serviceAccount);
				std::string getAnnotations()const;
				void setAnnotations(const std::string& annotations);
				std::string getPorts()const;
				void setPorts(const std::string& ports);
				std::string getServiceMeshId()const;
				void setServiceMeshId(const std::string& serviceMeshId);
				std::string getServiceName()const;
				void setServiceName(const std::string& serviceName);
				bool getUseWorkload()const;
				void setUseWorkload(bool useWorkload);
				std::string getIps()const;
				void setIps(const std::string& ips);
				std::string getLabels()const;
				void setLabels(const std::string& labels);
				std::string get_Namespace()const;
				void set_Namespace(const std::string& _namespace);
				bool getForce()const;
				void setForce(bool force);

            private:
				std::string serviceAccount_;
				std::string annotations_;
				std::string ports_;
				std::string serviceMeshId_;
				std::string serviceName_;
				bool useWorkload_;
				std::string ips_;
				std::string labels_;
				std::string _namespace_;
				bool force_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SERVICEMESH_MODEL_ADDVMAPPTOMESHREQUEST_H_
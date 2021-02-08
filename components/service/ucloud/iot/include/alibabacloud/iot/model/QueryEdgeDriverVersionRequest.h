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

#ifndef ALIBABACLOUD_IOT_MODEL_QUERYEDGEDRIVERVERSIONREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_QUERYEDGEDRIVERVERSIONREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/iot/IotExport.h>

namespace AlibabaCloud
{
	namespace Iot
	{
		namespace Model
		{
			class ALIBABACLOUD_IOT_EXPORT QueryEdgeDriverVersionRequest : public RpcServiceRequest
			{

			public:
				QueryEdgeDriverVersionRequest();
				~QueryEdgeDriverVersionRequest();

				int getVersionState()const;
				void setVersionState(int versionState);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getDriverId()const;
				void setDriverId(const std::string& driverId);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				int getPageSize()const;
				void setPageSize(int pageSize);
				std::string getDriverVersion()const;
				void setDriverVersion(const std::string& driverVersion);
				int getCurrentPage()const;
				void setCurrentPage(int currentPage);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);

            private:
				int versionState_;
				std::string accessKeyId_;
				std::string driverId_;
				std::string iotInstanceId_;
				int pageSize_;
				std::string driverVersion_;
				int currentPage_;
				std::string apiProduct_;
				std::string apiRevision_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_QUERYEDGEDRIVERVERSIONREQUEST_H_
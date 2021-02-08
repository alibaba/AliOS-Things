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

#ifndef ALIBABACLOUD_EAS_MODEL_GETSERVICESTATISTICSREQUEST_H_
#define ALIBABACLOUD_EAS_MODEL_GETSERVICESTATISTICSREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RoaServiceRequest.h>
#include <alibabacloud/eas/EasExport.h>

namespace AlibabaCloud
{
	namespace Eas
	{
		namespace Model
		{
			class ALIBABACLOUD_EAS_EXPORT GetServiceStatisticsRequest : public RoaServiceRequest
			{

			public:
				GetServiceStatisticsRequest();
				~GetServiceStatisticsRequest();

				std::string getMetric()const;
				void setMetric(const std::string& metric);
				std::string getService_name()const;
				void setService_name(const std::string& service_name);
				int getCount()const;
				void setCount(int count);
				std::string getFrom()const;
				void setFrom(const std::string& from);
				std::string getTo()const;
				void setTo(const std::string& to);
				std::string getRegion()const;
				void setRegion(const std::string& region);

            private:
				std::string metric_;
				std::string service_name_;
				int count_;
				std::string from_;
				std::string to_;
				std::string region_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_EAS_MODEL_GETSERVICESTATISTICSREQUEST_H_
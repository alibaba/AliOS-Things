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

#ifndef ALIBABACLOUD_SCHEDULERX2_MODEL_CREATEAPPGROUPREQUEST_H_
#define ALIBABACLOUD_SCHEDULERX2_MODEL_CREATEAPPGROUPREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/schedulerx2/Schedulerx2Export.h>

namespace AlibabaCloud
{
	namespace Schedulerx2
	{
		namespace Model
		{
			class ALIBABACLOUD_SCHEDULERX2_EXPORT CreateAppGroupRequest : public RpcServiceRequest
			{

			public:
				CreateAppGroupRequest();
				~CreateAppGroupRequest();

				int getMaxJobs()const;
				void setMaxJobs(int maxJobs);
				std::string getNamespaceName()const;
				void setNamespaceName(const std::string& namespaceName);
				std::string getNamespaceSource()const;
				void setNamespaceSource(const std::string& namespaceSource);
				std::string getMetricsThresholdJson()const;
				void setMetricsThresholdJson(const std::string& metricsThresholdJson);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				std::string getDescription()const;
				void setDescription(const std::string& description);
				std::string getAppName()const;
				void setAppName(const std::string& appName);
				std::string get_Namespace()const;
				void set_Namespace(const std::string& _namespace);
				std::string getAlarmJson()const;
				void setAlarmJson(const std::string& alarmJson);

            private:
				int maxJobs_;
				std::string namespaceName_;
				std::string namespaceSource_;
				std::string metricsThresholdJson_;
				std::string groupId_;
				std::string description_;
				std::string appName_;
				std::string _namespace_;
				std::string alarmJson_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SCHEDULERX2_MODEL_CREATEAPPGROUPREQUEST_H_
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

#ifndef ALIBABACLOUD_SCHEDULERX2_MODEL_STOPINSTANCEREQUEST_H_
#define ALIBABACLOUD_SCHEDULERX2_MODEL_STOPINSTANCEREQUEST_H_

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
			class ALIBABACLOUD_SCHEDULERX2_EXPORT StopInstanceRequest : public RpcServiceRequest
			{

			public:
				StopInstanceRequest();
				~StopInstanceRequest();

				long getJobId()const;
				void setJobId(long jobId);
				std::string getNamespaceSource()const;
				void setNamespaceSource(const std::string& namespaceSource);
				long getInstanceId()const;
				void setInstanceId(long instanceId);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				std::string get_Namespace()const;
				void set_Namespace(const std::string& _namespace);

            private:
				long jobId_;
				std::string namespaceSource_;
				long instanceId_;
				std::string groupId_;
				std::string _namespace_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_SCHEDULERX2_MODEL_STOPINSTANCEREQUEST_H_
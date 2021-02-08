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

#ifndef ALIBABACLOUD_IMM_MODEL_GETVIDEOTASKREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_GETVIDEOTASKREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT GetVideoTaskRequest : public RpcServiceRequest
			{

			public:
				GetVideoTaskRequest();
				~GetVideoTaskRequest();

				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getTaskId()const;
				void setTaskId(const std::string& taskId);
				std::string getTaskType()const;
				void setTaskType(const std::string& taskType);

            private:
				std::string project_;
				std::string accessKeyId_;
				std::string taskId_;
				std::string taskType_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_GETVIDEOTASKREQUEST_H_
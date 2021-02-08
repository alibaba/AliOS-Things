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

#ifndef ALIBABACLOUD_EAS_MODEL_GETTASKREQUEST_H_
#define ALIBABACLOUD_EAS_MODEL_GETTASKREQUEST_H_

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
			class ALIBABACLOUD_EAS_EXPORT GetTaskRequest : public RoaServiceRequest
			{

			public:
				GetTaskRequest();
				~GetTaskRequest();

				std::string getTask_name()const;
				void setTask_name(const std::string& task_name);
				std::string getRegion()const;
				void setRegion(const std::string& region);

            private:
				std::string task_name_;
				std::string region_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_EAS_MODEL_GETTASKREQUEST_H_
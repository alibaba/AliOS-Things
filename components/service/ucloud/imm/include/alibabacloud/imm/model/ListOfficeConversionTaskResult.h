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

#ifndef ALIBABACLOUD_IMM_MODEL_LISTOFFICECONVERSIONTASKRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_LISTOFFICECONVERSIONTASKRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT ListOfficeConversionTaskResult : public ServiceResult
			{
			public:
				struct TasksItem
				{
					std::string status;
					std::string tgtType;
					std::string taskId;
					int percent;
					std::string externalID;
					std::string createTime;
					std::string imageSpec;
					std::string notifyEndpoint;
					std::string tgtUri;
					int pageCount;
					std::string finishTime;
					std::string notifyTopicName;
					std::string srcUri;
				};


				ListOfficeConversionTaskResult();
				explicit ListOfficeConversionTaskResult(const std::string &payload);
				~ListOfficeConversionTaskResult();
				std::vector<TasksItem> getTasks()const;
				std::string getNextMarker()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<TasksItem> tasks_;
				std::string nextMarker_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_LISTOFFICECONVERSIONTASKRESULT_H_
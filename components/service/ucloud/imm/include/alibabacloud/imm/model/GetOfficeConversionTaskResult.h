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

#ifndef ALIBABACLOUD_IMM_MODEL_GETOFFICECONVERSIONTASKRESULT_H_
#define ALIBABACLOUD_IMM_MODEL_GETOFFICECONVERSIONTASKRESULT_H_

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
			class ALIBABACLOUD_IMM_EXPORT GetOfficeConversionTaskResult : public ServiceResult
			{
			public:
				struct FailDetail
				{
					std::string code;
				};


				GetOfficeConversionTaskResult();
				explicit GetOfficeConversionTaskResult(const std::string &payload);
				~GetOfficeConversionTaskResult();
				std::string getStatus()const;
				FailDetail getFailDetail()const;
				std::string getTgtType()const;
				std::string getTaskId()const;
				int getPercent()const;
				std::string getExternalID()const;
				std::string getCreateTime()const;
				std::string getImageSpec()const;
				std::string getNotifyEndpoint()const;
				std::string getTgtUri()const;
				int getPageCount()const;
				std::string getFinishTime()const;
				std::string getNotifyTopicName()const;
				std::string getSrcUri()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string status_;
				FailDetail failDetail_;
				std::string tgtType_;
				std::string taskId_;
				int percent_;
				std::string externalID_;
				std::string createTime_;
				std::string imageSpec_;
				std::string notifyEndpoint_;
				std::string tgtUri_;
				int pageCount_;
				std::string finishTime_;
				std::string notifyTopicName_;
				std::string srcUri_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_GETOFFICECONVERSIONTASKRESULT_H_
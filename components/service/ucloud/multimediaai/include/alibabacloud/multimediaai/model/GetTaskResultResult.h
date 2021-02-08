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

#ifndef ALIBABACLOUD_MULTIMEDIAAI_MODEL_GETTASKRESULTRESULT_H_
#define ALIBABACLOUD_MULTIMEDIAAI_MODEL_GETTASKRESULTRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/multimediaai/MultimediaaiExport.h>

namespace AlibabaCloud
{
	namespace Multimediaai
	{
		namespace Model
		{
			class ALIBABACLOUD_MULTIMEDIAAI_EXPORT GetTaskResultResult : public ServiceResult
			{
			public:
				struct Result
				{
					std::string errorName;
					std::string videoName;
					long analysisUseTime;
					std::string errorReason;
					std::string errorCode;
					std::string applicationId;
					std::string errorMessage;
					std::string processResultUrl;
					std::string videoUrl;
				};


				GetTaskResultResult();
				explicit GetTaskResultResult(const std::string &payload);
				~GetTaskResultResult();
				int getStatus()const;
				Result getResult()const;

			protected:
				void parse(const std::string &payload);
			private:
				int status_;
				Result result_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_MULTIMEDIAAI_MODEL_GETTASKRESULTRESULT_H_
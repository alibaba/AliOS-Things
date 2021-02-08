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

#ifndef ALIBABACLOUD_ALIMT_MODEL_GETDOCTRANSLATETASKRESULT_H_
#define ALIBABACLOUD_ALIMT_MODEL_GETDOCTRANSLATETASKRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/alimt/AlimtExport.h>

namespace AlibabaCloud
{
	namespace Alimt
	{
		namespace Model
		{
			class ALIBABACLOUD_ALIMT_EXPORT GetDocTranslateTaskResult : public ServiceResult
			{
			public:


				GetDocTranslateTaskResult();
				explicit GetDocTranslateTaskResult(const std::string &payload);
				~GetDocTranslateTaskResult();
				std::string getStatus()const;
				std::string getTranslateErrorMessage()const;
				std::string getTaskId()const;
				int getPageCount()const;
				std::string getTranslateFileUrl()const;
				std::string getTranslateErrorCode()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string status_;
				std::string translateErrorMessage_;
				std::string taskId_;
				int pageCount_;
				std::string translateFileUrl_;
				std::string translateErrorCode_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIMT_MODEL_GETDOCTRANSLATETASKRESULT_H_
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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_IMPORTSWAGGERRESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_IMPORTSWAGGERRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/cloudapi/CloudAPIExport.h>

namespace AlibabaCloud
{
	namespace CloudAPI
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDAPI_EXPORT ImportSwaggerResult : public ServiceResult
			{
			public:
				struct ApiImportSwaggerSuccess
				{
					std::string path;
					std::string apiUid;
					std::string apiOperation;
					std::string httpMethod;
				};
				struct ApiImportSwaggerFailed
				{
					std::string path;
					std::string errorMsg;
					std::string httpMethod;
				};
				struct ApiImportModelFailed
				{
					std::string modelName;
					std::string errorMsg;
					std::string groupId;
				};
				struct ApiImportModelSuccess
				{
					std::string modelUid;
					std::string modelOperation;
					std::string modelName;
					std::string groupId;
				};


				ImportSwaggerResult();
				explicit ImportSwaggerResult(const std::string &payload);
				~ImportSwaggerResult();
				std::vector<ApiImportSwaggerFailed> getFailed()const;
				std::vector<ApiImportModelFailed> getModelFailed()const;
				std::vector<ApiImportSwaggerSuccess> getSuccess()const;
				std::vector<ApiImportModelSuccess> getModelSuccess()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<ApiImportSwaggerFailed> failed_;
				std::vector<ApiImportModelFailed> modelFailed_;
				std::vector<ApiImportSwaggerSuccess> success_;
				std::vector<ApiImportModelSuccess> modelSuccess_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_IMPORTSWAGGERRESULT_H_
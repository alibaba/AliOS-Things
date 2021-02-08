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

#ifndef ALIBABACLOUD_IOT_MODEL_GETDATAAPISERVICEDETAILRESULT_H_
#define ALIBABACLOUD_IOT_MODEL_GETDATAAPISERVICEDETAILRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/iot/IotExport.h>

namespace AlibabaCloud
{
	namespace Iot
	{
		namespace Model
		{
			class ALIBABACLOUD_IOT_EXPORT GetDataAPIServiceDetailResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct SqlTemplateDTO
					{
						struct RequestParamsItem
						{
							std::string desc;
							std::string type;
							bool required;
							std::string example;
							std::string name;
						};
						struct ResponseParamsItem
						{
							std::string desc;
							std::string type;
							bool required;
							std::string example;
							std::string name;
						};
						std::string templateSql;
						std::vector<ResponseParamsItem> responseParams;
						std::string originSql;
						std::vector<RequestParamsItem> requestParams;
					};
					int status;
					std::string apiSrn;
					std::string description;
					SqlTemplateDTO sqlTemplateDTO;
					long lastUpdateTime;
					std::string requestMethod;
					long createTime;
					std::string dateFormat;
					std::string displayName;
					std::string requestProtocol;
					std::string apiPath;
				};


				GetDataAPIServiceDetailResult();
				explicit GetDataAPIServiceDetailResult(const std::string &payload);
				~GetDataAPIServiceDetailResult();
				Data getData()const;
				std::string getErrorMessage()const;
				std::string getCode()const;
				bool getSuccess()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;
				std::string errorMessage_;
				std::string code_;
				bool success_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_GETDATAAPISERVICEDETAILRESULT_H_
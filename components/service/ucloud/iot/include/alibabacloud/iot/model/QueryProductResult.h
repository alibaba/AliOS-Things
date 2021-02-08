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

#ifndef ALIBABACLOUD_IOT_MODEL_QUERYPRODUCTRESULT_H_
#define ALIBABACLOUD_IOT_MODEL_QUERYPRODUCTRESULT_H_

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
			class ALIBABACLOUD_IOT_EXPORT QueryProductResult : public ServiceResult
			{
			public:
				struct Data
				{
					bool owner;
					std::string description;
					std::string productName;
					std::string aliyunCommodityCode;
					std::string productStatus;
					std::string productSecret;
					long gmtCreate;
					std::string categoryKey;
					bool id2;
					std::string protocolType;
					int nodeType;
					int dataFormat;
					std::string categoryName;
					std::string authType;
					int netType;
					std::string productKey;
					int deviceCount;
				};


				QueryProductResult();
				explicit QueryProductResult(const std::string &payload);
				~QueryProductResult();
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
#endif // !ALIBABACLOUD_IOT_MODEL_QUERYPRODUCTRESULT_H_
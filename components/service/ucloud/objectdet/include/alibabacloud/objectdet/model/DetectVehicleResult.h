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

#ifndef ALIBABACLOUD_OBJECTDET_MODEL_DETECTVEHICLERESULT_H_
#define ALIBABACLOUD_OBJECTDET_MODEL_DETECTVEHICLERESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/objectdet/ObjectdetExport.h>

namespace AlibabaCloud
{
	namespace Objectdet
	{
		namespace Model
		{
			class ALIBABACLOUD_OBJECTDET_EXPORT DetectVehicleResult : public ServiceResult
			{
			public:
				struct Data
				{
					struct DetectObjectInfo
					{
						float score;
						std::string type;
						int id;
						std::vector<std::string> boxes;
					};
					std::vector<DetectObjectInfo> detectObjectInfoList;
					int height;
					int width;
				};


				DetectVehicleResult();
				explicit DetectVehicleResult(const std::string &payload);
				~DetectVehicleResult();
				Data getData()const;

			protected:
				void parse(const std::string &payload);
			private:
				Data data_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_OBJECTDET_MODEL_DETECTVEHICLERESULT_H_
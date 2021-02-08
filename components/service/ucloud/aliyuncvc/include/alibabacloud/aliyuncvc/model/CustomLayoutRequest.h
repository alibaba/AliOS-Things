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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_CUSTOMLAYOUTREQUEST_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_CUSTOMLAYOUTREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/aliyuncvc/AliyuncvcExport.h>

namespace AlibabaCloud
{
	namespace Aliyuncvc
	{
		namespace Model
		{
			class ALIBABACLOUD_ALIYUNCVC_EXPORT CustomLayoutRequest : public RpcServiceRequest
			{

			public:
				CustomLayoutRequest();
				~CustomLayoutRequest();

				std::string getLiveUUID()const;
				void setLiveUUID(const std::string& liveUUID);
				std::string getLayoutInfo()const;
				void setLayoutInfo(const std::string& layoutInfo);

            private:
				std::string liveUUID_;
				std::string layoutInfo_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_CUSTOMLAYOUTREQUEST_H_
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

#ifndef ALIBABACLOUD_IMAGEAUDIT_MODEL_SCANIMAGEREQUEST_H_
#define ALIBABACLOUD_IMAGEAUDIT_MODEL_SCANIMAGEREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/imageaudit/ImageauditExport.h>

namespace AlibabaCloud
{
	namespace Imageaudit
	{
		namespace Model
		{
			class ALIBABACLOUD_IMAGEAUDIT_EXPORT ScanImageRequest : public RpcServiceRequest
			{
			public:
				struct Task
				{
					std::string dataId;
					std::string imageURL;
					int maxFrames;
					int interval;
					long imageTimeMillisecond;
				};

			public:
				ScanImageRequest();
				~ScanImageRequest();

				std::vector<std::string> getScene()const;
				void setScene(const std::vector<std::string>& scene);
				std::vector<Task> getTask()const;
				void setTask(const std::vector<Task>& task);

            private:
				std::vector<std::string> scene_;
				std::vector<Task> task_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMAGEAUDIT_MODEL_SCANIMAGEREQUEST_H_
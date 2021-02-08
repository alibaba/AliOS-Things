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

#ifndef ALIBABACLOUD_VIDEOENHAN_MODEL_GENERATEVIDEOREQUEST_H_
#define ALIBABACLOUD_VIDEOENHAN_MODEL_GENERATEVIDEOREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/videoenhan/VideoenhanExport.h>

namespace AlibabaCloud
{
	namespace Videoenhan
	{
		namespace Model
		{
			class ALIBABACLOUD_VIDEOENHAN_EXPORT GenerateVideoRequest : public RpcServiceRequest
			{
			public:
				struct FileList
				{
					std::string fileName;
					std::string fileUrl;
					std::string type;
				};

			public:
				GenerateVideoRequest();
				~GenerateVideoRequest();

				std::string getTransitionStyle()const;
				void setTransitionStyle(const std::string& transitionStyle);
				std::string getScene()const;
				void setScene(const std::string& scene);
				float getDuration()const;
				void setDuration(float duration);
				bool getPuzzleEffect()const;
				void setPuzzleEffect(bool puzzleEffect);
				int getHeight()const;
				void setHeight(int height);
				bool getDurationAdaption()const;
				void setDurationAdaption(bool durationAdaption);
				std::vector<FileList> getFileList()const;
				void setFileList(const std::vector<FileList>& fileList);
				bool getMute()const;
				void setMute(bool mute);
				bool getAsync()const;
				void setAsync(bool async);
				bool getSmartEffect()const;
				void setSmartEffect(bool smartEffect);
				int getWidth()const;
				void setWidth(int width);
				std::string getStyle()const;
				void setStyle(const std::string& style);

            private:
				std::string transitionStyle_;
				std::string scene_;
				float duration_;
				bool puzzleEffect_;
				int height_;
				bool durationAdaption_;
				std::vector<FileList> fileList_;
				bool mute_;
				bool async_;
				bool smartEffect_;
				int width_;
				std::string style_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_VIDEOENHAN_MODEL_GENERATEVIDEOREQUEST_H_
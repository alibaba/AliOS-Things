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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_GETALBUMSBYNAMESRESULT_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_GETALBUMSBYNAMESRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/cloudphoto/CloudPhotoExport.h>

namespace AlibabaCloud
{
	namespace CloudPhoto
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT GetAlbumsByNamesResult : public ServiceResult
			{
			public:
				struct Album
				{
					struct Cover
					{
						bool isVideo;
						std::string idStr;
						std::string state;
						long ctime;
						std::string title;
						long mtime;
						std::string fileId;
						long height;
						long id;
						long width;
						std::string md5;
						std::string remark;
					};
					Cover cover;
					std::string idStr;
					std::string state;
					long ctime;
					long photosCount;
					long mtime;
					long id;
					std::string name;
				};


				GetAlbumsByNamesResult();
				explicit GetAlbumsByNamesResult(const std::string &payload);
				~GetAlbumsByNamesResult();
				std::string getAction()const;
				std::string getMessage()const;
				std::string getCode()const;
				std::vector<Album> getAlbums()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string action_;
				std::string message_;
				std::string code_;
				std::vector<Album> albums_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_GETALBUMSBYNAMESRESULT_H_
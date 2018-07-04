/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef __LITE_UTILS_CONFIG_H__
#define __LITE_UTILS_CONFIG_H__

#ifndef WITH_MEM_STATS
#ifdef _PLATFORM_IS_LINUX_
#define WITH_MEM_STATS              1
#define WITH_MEM_STATS_PER_MODULE   1
#else
#define WITH_MEM_STATS              0
#define WITH_MEM_STATS_PER_MODULE   0
#endif
#endif

#endif  /* __LITE_UTILS_CONFIG_H__ */

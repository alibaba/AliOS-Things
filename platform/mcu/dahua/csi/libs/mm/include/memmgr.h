/****************************************************************************
 * Copyright (C) 2015-2016 The YunOS Open Source Project
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
 *
 ****************************************************************************/
#ifndef _MEM_MGR_H_
#define _MEM_MGR_H_

#ifdef __cplusplus
extern "C"
{
#endif

void mm_heap_initialize(void);
int32_t mm_get_mallinfo(int32_t *total, int32_t *used, int32_t *free, int32_t *peak);
void mm_leak_dump(void);

#ifdef __cplusplus
}
#endif

#endif

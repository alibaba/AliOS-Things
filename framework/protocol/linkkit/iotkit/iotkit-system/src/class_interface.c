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
 
#include <stdio.h>
#include "class_interface.h"
#include "lite-utils.h"


void* new_object(const void* _class, ...)
{
    const abstract_class_t* ab_class = _class;

    void* p = LITE_calloc(1, ab_class->_size);

    if (p == NULL) return NULL;

    *(const abstract_class_t**)p = ab_class;

    if (ab_class->ctor) {
        va_list params;

        va_start(params, _class);
        p = ab_class->ctor(p, &params);
        va_end(params);
    }

    printf("%s %s @%p\n", ab_class->_class_name, "constructed", p);

    return p;
}

void delete_object(void* _object)
{
    const abstract_class_t** ab_class = _object;

    if (_object && *ab_class && (*ab_class)->dtor) {
        _object = (*ab_class)->dtor(_object);

        printf("%s %s @%p\n", (*ab_class)->_class_name, "destructed", _object);

        LITE_free(_object);
    }
}